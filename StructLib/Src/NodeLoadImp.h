/*+
 *	File:		NodeLoadImp.h
 *
 *	Contains:	Frame Element Load internal interface
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

#ifndef _H_NodeLoadImp
#define _H_NodeLoadImp

//---------------------------------- Includes ----------------------------------

#include "StrDefines.h"
#include "UnitTable.h"
#include "NodeLoad.h"

//---------------------------------- Class -------------------------------------

class StrInputStream;
class StrOutputStream;
class frame_data;

const UnitType kNodeLoadUnits[2][DOF_PER_NODE] = {
	{UnitsForce, UnitsForce, UnitsMoment},
	{UnitsDisplacement,	UnitsDisplacement, UnitsRotation}
};

class NodeLoadImp
{
public:
	NodeLoadImp();
	NodeLoadImp(StrInputStream& input, const frame_data& data);
//	NodeLoadImp(DlFloat64 values[DOF_PER_NODE]);

	DlFloat64 GetValue(DlInt32 whichDof) const { return _load[whichDof]; }
	void SetValue(DlInt32 whichDof, double value);
	NodeLoadType	GetType(DlInt32 whichDof) const { return _type[whichDof]; }
	void SetType(DlInt32 whichDof, NodeLoadType whichType) { _type[whichDof] = whichType; }

	bool	operator==(const NodeLoadImp& p) const;

	void	Write(StrOutputStream& output, const frame_data& data) const;

	void	Clear() {
		memset(_type, 0, sizeof(_type));
		memset(_load, 0, sizeof(_load));
 	}
	
	void Add(const NodeLoadImp* imp, DlFloat32 fact);
	bool IsCombo() const { return _isCombo; }
	
private:

	NodeLoadType	_type[DOF_PER_NODE];
	DlFloat32		_load[DOF_PER_NODE];
	bool			_isCombo;
};

#endif

//	eof