//
//  MP3Decoder.h
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#ifndef __WoodenRadio__MP3Decoder__
#define __WoodenRadio__MP3Decoder__

//
//	Standard Includes
//

#include <string>

//
//	Library Includes
//

#include <mpg123.h>






class
MP3Decoder
{
public:
					MP3Decoder();
	virtual			~MP3Decoder();
	
	bool			open(const std::string& inPath);
	void			close();
	
	bool			read(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded);
	bool			done()								const				{ return mDone; }
	
	int				encoding()							const				{ return mEncoding; }
	int				numChannels()						const				{ return mNumChannels; }
	long			rate()								const				{ return mRate; }
	
	off_t			currentFrame()						const;
	void			setCurrentFrame(off_t inFrame);
	
	size_t			minimumBufferSize()					const;
	
protected:
	bool			createDecoder();
	void			releaseDecoder();
	
private:
	mpg123_handle*	mMPG;
	bool			mDone;
	long			mRate;
	int				mNumChannels;
	int				mEncoding;


	class
	Init
	{
	public:
		Init();
		~Init();
		
		bool		mInitialized;
	};
	
	static	Init				sInit;
};


















#endif /* defined(__WoodenRadio__MP3Decoder__) */
