//
//  Spectrum.cpp
//  Podtique
//
//  Created by Roderick Mann on 12/6/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#include "Spectrum.h"



//
//	Stamdard Includes
//

#include <cmath>
#include <fstream>

//
//	Project Includes
//

#include "picojson.h"

#include "MP3Decoder.h"
#include "RadioConstants.h"
#include "RadioDebug.h"





#pragma mark -
#pragma mark • Station


Station::Station(float inFrequency)
	:
	mFrequency(inFrequency),
	mCurrentTrackIdx(0),
	mLastPausedFrame(0)
{
}

void
Station::addTrack(const std::string& inPath)
{
	mTracks.push_back(inPath);
	if (mCurrentTrack.empty())
	{
		mCurrentTrack = inPath;
	}
}

/**
	Advances the track index to the next track,
	wrapping back to 0 when reaching the end.
*/

void
Station::nextTrack()
{
	mCurrentTrackIdx += 1;
	if (mCurrentTrackIdx >= mTracks.size())
	{
		mCurrentTrackIdx = 0;
	}
	
	mCurrentTrack = mTracks[mCurrentTrackIdx];
	mLastPausedFrame = 0;
}


#pragma mark -
#pragma mark • Spectrum

Spectrum::Spectrum(const std::string& inDataDirectory)
	:
	mDataDirectory(inDataDirectory),
	mFrequency(0.0),
	mNeedsTuning(true),
	mCurrentStationIndex(-1),
	mCurrentTrack(NULL)
{
	//	For now, hard-code some stations and playlists…
	
#if 0
	Station s1(0.3);
	s1.addTrack(mDataDirectory + "/TAH__2__Sparks_Nevada--__Inside_Out_in_Outer_Space_.mp3");
	s1.addTrack(mDataDirectory + "/09_WorkJuice_anthem.mp3");
	s1.addTrack(mDataDirectory + "/TAH__6_Sparks_Nevada_Marshal_on_Mars-_The_Agony_of_the_Feet_.mp3");
	s1.addTrack(mDataDirectory + "/43_Sparks_Nevada_Marshal_on_Mars_Companeros.mp3");
	addStation(s1);
	
	Station s2(0.6);
	s2.addTrack(mDataDirectory + "/TAH__1__Beyond_Belief___Hell_Is_the_Loneliest_Number_.mp3");
	s2.addTrack(mDataDirectory + "/Beyond_Belief_-_Rosemarys_Baby_Shower.mp3");
	s2.addTrack(mDataDirectory + "/Beyond_Belief_-_Chitty_Chitty_Bang_Bang_Youre_Dead.mp3");
	addStation(s2);
#else

	//	Load the spectrum.json file…
	
	std::string sf = mDataDirectory + "/spectrum.json";
	std::ifstream is(sf);
	
	picojson::value json;
	std::string err = picojson::parse(json, is);
	if (!err.empty())
	{
		LogDebug("Failed to parse spectrum.json: %s", err.c_str());
	}
	
	parseSpectrum(json);
#endif

	//	Create the MP3 decoder…
	
	mCurrentTrack = new MP3Decoder();
}

bool
getFromJSONIter(picojson::value::array::const_iterator& inIter, const std::string& inKey, std::string& outVal)
{
	if (!inIter->is<picojson::value::object>())
	{
		return false;
	}
	
	picojson::value::object obj = inIter->get<picojson::value::object>();
	const picojson::value& v = obj[inKey];
	if (!v.is<std::string>())
	{
		return false;
	}
	outVal = v.get<std::string>();
	
	return true;
}

bool
getFromJSONIter(picojson::value::array::const_iterator& inIter, const std::string& inKey, double& outVal)
{
	if (!inIter->is<picojson::value::object>())
	{
		return false;
	}
	
	picojson::value::object obj = inIter->get<picojson::value::object>();
	const picojson::value& v = obj[inKey];
	if (!v.is<double>())
	{
		return false;
	}
	outVal = v.get<double>();
	
	return true;
}

bool
getFromJSONIter(picojson::value::array::const_iterator& inIter, const std::string& inKey, picojson::value::array& outVal)
{
	if (!inIter->is<picojson::value::object>())
	{
		return false;
	}
	
	picojson::value::object obj = inIter->get<picojson::value::object>();
	const picojson::value& v = obj[inKey];
	if (!v.is<picojson::value::array>())
	{
		return false;
	}
	outVal = v.get<picojson::value::array>();
	
	return true;
}

bool
Spectrum::parseSpectrum(const picojson::value& inJSON)
{
	//	Top level is an array…
	
	if (!inJSON.is<picojson::array>())
	{
		return false;
	}
	
	const picojson::value::array& stations = inJSON.get<picojson::array>();
	for (picojson::value::array::const_iterator iter = stations.begin(); iter != stations.end(); ++iter)
	{
		std::string desc;
		if (!getFromJSONIter(iter, "desc", desc)) { continue; }
		
		double freq;
		if (!getFromJSONIter(iter, "freq", freq)) { continue; }
		
		picojson::value::array playlist;
		if (!getFromJSONIter(iter, "playlist", playlist)) { continue; }
		
		LogDebug("Station: %s, freq: %f, tracks: %ju", desc.c_str(), freq, (uintmax_t) playlist.size());
		
		Station station(freq);
		for(size_t idx = 0; idx < playlist.size(); ++idx)
		{
			const picojson::value& v = playlist[idx];
			if (!v.is<std::string>()) { continue; }
			std::string track = v.get<std::string>();
			LogDebug("Track %02ju: %s", (uintmax_t) idx, track.c_str());
			station.addTrack(mDataDirectory + "/" + track);
		}
		
		addStation(station);
	}
	return true;
}

