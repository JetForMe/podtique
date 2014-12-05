//
//  AppDelegate.m
//  Podtique
//
//  Created by Roderick Mann on 12/3/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#import "AppDelegate.h"

#import <string>
#import "Radio.h"



@interface AppDelegate()
{
	Radio*				mRadio;
}

@end

@implementation AppDelegate



- (void)
applicationDidFinishLaunching: (NSNotification*) inNotification
{
	NSURL* dir = [[NSFileManager defaultManager] URLsForDirectory: NSApplicationSupportDirectory inDomains: NSUserDomainMask].lastObject;
	dir = [dir URLByAppendingPathComponent: @"WoodenRadio"];
	if (dir != nil)
	{
		mRadio = new Radio();
		
		const char* path = dir.path.UTF8String;
		std::string p(path, std::strlen(path));
		mRadio->start(p);
	}
}

@end
