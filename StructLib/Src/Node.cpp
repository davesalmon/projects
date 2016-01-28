/*+
 *	File:		Node.cpp
 *
 *	Contains:	Node point interface
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

#include "DlPlatform.h"
#include "Node.h"
#include "NodeImp.h"
#include "NodeLoad.h"
#include "WorldPoint.h"
#include "DlString.h"
#include "Property.h"

#if DlDebugging
//----------------------------------------------------------------------------------------
//  Node::id
//
//      return the id for the node. for debugging.
//
//  returns int    <- 
//----------------------------------------------------------------------------------------
int
Node::id() const { return imp->_id; }
#endif

const PropertyType kDOFForceProperties[] = {
	{"Force, X", 	UnitsForce,		PropDataFloat, false}
	, {"Force, Y",	UnitsForce,		PropDataFloat, false}
	, {"Force, R",	UnitsMoment,	PropDataFloat, false}
};

const PropertyType kDOFReactionProperties[] = {
	{"Reaction, X", 	UnitsForce,		PropDataFloat, false}
	, {"Reaction, Y",	UnitsForce,		PropDataFloat, false}
	, {"Reaction, R",	UnitsMoment,	PropDataFloat, false}
};

const PropertyType kDOFDispProperties[] = {
	{"Displacement, X",		UnitsDisplacement,	PropDataFloat, false}
	, {"Displacement, Y",	UnitsDisplacement,	PropDataFloat, false}
	, {"Displacement, R",	UnitsRotation,		PropDataFloat, false}
};

const PropertyType kDOFSettleProperties[] = {
	{"Settlement, X",	UnitsDisplacement,	PropDataFloat, false}
	, {"Settlement, Y",	UnitsDisplacement,	PropDataFloat, false}
	, {"Settlement, R",	UnitsRotation,		PropDataFloat, false}
};

//----------------------------------------------------------------------------------------
//  Node::GetCoords
//
//      return the coordinate for the node.
//
//  returns const WorldPoint&  <- 
//----------------------------------------------------------------------------------------
const WorldPoint&
Node::GetCoords() const
{
	return imp->GetCoords();
}

//----------------------------------------------------------------------------------------
//  Node::GetCoord
//
//      return the coordinate value as a string.
//
//  DlInt32 which      -> the coordinate index.
//  bool wantUnits     -> the true to include units.
//
//  returns DlString   <- the coordinate as a string.
//----------------------------------------------------------------------------------------
DlString
Node::GetCoord(DlInt32 which, bool wantUnits)
{
	_DlAssert(which >= 0 && which < 2);
	if (which == 0)
	{
		return UnitTable::FormatValue(imp->GetCoords().x(), UnitsLength,
					DlFloatFormat(4, 0, DlFloatFormatType::Fit), wantUnits);
	} else {
		return UnitTable::FormatValue(imp->GetCoords().y(), UnitsLength,
					DlFloatFormat(4, 0, DlFloatFormatType::Fit), wantUnits);
	}
}

//----------------------------------------------------------------------------------------
//  Node::SetCoord
//
//      set the coordinate value as a string. FIXME: why?
//
//  DlInt32 which      -> the coord index (x == 0, y == 1).
//  const char* str    -> the string coordinate value.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
Node::SetCoord(DlInt32 which, const char* str)
{
	_DlAssert(which >= 0 && which < 2);
	if (which == 0)
		imp->GetCoords().x() = UnitTable::ParseValue(str, UnitsLength);
	else
		imp->GetCoords().y() = UnitTable::ParseValue(str, UnitsLength);
}

//----------------------------------------------------------------------------------------
//  Node::SetCoords
//
//      set the nodal coordinates.
//
//  const WorldPoint& pt   -> the coordinates.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
Node::SetCoords(const WorldPoint& pt)
{
	imp->GetCoords() = pt;
}

//	restraint access
//----------------------------------------------------------------------------------------
//  Node::GetRestraint
//
//      return the node restraint code.
//
//  returns DlUInt32   <- the restraint code for the node.
//----------------------------------------------------------------------------------------
DlUInt32
Node::GetRestraint() const
{
	return imp->GetRestraint();
}

//----------------------------------------------------------------------------------------
//  Node::GetRestrained
//
//      return true if the node is restrained in the specified dof.
//
//  DlInt32 whichDof   -> the dof.
//
//  returns bool       <- true if restrained.
//----------------------------------------------------------------------------------------
bool
Node::GetRestrained(DlInt32 whichDof) const
{
	return imp->GetRestrained(whichDof);
}

//----------------------------------------------------------------------------------------
//  Node::GetLoad
//
//      return the load assigned for the specified load case.
//
//  LoadCase lc        -> the load case.
//
//  returns NodeLoad   <- the load.
//----------------------------------------------------------------------------------------
NodeLoad
Node::GetLoad(LoadCase lc) const
{
	return imp->GetLoad(lc);
}

//----------------------------------------------------------------------------------------
//  Node::GetEquationNumber
//
//      return the equation number for the specified dof.
//
//  DlInt32 i          -> the dof.
//
//  returns DlInt32    <- 
//----------------------------------------------------------------------------------------
DlInt32
Node::GetEquationNumber(DlInt32 i) const
{
	return imp->GetEquationNumber(i);
}

//----------------------------------------------------------------------------------------
//  Node::GetLoadTypeForDOF
//
//      return the load type description for the specified dof.
//
//  DlInt32 dof                    -> the dof.
//
//  returns const PropertyType*    <- the load type description for assigned load values.
//----------------------------------------------------------------------------------------
const PropertyType*
Node::GetLoadTypeForDOF(DlInt32 dof) const
{
	_DlAssert(dof >= 0 && dof < DOF_PER_NODE);
	return (imp->GetRestrained(dof) ? kDOFSettleProperties : kDOFForceProperties) + dof;
}

//----------------------------------------------------------------------------------------
//  Node::GetNodeLoadType
//
//      return the load type for the specified dof.
//
//  DlInt32 dof            -> the dof.
//
//  returns NodeLoadType   <- the load type.
//----------------------------------------------------------------------------------------
NodeLoadType
Node::GetNodeLoadType(DlInt32 dof) const
{
	return imp->GetRestrained(dof) ? NodeLoadIsDisp : NodeLoadIsForce;
}

//----------------------------------------------------------------------------------------
//  Node::GetReactionTypeForDOF
//
//      return the reaction type description for the specified dof.
//
//  DlInt32 dof                    -> the dof.
//
//  returns const PropertyType*    <- the type description for the reaction values.
//----------------------------------------------------------------------------------------
const PropertyType*
Node::GetReactionTypeForDOF(DlInt32 dof) const
{
	_DlAssert(dof >= 0 && dof < DOF_PER_NODE);
	return (imp->GetRestrained(dof) ? kDOFReactionProperties : kDOFDispProperties) + dof;
}

// eof
