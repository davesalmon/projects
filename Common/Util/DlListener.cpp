/*
 *  DlListener.cpp
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

#include "DlPlatform.h"
#include "DlListener.h"

#if !defined(__PowerPlant__)

#include "DlBroadcaster.h"

#include <algorithm>

/*------------------------------------------------------------------------------
 * DlListener::~DlListener                                    virtual destructor
 *
 * Describe function
 *
 *----------------------------------------------------------------------------*/
DlListener::~DlListener()
{
	_deleting = true;
	_Iterator i = _broadcasters.begin();
	for( ; i != _broadcasters.end(); i++) {
		(*i)->RemoveListener(this);
	}
}

/*------------------------------------------------------------------------------
 * DlListener::AddBroadcaster                                                   
 *
 * Describe function
 *
 * DlBroadcaster* b	-> argument description here
 *----------------------------------------------------------------------------*/
void
DlListener::AddBroadcaster(DlBroadcaster* b)
{
	_broadcasters.push_back(b);
}

/*------------------------------------------------------------------------------
 * DlListener::RemoveBroadcaster                                                
 *
 * Describe function
 *
 * DlBroadcaster* b	-> argument description here
 *----------------------------------------------------------------------------*/
void
DlListener::RemoveBroadcaster(DlBroadcaster* b)
{
	if (!_deleting) {
		_Iterator i = std::find(_broadcasters.begin(), _broadcasters.end(), b);
		_broadcasters.erase(i);
	}
}

#endif
