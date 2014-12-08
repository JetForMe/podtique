//
//  BBBMain.cpp
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#include <stdio.h>

#include "Radio.h"
#include "RadioDebug.h"


int
main(int inArgCount, const char** inArgs)
{
	//	Validate arguments…
	
	if (inArgCount < 2)
	{
		std::fprintf(stderr, "usage: %s <path to data directory>\n", inArgs[0]);
		return -1;
	}
	
	//	Create the radio…
	
	std::string p(inArgs[1]);
	Radio* mRadio = new Radio(p);
	mRadio->start();
	
	//	Walk the radio dial…
	
#if 0
	float f = 0.0;
	float df = 0.001;
	float min = 0.25;
	float max = 0.35;
	while (true)
	{
		LogDebug("Set frequency to: %.3f", f);
		mRadio->setFrequency(f);
		f += df;
		if (f > max)
		{
			f = max;
			df = -df;
		}
		else if (f < min)
		{
			f = min;
			df = -df;
		}
		
		//	Delay a bit…
		
		std::chrono::milliseconds dur(1000);
		std::this_thread::sleep_for(dur);
	}
#else
	mRadio->setFrequency(0.3);
#endif

	//	Hang out while the radio runs…
	
	mRadio->join();
	
	return 0;
}