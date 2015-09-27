//
//  SndFile.cpp
//  Podtique
//
//  Created by Roderick Mann on 12/8/14.
//  Copyright (c) 2015 Latency: Zero, LLC. All rights reserved.
//

#include "SndFile.h"



//
//	Project Includes
//

#include "RadioDebug.h"







SndFile::SndFile(const std::string& inPath)
	:
	mSndFile(NULL),
	mInfo({ 0 })
{
	mSndFile = ::sf_open(inPath.c_str(), SFM_READ, &mInfo);
	if (mSndFile == NULL)
	{
		LogDebug("Error opening file '%s': %s", inPath.c_str(), ::sf_strerror(NULL));
	}
}

SndFile::~SndFile()
{
	if (mSndFile != NULL)
	{
		::sf_close(mSndFile);
	}
}



size_t
SndFile::read(int16_t* inBuffer, size_t inNumFrames) const
{
	return ::sf_readf_short(mSndFile, inBuffer, inNumFrames);
}