/**
	Frequency is adjustable from 0.0 to 1.0.
*/

void
Spectrum::setFrequency(float inFrequency)
{
	//	Ignore minor changes in frequency…
	//	TODO: Low-pass filter the frequency?
	
	float df = std::abs(inFrequency - mFrequency);
	if (df >= kMinimumFrequencyChange)
	{
		mFrequency = inFrequency;
		mNeedsTuning = true;
	}
}

/**
	updateTuning() ensures that the right sound file is open for the given frequency. It must
	be called before any operation that accesses mCurrentTrack to ensure the correct state,
	particularly before calling getStationAudioData() and minimumBufferSize(), generally
	once before each in the main Radio loop.
*/

void
Spectrum::updateTuning()
{
	//	Select the appropriate station…
	
	if (mNeedsTuning)
	{
		//	First pass at this code is to just iterate through the list of stations, and find
		//	the one that’s close enough to the tuned frequency…
		
		mLastFrequency = mFrequency;
		mNeedsTuning = false;
		
		int32_t stationIdx = -1;
		for (uint32_t i = 0; i < mStations.size(); ++i)
		{
			const Station& station = mStations[i];
			float minSF = station.frequency() - kStationHalfBand;
			float maxSF = station.frequency() + kStationHalfBand;
			if (minSF <= mLastFrequency && mLastFrequency <= maxSF)
			{
				stationIdx = i;
				break;
			}
		}
		
		if (stationIdx < 0)
		{
			//	The new frequency tunes no station, so note the playback
			//	position any current station, and return false, indicating
			//	no audio data is available…
			
			if (mCurrentStationIndex >= 0)
			{
				Station& station = mStations[mCurrentStationIndex];
				station.setLastPausedFrame(mCurrentTrack->currentFrame());
				
				mCurrentStationIndex = -1;
				//	TODO: persist the station data
			}
			
			//	Fall through, will return false…
		}
		else
		{
			//	There is a station tuned. If it’s different from the currently-
			//	tuned station, then reset the decoder for the new track…
			
			if (mCurrentStationIndex != stationIdx)
			{
				//	Set the new station…
				
				mCurrentStationIndex = stationIdx;
				
				if (!openStationTrack())
				{
					return;
				}
			}
		}
	}
}

/**
	Opens the current station’s current track.
*/

bool
Spectrum::openStationTrack()
{
	//	TODO: If we fail to open the current track, try
	//			the next. Keep trying until we wrap back
	//			to the current track index, in which case we
	//			return false.
	
	const Station& station = mStations[mCurrentStationIndex];
	const std::string& path = station.trackPath();
	bool success = mCurrentTrack->open(path);
	if (!success)
	{
		LogDebug("Error opening track '%s'", path.c_str());
		return false;
	}
	else
	{
		LogDebug("Opened track '%s'", path.c_str());
	}
	
	//	Set the track to where it last left off…
	
	if (station.lastPausedFrame() != 0)
	{
		mCurrentTrack->setCurrentFrame(station.lastPausedFrame());
	}
	
	//	Verify the encoder parameters…
	//	TODO: Need to re-configure everything when this changes.
	
	if (mCurrentTrack->encoding() != MPG123_ENC_SIGNED_16
		|| (mCurrentTrack->numChannels() != 2 && mCurrentTrack->numChannels() != 1)
		|| mCurrentTrack->rate() != 44100)
	{
		LogDebug("Unexpected encoding (%d), rate (%ld), or num channels (%d)",
					mCurrentTrack->encoding(),
					mCurrentTrack->rate(),
					mCurrentTrack->numChannels());
		return false;
	}

	return true;
}

/**
	Gets the next bufferful of decoded audio data from the current station.
	Returns true if successful, false if there’s no more data or an error
	occurred.
	
	This method ensures that the correct audio stream is being read, based
	on the configuration and currently-selected frequency.
*/

bool
Spectrum::getStationAudioData(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded)
{
	//	This call happens hundreds of times per second, so
	//	everything in here or called from here needs to be fast…
	
	//	If there's a station, get the audio data from the current track,
	//	otherwise return false…
	
	if (mCurrentStationIndex < 0)
	{
		return false;
	}
	
	bool success = mCurrentTrack->read(inBuffer, inBufferSize, outBytesDecoded);
	if (!success)
	{
		//	If this track is done, advance to the next…
		//	TOOD: Verify that no data is decoded along with the done flag.
		//			If there is decoded data, it needs to be played first,
		//			and the track advanced after…
		
		if (mCurrentTrack->done())
		{
			Station& station = mStations[mCurrentStationIndex];
			station.nextTrack();
			openStationTrack();
			success = mCurrentTrack->read(inBuffer, inBufferSize, outBytesDecoded);
		}
	}
	
	return success;
}


size_t
Spectrum::minimumBufferSize() const
{
	return mCurrentTrack->minimumBufferSize();
}

int
Spectrum::numChannels() const
{
	return mCurrentTrack->numChannels();
}

uint32_t
Spectrum::rate() const
{
	return (uint32_t) mCurrentTrack->rate();
}

void
Spectrum::addStation(const Station& inStation)
{
	mStations.push_back(inStation);
}
