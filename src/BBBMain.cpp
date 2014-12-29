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

//
//	Library Includes
//

#include <pruio.h>

//
//	Project Includes
//

#include "GPIO.h"
#include "Radio.h"
#include "RadioDebug.h"













float
readADC(int inChannel)
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
configGPIOs()
{
	
}

int
main(int inArgCount, const char** inArgs)
{
	//	Validate arguments…
	
	if (inArgCount < 2)
	{
		LogDebug("usage: %s <path to data directory>\n", inArgs[0]);
		return -1;
	}
	
	//	Create the GPIOs…
	
	GPIO	offOn(66);				//	"on" when low
	offOn.setInput();
	
	GPIO	audioEnable(27);		//	Audio is enabled when GPIO is high (FET pulls STBY line low)
	audioEnable.setOutput();
	
	//	Create the radio…
	
	std::string p(inArgs[1]);
	Radio* mRadio = new Radio(p);
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
	int count = 0;
	while (true)
	{
		//	Every few times through the loop,
		//	check the GPIOs…
		
#if 0
		if (count-- <= 0)
		{
			count = 5;
#endif			
			//	Check to see if the radio is on. It is on when the GPIO is low…
			
			std::this_thread::sleep_for(dur);
			
			bool off = offOn.get();
			mRadio->setOn(!off);
			//LogDebug("On: %u", !off);
			
			//	Enable the audio with the radio…
			
			std::this_thread::sleep_for(dur);
			audioEnable.set(!off);
#if 0
		}
#endif
		
		std::this_thread::sleep_for(dur);
		
		float f = readADC(0);
		if (f < 0.0) f = 0.100;
		mRadio->setFrequency(f);
		//LogDebug("Set frequency to: %.3f", f);
		
		std::this_thread::sleep_for(dur);
		
		float v = readADC(1);
		if (v < 0.0) v = 0.4;
		mRadio->setVolume(v);
		//LogDebug("Set vol: %.3f", v);
		
		//std::this_thread::sleep_for(dur);
	}

	//	Hang out while the radio runs…
	
	mRadio->join();
	
	return 0;
}
