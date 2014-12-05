//
//  RadioDebug.h
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#ifndef WoodenRadio_RadioDebug_h
#define WoodenRadio_RadioDebug_h

#include <string.h>
#include <stdio.h>

#define LogDebug(inFormat, ...)									\
	fprintf(stderr, "<%s:%d>: " inFormat "\n",					\
	strrchr("/" __FILE__, '/') + 1,								\
	__LINE__,													\
	## __VA_ARGS__)


#endif
