//
//  AACDecoder.cpp
//  Podtique
//
//  Created by Roderick Mann on 10/14/2015.
//  Copyright © 2015 Latency: Zero. All rights reserved.
//

#include "AACDecoder.h"

//
//	Standard Includes
//

#include <cassert>
#include <fstream>

//
//	Library Includes
//

#include <mp4ff.h>
#include <neaacdec.h>

//
//	Project Includes
//

#include "RadioDebug.h"




AACDecoder::AACDecoder()
	:
	mHandle(NULL),
	mMP4Callbacks({ 0 }),
	mMP4File(NULL),
	mFrameSize(0),
	mInputBuffer(NULL),
	mInputBufferSize(0),
	mInputDataSize(0),
	mDone(false),
	mRate(0),
	mNumChannels(0),
	mEncoding(0),
	mDuration(0.0),
	mNumFrames(0),
	mCurrentFrameIdx(0)
{
	mMP4Callbacks.read = MP4Read;
	mMP4Callbacks.seek = MP4Seek;
	mMP4Callbacks.user_data = reinterpret_cast<void*> (this);
}

AACDecoder::~AACDecoder()
{
	close();
}


bool
AACDecoder::open(const std::string& inPath)
{
	//	Open the file for reading…
	
	mInputStream.open(inPath, std::ifstream::binary | std::ifstream::in);
	if (!mInputStream.good())
	{
		LogDebug("Unable to open file [%s]", inPath.c_str());
		return false;
	}
	
	mHandle = ::NeAACDecOpen();
	if (mHandle == NULL)
	{
		LogDebug("Unable to create new AAC decoder");
		close();
		return false;
	}
	
	//	Set the output configuration (probably redundant)…
	
	NeAACDecConfiguration* config = ::NeAACDecGetCurrentConfiguration(mHandle);
	config->outputFormat = FAAD_FMT_16BIT;
	bool success = ::NeAACDecSetConfiguration(mHandle, config);
	if (!success)
	{
		LogDebug("Error setting AAC config");
		close();
		return false;
	}
	
	//	Read in the first 1K of the file; that should
	//	be enough for the header…
	
	const size_t kBufSize = 128 * 1024;
	
	assert(mInputBuffer == NULL);
	
	mInputBuffer = new uint8_t[kBufSize];
	mInputBufferSize = kBufSize;
	mInputStream.read(reinterpret_cast<char*>(mInputBuffer), kBufSize);
	mInputDataSize = mInputStream.gcount();
	if (!mInputStream.good())
	{
		LogDebug("WARNING: Unable to read %zd bytes from file [%s]", mInputBufferSize, inPath.c_str());
	}
	
	mInputStream.seekg(0);				//	Return to the start of the file
	
	//	Check to see if it’s an MP4 file…
	
	if (mInputDataSize < 8)
	{
		LogDebug("WARNING: Invalid audio file; too short");
		close();
		return false;
	}
	
	if (!(mInputBuffer[4] == 'f'
		&& mInputBuffer[5] == 't'
		&& mInputBuffer[6] == 'y'
		&& mInputBuffer[7] == 'p'))
	{
		//	Right now, we only handle MP4 files…
		
		LogDebug("Not an MP4 (.m4a) file");
		close();
		return false;
	}
	
	//	It’s an MP4 file, we gotta find the AAC track…
	
	mMP4File = ::mp4ff_open_read(&mMP4Callbacks);
	if (mMP4File == NULL)
	{
		LogDebug("Unable to open MP4File");
		close();
		return false;
	}
	
	//	Find the AAC track…
	
	mTrack = findAACTrack();
	LogDebug("Found AAC track: %d", mTrack);
	
	//	Initialize the decoder…
	
	uint8_t* buf = NULL;
	uint32_t bufSize = 0;
	int result = ::mp4ff_get_decoder_config(mMP4File, mTrack, &buf, &bufSize);
	if (result != 0 || buf == NULL)
	{
		LogDebug("Unable to get_decoder_config");
		std::free(buf);
		close();
		return false;
	}
	
	unsigned long sampleRate;
	unsigned char numChannels;
	result = ::NeAACDecInit2(mHandle, buf, bufSize, &sampleRate, &numChannels);
	if (result != 0)
	{
		LogDebug("Error initializing AAC decoder from [%s]: %s", inPath.c_str(), NeAACDecGetErrorMessage(result));
		std::free(buf);
		close();
		return false;
	}
	
	LogDebug("File [%s] rate: %lu, channels: %u", inPath.c_str(), sampleRate, numChannels);
	mRate = static_cast<uint32_t> (sampleRate);
	mNumChannels = numChannels;
	
	//	Get the frame size…
	
	mp4AudioSpecificConfig mp4Config;
	result = ::NeAACDecAudioSpecificConfig(buf, bufSize, &mp4Config);
	if (result < 0)
	{
		LogDebug("Unable to get frame size or other track information");
		std::free(buf);
		close();
		return false;
	}
	
	mFrameSize = 1024;
	double f = 1024.0;
	if (mp4Config.frameLengthFlag == 1)
	{
		mFrameSize = 960;
	}
	if (mp4Config.sbr_present_flag == 1)
	{
		mFrameSize *= 2;
		f *= 2.0;
	}
	
	std::free(buf);
	buf = NULL;
	bufSize = 0;
	
	//	Calculate track duration…
	
	mNumFrames = ::mp4ff_num_samples(mMP4File, mTrack);
	mDuration = mNumFrames * (f - 1.0) / mp4Config.samplingFrequency;
	
	mDone = false;
	
	return true;
}

