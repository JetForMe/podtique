//
//  Radio.cpp
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2015 Latency: Zero, LLC. All rights reserved.
//

#include "Radio.h"

//
//	Standard Includes
//

#include <cstdio>
#include <mutex>
#include <thread>

//
//	Library Includes
//

#include <mpg123.h>

#include "picojson.h"

//
//	Project Includes
//

#include "AudioDevice.h"
#include "MP3Decoder.h"
#include "RadioConstants.h"
#include "RadioDebug.h"
#include "Spectrum.h"
#include "SndFile.h"








#pragma mark -
#pragma mark • Radio

const size_t	kPinkNoiseNumFrames				=	64 * 1024;
const size_t	kEmptyBufferSize				=	4 * 1024;

const float		kVolumeR1 = 3.5;
const float		kVolumeR0 = 1.0 / (std::exp(kVolumeR1) - 1.0);
		


Radio::Radio(const std::string& inDataDirectory)
	:
	mDataDirectory(inDataDirectory),
	mSpectrum(NULL),
	mVolume(0.0),
	mOn(false),
	mPinkNoise(NULL),
	mOutputDevice(NULL)
{
	LogDebug("Using data directory: '%s'", mDataDirectory.c_str());
	
	mPinkNoise = new SndFile(mDataDirectory + "/pinknoise.wav");
	mPinkNoiseBuffer = new int16_t[kPinkNoiseNumFrames];
	size_t framesRead = mPinkNoise->read(mPinkNoiseBuffer, kPinkNoiseNumFrames);
	if (framesRead != kPinkNoiseNumFrames)
	{
		LogDebug("Unable to read all frames");
	}
	mPNBufIdx = 0;
	
	mZeroBuffer = new int16_t[kEmptyBufferSize];
	::memset(mZeroBuffer, 0, sizeof (int16_t) * kEmptyBufferSize);
	
	mSpectrum = new Spectrum(mDataDirectory);
	mOutputDevice = new AudioDevice();
	mOutputDevice->setFormat(1, 44100);
}

void
Radio::start()
{
	
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
	
	//LogDebug("Frequency set: %f", mFrequency);
}

float
Radio::frequency() const
{
	Radio* self = const_cast<Radio*> (this);
	std::lock_guard<std::mutex>		lock(self->mConfigMutex);
	return mFrequency;
}

void
Radio::setVolume(float inVal)
{
	//	Logarithmic volume from https://mathscinotes.wordpress.com/2011/12/22/potentiometer-math/
	
	float v = kVolumeR0 * (std::exp(kVolumeR1 * inVal) - 1.0);
	if (v > 1.0)
	{
		v = 1.0;
	}
	else if (v < 0.0)
	{
		v = 0.0;
	}
	
	std::lock_guard<std::mutex>		lock(mConfigMutex);
	mVolume = v;
	
	//LogDebug("Volume set: %f", mVolume);
}

void
Radio::setOn(bool inVal)
{
	std::lock_guard<std::mutex>		lock(mConfigMutex);
	
	if (inVal == mOn)
	{
		return;
	}
	
	mOn = inVal;
	mOnOff.set(mOn);
}

void
Radio::entry()
{
	LogDebug("Radio::entry()");
	
	mOutputDevice->setFormat(1, 44100);
	
	//	Decode bytes, send them to the output channel…
	
	uint8_t* buffer = NULL;
	size_t lastBufferSize = 0;
	do
	{
		//	Wait until we’re turned on…
		
		if (!mOn)
		{
			LogDebug("Radio off");
			mOnOff.waitReady();
			LogDebug("Radio on");
		}
		
		//	Update the tuning at the start of each pass through the
		//	loop…
		
		mSpectrum->setFrequency(frequency());
		mSpectrum->updateTuning();
		
#if 0
		if (mSpectrum->stationTuned())
		{
			mOutputDevice->setFormat(mSpectrum->numChannels(), mSpectrum->rate());
		}
		else
		{
			mOutputDevice->setFormat(1, 44100);
		}
#endif

		//	Reallocate buffer if minimum size changes…
		
		size_t bufSize = mSpectrum->minimumBufferSize();
		if (lastBufferSize != bufSize)
		{
			delete [] buffer;
			buffer = new uint8_t[bufSize];
			lastBufferSize = bufSize;
		}
	
		size_t bytesDecoded;
		bool success = mSpectrum->getStationAudioData(buffer, bufSize, bytesDecoded);
		if (success)
		{
			//LogDebug("Decoded %lu bytes", bytesDecoded);
			
			processAudioAndOutput(buffer, bytesDecoded);
		}
		else
		{
			processAudioAndOutput(mZeroBuffer, kEmptyBufferSize);
		}
	}
	while (true);
	
	//std::chrono::milliseconds dur(2000);
	//std::this_thread::sleep_for(dur);
	
	//LogDebug("Radio::entry() exit");
}

void
Radio::processAudioAndOutput(void* ioBuffer, size_t inBufSize)
{
	//	TODO: We're assuming a lot about the structure of the buffer here!
	
	int16_t* p = reinterpret_cast<int16_t*> (ioBuffer);
	size_t numSamples = inBufSize / sizeof (int16_t);
	for (size_t i = 0; i < numSamples; ++i)
	{
		int16_t noise = mPinkNoiseBuffer[mPNBufIdx++];
		if (mPNBufIdx >= kPinkNoiseNumFrames)
		{
			mPNBufIdx = 0;
		}
		
		p[i] = mVolume * (p[i] * mSpectrum->contentWeight() + noise * mSpectrum->staticWeight() * 0.2);
	}
	
	//	Output the result…
	
	mOutputDevice->play(ioBuffer, inBufSize);
}

void
Radio::join()
{
	mRadioThread.join();
}

