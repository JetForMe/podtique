//
//  DBusMessage.h
//  Podtique
//
//  Created by Roderick Mann on 3/11/15.
//  Copyright (c) 2015 Latency: Zero. All rights reserved.
//

#ifndef __Podtique__DBusMessage__
#define __Podtique__DBusMessage__

//
//	Library Includes
//

#include <dbus/dbus.h>

class DBUS;

namespace DBus
{

class
Message
{
public:
								Message(const char* inBusName,
										const char* inPath,
										const char* inInterface,
										const char* inMethod);
	
	virtual						~Message();
	
	void						setNoReply(bool inVal);
	
	void						append(bool inVal);
	
protected:
	DBusMessage*				dbusMessage() const						{ return mMsg; }
	
private:
	DBusMessage*				mMsg;
	
	friend class ::DBUS;
};

};	//	namespace DBus

#endif /* defined(__Podtique__DBusMessage__) */