void
AACDecoder::close()
{
	if (mHandle != NULL)
	{
		::NeAACDecClose(mHandle);
		mHandle = NULL;
	}
	
	if (mMP4File != NULL)
	{
		::mp4ff_close(mMP4File);
		mMP4File = NULL;
	}
	
	if (mInputStream.is_open())
	{
		mInputStream.close();
	}
	
	if (mInputBuffer != NULL)
	{
		delete [] mInputBuffer;
		mInputBuffer = NULL;
	}
	
	mInputBufferSize = 0;
	mInputDataSize = 0;
	mFrameSize = 0;
	mNumFrames = 0;
	mDuration = 0.0;
}

bool
AACDecoder::read(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded)
{
	//	If we’ve run out of frames, return false…
	
	if (mCurrentFrameIdx >= mNumFrames)
	{
		LogDebug("End of track");
		mDone = true;
		return false;
	}
	
	//	We should still have frames, so decode the next frame…
	
	//int32_t dur = ::mp4ff_get_sample_duration(mMP4File, mTrack, mCurrentFrameIdx);
	
	uint8_t* buf = NULL;
	uint32_t bufSize = 0;
	int result = ::mp4ff_read_sample(mMP4File, mTrack, mCurrentFrameIdx, &buf, &bufSize);
	mCurrentFrameIdx += 1;
	if (result == 0)
	{
		LogDebug("Unable to read sample");
		return false;
	}
	
	NeAACDecFrameInfo frameInfo;
	::NeAACDecDecode2(mHandle, &frameInfo, buf, bufSize, &inBuffer, inBufferSize);
	std::free(buf);
	
	size_t sampleCount = frameInfo.samples;
	//LogDebug("Samples: %zd", sampleCount);
	outBytesDecoded = sampleCount * 2;		//	TODO: Don't hard-code this. Get it from output format setting.
	
	return true;
}

off_t
AACDecoder::currentFrame() const
{
	return 0;
}

void
AACDecoder::setCurrentFrame(off_t inFrame)
{
}

size_t
AACDecoder::minimumBufferSize() const
{
	//	10 seconds’ worth…
	
	return numChannels() * sizeof(int16_t) * rate() * 10;
}

#pragma mark - • MP4

uint32_t
AACDecoder::MP4Read(void* inUserData, void* inBuffer, uint32_t inLength)
{
	try
	{
		AACDecoder* self = reinterpret_cast<AACDecoder*>(inUserData);
		return self->mp4Read(inBuffer, inLength);
	}
	
	catch (...)
	{
		LogDebug("Unexpected exception in MP4Read");
		return 0;
	}
}

uint32_t
AACDecoder::MP4Seek(void* inUserData, uint64_t inPosition)
{
	try
	{
		AACDecoder* self = reinterpret_cast<AACDecoder*>(inUserData);
		return self->mp4Seek(inPosition);
	}
	
	catch (...)
	{
		LogDebug("Unexpected exception in MP4Seek");
		return 0;
	}
}

uint32_t
AACDecoder::mp4Read(void* inBuffer, uint32_t inLength)
{
	mInputStream.read(reinterpret_cast<char*>(inBuffer), inLength);
	if (mInputStream.eof())
	{
		mInputStream.clear();
		return 0;
	}
	
	if (!mInputStream.good())
	{
		mInputStream.clear();
		return -1;
	}
	
	uint32_t len = static_cast<uint32_t> (mInputStream.gcount());
	return len;
}

uint32_t
AACDecoder::mp4Seek(uint64_t inPosition)
{
	mInputStream.seekg(inPosition);
	//int64_t pos = mInputStream.tellg();
	if (mInputStream.good())
	{
		return 0;
	}
	else
	{
		mInputStream.clear();
		return -1;
	}
}


int
AACDecoder::findAACTrack() const
{
	int numTracks = ::mp4ff_total_tracks(mMP4File);
	for (int i = 0; i < numTracks; ++i)
	{
		uint8_t* buf = NULL;
		unsigned int bufSize = 0;
		::mp4ff_get_decoder_config(mMP4File, i, &buf, &bufSize);
		if (buf != NULL)
		{
			mp4AudioSpecificConfig config;
			int result = ::NeAACDecAudioSpecificConfig(buf, bufSize, &config);
			std::free(buf);
			
			if (result >= 0)
			{
				return i;
			}
		}
	}
	
	return -1;
}
