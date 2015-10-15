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

#include <string>

//
//	Library Includes
//

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
	virtual	long			rate()								const				{ return mRate; }
	
	virtual	off_t			currentFrame()						const;
	virtual	void			setCurrentFrame(off_t inFrame);
	
	virtual	size_t			minimumBufferSize()					const;

private:
	NeAACDecHandle			mHandle;
	bool					mDone;
	long					mRate;
	int						mNumChannels;
	int						mEncoding;
};

#endif	//	AACDecoder_hpp
