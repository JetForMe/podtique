//
//  BBBMain.cpp
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2015 Latency: Zero, LLC. All rights reserved.
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




static sig_atomic_t				sHandledSignal;



class
Podtique
{
public:
					Podtique();
	virtual			~Podtique();
	
	void			setDataDir(const std::string& inDataDir)				{ mDataDir = inDataDir; }
	void			run();
	void			stop();
	virtual	void	mute(bool inMute) = 0;
	
protected:
	virtual	void	init();
	void			initLEDs(uint16_t inNumPixels, const std::string& inPRU0FirmwarePath, const std::string& inPRU1FirmwarePath);
	void			setBacklightColor(uint8_t inRed, uint8_t inGreen, uint8_t inBlue);
	
	virtual	bool	isOn() const = 0;
	
	virtual	float	readVol() const = 0;
	virtual	float	readFreq() const = 0;
	
	static	float	readADC(int inChannel);

private:
	std::string		mDataDir;
	Radio*			mRadio;
	
	uint32_t		mNumLEDs;
	ledscape_t* 	mLEDs;
	uint8_t			mLEDBufferIdx;
};


Podtique::Podtique()
	:
	mRadio(NULL),
	mNumLEDs(0),
	mLEDBufferIdx(0)
{
}

Podtique::~Podtique()
{
}

void
Podtique::init()
{
}

void
Podtique::initLEDs(uint16_t inNumLEDs, const std::string& inPRU0FirmwarePath, const std::string& inPRU1FirmwarePath)
{
	//	Init the LEDs…
	
	LogDebug("Loading PRU firmware:\n    %s\n    %s", inPRU0FirmwarePath.c_str(), inPRU1FirmwarePath.c_str());
	
	mNumLEDs = inNumLEDs;
	mLEDs = ::ledscape_init_with_programs(inNumLEDs, inPRU0FirmwarePath.c_str(), inPRU1FirmwarePath.c_str());
	
	//	Color-cycle the LEDs to indicate we’re running…
	
	setBacklightColor(255, 0, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(333));
	setBacklightColor(0, 255, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(333));
	setBacklightColor(0, 0, 255);
	std::this_thread::sleep_for(std::chrono::milliseconds(333));
	setBacklightColor(0, 0, 0);
}

float
Podtique::readADC(int inChannel)
{
	char path[128];
	snprintf(path, 128, "/sys/bus/iio/devices/iio:device0/in_voltage%u_raw", inChannel);
	int fd = ::open(path, O_RDONLY);
	if (fd < 0)
	{
		LogDebug("Unable to open ADC%u: %s", inChannel, std::strerror(errno));
		return -1.0;
	}
	
	float result = -1.0;
	
	char buf[16];
	ssize_t bytesRead = ::read(fd, buf, sizeof (buf) - 1);
	if (bytesRead > 0)
	{
		buf[bytesRead] = 0;
//		LogDebug("Read ADC%u: %s", inChannel, buf);
		int v = ::atoi(buf);
		result = v / 4095.0;
	}
	else
	{
		LogDebug("Read %d bytes out of %u from path %s", bytesRead, sizeof (buf) - 1, path);
	}
	
	::close(fd);
	
	return result;
}


void
Podtique::run()
{
	init();
	
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
	bool lastOn = true;
	while (true)
	{
		if (sHandledSignal > 0)
		{
			stop();
			::signal(sHandledSignal, SIG_DFL);
			::raise(sHandledSignal);
			break;
		}
		
		if (mLEDs != NULL)			//	Allow running even if PRUs aren't working
		{
			::ledscape_wait(mLEDs);
			::ledscape_draw(mLEDs, 0);
		}
		
		//	Read GPIOs and ADCs (with a delay before each, since
		//	reading these too rapidly leads to a hang)…
		
		bool on = isOn();
		
		//	Enable the audio with the radio…
		
		if (lastOn != !on)
		{
			std::this_thread::sleep_for(dur);
			mute(!on);
			lastOn = !on;
			
			//	If turning on, delay a bit before turning on
			//	the radio to reduce the content missed…
			
			if (on)
			{
				std::chrono::milliseconds turnOnDelay(500);
				std::this_thread::sleep_for(turnOnDelay);
			}
		}	
		
		//	Update the radio’s state…
			
		mRadio->setOn(on);
		if (on)
		{
			setBacklightColor(30, 30, 10);
		}
		else
		{
			setBacklightColor(0, 0, 0);
		}
		
		//	Update the analog values…
		
		float f = readFreq();
		float v = readVol();
		
		mRadio->setFrequency(f);
		mRadio->setVolume(v);
		
#if 0
		LogDebug("On: %u", on);
		LogDebug("Set frequency to: %.3f", f);
		LogDebug("Set vol: %.3f", v);
#endif
		
		//std::this_thread::sleep_for(dur);
	}

	//	Hang out while the radio runs…
	
	//mRadio->join();	no, that's not the right thing to do here
}

