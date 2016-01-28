/*+
 *	File:		NodeLoad.h
 *
 *	Contains:	Node Load public interface
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

#ifndef _H_NodeLoad
#define _H_NodeLoad

//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"
#include "StrDefines.h"

//---------------------------------- Class -------------------------------------

class NodeLoadImp;
class DlString;

// Types for node loads
enum NodeLoadType {
	NodeLoadUndefined = -1,
	NodeLoadIsForce = 0,
	NodeLoadIsDisp = 1
};

class NodeLoad
{
public:
	
	NodeLoad() : imp(nullptr) {}
	NodeLoad(NodeLoadImp* load) : imp(load) {}

	operator NodeLoadImp* () const { return imp; }

	bool 		Empty() const { return imp == 0; }
	void		GetValues(DlFloat32 vals[DOF_PER_NODE], NodeLoadType types[DOF_PER_NODE]) const;
	DlString	GetValue(DlInt32 whichDof, bool wantUnits) const;
	
	void	 	SetValue(DlInt32 whichDof, const char* value);
	void	 	SetValue(DlInt32 whichDof, DlFloat64 value);
	
	NodeLoadType	GetType(DlInt32 whichDof) const;
	void			SetType(DlInt32 whichDof, NodeLoadType type);

	static DlFloat64 ConvertNodeLoadValue(NodeLoadImp* l, DlInt32 whichDof, const char* value);

private:

	NodeLoadImp*	imp;

};

//---------------------------------- Inlines -----------------------------------

#endif

//	eof