//
//  ViewController.m
//  Podtique
//
//  Created by Roderick Mann on 12/5/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#import "ViewController.h"

#import <string>
#import "Radio.h"

@interface ViewController()
{
	Radio*				mRadio;
}

@property (nonatomic, assign)	float				frequency;
@property (nonatomic, assign)	float				volume;
@property (nonatomic, assign)	bool				radioOn;

@end


@implementation ViewController

- (void)
viewDidLoad
{
	[super viewDidLoad];

//	NSURL* dir = [[NSFileManager defaultManager] URLsForDirectory: NSDesktopDirectory inDomains: NSUserDomainMask].lastObject;
//	dir = [dir URLByAppendingPathComponent: @"TAHReencoded"];
	NSURL* dir = [NSURL fileURLWithPath: @"/Users/rmann/Projects/Podtique/TAHReencoded"];
	NSLog(@"Content dir: '%@'", dir.path);
	
	if (dir != nil)
	{
		const char* path = dir.path.UTF8String;
		std::string p(path, std::strlen(path));
		mRadio = new Radio(p);
		
		mRadio->start();
	}
	
	self.volume = 0.7;
}

- (void)
setFrequency: (float) inVal
{
	if (inVal == mFrequency)
	{
		return;
	}
	
	mFrequency = inVal;
	mRadio->setFrequency(self.frequency);
}

@synthesize frequency				=	mFrequency;

- (void)
setVolume: (float) inVal
{
	if (inVal == mVolume)
	{
		return;
	}
	
	mVolume = inVal;
	mRadio->setVolume(self.volume);
}

@synthesize volume					=	mVolume;

- (void)
setRadioOn: (bool) inVal
{
	if (inVal == mRadioOn)
	{
		return;
	}
	
	mRadioOn = inVal;
	mRadio->setOn(self.radioOn);
}

@synthesize radioOn					=	mRadioOn;

@end
