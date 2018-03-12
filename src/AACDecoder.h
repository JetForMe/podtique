//
//  AACDecoder.hpp
//  Podtique
//
//  Created by Roderick Mann on 10/14/2015.
//  Copyright © 2015 Latency: Zero. All rights reserved.
//

#ifndef AACDecoder_hpp
#define AACDecoder_hpp

#include "Decoder.h"

//
//	Standard Includes
//

#include <fstream>
#include <string>

//
//	Library Includes
//

#include <mp4ff.h>
#include <neaacdec.h>



class
AACDecoder : public Decoder
{
public:
							AACDecoder();
	virtual					~AACDecoder();
	
	virtual	bool			open(const std::string& inPath);
	virtual	void			close();
	
	virtual	bool			read(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded);

	virtual	bool			done()								const				{ return mDone; }
	virtual	int				encoding()							const				{ return mEncoding; }
	virtual	int				numChannels()						const				{ return mNumChannels; }
	virtual	uint32_t		rate()								const				{ return mRate; }
	
	virtual	off_t			currentFrame()						const;
	virtual	void			setCurrentFrame(off_t inFrame);
	
	virtual	size_t			minimumBufferSize()					const;

protected:
			int				findAACTrack()						const;
			uint32_t		mp4Read(void* inBuffer, uint32_t inLength);
			uint32_t		mp4Seek(uint64_t inPosition);
	
private:
	static	uint32_t		MP4Read(void* inUserData, void* inBuffer, uint32_t inLength);
	static	uint32_t		MP4Seek(void* inUserData, uint64_t inPosition);
	
private:
	NeAACDecHandle			mHandle;
	mp4ff_callback_t		mMP4Callbacks;
	mp4ff_t*				mMP4File;
	uint32_t				mFrameSize;
	int						mTrack;
	
	std::ifstream			mInputStream;
	uint8_t*				mInputBuffer;			//	Remove this from object
	size_t					mInputBufferSize;
	size_t					mInputDataSize;
	
	bool					mDone;
	uint32_t				mRate;
	int						mNumChannels;
	int						mEncoding;
	double					mDuration;
	
	int32_t					mNumFrames;
	int32_t					mCurrentFrameIdx;
};

#endif	//	AACDecoder_hpp
