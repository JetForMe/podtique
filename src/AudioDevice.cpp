//
//  AudioDevice.cpp
//  Podtique
//
//  Created by Roderick Mann on 12/7/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#include "AudioDevice.h"

//
//	Standard Includes
//

#include <stdint.h>

//
//	Library Includes
//

#include <ao/ao.h>

//
//	Project Includes
//

#include "RadioDebug.h"




AudioDevice::AudioDevice()
	:
	mDevice(NULL),
	mFormat({ 0 })
{
	//	Initialize the audio APIs…
	
	::ao_initialize();
}


AudioDevice::~AudioDevice()
{
	close();
}

void
AudioDevice::close()
{
	if (mDevice != NULL)
	{
		::ao_close(mDevice);
		mDevice = NULL;
	}
}

void
AudioDevice::setFormat(int inNumChannels, int inRate)
{
	if (inNumChannels == mNumChannels && inRate == mRate)
	{
		return;
	}
	
	mNumChannels = inNumChannels;
	mRate = inRate;
	
	close();
	
	//	Open the output audio channel…
	
	int driverID = ::ao_default_driver_id();
	ao_sample_format format = { 0 };
	format.bits = 16;
	format.channels = inNumChannels;
	format.rate = inRate;
	format.byte_format = AO_FMT_LITTLE;
	
	mDevice = ::ao_open_live(driverID, &format, NULL);
	if (mDevice == NULL)
	{
		LogDebug("Error opening output device");
	}
}


void
AudioDevice::play(const void* inBuffer, size_t inBufferSize) const
{
	::ao_play(mDevice, (char*) inBuffer, (uint32_t) inBufferSize);
}


