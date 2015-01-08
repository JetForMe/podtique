//
//  BBBMain.cpp
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

//
//	Standard Includes
//

#include <fcntl.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

//
//	Library Includes
//

#include "ledscape.h"

//
//	Project Includes
//

#include "GPIO.h"
#include "Radio.h"
#include "RadioDebug.h"



const uint32_t kNumPixels = 24;

static sig_atomic_t				sHandledSignal;



class
Podtique
{
public:
	Podtique(const std::string& inDataDir);
	void			run();
	void			stop();
	
protected:
	float			readADC(int inChannel);
	void			setBacklightColor(uint8_t inRed, uint8_t inGreen, uint8_t inBlue);
	
private:
	std::string		mDataDir;
	GPIO			mOffOn;				//	"on" when low
	GPIO			mAudioMute;			//	Audio is muted when GPIO is high (FET pulls STBY line low, which mutes things, despite what the data sheet says)
	Radio*			mRadio;
	ledscape_t* 	mLEDs;
	uint8_t			mLEDBufferIdx;
};


Podtique::Podtique(const std::string& inDataDir)
	:
	mDataDir(inDataDir),
	mOffOn(66),
	mAudioMute(27),
	mRadio(NULL),
	mLEDBufferIdx(0)
{
}

float
Podtique::readADC(int inChannel)
{
	char path[128];
	snprintf(path, 128, "/sys/bus/iio/devices/iio:device0/in_voltage%u_raw", inChannel);
	int fd = ::open(path, O_RDONLY);
	if (fd < 0)
	{
		LogDebug("Unable to open ADC0: %s", std::strerror(errno));
		return -1.0;
	}
	
	float result = 0.0;
	
	char buf[16];
	ssize_t bytesRead = ::read(fd, buf, sizeof (buf) - 1);
	if (bytesRead > 0)
	{
		buf[bytesRead] = 0;
		//LogDebug("Read %s", buf);
		int v = ::atoi(buf);
		result = v / 4095.0;
	}
	else
	{
		LogDebug("Bytes read: %u/%u", bytesRead, sizeof (buf) - 1);
	}
	
	::close(fd);
	
	return result;
}

void
Podtique::run()
{
	//	Configure the GPIOs…
	
	mOffOn.setInput();
	
	mAudioMute.setOutput();
	mAudioMute.set(true);
	
	//	Init the LEDs…
	
	mLEDs = ::ledscape_init_with_programs(kNumPixels,
													"/home/rmann/LEDscape/pru/bin/ws281x-inverted-single-channel-pru0.bin",
													"/home/rmann/LEDscape/pru/bin/ws281x-inverted-single-channel-pru1.bin");
	setBacklightColor(255, 0, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(333));
	setBacklightColor(0, 255, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(333));
	setBacklightColor(0, 0, 255);
	std::this_thread::sleep_for(std::chrono::milliseconds(333));
	setBacklightColor(0, 0, 0);
	
	
	//	Create the radio…
	
	LogDebug("Creating radio with data dir: %s", mDataDir.c_str());
	mRadio = new Radio(mDataDir);
	mRadio->start();
	
	//	Hacky delay to let the radio thread get
	//	to the wait to be turned on. Really there
	//	should be a radio ready wait…
	
	std::chrono::milliseconds dur(100);
	std::this_thread::sleep_for(dur);
	LogDebug("Radio Started");
	
	//	Loop over reading the input state,
	//	and updating the radio…
	
	dur = std::chrono::milliseconds(50);
	bool lastOff = true;
	while (true)
	{
		if (sHandledSignal > 0)
		{
			stop();
			::signal(sHandledSignal, SIG_DFL);
			::raise(sHandledSignal);
			break;
		}
		
		::ledscape_wait(mLEDs);
		::ledscape_draw(mLEDs, 0);
		
		//	Read GPIOs and ADCs (with a delay before each, since
		//	reading these too rapidly leads to a hang)…
		
		std::this_thread::sleep_for(dur);
		bool off = mOffOn.get();
		
		std::this_thread::sleep_for(dur);
		float f = readADC(0);
		if (f < 0.0) f = 0.100;
		
		std::this_thread::sleep_for(dur);
		float v = readADC(1);
		if (v < 0.0) v = 0.4;
		
		//	Enable the audio with the radio…
		
		if (lastOff != off)
		{
			std::this_thread::sleep_for(dur);
			mAudioMute.set(off);
			lastOff = off;
			
			//	If turning on, delay a bit before turning on
			//	the radio to reduce the content missed…
			
			if (!off)
			{
				std::chrono::milliseconds turnOnDelay(500);
				std::this_thread::sleep_for(turnOnDelay);
			}
		}	
		
		//	Update the radio’s state…
			
		mRadio->setOn(!off);
		if (off)
		{
			setBacklightColor(0, 0, 0);
		}
		else
		{
			setBacklightColor(30, 30, 10);
		}
		//LogDebug("On: %u", !off);
		mRadio->setFrequency(f);
		//LogDebug("Set frequency to: %.3f", f);
		mRadio->setVolume(v);
		//LogDebug("Set vol: %.3f", v);
		
		//std::this_thread::sleep_for(dur);
	}

	//	Hang out while the radio runs…
	
	//mRadio->join();	no, that's not the right thing to do here
}

