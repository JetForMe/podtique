//
//  MP3Decoder.h
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#ifndef __WoodenRadio__MP3Decoder__
#define __WoodenRadio__MP3Decoder__

#include "Decoder.h"

//
//	Standard Includes
//

#include <string>

//
//	Library Includes
//

#include <mpg123.h>






class
MP3Decoder : public Decoder
{
public:
							MP3Decoder();
	virtual					~MP3Decoder();
	
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
	bool					createDecoder();
	void					releaseDecoder();
	
private:
	mpg123_handle*			mMPG;
	bool					mDone;
	uint32_t				mRate;
	int						mNumChannels;
	int						mEncoding;


	class
	Init
	{
	public:
		Init();
		~Init();
		
		bool				mInitialized;
	};
	
	static	Init			sInit;
};


















#endif /* defined(__WoodenRadio__MP3Decoder__) */
