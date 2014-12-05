//
//  Radio.cpp
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#include "Radio.h"

//
//	Standard Includes
//

#include <cstdio>
#include <thread>

//
//	Library Includes
//

#include <ao/ao.h>
#include <mpg123.h>

#include "picojson.h"

//
//	Project Includes
//

#include "MP3Decoder.h"
#include "RadioDebug.h"





Radio::Radio()
	:
	mCurrentTrack(NULL),
	mFrequency(0.0),
	mVolume(0.0)
{
}

void
Radio::start(const std::string& inDataPath)
{
	//	Initialize the audio APIs…
	
	::ao_initialize();
	
	mCurrentTrack = new MP3Decoder();
	
	mDataDirectory = inDataPath;
	LogDebug("Using data directory: '%s'", mDataDirectory.c_str());
	
	//	Open the JSON file…
	
	//	TODO: For now, just open a specific MP3 file and play it…
	
	//	Start the primary thread…
	
	mRadioThread = std::thread(&Radio::entry, this);
}

void
Radio::setFrequency(float inVal)
{
	std::lock_guard<std::mutex>		lock(mConfigMutex);
	mFrequency = inVal;
}

void
Radio::setVolume(float inVal)
{
	std::lock_guard<std::mutex>		lock(mConfigMutex);
	mVolume = inVal;
}

void
Radio::entry()
{
	LogDebug("Radio::entry()");
	
	//	For now, open a known MP3 file…
	
	std::string path = mDataDirectory + "/09_WorkJuice_anthem.mp3";
	if (!mCurrentTrack->open(path))
	{
		return;
	}
	
	if (mCurrentTrack->encoding() != MPG123_ENC_SIGNED_16)
	{
		LogDebug("Unexpected encoding: %d", mCurrentTrack->encoding());
		return;
	}
	
	
	size_t bufSize = mCurrentTrack->recommendedBufferSize();
	uint8_t* buffer = new uint8_t[bufSize];
	
	//	Open the output audio channel…
	
	int driverID = ::ao_default_driver_id();
	ao_sample_format format = { 0 };
	format.bits = 16;
	format.channels = mCurrentTrack->numChannels();
	format.rate = (int) mCurrentTrack->rate();
	format.byte_format = AO_FMT_LITTLE;
	
	ao_device* device = ::ao_open_live(driverID, &format, NULL);
	if (device == NULL)
	{
		LogDebug("Error opening output device");
		return;
	}
	
	//	Decode bytes, send them to the output channel…
	
	bool success;
	do
	{
		size_t bytesDecoded;
		success = mCurrentTrack->read(buffer, bufSize, bytesDecoded);
		if (success)
		{
			LogDebug("Decoded %lu bytes", bytesDecoded);
		}
		
		::ao_play(device, (char*) buffer, (uint32_t) bufSize);
	}
	while (success);
	
	::ao_close(device);
	
	std::chrono::milliseconds dur(2000);
	std::this_thread::sleep_for(dur);
	
	LogDebug("Radio::entry() exit");
}

void
Radio::join()
{
	mRadioThread.join();
}

