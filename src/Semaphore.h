//
//  Semaphore.h
//  Podtique
//
//  Created by Roderick Mann on 12/28/14.
//  Copyright (c) 2015 Latency: Zero, LLC. All rights reserved.
//

#ifndef __Podtique__Semaphore__
#define __Podtique__Semaphore__

#include <mutex>
#include <condition_variable>

class
Semaphore
{
public:
	Semaphore(int inCount = 0)
		:
		mCount(inCount)
	{
	}
	
	void
	notify()
	{
		{
			std::lock_guard<std::mutex>		lock(mMutex);
			++mCount;
		}
		mCondition.notify_one();
	}
	
	void
	wait()
	{
		std::unique_lock<std::mutex>	lock(mMutex);
		mCondition.wait(lock,
						[this]()
						{
							return mCount > 0;
						});
		mCount--;
	}
	
private:
	std::mutex					mMutex;
	std::condition_variable		mCondition;
	int							mCount;
};

class
BinarySemaphore
{
public:
	BinarySemaphore()
		:
		mReady(false)
	{
	}
	
	void
	set(bool inVal = true)
	{
		{
			std::lock_guard<std::mutex>		lock(mMutex);
			mReady = inVal;
		}
		if (inVal)
		{
			mCondition.notify_one();
		}
	}
	
	void
	clear()
	{
		{
			std::lock_guard<std::mutex>		lock(mMutex);
			mReady = false;
		}
	}
	
	void
	waitReady()
	{
		std::unique_lock<std::mutex>	lock(mMutex);
		mCondition.wait(lock,
						[this]()
						{
							return mReady;
						});
	}
	
private:
	std::mutex					mMutex;
	std::condition_variable		mCondition;
	bool						mReady;
};

#endif /* defined(__Podtique__Semaphore__) */
