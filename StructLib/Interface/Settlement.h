/*+
 *	File:		Settlement.h
 *
 *	Contains:	Node settlement interface
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#ifndef _H_Settlement
#define _H_Settlement

#include "DlTypes.h"

class Settlement {
public:
	Settlement() { distance[0] = distance[1] = distance[2] = 0; }
	Settlement(DlFloat32 x, DlFloat32 y, DlFloat32 theta) { distance[0] = x; distance[1] = y; distance[2] = theta; }

	DlFloat32	GetDistance(int whichDof) { return distance[whichDof]; }
	void		SetDistance(int whichDof, DlFloat dist) { distance[whichDof] = dist; }

//	static void MapUnits(Settlement & settle, UnitType from, UnitType to);

private:
	DlFloat32	distance[DOF_PER_NODE];
};

#endif