void
Podtique::setBacklightColor(uint8_t inRed, uint8_t inGreen, uint8_t inBlue)
{
	if (mLEDs == NULL)			//	Allow running even if PRUs aren't working
	{
		return;
	}
	
	mLEDBufferIdx = (mLEDBufferIdx + 1) % 2;
	ledscape_frame_t* const frame = ::ledscape_frame(mLEDs, mLEDBufferIdx);
	for (uint32_t idx = 0; idx < mNumLEDs; ++idx)
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
	mute(true);
	setBacklightColor(0, 0, 0);
}


#pragma mark -

/**
	The original proof-of-concept prototype, built with USB
	sound card, SparkFun amplifier, inverted WS2812 LED channel,
	etc.
*/

class
PodtiquePrototype1 : public Podtique
{
public:
	PodtiquePrototype1();

	virtual	void	mute(bool inMute);

protected:
	virtual	void	init();
	virtual	bool	isOn() const;

	virtual	float	readVol() const;
	virtual	float	readFreq() const;

private:
	GPIO			mOffOn;				//	"on" when low
	GPIO			mAudioMute;			//	Audio is muted when GPIO is high (FET pulls STBY line low, which mutes things, despite what the data sheet says)
};

PodtiquePrototype1::PodtiquePrototype1()
{
}

void
PodtiquePrototype1::init()
{
	Podtique::init();
	
	//	Configure the GPIOs…
	
	mOffOn.setGPIONumber(66);
	mOffOn.setInput();
	
	mAudioMute.setGPIONumber(27);
	mAudioMute.setOutput();
	mAudioMute.set(true);
	
	//	Init LEDs…
	
	initLEDs(24,
				"/lib/firmware/ws281x-inverted-single-channel-pru0.bin",
				"/lib/firmware/ws281x-inverted-single-channel-pru1.bin");
}

void
PodtiquePrototype1::mute(bool inMute)
{
	mAudioMute.set(inMute);
}

bool
PodtiquePrototype1::isOn() const
{
	std::chrono::milliseconds dur(100);
	std::this_thread::sleep_for(dur);
	bool off = mOffOn.get();
	return !off;
}

float
PodtiquePrototype1::readVol() const
{
	if (isOn())
	{
		std::chrono::milliseconds dur(50);
		std::this_thread::sleep_for(dur);
		float v = readADC(1);
		if (v < 0.0) v = 0.4;

		return v;
	}
	else
	{
		return 0.0;
	}
}

float
PodtiquePrototype1::readFreq() const
{
	if (isOn())
	{
		std::chrono::milliseconds dur(50);
		std::this_thread::sleep_for(dur);
		float f = readADC(0);
		if (f < 0.0) f = 0.100;
	
		return f;
	}
	else
	{
		return 0.0;
	}
}











#pragma mark -

/**
	First custom Cape, Podtique1 or PT1.
	
	• TLV320AIC3104 CODEC connected to McASP/I2S
	• TPA3140D2 10 W audio power amp
	• Non-inverted WS2812 with proper level shifter
	• Low-noise LDOs for analog supplies, GPIO-enableable.
	• Differnet GPIOs and ADCs for front panel
*/

class
PodtiquePT1 : public Podtique
{
public:
	PodtiquePT1();
	
	virtual	void	mute(bool inMute);

protected:
	virtual	void	init();
	virtual	bool	isOn() const;

	virtual	float	readVol() const;
	virtual	float	readFreq() const;

private:
	GPIO			mOffOn;				//	"on" when low
};

PodtiquePT1::PodtiquePT1()
{
}

void
PodtiquePT1::init()
{
	Podtique::init();
	
	//	Configure the GPIOs…
	
	mOffOn.setGPIONumber(26);		//	P8_14
	mOffOn.setInput();
	
	//	Init LEDs…
	
	initLEDs(16,
				"/lib/firmware/ws281x-single-channel-pru0.bin",
				"/lib/firmware/ws281x-single-channel-pru1.bin");
}

void
PodtiquePT1::mute(bool inMute)
{
}

#define kDelay		50

bool
PodtiquePT1::isOn() const
{
	std::chrono::milliseconds dur(kDelay);
	std::this_thread::sleep_for(dur);
	bool off = mOffOn.get();
	return !off;
}

float
PodtiquePT1::readVol() const
{
	if (isOn())
	{
		std::chrono::milliseconds dur(kDelay);
		std::this_thread::sleep_for(dur);
		float v = readADC(0);
		if (v < 0.0) v = 0.6;

		return v;
	}
	else
	{
		return 0.0;
	}
}

float
PodtiquePT1::readFreq() const
{
	if (isOn())
	{
		std::chrono::milliseconds dur(kDelay);
		std::this_thread::sleep_for(dur);
		float f = readADC(1);
		if (f < 0.0) f = 0.000;
	
		return f;
	}
	else
	{
		return 0.0;
	}
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
	
	const char* dd = getenv("DATA_DIR");
	if (dd == NULL)
	{
		fprintf(stderr, "DATA_DIR environment variable not set\n");
		return -1;
	}
	
	std::string dataDir(dd);
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
	
	//sPodtique = new PodtiquePrototype1;
	sPodtique = new PodtiquePT1;
	std::string p(dataDir);
	sPodtique->setDataDir(p);
	sPodtique->run();
	
	return 0;
}
