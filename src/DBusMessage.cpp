//
//  DBusMessage.cpp
//  Podtique
//
//  Created by Roderick Mann on 3/11/15.
//  Copyright (c) 2015 Latency: Zero. All rights reserved.
//

#include "DBusMessage.h"


//
//	Library Includes
//

#include <dbus/dbus.h>

//
//	Project Includes
//

#include "RadioDebug.h"




namespace DBus
{

Message::Message(const char* inBusName,
					const char* inPath,
					const char* inInterface,
					const char* inMethod)
	:
	mMsg(NULL)
{
	LogDebug("Creating message %s: %p", inMethod, this);
	mMsg = ::dbus_message_new_method_call(inBusName,
											inPath,
											inInterface,
											inMethod);
	if (mMsg == NULL)
	{
		LogDebug("Unable to allocate DBusMessage");
	}
}

Message::~Message()
{
	if (mMsg != NULL)
	{
		::dbus_message_unref(mMsg);
	}
}

void
Message::setNoReply(bool inVal)
{
	::dbus_message_set_no_reply(mMsg, inVal);
}

void
Message::append(bool inVal)
{
	int val = inVal;
	bool success = ::dbus_message_append_args(mMsg, DBUS_TYPE_BOOLEAN, &val, DBUS_TYPE_INVALID);
	if (!success)
	{
		LogDebug("Unable to append bool argument");
		return;
	}
}



};	//	namespace DBus
