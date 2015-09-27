//
//  GPIO.cpp
//  Podtique
//
//  Created by Roderick Mann on 12/28/14.
//  Copyright (c) 2015 Latency: Zero, LLC. All rights reserved.
//

#include "GPIO.h"

//
//	Standard Includes
//

#include <fstream>
#include <sstream>
#include <thread>


//
//	Project Includes
//

#include "RadioDebug.h"




std::string			GPIO::sGPIOPath			=	"/sys/class/gpio/";

GPIO::GPIO()
	:
	mNumber(0)
{
}

void
GPIO::setGPIONumber(uint16_t inNumber)
{
	mNumber = inNumber;
	std::stringstream ss;
	ss << sGPIOPath << "gpio" << mNumber << "/";
	mPath = ss.str();
	exportGPIO();
}

void
GPIO::exportGPIO()
{
	write(sGPIOPath, "export", mNumber);
	
	//	Give sysfs a chance to set up the file. Gross…
	
	std::chrono::milliseconds dur(250);
	std::this_thread::sleep_for(dur);
	
	//LogDebug("Exported gpio%u", mNumber);
}

void
GPIO::setInput()
{
	//LogDebug("Setting gpio%u as input", mNumber);
	write(mPath, "direction", "in");
	//LogDebug("Set gpio%u as input", mNumber);
}

void
GPIO::setOutput()
{
	//LogDebug("Setting gpio%u as output", mNumber);
	write(mPath, "direction", "out");
	//LogDebug("Set gpio%u as output", mNumber);
}

bool
GPIO::get() const
{
	std::string v = read(mPath, "value");
	return v == "1";
}

void
GPIO::set(bool inVal)
{
	if (inVal)
	{
		write(mPath, "value", "1");
	}
	else
	{
		write(mPath, "value", "0");
	}
}

void
GPIO::write(const std::string& inDir, const std::string& inFile, const std::string& inVal) const
{
	std::ofstream fs;
	std::string path = inDir + inFile;
	fs.open(path.c_str());
	if (!fs.is_open())
	{
		LogDebug("Unable to open GPIO %u (%s) for writing: %s", mNumber, path.c_str(), ::strerror(errno));
		exit(1);
	}
	
	fs << inVal;
	fs.close();
}

void
GPIO::write(const std::string& inDir, const std::string& inFile, uint16_t inVal) const
{
	std::stringstream ss;
	ss << inVal;
	write(inDir, inFile, ss.str());
}

std::string
GPIO::read(const std::string& inDir, const std::string& inFile) const
{
	std::ifstream fs;
	std::string path = inDir + inFile;
	fs.open(path.c_str());
	if (!fs.is_open())
	{
		LogDebug("Unable to open GPIO %u (%s) for reading: %s", mNumber, path.c_str(), ::strerror(errno));
		exit(1);
		return "";
	}
	
	std::string input;
	std::getline(fs, input);
	fs.close();
	return input;
}
