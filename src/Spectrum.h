//
//  Spectrum.h
//  Podtique
//
//  Created by Roderick Mann on 12/6/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#ifndef __Podtique__Spectrum__
#define __Podtique__Spectrum__

//
//	Standard Includes
//

#include <string>
#include <vector>


//
//	Project Imports
//

#include "picojson.h"



class Decoder;



/**
	A Station encapsulates everything about a how a particular
	frequency maps to a playlist. It contains the frequency of the
	station, the list of tracks, the current track being played
	(or was last playing), as well as the frame when the last
	track was paused, to allow resuming the playlist if the user
	returns to this station.
	
	mFrequency			0.0 - 1.0
	mTracks				Vector of absolute paths to sound files.
	mCurrentTrack		Index in mTracks of current track.
	mLastPausedFrame	When a station is paused, the current
						frame of the track is recorded here.
*/

class
Station
{
public:
									Station(float inFrequency, const std::string& inDesc);
	virtual							~Station();
	
	void							addTrack(const std::string& inPath);
	
	float							frequency()							const		{ return mFrequency; }
	const std::string&				desc()								const		{ return mDesc; }
	const std::string&				trackPath()							const		{ return mCurrentTrack; }
	off_t							lastPausedFrame()					const		{ return mLastPausedFrame; }
	void							setLastPausedFrame(off_t inVal)					{ mLastPausedFrame = inVal; }

	void							nextTrack();
	uint32_t						trackIdx()							const		{ return mCurrentTrackIdx; }
	
	Decoder*						decoder()							const;
	
	size_t							minimumBufferSize()					const;
	int								numChannels()						const;
	uint32_t						rate()								const;
	
	bool							getAudioData(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded) const;

protected:
	bool							openTrack();

private:
	float							mFrequency;
	std::string						mDesc;
	
	std::vector<std::string>		mTracks;
	std::string						mCurrentTrack;
	uint32_t						mCurrentTrackIdx;
	off_t							mLastPausedFrame;

	Decoder*						mDecoder;
};







/**
	The Spectrum class manages the set of stations, and ensures the right
	sound file is being decoded for a the tuned frequency.
	
	TOOD: Rename this "tuner?" It started off as the collection of stations, but
			now has a lot of tuner-like functionality.
*/

class
Spectrum
{
public:
									Spectrum(const std::string& inDataDirectory);
	
	void							setFrequency(float inFrequency);
	float							frequency()							const		{ return mFrequency; }
	
	float							stationFrequency()					const;
	bool							stationTuned()						const		{ return mCurrentStationIndex >= 0; }
	
	float							contentWeight()						const		{ return mContentWeight; }
	float							staticWeight()						const		{ return mStaticWeight; }
	
	bool							updateTuning();
	
	void							addStation(const Station& inStation);
	
	size_t							minimumBufferSize()					const;
	bool							getStationAudioData(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded) const;
	const Station&					currentStation()					const		{ return mStations[mCurrentStationIndex]; }
	
protected:
	bool							parseSpectrum(const picojson::value& inJSON);
	
private:
	std::string						mDataDirectory;
	float							mFrequency;
	float							mLastFrequency;
	float							mOffset;
	bool							mNeedsTuning;
	
	float							mContentWeight;
	float							mStaticWeight;
	
	std::vector<Station>			mStations;
	int32_t							mCurrentStationIndex;
};


inline
float
Spectrum::stationFrequency() const
{
	if (mCurrentStationIndex < 0)
	{
		return 0.0;
	}
	else
	{
		return mStations[mCurrentStationIndex].frequency();
	}
}


#endif /* defined(__Podtique__Spectrum__) */
