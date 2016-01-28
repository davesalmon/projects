/*+
 *	File:		StrMessage.h
 *
 *	Contains:	message ids for frame broadcasts to listeners
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
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

#ifndef _H_StrMessage
#define _H_StrMessage

const DlUInt32 NodeListID 			= 'node';
const DlUInt32 ElementListID 		= 'elem';
const DlUInt32 NodeLoadListID 		= 'nlod';
const DlUInt32 ElementLoadListID 	= 'elod';
const DlUInt32 PropertyListID 		= 'prop';
const DlUInt32 PropertyTypeListID	= 'ptyp';

const DlUInt32 MessageListAddOne			= 5000;
const DlUInt32 MessageListRemoveOne 		= 5001;
const DlUInt32 MessageListAddMultiple		= 5002;
const DlUInt32 MessageListRemoveMultiple	= 5003;

const DlUInt32 MessageNodeLoadAssignmentChanged	= 6000;
const DlUInt32 MessageElemLoadAssignmentChanged	= 6001;

struct ListMessage
{
	DlUInt32 	id;
	void*		element;
};

#endif