void
Podtique::setBacklightColor(uint8_t inRed, uint8_t inGreen, uint8_t inBlue)
{
	//	Turn off the LEDs…
	
	mLEDBufferIdx = (mLEDBufferIdx + 1) % 2;
	ledscape_frame_t* const frame = ::ledscape_frame(mLEDs, mLEDBufferIdx);
	for (uint32_t idx = 0; idx < kNumPixels; ++idx)
	{
		::ledscape_set_color(
			frame,
			COLOR_ORDER_BRG,
			0,
			idx,
			inRed,
			inGreen,
			inBlue);
	}
	::ledscape_wait(mLEDs);
	::ledscape_draw(mLEDs, mLEDBufferIdx);
}

/**
	Attempts to restore GPIO outputs to “safe” values…
*/

void
Podtique::stop()
{
	mAudioMute.set(true);		//	Mute audio
	
}

#pragma mark -
#pragma mark • Startup Code

static Podtique*				sPodtique;

void
exitHandler()
{
	sPodtique->stop();
	
	LogDebug("Exit handler called");
}

void
intHandler(int inSignal)
{
	sHandledSignal = inSignal;
}

int
main(int inArgCount, const char** inArgs)
{
	//	Process arguments…
	
	std::string dataDir("/home/rmann/data");
	bool daemon = false;
	if (inArgCount > 1)
	{
		//LogDebug("usage: %s <path to data directory>\n", inArgs[0]);
		//return -1;
		
		std::string arg1(inArgs[1]);
		if (arg1 == "-d")
		{
			daemon = true;
			
			if (inArgCount > 2)
			{
				dataDir = inArgs[2];
			}
		}
		else
		{
			dataDir = inArgs[1];
		}
	}
	
	//	If running as a daemon, fork…
	
	if (daemon)
	{
		LogDebug("Forking");
		
		pid_t pid = ::fork();
		if (pid < 0)
		{
			exit(1);
		}
		
		if (pid > 0)
		{
			LogDebug("Child process id is %d, parent exiting", pid);
			exit(0);
		}
		
		//	If we get here, we’re a child…
		
		::umask(0);
		LogDebug("Child running");
		
		pid_t sid = ::setsid();
		if (sid < 0)
		{
			LogDebug("Child failed to setsid");
			exit(1);
		}
	}
	
	//	Install handlers to try to clean up GPIOs when
	//	we’re done…
	
	std::atexit(exitHandler);
	::signal(SIGHUP, intHandler);
	::signal(SIGINT, intHandler);
	
	std::string p(dataDir);
	sPodtique = new Podtique(p);
	sPodtique->run();
	
	return 0;
}
