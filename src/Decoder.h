//
//  Decoder.hpp
//  Podtique
//
//  Created by Roderick Mann on 10/14/2015.
//  Copyright © 2015 Latency: Zero. All rights reserved.
//

#ifndef Decoder_hpp
#define Decoder_hpp

//
//	Standard Includes
//

#include <cstdlib>
#include <string>

class
Decoder
{
public:
						Decoder();
	virtual				~Decoder();
	
	virtual	bool		open(const std::string& inPath)											=	0;
	virtual	void		close()																	=	0;
	
	virtual	bool		read(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded)		=	0;
	
	virtual	bool		done()								const								=	0;

	virtual	int			encoding()							const								=	0;
	virtual	int			numChannels()						const								=	0;
	virtual	long		rate()								const								=	0;
	
	virtual	off_t		currentFrame()						const								=	0;
	virtual	void		setCurrentFrame(off_t inFrame)											=	0;
	
	virtual	size_t		minimumBufferSize()					const								=	0;

private:
};

#endif	//	Decoder_hpp
