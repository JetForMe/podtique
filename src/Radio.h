//
//  Radio.h
//  WoodenRadio
//
//  Created by Roderick Mann on 12/4/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#ifndef __WoodenRadio__Radio__
#define __WoodenRadio__Radio__

#include <string>
#include <thread>

#include <mpg123.h>



class MP3Decoder;


class
Radio
{
public:
					Radio();
	
	void			start(const std::string& inDataPath);
	
	void			setFrequency(float inFrequency);
	void			setVolume(float inVolume);
	
	void			join();
	
protected:
	void			entry();
	
private:
	std::thread		mRadioThread;
	std::string		mDataDirectory;
	
	std::mutex		mConfigMutex;
	
	MP3Decoder*		mCurrentTrack;
	float			mFrequency;
	float			mVolume;
};

#endif /* defined(__WoodenRadio__Radio__) */
