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

#include "AACDecoder.h"
#include "MP3Decoder.h"
#include "RadioConstants.h"
#include "RadioDebug.h"





#pragma mark -
#pragma mark • Station


Station::Station(float inFrequency, const std::string& inDesc)
	:
	mFrequency(inFrequency),
	mDesc(inDesc),
	mCurrentTrackIdx(0),
	mLastPausedFrame(0),
	mDecoder(NULL)
{
}

Station::~Station()
{
	delete mDecoder;
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

/**
	Gets the next bufferful of decoded audio data from the current station.
	Returns true if successful, false if there’s no more data or an error
	occurred.
	
	This method ensures that the correct audio stream is being read, based
	on the configuration and currently-selected frequency.
*/

bool
Station::getAudioData(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded) const
{
	//	This call happens hundreds of times per second, so
	//	everything in here or called from here needs to be fast…
	
	bool success = decoder()->read(inBuffer, inBufferSize, outBytesDecoded);
	if (!success)
	{
		//	If this track is done, advance to the next…
		//	TOOD: Verify that no data is decoded along with the done flag.
		//			If there is decoded data, it needs to be played first,
		//			and the track advanced after…
		
		if (decoder()->done())
		{
			Station* self = const_cast<Station*> (this);
			self->nextTrack();
			self->openTrack();
			success = decoder()->read(inBuffer, inBufferSize, outBytesDecoded);
		}
	}
	
	return success;
}

bool
hasSuffix(const std::string& inStr, const std::string& inSuffix)
{
    return inStr.size() >= inSuffix.size() &&
           inStr.compare(inStr.size() - inSuffix.size(), inSuffix.size(), inSuffix) == 0;
}

/**
	Opens the current station’s current track.
*/

bool
Station::openTrack()
{
	//	Open the station’s current track. If that fails,
	//	move on to the next one. Return false if none could be
	//	opened…
	
	uint32_t startTrackIdx = trackIdx();
	while (true)
	{
		const std::string& path = trackPath();
		if (hasSuffix(path, ".m4a"))
		{
			LogDebug("Opening m4a track");
			mDecoder = new AACDecoder();
		}
		else //if (hasSuffix(path, ".mp3"))
		{
			LogDebug("Opening other (mp3?) track");
			mDecoder = new MP3Decoder();
		}
		
		bool success = mDecoder->open(path);
		if (!success)
		{
			//	The current track failed to open, so we must reset
			//	any remembered frame…
			
			LogDebug("Error opening track '%s'", path.c_str());
			setLastPausedFrame(0);
			
			//	Try the next track. Station will automatically wrap.
			//	If we come back to the one we were on, bail…
			
			nextTrack();
			if (trackIdx() == startTrackIdx)
			{
				LogDebug("Unable to open any track for station “%s”", desc().c_str());
				return false;
			}
		}
		else
		{
			LogDebug("Opened track '%s'", path.c_str());
			break;
		}
	}
	
	//	Set the set the decoder to where it last left off…
	
	if (lastPausedFrame() != 0)
	{
		mDecoder->setCurrentFrame(lastPausedFrame());
	}
	
	//	Verify the encoder parameters…
	//	TODO: Need to re-configure everything when this changes.
	
	LogDebug("Encoding (%d), rate (%u), num channels (%d)",
				mDecoder->encoding(),
				mDecoder->rate(),
				mDecoder->numChannels());
	if (mDecoder->encoding() != MPG123_ENC_SIGNED_16)
	{
		LogDebug("Unexpected encoding (%d)", mDecoder->encoding());
		return false;
	}
	
	if (mDecoder->numChannels() != 2 && mDecoder->numChannels() != 1)
	{
		LogDebug("Unexpected num channels (%d)", mDecoder->numChannels());
		return false;
	}
	
	if (mDecoder->rate() != 44100)
	{
		LogDebug("Unexpected rate (%u)", mDecoder->rate());
		return false;
	}

	return true;
}

Decoder*
Station::decoder() const
{
	if (mDecoder == NULL)
	{
		Station* self = const_cast<Station*> (this);
		//self->mDecoder = new MP3Decoder();
		self->openTrack();
	}
	
	return mDecoder;
}


size_t
Station::minimumBufferSize() const
{
	return decoder()->minimumBufferSize();
}

int
Station::numChannels() const
{
	return decoder()->numChannels();
}

uint32_t
Station::rate() const
{
	return (uint32_t) decoder()->rate();
}


#pragma mark -
#pragma mark • Spectrum

Spectrum::Spectrum(const std::string& inDataDirectory)
	:
	mDataDirectory(inDataDirectory),
	mFrequency(0.0),
	mNeedsTuning(true),
	mCurrentStationIndex(-1)
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
		
		//LogDebug("Station: %s, freq: %f, tracks: %ju", desc.c_str(), freq, (uintmax_t) playlist.size());
		
		Station station(freq, desc);
		for(size_t idx = 0; idx < playlist.size(); ++idx)
		{
			const picojson::value& v = playlist[idx];
			if (!v.is<std::string>()) { continue; }
			std::string track = v.get<std::string>();
			//LogDebug("Track %02ju: %s", (uintmax_t) idx, track.c_str());
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
		LogDebug("Selected frequency: %0.3f", mFrequency);
	}
}

