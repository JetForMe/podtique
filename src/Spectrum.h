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



class MP3Decoder;



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
									Station(float inFrequency);
	void							addTrack(const std::string& inPath);
	
	float							frequency()							const		{ return mFrequency; }
	const std::string&				trackPath()							const		{ return mCurrentTrack; }
	off_t							lastPausedFrame()					const		{ return mLastPausedFrame; }
	void							setLastPausedFrame(off_t inVal)					{ mLastPausedFrame = inVal; }

	void							nextTrack();
	
private:
	float							mFrequency;
	
	std::vector<std::string>		mTracks;
	std::string						mCurrentTrack;
	uint32_t						mCurrentTrackIdx;
	off_t							mLastPausedFrame;
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
	
	int								numChannels()						const;
	uint32_t						rate()								const;
	
	void							updateTuning();
	bool							openStationTrack();
	size_t							minimumBufferSize()					const;
	bool							getStationAudioData(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded);
	
	void							addStation(const Station& inStation);
	
protected:
	bool							parseSpectrum(const picojson::value& inJSON);
	
private:
	std::string						mDataDirectory;
	float							mFrequency;
	float							mLastFrequency;
	float							mOffset;
	bool							mNeedsTuning;
	
	std::vector<Station>			mStations;
	int32_t							mCurrentStationIndex;
	MP3Decoder*						mCurrentTrack;
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
