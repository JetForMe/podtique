//
//  GPIO.cpp
//  Podtique
//
//  Created by Roderick Mann on 12/28/14.
//  Copyright (c) 2014 Latency: Zero. All rights reserved.
//

#include "GPIO.h"

//
//	Standard Includes
//

#include <fstream>
#include <sstream>


//
//	Project Includes
//

#include "RadioDebug.h"




std::string			GPIO::sGPIOPath			=	"/sys/class/gpio/";

GPIO::GPIO(uint16_t inNumber)
	:
	mNumber(inNumber)
{
	std::stringstream ss;
	ss << sGPIOPath << "gpio" << mNumber << "/";
	mPath = ss.str();
}


void
GPIO::exportGPIO()
{
	write(sGPIOPath, "export", mNumber);
}

void
GPIO::setInput()
{
	write(mPath, "direction", "in");
}

void
GPIO::setOutput()
{
	write(mPath, "direction", "out");
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
		LogDebug("Unable to open GPIO %u for writing", mNumber);
		return;
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
		LogDebug("Unable to open GPIO %u for reading", mNumber);
		return "";
	}
	
	std::string input;
	getline(fs, input);
	fs.close();
	return input;
}
