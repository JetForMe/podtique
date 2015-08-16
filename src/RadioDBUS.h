//
//  RadioDBUS.hpp
//  Podtique
//
//  Created by Roderick Mann on 8/15/2015.
//  Copyright © 2015 Latency: Zero. All rights reserved.
//

#ifndef RadioDBUS_hpp
#define RadioDBUS_hpp

#include "DBUS.h"


class Radio;



class
RadioDBUS : public DBUS
{
public:
	RadioDBUS(Radio* inRadio);
	
	bool						sendRadioState(bool inOn);
	
	virtual	void				open(const char* inPath);

protected:
	virtual
	DBusHandlerResult			handleMessage(DBusMessage* inMsg);


private:
	Radio*						mRadio;
};

extern const char* kPath;


#endif /* RadioDBUS_hpp */
