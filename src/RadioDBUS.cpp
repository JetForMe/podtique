//
//  RadioDBUS.cpp
//  Podtique
//
//  Created by Roderick Mann on 8/15/2015.
//  Copyright © 2015 Latency: Zero. All rights reserved.
//

#include "RadioDBUS.h"


//
//	Project Includes
//

#include "DBusMessage.h"
#include "Radio.h"
#include "RadioDebug.h"



const char* kPath						=	"/com/latencyzero/podtique";
const char*	kDestination				=	"com.latencyzero.podtique";
const char*	kDestinationWeb				=	"com.latencyzero.podtique.ui";
const char* kInterface					=	"com.latencyzero.podtique";
const char* kMethodRadioStatus			=	"RadioStatus";


RadioDBUS::RadioDBUS(Radio* inRadio)
	:
	mRadio(inRadio)
{
}

bool
RadioDBUS::open(const char* inPath)
{
	bool success = DBUS::open(inPath);
	if (!success)
	{
		return false;
	}
	
	//	Test names…
	
	const char* uniqueName = getUniqueName();
	LogDebug("Unique name: [%s]", uniqueName);
	
	//requestName(kDestination);
	
	//	Test filter…
	
#if 0
	bool success = ::dbus_connection_add_filter(mBus, filterFunc1, this, NULL);
	if (!success)
	{
		LogDebug("Unable to register filter");
	}
#endif

	return true;
}

bool
RadioDBUS::sendRadioState(bool inOn)
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
// 		LogDebug("Unable to send message");
		return false;
	}
	
	return true;
}


DBusHandlerResult
RadioDBUS::handleMessage(DBusMessage* inMsg)
{
	const char* path = ::dbus_message_get_path(inMsg);
	LogDebug("HandleMessage. path: %s", path);
	const char* member = ::dbus_message_get_member(inMsg);
	LogDebug("HandleMessage. member: %s", member);
	
	if (path != NULL && ::strcmp(path, "/com/latencyzero/podtique") == 0)
	{
		const char* member = ::dbus_message_get_member(inMsg);
		const char* iface = ::dbus_message_get_interface(inMsg);
		
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
		else if (::dbus_message_is_method_call(inMsg, "com.latencyzero.podtique", "RadioStatus"))
		{
			LogDebug("RadioStatus");
			DBusError err;
			::dbus_error_init(&err);
			dbus_bool_t	status;
			if (!::dbus_message_get_args(inMsg, &err, DBUS_TYPE_BOOLEAN, &status, DBUS_TYPE_INVALID))
			{
				LogDebug("dbus_message_get_args returned error");
			}
			else
			{
				LogDebug("RadioStatus: %s", status ? "On" : "Off");
				mRadio->setOn(status);
				return DBUS_HANDLER_RESULT_HANDLED;
			}
		}
	}
	
	return DBUS::handleMessage(inMsg);
}

