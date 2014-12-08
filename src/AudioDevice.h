//
//  AudioDevice.h
//  Podtique
//
//  Created by Roderick Mann on 12/7/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#ifndef __Podtique__AudioDevice__
#define __Podtique__AudioDevice__

//
//	Library Includes
//

#include <ao/ao.h>






class
AudioDevice
{
public:
							AudioDevice();
							~AudioDevice();
	
	void					close();
	void					setFormat(int inNumChannels, int inRate);
	
	void					play(const void* inBuffer, size_t inBufferSize) const;
	
	
private:
	ao_device*				mDevice;
	ao_sample_format		mFormat;
	int						mNumChannels;
	int						mRate;
};







#endif	//	__Podtique__AudioDevice__
