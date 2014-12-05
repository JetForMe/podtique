//
//  MP3Decoder.cpp
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#include "MP3Decoder.h"

//
//	Standard Includes
//

#include <cstdio>
#include <string>

//
//	Library Includes
//

#include <mpg123.h>

//
//	Project Includes
//

#include "RadioDebug.h"








MP3Decoder::MP3Decoder()
	:
	mMPG(NULL)
{
}

MP3Decoder::~MP3Decoder()
{
	releaseDecoder();
}


bool
MP3Decoder::createDecoder()
{
	releaseDecoder();
	
	int err = 0;
	mMPG = ::mpg123_new(NULL, &err);
	if (mMPG == NULL)
	{
		LogDebug("Unable to create new decoder: %s", ::mpg123_plain_strerror(err));
		return false;
	}
	
	return true;
}

void
MP3Decoder::releaseDecoder()
{
	if (mMPG != NULL)
	{
		close();
		::mpg123_delete(mMPG);
		mMPG = NULL;
	}
}

bool
MP3Decoder::open(const std::string& inPath)
{
	//	Ensure there’s an underlying decoder…
	
	if (!createDecoder())
	{
		return false;
	}
	
	//	Open the file…
	
	int err = ::mpg123_open(mMPG, inPath.c_str());
	if (err != MPG123_OK)
	{
		LogDebug("Unable to open sound file '%s': %s", inPath.c_str(), ::mpg123_plain_strerror(err));
		releaseDecoder();
		return false;
	}
	
	//	Get its format…
	
	err = ::mpg123_getformat(mMPG, &mRate, &mNumChannels, &mEncoding);
	if (err != MPG123_OK)
	{
		LogDebug("Unable to get mp3 format: %s", ::mpg123_plain_strerror(err));
		
		close();
		return false;
	}
	
	//	Example code says this prevents format changes?
	
	::mpg123_format_none(mMPG);
	::mpg123_format(mMPG, mRate, mNumChannels, mEncoding);
	
	return true;
}

void
MP3Decoder::close()
{
	if (mMPG != NULL)
	{
		::mpg123_close(mMPG);
	}
}

bool
MP3Decoder::read(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded)
{
	int err = ::mpg123_read(mMPG, reinterpret_cast<unsigned char*> (inBuffer), inBufferSize, &outBytesDecoded);
	return err == MPG123_OK;
}

size_t
MP3Decoder::recommendedBufferSize() const
{
	return ::mpg123_outblock(mMPG);
}






MP3Decoder::Init			MP3Decoder::sInit;

MP3Decoder::Init::Init()
	:
	mInitialized(false)
{
	int err = ::mpg123_init();
	if (err != MPG123_OK)
	{
		LogDebug("Unable to initialize libmpg123: %s", ::mpg123_plain_strerror(err));
	}
	else
	{
		mInitialized = true;
	}
}

MP3Decoder::Init::~Init()
{
	::mpg123_exit();
}