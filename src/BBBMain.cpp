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
		return 0.0;
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
	//	Test pruio…
	
#if 0
	pruIo* io = ::pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1);
	if (io == NULL)
	{
		LogDebug("pruio_new() returned NULL");
		return 1;
	}
	 
	char* pruErr = ::pruio_config(io, 1, 0x1FE, 0, 4);
	if (pruErr != NULL)
	{
		LogDebug("Error in pruio_config: %s", pruErr);
		return 1;
	}
#endif
	
	//	Validate arguments…
	
	if (inArgCount < 2)
	{
		LogDebug("usage: %s <path to data directory>\n", inArgs[0]);
		return -1;
	}
	
	//	Create the radio…
	
	std::string p(inArgs[1]);
	Radio* mRadio = new Radio(p);
	mRadio->start();
	
	mRadio->setOn(true);
	
	//	Loop over reading the input state,
	//	and updating the radio…
	
	while (true)
	{
		float f = readADC(0);
		mRadio->setFrequency(f);
		//LogDebug("Set frequency to: %.3f", f);
		
		float v = readADC(1);
		mRadio->setVolume(v);
		//LogDebug("Set vol: %.3f", v);
		
		std::chrono::milliseconds dur(100);
		std::this_thread::sleep_for(dur);
	}

	//	Hang out while the radio runs…
	
	mRadio->join();
	
	return 0;
}
