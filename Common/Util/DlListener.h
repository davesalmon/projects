/*
 *  DlListener.h
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 *  Contains DlListener base class to list to message broadcasts. See DlBroadcaster.
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

#ifndef _H_DlListener
#define _H_DlListener

#if defined(__PowerPlant__)

#include <LListener.h>
class DlListener : public LListener
{
//public:
//	DlListener() : LListener() {}
};

#else

#include <vector>

class DlBroadcaster;

/**
 * Define a listener class. Note that the listener does not own any of
 *	the broadcasters associated. The management of each individual object
 *	is outside the scope of this class.
 */
class DlListener {
public:
	DlListener() 
		: _broadcasters()
		, _deleting(false)
	{}
	virtual ~DlListener();
	
	virtual void ListenToMessage(DlUInt32 msg, void* data) const = 0;

	void AddBroadcaster(DlBroadcaster* b);
	void RemoveBroadcaster(DlBroadcaster* b);
	
	size_t BroadcasterCount() const;
	
private:
	DlListener(const DlListener&);
	DlListener& operator =(const DlListener&);
		
	typedef std::vector<DlBroadcaster*>	_Broadcasters;
	typedef _Broadcasters::iterator		_Iterator;

	_Broadcasters	_broadcasters;
	bool			_deleting;
};

inline
size_t DlListener::BroadcasterCount() const
{
	return _broadcasters.size();
}

#endif
#endif


