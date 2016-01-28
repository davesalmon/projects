/*+
 *	File:		Element.h
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

#ifndef _H_Element
#define _H_Element

#include "StrDefines.h"
#include "Node.h"
#include "WorldRect.h"
#include "PointEnumerator.h"
#include "PropertyTypeEnumerator.h"

//---------------------------------- Class -------------------------------------

class Property;
class ElementImp;
class ElementLoad;
class LoadCaseResults;
class ElementForce;

// property names
const char* const kPropertyModulus			{ "Modulus of Elasticity" };
const char* const kPropertyMomentOfInertia	{ "Moment of Inertia" };
const char* const kPropertyArea				{ "Area" };
const char* const kPropertyStartPinned		{ "Start Pinned" };
const char* const kPropertyEndPinned		{ "End Pinned" };

class Element
{
public:
	
	Element() : imp(0) {}
	Element(ElementImp* elem) : imp(elem) {}
	
	operator ElementImp* () const { return imp; }

	bool 			Empty() const { return imp == 0; }

	//	return the starting node for this element
	Node 			StartNode();
	
	//	return the ending node for this element
	Node 			EndNode();
	
	//	return the direction cosines for this element
	WorldPoint		Cosines() const;
	
	// return the element length
	DlFloat64		Length() const;
	
	DlInt32			GetCoords(PointEnumerator pts) const;
	
	// Return the property type for the  force at the specified DOF
	const PropertyType* GetForceTypeForDOF(DlInt32 dof) const;

	//	input are fixed end forces, output are forces at x
	void			GetElementForce(double where, LoadCase lc, const ElementForce& feForce, 
                                    ElementForce& force) const;
	
	// return a graph of the force in the specified DOF
	DlInt32			ForceDOFCoords(DlInt32 dof, DlUInt32 index, PointEnumerator pts,
								   const LoadCaseResults& res) const;

	// return the force in DOF at the specified relative coordinate for element index.
	DlFloat64		DOFForceAt(DlInt32 dof, DlInt32 index, DlFloat64 u,
							   const LoadCaseResults& res) const;
	
	//	the displaced shape of the element
	DlInt32			GetDisplacedCoords(DlUInt32 index, PointEnumerator pts,
									   const LoadCaseResults& res) const;

	// return the property types for displacements
	const PropertyType* GetDispTypeForDOF(DlInt32 dof) const;

	// return the displacement in DOF at the specified relative location for element index.
	DlFloat64 DOFDispAt(DlInt32 dof, DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const;

	// return the number of result types defined for this element.
	DlInt32 CountResultTypes() const;
	
	// return the property for the specified result type
	const PropertyType* GetResultType(DlInt32 whichType) const;

	// These values correspond to the names returned by get result types
	DlFloat64		GetResultValue(DlUInt32 whichType, const ElementForce& frc,
								   const LoadCaseResults& res) const;

	WorldRect		GetBounds()
						{ return WorldRect(StartNode().GetCoords(), EndNode().GetCoords()); }
	
	//	get the propery object assigned to this type
	Property 		GetProperty() const;
	void			SetProperty(const Property& prop);
	
	ElementLoad		GetLoad(LoadCase which) const;
//	void			SetLoad(LoadCase which, const ElementLoad& load);
	
	//	return the type of the element
	const char* 	GetType() const;

	//	return true if the property object is acceptable to this element
	bool			CanAssignProperty(const Property& prop);

private:
	
	ElementImp*	imp;

};

#endif

