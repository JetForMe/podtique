//
//  SndFile.h
//  Podtique
//
//  Created by Roderick Mann on 12/8/14.
//  Copyright (c) 2015 Latency: Zero, LLC. All rights reserved.
//

#ifndef __Podtique__SndFile__
#define __Podtique__SndFile__

//
//	Standard Includes
//

#include <string>

//
//	Library Includes
//

#include <sndfile.h>



class
SndFile
{
public:
					SndFile(const std::string& inPath);
					~SndFile();
	
	int				channels()		const			{ return mInfo.channels; }
	int				rate()			const			{ return mInfo.samplerate; }
	int				format()		const			{ return mInfo.format; }
	
	size_t			read(int16_t* inBuffer, size_t inNumFrames) const;
	
private:
	SNDFILE*		mSndFile;
	SF_INFO			mInfo;
};




#endif /* defined(__Podtique__SndFile__) */
