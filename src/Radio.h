//
//  Radio.h
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#ifndef __WoodenRadio__Radio__
#define __WoodenRadio__Radio__

//
//	Standard Includes
//

#include <string>
#include <mutex>
#include <thread>
#include <vector>

//
//	Library Includes
//

#include <mpg123.h>

//
//	Project Includes
//

#include "Semaphore.h"

class AudioDevice;
class MP3Decoder;
class Spectrum;
class SndFile;


/**
*/

class
Radio
{
public:
						Radio(const std::string& inDataPath);
	
	void				start();
	
	void				setFrequency(float inFrequency);
	float				frequency()							const;
	void				setVolume(float inVolume);
	void				setOn(bool inVal);
	
	void				join();
	
protected:
	void				entry();
	void				processAudioAndOutput(void* ioBuffer, size_t inBufSize);
	
private:
	std::thread			mRadioThread;
	std::string			mDataDirectory;
	
	std::mutex			mConfigMutex;
	BinarySemaphore		mOnOff;
	
	Spectrum*			mSpectrum;
	float				mFrequency;
	float				mVolume;
	bool				mOn;
	float				mContentFraction;
	float				mNoiseFraction;
	
	SndFile*			mPinkNoise;
	int16_t*			mPinkNoiseBuffer;
	size_t				mPNBufIdx;
	int16_t*			mZeroBuffer;
	
	AudioDevice*		mOutputDevice;
};

#endif /* defined(__WoodenRadio__Radio__) */
