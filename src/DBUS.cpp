//
//  DBUS.cpp
//  Podtique
//
//  Created by Roderick Mann on 3/10/15.
//  Copyright (c) 2015 Latency: Zero. All rights reserved.
//

#include "DBUS.h"


//
//	Standard Includes
//

#include <cstdio>

//
//	Library Includes
//

#include <dbus/dbus.h>

//
//	Project Includes
//

#include "RadioDebug.h"
#include "DBusMessage.h"






const char*	kDestination				=	"com.latencyzero.podtique";
const char*	kDestinationWeb				=	"com.latencyzero.podtique.ui";
const char* kPath						=	"/com/latencyzero/podtique";
const char* kInterface					=	"com.latencyzero.podtique";
const char* kMethodRadioStatus			=	"RadioStatus";



void
DBUS::Unregister(DBusConnection* inBus, void* inUserData)
{
	LogDebug("Unregister");
}

DBusHandlerResult
DBUS::HandleMessage(DBusConnection* inBus, DBusMessage* inMsg, void* inUserData)
{
	const char* path = ::dbus_message_get_path(inMsg);
	LogDebug("HandleMessage. path: %s", path);
	const char* member = ::dbus_message_get_member(inMsg);
	LogDebug("HandleMessage. member: %s", member);
	
	if (path != NULL && ::strcmp(path, "/com/latencyzero/podtique"))
	{
		if (::dbus_message_is_method_call(inMsg, "com.latencyzero.podtique", "RadioOn"))
		{
			LogDebug("RadioOn");
			return DBUS_HANDLER_RESULT_HANDLED;
		}
		else if (::dbus_message_is_method_call(inMsg, "com.latencyzero.podtique", "RadioOff"))
		{
			LogDebug("RadioOff");
			return DBUS_HANDLER_RESULT_HANDLED;
		}
	}
	
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

DBusObjectPathVTable
DBUS::sVTable =
{
	Unregister,
	HandleMessage,
	NULL,
};



DBUS::DBUS()
	:
	mBus(NULL)
{
}

DBusHandlerResult
filterFunc1(DBusConnection* inConn, DBusMessage* inMsg, void* inUserData)
{
	LogDebug("filterFunc1");
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

void
DBUS::open()
{
	::dbus_threads_init_default();
	
	DBusError error;
	::dbus_error_init(&error);
	
	mBus = ::dbus_bus_get(DBUS_BUS_SESSION, &error);
	if (testAndLogError("Error opening DBUS", error))
	{
	}
	
	//	Test names…
	
	const char* uniqueName = ::dbus_bus_get_unique_name(mBus);
	LogDebug("Unique name: [%s]", uniqueName);
	
	::dbus_error_init(&error);
	int r = ::dbus_bus_request_name(mBus, kDestination, 0, &error);
	if (testAndLogError("Error requesting name", error))
	{
	}
	LogDebug("request name result: %d", r);
	
	//	Test filter…
	
	bool success;
#if 0
	success = ::dbus_connection_add_filter(mBus, filterFunc1, this, NULL);
	if (!success)
	{
		LogDebug("Unable to register filter");
	}
#endif

	//	Test registering an object…
	
	success = ::dbus_connection_register_object_path(mBus, kPath, &sVTable, this);
	if (!success)
	{
		LogDebug("Unable to register object");
	}
}

bool
DBUS::testAndLogError(const char* inMsg, const DBusError& inError)
{
	if (::dbus_error_is_set(&inError))
	{
		if (inMsg != NULL)
		{
			LogDebug("%s", inMsg);
		}
		LogDebug("DBusError.name: %s", inError.name);
		LogDebug("DBusError.message: %s", inError.message);
		::dbus_error_free((DBusError*) &inError);
		return true;
	}
	
	return false;
}

bool
DBUS::sendRadioState(bool inOn)
{
	DBus::Message msg(kDestinationWeb,
						kPath,
						kInterface,
						kMethodRadioStatus);
	msg.setNoReply(true);
	msg.append(inOn);
	
	bool success = send(msg);
	if (!success)
	{
		LogDebug("Unable to send message");
		return false;
	}
	
	return true;
}

bool
DBUS::send(const DBus::Message& inMsg) const
{
	bool success = ::dbus_connection_send(mBus, inMsg.dbusMessage(), NULL);
	if (!success)
	{
		LogDebug("Unable to send message");
		return false;
	}
	LogDebug("Sent message %p", &inMsg);
	return true;
}

#pragma mark -
#pragma mark • Thread

void
DBUS::start()
{
	
	//	Open the JSON file…
	
	//	TODO: For now, just open a specific MP3 file and play it…
	
	//	Start the primary thread…
	
	mDispatchThread = std::thread(&DBUS::entry, this);
}

void
DBUS::entry()
{
	LogDebug("DBUS::entry()");
	
	do
	{
		bool cont = ::dbus_connection_read_write_dispatch(mBus, 1000);
		if (!cont)
		{
			LogDebug("DISCONNECT MESSAGE");
			break;
		}
	}
	while (true);
}
