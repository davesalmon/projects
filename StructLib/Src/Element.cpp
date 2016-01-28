/*+
 *	File:		Element.cpp
 *
 *	Contains:	Interface for element
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

//+--------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "Element.h"

#include "Node.h"
#include "Property.h"
#include "ElementImp.h"
#include "ElementLoad.h"

//+--------------------------------- Methods -----------------------------------

//----------------------------------------------------------------------------------------
//  Element::StartNode
//
//      return the starting node for this element.
//
//  returns Node   <- the start node.
//----------------------------------------------------------------------------------------
Node 
Element::StartNode()
{
	return imp->StartNode();
}

//----------------------------------------------------------------------------------------
//  Element::EndNode
//
//      return the end node for this element.
//
//  returns Node   <- the end node.
//----------------------------------------------------------------------------------------
Node 
Element::EndNode()
{
	return imp->EndNode();
}

//----------------------------------------------------------------------------------------
//  Element::Cosines
//
//      Compute the direction cosines for the element.
//
//  returns WorldPoint <- direction cosines (x, y)
//----------------------------------------------------------------------------------------
WorldPoint
Element::Cosines() const 
{
	return imp->Cosines();	
}

//----------------------------------------------------------------------------------------
//  Element::Length
//
//      return the length of the element
//
//  returns DlFloat64  <- the length
//----------------------------------------------------------------------------------------
DlFloat64
Element::Length() const
{
	return imp->Length();
}

//----------------------------------------------------------------------------------------
//  Element::GetCoords
//
//      return the coordinates.
//
//  PointEnumerator pts    -> the point enumerator to add points to.
//
//  returns DlInt32        <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32
Element::GetCoords(PointEnumerator pts) const 
{
	return imp->Coords(pts);
}

const PropertyType* 
Element::GetForceTypeForDOF(DlInt32 dof) const
{
	return imp->GetForceTypeForDOF(dof);
}

void
Element::GetElementForce(double where, LoadCase lc, const ElementForce& feForce, 
                         ElementForce& force) const
{
	imp->GetElementForce(where, feForce, lc, force);
}

DlInt32
Element::ForceDOFCoords(DlInt32 dof, DlUInt32 index, PointEnumerator pts, const LoadCaseResults& res) const
{
	return imp->ForceDOFCoords(dof, index, pts, res);
}

DlFloat64
Element::DOFForceAt(DlInt32 dof, DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const
{
	return imp->DOFForceAt(dof, index, u, res);
}

//----------------------------------------------------------------------------------------
//  Element::GetDisplacedCoords
//
//      return the displaced coordinates.
//
//  DlUInt32 index             -> the element index.
//  PointEnumerator pts        -> the point enumerator to add to.
//  const LoadCaseResults& res -> the load case results.
//
//  returns DlInt32            <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32
Element::GetDisplacedCoords(DlUInt32 index, PointEnumerator pts, const LoadCaseResults& res) const 
{
	return imp->DisplacedCoords(index, pts, res);
}

const PropertyType* 
Element::GetDispTypeForDOF(DlInt32 dof) const
{
	return imp->GetDispTypeForDOF(dof);
}

DlFloat64
Element::DOFDispAt(DlInt32 dof, DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const
{
	return imp->DOFDispAt(dof, index, u, res);
}

DlInt32
Element::CountResultTypes() const
{
	return imp->CountResultTypes();
}

const PropertyType* 
Element::GetResultType(DlInt32 whichType) const
{
	return imp->GetResultType(whichType);
}

DlFloat64
Element::GetResultValue(DlUInt32 valIndex, const ElementForce& frc, const LoadCaseResults& res) const
{
    return imp->GetResultValue(valIndex, frc, res);
}

#if 0
//----------------------------------------------------------------------------------------
//  Element::GetAxialCoords
//
//      return the axial coordinates.
//
//  DlUInt32 index             -> the element index.
//  PointEnumerator pts        -> the point enumerator to add to.
//  const LoadCaseResults& res -> the load case results.
//
//  returns DlInt32            <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32
Element::GetAxialCoords(DlUInt32 index, PointEnumerator pts, const LoadCaseResults& res) const
{
	return imp->AxialCoords(index, pts, res);
}

//----------------------------------------------------------------------------------------
//  Element::GetMomentCoords
//
//      return the moment coordinates.
//
//  DlUInt32 index             -> the element index.
//  PointEnumerator pts        -> the point enumerator to add to.
//  const LoadCaseResults& res -> the load case results.
//
//  returns DlInt32            <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32
Element::GetMomentCoords(DlUInt32 index, PointEnumerator pts, const LoadCaseResults& res) const 
{
	return imp->MomentCoords(index, pts, res);
}

//----------------------------------------------------------------------------------------
//  Element::GetShearCoords
//
//      return the shear coordinates.
//
//  DlUInt32 index             -> the element index.
//  PointEnumerator pts        -> the point enumerator to add to.
//  const LoadCaseResults& res -> the load case results.
//
//  returns DlInt32            <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32
Element::GetShearCoords(DlUInt32 index, PointEnumerator pts, const LoadCaseResults& res) const 
{
	return imp->ShearCoords(index, pts, res);
}

DlFloat64 Element::AxialForceAt(DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const
{
	return imp->AxialForceAt(index, u, res);
}

DlFloat64 Element::ShearForceAt(DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const
{
	return imp->ShearForceAt(index, u, res);
}

DlFloat64 Element::MomentAt(DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const
{
	return imp->MomentAt(index, u, res);
}

DlFloat64 Element::LateralDisplacementAt(DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const
{
	return imp->LateralDisplacementAt(index, u, res);
}

DlFloat64 Element::AxialDisplacementAt(DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const
{
	return imp->AxialDisplacementAt(index, u, res);
}

DlFloat64 Element::MaxMoment(DlInt32 index, DlFloat64& x, const LoadCaseResults& res) const
{
	return imp->MaxMoment(index, x, res);
}

#endif

//	get the propery object assigned to this type
//----------------------------------------------------------------------------------------
//  Element::GetProperty
//
//      return the property.
//
//  returns Property   <- the property object associated with this element.
//----------------------------------------------------------------------------------------
Property
Element::GetProperty() const
{
	return imp->GetProperty();
}

//----------------------------------------------------------------------------------------
//  Element::SetProperty
//
//      set the property.
//
//  const Property& prop   -> the property to associate with this element.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
Element::SetProperty(const Property& prop)
{
	if (prop.CanAssignToElement(imp->GetType()))
		imp->SetProperty(prop);
	else
		_DlAssert("Bad property type" == NULL);
}

//----------------------------------------------------------------------------------------
//  Element::GetLoad
//
//      return the load.
//
//  LoadCase which         -> the load case
//
//  returns ElementLoad    <- the load
//----------------------------------------------------------------------------------------
ElementLoad
Element::GetLoad(LoadCase which) const
{
	return imp->GetLoad(which);
}

////----------------------------------------------------------------------------------------
////  Element::SetLoad
////
////      set the load.
////
////  LoadCase which             -> the load case
////  const ElementLoad& load    -> the new load for the element.
////
////  returns nothing
////----------------------------------------------------------------------------------------
//void
//Element::SetLoad(LoadCase which, const ElementLoad& load)
//{
//	return imp->AssignLoad(which, load);
//}
//

//----------------------------------------------------------------------------------------
//  Element::GetType
//
//      return the type
//
//  returns const char*    <- the element type.
//----------------------------------------------------------------------------------------
const char* 
Element::GetType() const
{
	return imp->GetType();
}

//----------------------------------------------------------------------------------------
//  Element::CanAssignProperty
//
//      return true if the property can be assigned to this element.
//
//  const Property& prop   -> the property
//
//  returns bool           <- true if this property can be assigned.
//----------------------------------------------------------------------------------------
bool
Element::CanAssignProperty(const Property& prop)
{
	return prop.CanAssignToElement(imp->GetType());
}

//	eof

