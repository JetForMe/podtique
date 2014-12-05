//
//  BBBMain.cpp
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#include <stdio.h>

#include "Radio.h"


int
main(int inArgCount, const char** inArgs)
{
	if (inArgCount < 2)
	{
		std::fprintf(stderr, "usage: %s <path to data directory>\n", inArgs[0]);
		return -1;
	}
	
	Radio* mRadio = new Radio();
	
	std::string p(inArgs[1]);
	mRadio->start(p);
	
	mRadio->join();
	
	return 0;
}