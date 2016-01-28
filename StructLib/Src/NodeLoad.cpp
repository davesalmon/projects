/*+
 *	File:		NodeLoad.cpp
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "NodeLoad.h"
#include "NodeLoadImp.h"
#include "DlString.h"
#include "UnitTable.h"

//----------------------------------------------------------------------------------------
//  NodeLoad::GetType
//
//      return the type (load/disp) for the specified dof.
//
//  DlInt32 whichDof       -> the dof.
//
//  returns NodeLoadType   <- the type.
//----------------------------------------------------------------------------------------
NodeLoadType
NodeLoad::GetType(DlInt32 whichDof) const
{
	_DlAssert(whichDof >= 0 && whichDof < DOF_PER_NODE);
	return imp->GetType(whichDof);
}

//----------------------------------------------------------------------------------------
//  NodeLoad::SetType
//
//      set the type for the specified dof.
//
//  DlInt32 whichDof   -> the dof.
//  NodeLoadType type  -> the type.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeLoad::SetType(DlInt32 whichDof, NodeLoadType type)
{
	_DlAssert(whichDof >= 0 && whichDof < DOF_PER_NODE);
	_DlAssert(type == NodeLoadIsForce || type == NodeLoadIsDisp);
	imp->SetType(whichDof, type);
}

//----------------------------------------------------------------------------------------
//  NodeLoad::GetValues
//
//      return the values and types at each dof.
//
//  DlFloat32 vals[DOF_PER_NODE]       <-> array of values
//  NodeLoadType types[DOF_PER_NODE]   <-> array of types.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeLoad::GetValues(DlFloat32 vals[DOF_PER_NODE], NodeLoadType types[DOF_PER_NODE]) const
{
	for (int i = 0; i < DOF_PER_NODE; i++) {
		vals[i] = imp->GetValue(i);
		types[i] = imp->GetType(i);
	}
}

//----------------------------------------------------------------------------------------
//  NodeLoad::GetValue
//
//      return the value at teh specified dof.
//
//  DlInt32 whichDof   -> the dof
//  bool wantUnits     -> true to include units.
//
//  returns DlString   <- the string value.
//----------------------------------------------------------------------------------------
DlString
NodeLoad::GetValue(DlInt32 whichDof, bool wantUnits) const
{
	_DlAssert(whichDof >= 0 && whichDof < DOF_PER_NODE);
	return UnitTable::FormatValue(imp->GetValue(whichDof), kNodeLoadUnits[imp->GetType(whichDof)][whichDof],
				DlFloatFormat(4, 0, DlFloatFormatType::Fit), wantUnits);
}

//----------------------------------------------------------------------------------------
//  NodeLoad::SetValue
//
//      set the value for the specified dof.
//
//  DlInt32 whichDof   -> the dof.
//  const char* value  -> the value.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeLoad::SetValue(DlInt32 whichDof, const char* value)
{
	_DlAssert(whichDof >= 0 && whichDof < DOF_PER_NODE);
	imp->SetValue(whichDof, ConvertNodeLoadValue(imp, whichDof, value));
}

//----------------------------------------------------------------------------------------
//  NodeLoad::SetValue
//
//      set the value for the specified dof.
//
//  DlInt32 whichDof   -> the dof
//  DlFloat64 value    -> the value.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeLoad::SetValue(DlInt32 whichDof, DlFloat64 value)
{
	_DlAssert(whichDof >= 0 && whichDof < DOF_PER_NODE);
	imp->SetValue(whichDof, value);
}

//----------------------------------------------------------------------------------------
//  NodeLoad::ConvertNodeLoadValue
//
//      convert the string value for the specified dof to a numeric value.
//
//  NodeLoadImp* l     -> the load.
//  DlInt32 whichDof   -> the dof
//  const char* value  -> the string value.
//
//  returns DlFloat64  <- the numeric value.
//----------------------------------------------------------------------------------------
DlFloat64 
NodeLoad::ConvertNodeLoadValue(NodeLoadImp* l, DlInt32 whichDof, const char* value) {
	return UnitTable::ParseValue(value, kNodeLoadUnits[l->GetType(whichDof)][whichDof]);
}


//	eof