/*+
 *
 *  FrameListener.h
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Define a listener for frame operations.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
-*/

#include "DlPlatform.h"
#include "DlListener.h"

@class FrameDocument;

class FrameListener : public DlListener
{
public:
	FrameListener(FrameDocument* doc) : _frameDocument(doc) {}
	
	virtual void ListenToMessage(DlUInt32 msg, void* data) const;

private:
    FrameListener(const FrameListener& f);
    FrameListener& operator=(const FrameListener& f);
    
	FrameDocument* _frameDocument;
};

