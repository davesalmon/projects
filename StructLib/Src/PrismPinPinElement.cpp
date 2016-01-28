/*+
 *	File:		PrismPinPinElement.cpp
 *
 *	Contains:	Implement truss-type prismatic element. This is only for testing the multiple
 * 				element type code.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2014 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

// only build this if debugging since it is only for testing.
#if DlDebugging

#include "DlPlatform.h"
#include "DlMacros.h"

#include "PrismPinPinElement.h"

#include "PropertyTypeList.h"
#include "PropertyImp.h"
#include "NodeImp.h"
#include "ElemLoadImp.h"
#include "LoadCaseResults.h"
#include "PointEnumeratorImp.h"

const char* PrismPinPinElement::sElemType = ("Pin-End Prism");

// The element properties for Prism elements.
const PropertyType	kPinPinProperties[] = {
	  {kPropertyModulus,			UnitsModulus, 	PropDataFloat, 	true}
	, {kPropertyMomentOfInertia,	UnitsMOI, 		PropDataFloat, 	false}
	, {kPropertyArea,				UnitsArea, 		PropDataFloat, 	true}
	, {"test me",					UnitsLength, 	PropDataFloat, 	true}
	, {kPropertyStartPinned, 		UnitsNone, 		PropDataBool,	false}
	, {kPropertyEndPinned,			UnitsNone, 		PropDataBool,	false}
};

// element factory methods.
const ElementFactory::ElementProcs PrismPinPinElement::sElementProcs = {
	PrismPinPinElement::CreateElement,
	PrismPinPinElement::ReadElement,

	PrismPinPinElement::CreatePropertyTypes,
	PrismElement::CreateResultTypes
};

#pragma mark -
#pragma mark ------ class methods ------

//----------------------------------------------------------------------------------------
//  PrismElement::PrismElement                                                constructor
//
//      Construct a prism element.
//
//  NodeImp* startNode     -> the start node
//  NodeImp* endNode       -> the end node
//  PropertyImp* property  -> the properties for this element.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PrismPinPinElement::PrismPinPinElement(NodeImp* startNode, NodeImp* endNode,
						   PropertyImp* property)
	: PrismElement(startNode, endNode, property)
{
}

//----------------------------------------------------------------------------------------
//  PrismElement::PrismElement                                                constructor
//
//      Construct a prism element from the input stream
//
//  StrInputStream& inp    -> the data stream
//  const frame_data& data -> the frame.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PrismPinPinElement::PrismPinPinElement(StrInputStream& inp, const frame_data& data)
	: PrismElement(inp, data)
{
}

#pragma mark -
#pragma mark ------ static methods ------


//----------------------------------------------------------------------------------------
//  PrismElement::CreateElement
//
//      create a new prism element.
//
//  NodeImp* startNode     -> the start node.
//  NodeImp* endNode       -> the end node.
//  PropertyImp* property  -> the property set.
//
//  returns ElementImp*    <- the element.
//----------------------------------------------------------------------------------------
ElementImp*
PrismPinPinElement::CreateElement(NodeImp* startNode, NodeImp* endNode, PropertyImp* property)
{
	return NEW PrismPinPinElement(startNode, endNode, property);
}

//----------------------------------------------------------------------------------------
//  PrismElement::ReadElement
//
//      Create a new element of this type from the data in the file.
//
//  StrInputStream& inp    -> the input stream
//  const frame_data& data -> the fram data.
//
//  returns ElementImp*    <- the new element.
//----------------------------------------------------------------------------------------
ElementImp*
PrismPinPinElement::ReadElement(StrInputStream& inp, const frame_data& data)
{
	return NEW PrismPinPinElement(inp, data);
}

const PropertyTypeList*
PrismPinPinElement::CreatePropertyTypes()
{
	return NEW PropertyTypeList(kPinPinProperties, DlArrayElements(kPinPinProperties));
}

#endif // DlDebugging
// eof