/**
	updateTuning() ensures that the right sound file is open for the given frequency. It must
	be called before any operation that accesses mDecoder to ensure the correct state,
	particularly before calling getStationAudioData() and minimumBufferSize(), generally
	once before each in the main Radio loop.
	
	Return true if the station changed.
*/

bool
Spectrum::updateTuning()
{
	//	Select the appropriate station…
	
	if (mNeedsTuning)
	{
		int32_t lastStationIdx = mCurrentStationIndex;
		
		//	First pass at this code is to just iterate through the list of stations, and find
		//	the one that’s close enough to the tuned frequency…
		
		mLastFrequency = mFrequency;
		mNeedsTuning = false;
		
		int32_t newStationIdx = -1;
		for (uint32_t i = 0; i < mStations.size(); ++i)
		{
			const Station& station = mStations[i];
			float minSF = station.frequency() - kStationHalfBand;
			float maxSF = station.frequency() + kStationHalfBand;
			if (minSF <= mLastFrequency && mLastFrequency <= maxSF)
			{
				newStationIdx = i;
				break;
			}
		}
		
		//	The new frequency does not tune the current station, so note the playback
		//	position any current station, and return false, indicating
		//	no audio data is available…
			
		if (newStationIdx != mCurrentStationIndex)
		{
			if (mCurrentStationIndex >= 0)		//	There was a station, remember its state…
			{
				mCurrentStationIndex = -1;
				LogDebug("Tuned no station");
				
				//	TODO: persist the station data
			}
			else if (newStationIdx >= 0)
			{
				//	There is a station tuned. If it’s different from the currently-
				//	tuned station, then reset the decoder for the new track…
			
				mCurrentStationIndex = newStationIdx;
				LogDebug("Tuned station: %s", mStations[mCurrentStationIndex].desc().c_str());
			
				//if (!openStationTrack())
				{
				}
			}
		}
		
		//	Update the weighting factors…
		
		if (stationTuned())
		{
			float df = stationFrequency() - mFrequency;
			df = std::fabs(df);
			if (df > kStationHalfBand) df = kStationHalfBand;
			
			//df = kStationHalfBand - df;
			mContentWeight = 1.0 - (df / kStationHalfBand);
			
			//	If the content weight is nearly one, round it up…
			
			if (mContentWeight > 0.99)
			{
				mContentWeight = 1.0;
			}
			
			mStaticWeight = 1.0 - mContentWeight;
			//LogDebug("df: %f %f", df, f);
		}
		else
		{
			mContentWeight = 0.0;
			mStaticWeight = 1.0;
		}
		
		return lastStationIdx != mCurrentStationIndex;
	}
	
	return false;	//	TODO: finish this implementation!
}

void
Spectrum::addStation(const Station& inStation)
{
	mStations.push_back(inStation);
}

size_t
Spectrum::minimumBufferSize() const
{
	if (mCurrentStationIndex < 0)
	{
		return 0;
	}
	
	const Station& station = mStations[mCurrentStationIndex];
	return station.minimumBufferSize();
}

bool
Spectrum::getStationAudioData(void* inBuffer, size_t inBufferSize, size_t& outBytesDecoded) const
{
	if (mCurrentStationIndex < 0)
	{
		return false;
	}
	
	const Station& station = mStations[mCurrentStationIndex];
	return station.getAudioData(inBuffer, inBufferSize, outBytesDecoded);
}

