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

#include <fstream>

//
//	Library Includes
//

#include <neaacdec.h>

//
//	Project Includes
//

#include "RadioDebug.h"




AACDecoder::AACDecoder()
	:
	mHandle(NULL),
	mDone(false),
	mRate(0),
	mNumChannels(0),
	mEncoding(0)
{
}

AACDecoder::~AACDecoder()
{
	close();
}


bool
AACDecoder::open(const std::string& inPath)
{
	mHandle = ::NeAACDecOpen();
	if (mHandle == NULL)
	{
		LogDebug("Unable to create new AAC decoder");
		return false;
	}
	
	//	Read in the first 1K of the file; that should
	//	be enough for the header…
	
	std::ifstream is(inPath, std::ifstream::binary | std::ifstream::in);
	if (!is.good())
	{
		LogDebug("Unable to open file [%s]", inPath.c_str());
		return false;
	}
	
	const size_t kBufSize = 1024;
	uint8_t* buf = new uint8_t[kBufSize];
	is.read(reinterpret_cast<char*>(buf), kBufSize);
	size_t len = is.gcount();
	if (is.eof())
	{
		LogDebug("WARNING: Unable to read %lu bytes from file [%s]", kBufSize, inPath.c_str());
	}
	
	//	Initialize the decoder…
	
	unsigned long sampleRate;
	unsigned char numChannels;
	char err = ::NeAACDecInit(mHandle, buf, len, &sampleRate, &numChannels);
	if (err < 0)
	{
		LogDebug("Error initializing AAC decoder from [%s]: %s", inPath.c_str(), NeAACDecGetErrorMessage(err));
		return false;
	}
	
	LogDebug("File [%s] rate: %lu, channels: %u", inPath.c_str(), sampleRate, numChannels);
	mRate = sampleRate;
	mNumChannels = numChannels;
	
	delete [] buf;
	
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
}

bool
AACDecoder::read(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded)
{
	return false;
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
	return 1024;
}
