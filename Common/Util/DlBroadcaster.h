/*
 *  DlBroadcaster.h
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 */
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

#ifndef _H_DlBroadcaster
#define _H_DlBroadcaster

#if defined(__PowerPlant__)

#include <LBroadcaster.h>
class DlBroadcaster : public LBroadcaster {
};

#else

#include <vector>

class DlListener;

/**
 * Define a broadcaster class. The broadcaster class maintains a 
 *	view of classes that implement DlListener. When a message is
 *	broadcast it is sent to each of the listeners. When a broadcaster
 *	is deleted, all of its listeners are notified.
 */
class DlBroadcaster {

public:
	DlBroadcaster();
	DlBroadcaster(const DlBroadcaster&);// {}

	virtual ~DlBroadcaster();
	
	void AddListener(DlListener* l);
	void RemoveListener(DlListener* l);

	void BroadcastMessage(DlUInt32 msg, void* data) const;
	void BroadcastString(DlUInt32 msg, const char* fmt, ...) const;
	void BroadcastString(DlUInt32 msg, const char* fmt, va_list ap) const;

	size_t ListenerCount() const;
	
private:
//	DlBroadcaster(const DlBroadcaster&);
	DlBroadcaster& operator=(const DlBroadcaster&);

	typedef std::vector<DlListener*>	_Listeners;
	typedef _Listeners::iterator		_Iterator;
	typedef _Listeners::const_iterator	_ConstIterator;

	_Listeners	_listeners;
};

inline
size_t
DlBroadcaster::ListenerCount() const
{
	return _listeners.size();
}

#endif
#endif

