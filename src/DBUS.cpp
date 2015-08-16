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






void
DBUS::Unregister(DBusConnection* inBus, void* inUserData)
{
	LogDebug("Unregister");
}

DBusHandlerResult
DBUS::HandleMessage(DBusConnection* inBus, DBusMessage* inMsg, void* inUserData)
{
	try
	{
		DBUS* self = reinterpret_cast<DBUS*> (inUserData);
		return self->handleMessage(inMsg);
	}
	
	catch (...)
	{
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
}

DBusHandlerResult
DBUS::handleMessage(DBusMessage* inMsg)
{
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
DBUS::open(const char* inPath)
{
	::dbus_threads_init_default();
	
	DBusError error;
	::dbus_error_init(&error);
	
	mBus = ::dbus_bus_get(DBUS_BUS_SESSION, &error);
	if (testAndLogError("Error opening DBUS", error))
	{
	}
	
	//	Register an object…
	
	bool success = ::dbus_connection_register_object_path(mBus, inPath, &sVTable, this);
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

int
DBUS::requestName(const char* inName, unsigned int inFlags)
{
	DBusError error;
	::dbus_error_init(&error);
	int r = ::dbus_bus_request_name(mBus, inName, inFlags, &error);
	if (testAndLogError("Error in dbus_bus_request_name", error))
	{
	}
	else
	{
		LogDebug("request name result: %d", r);
	}
	
	return r;
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
