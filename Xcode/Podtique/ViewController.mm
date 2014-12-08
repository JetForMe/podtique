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

@end


@implementation ViewController

- (void)
viewDidLoad
{
	[super viewDidLoad];

	NSURL* dir = [[NSFileManager defaultManager] URLsForDirectory: NSApplicationSupportDirectory inDomains: NSUserDomainMask].lastObject;
	dir = [dir URLByAppendingPathComponent: @"WoodenRadio"];
	if (dir != nil)
	{
		const char* path = dir.path.UTF8String;
		std::string p(path, std::strlen(path));
		mRadio = new Radio(p);
		
		mRadio->start();
	}
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

@end
