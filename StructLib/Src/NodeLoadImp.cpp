/*+
 *	File:		NodeLoadImp.cpp
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

#include "DlPlatform.h"
#include "NodeLoadImp.h"

//---------------------------------- Includes ----------------------------------

#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "DlAssert.h"

//---------------------------------- Class -------------------------------------

NodeLoadImp::NodeLoadImp()
{
	memset(_type, 0, sizeof(_type));
	memset(_load, 0, sizeof(_load));
}

NodeLoadImp::NodeLoadImp(StrInputStream& input, const frame_data& )
{
	for (int i = 0; i < DOF_PER_NODE; i++) {
		_type[i] = (NodeLoadType)input.GetInt();
		_load[i] = input.GetDouble();
	}
}

void
NodeLoadImp::SetValue(DlInt32 whichDof, double value)
{
	_DlAssert(whichDof >= 0 && whichDof < DOF_PER_NODE);
	_load[whichDof] = value;
}

bool
NodeLoadImp::operator==(const NodeLoadImp& p) const
{
	for (auto i = 0; i < DOF_PER_NODE; i++)
	{
		if (_type[i] != p._type[i])
			return false;
		if (_load[i] != p._load[i])
			return false;
	}
	
	return true;
}

void
NodeLoadImp::Write(StrOutputStream& output, const frame_data& ) const
{
	for (int i = 0; i < DOF_PER_NODE; i++) {
		output.PutInt((DlInt32)_type[i]);
		output.PutDouble(_load[i]);
	}
}

void NodeLoadImp::Add(const NodeLoadImp* imp, DlFloat32 fact)
{
	_isCombo = true;
	for (DlInt32 i = 0; i < DOF_PER_NODE; i++) {
		// TODO: check the type.
		if (_type[i] == NodeLoadUndefined || _type[i] == imp->_type[i]) {
			_type[i] = imp->_type[i];
			_load[i] += fact * imp->_load[i];
		}
	}
}

//	eof