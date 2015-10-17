//
//  DBUS.h
//  Podtique
//
//  Created by Roderick Mann on 3/10/15.
//  Copyright (c) 2015 Latency: Zero. All rights reserved.
//

#ifndef __Podtique__DBUS__
#define __Podtique__DBUS__

//
//	Standard Includes
//

#include <thread>

//
//	Library Includes
//

#include <dbus/dbus.h>





namespace DBus {
	class Message;
};





class
DBUS
{
public:
	DBUS();
	
	void						start();
	
	virtual	bool				open(const char* inPath);
	
protected:
	void						entry();
	
	bool						testAndLogError(const char* inMsg, const DBusError& inError);
	bool						send(const DBus::Message& inMsg) const;
	
	static
	void						Unregister(DBusConnection* inBus, void* inUserData);
	
	static
	DBusHandlerResult			HandleMessage(DBusConnection* inBus, DBusMessage* inMsg, void* inUserData);
	
	virtual
	DBusHandlerResult			handleMessage(DBusMessage* inMsg);
	
	DBusConnection*				connection()				const		{ return mBus; }
	
	const char*					getUniqueName()				const		{ return ::dbus_bus_get_unique_name(mBus); }
	int							requestName(const char* inName, unsigned int inFlags = 0);
	
private:
	std::thread					mDispatchThread;
	DBusConnection*				mBus;
	
	static
	DBusObjectPathVTable		sVTable;
};


#endif /* defined(__Podtique__DBUS__) */
