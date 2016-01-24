/*+
 *
 *  DlBroadcaster.cpp
 *
 *  Copyright © 2006 David C. Salmon. All Rights Reserved.
 *
 *  Define a broadcaster. Broadcasters track a group of listeners and send messages to
 *  each listener.
 *
-*/
/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include "DlPlatform.h"
#include "DlBroadcaster.h"
#include "DlListener.h"

#include "stdarg.h"

#if defined(__PowerPlant__)

#else

#include <algorithm>

using std::vector;

//----------------------------------------------------------------------------------------
//  DlBroadcaster::DlBroadcaster                                              constructor
//
//      construct a broadcaster.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlBroadcaster::DlBroadcaster()
{
}

//----------------------------------------------------------------------------------------
//  DlBroadcaster::DlBroadcaster                                               destructor
//
//      destruct a broadcaster. Remove this broadcaster from each listener.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlBroadcaster::~DlBroadcaster()
{
	_Iterator i = _listeners.begin();
	for( ; i != _listeners.end(); i++) {
		(*i)->RemoveBroadcaster(this);
	}
}

//----------------------------------------------------------------------------------------
//  DlBroadcaster::DlBroadcaster                                              constructor
//
//      copy construct a broadcaster.
//
//  const DlBroadcaster&   -> the broadcaster to copy.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlBroadcaster::DlBroadcaster(const DlBroadcaster&)
{
    // tell the listeners about this broadcaster
	for (_Iterator i = _listeners.begin(); i != _listeners.end(); i++)
        (*i)->AddBroadcaster(this);
}


/*------------------------------------------------------------------------------
 * DlBroadcaster::AddListener
 *
 * 	Add a listener to this broadcaster.
 *
 * DlListener* l	-> make l listen to this broadcaster.
 *----------------------------------------------------------------------------*/
void
DlBroadcaster::AddListener(DlListener* l)
{
	if (l) {
		_listeners.push_back(l);
		l->AddBroadcaster(this);
	}
}


/*------------------------------------------------------------------------------
 * DlBroadcaster::RemoveListener                                                
 *
 * 	remove a listener from this broadcaster.
 *
 * DlListener* l	-> the listener to remove.
 *----------------------------------------------------------------------------*/
void
DlBroadcaster::RemoveListener(DlListener* l)
{
	_Iterator i = std::find(_listeners.begin(), _listeners.end(), l);
	_listeners.erase(i);
	l->RemoveBroadcaster(this);
}

/*------------------------------------------------------------------------------
 * DlBroadcaster::BroadcastMessage
 *
 * 	Broadcast a message identified by msg. The ids specified by msg must be
 *	unique to a given application. Listeners that do not know what msg is
 *	should do nothing. data is specific to msg and should not be interpreted
 *	unless msg is recognized.
 *
 * DlUInt32 msg	-> the message id.
 * void* data	-> associated data.
 *----------------------------------------------------------------------------*/
void
DlBroadcaster::BroadcastMessage(DlUInt32 msg, void* data) const
{
	_ConstIterator i = _listeners.begin();
	for( ; i != _listeners.end(); i++) {
		(*i)->ListenToMessage(msg, data);
	}
}

//----------------------------------------------------------------------------------------
//  DlBroadcaster::BroadcastString
//
//      broadcast a string as the message data.
//
//  DlUInt32 msg       -> the msg id
//  const char* fmt    -> the string format
//  ...                -> format args.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlBroadcaster::BroadcastString(DlUInt32 msg, const char* fmt, ...) const 
{
	va_list ap;
	va_start(ap, fmt);
	BroadcastString(msg, fmt, ap);
}

//----------------------------------------------------------------------------------------
//  DlBroadcaster::BroadcastString
//
//      broadcast a string.
//
//  DlUInt32 msg       -> the message id
//  const char* fmt    -> the message format
//  va_list ap         -> the arg list
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
DlBroadcaster::BroadcastString(DlUInt32 msg, const char* fmt, va_list ap) const 
{
    const size_t DEFAULT_SIZE = 2048;
    const size_t MAXIMUM_SIZE = (DEFAULT_SIZE << 4);
    
	char buf[DEFAULT_SIZE];
	int count = vsnprintf(buf, DlArrayElements(buf), fmt, ap);
	if (count >= DlArrayElements(buf)) {
        size_t bufSize = DEFAULT_SIZE << 1;
        
        // try to get a large enough buffer. If the buffer required is larger than 16 times
        //	the default, then just send the truncated message.
        while(true)
        {
			std::auto_ptr<char> mBuf((char*)OPERATORNEW(bufSize));
			if (vsnprintf(mBuf.get(), bufSize, fmt, ap) >= bufSize 
                	&& bufSize < MAXIMUM_SIZE)
            {
                bufSize <<= 1;
            }
            else
            {
				BroadcastMessage(msg, mBuf.get());
            }
        }
        
	} else {
		BroadcastMessage(msg, buf);
	}
}

#endif
