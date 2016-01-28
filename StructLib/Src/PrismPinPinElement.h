/*+
 *	File:		PrismPinPinElement.h
 *
 *	Contains:	Interface for truss-type prismatic element. This is only for testing the multiple
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

#ifndef __StructLib__PrismPinPinElement__
#define __StructLib__PrismPinPinElement__

// only build this if debugging since it is only for testing.
#if DlDebugging

#include "PrismElement.h"
#include "DlString.h"

//---------------------------------- Class -------------------------------------

class NodeImp;
class PropertyImp;
class PropertyTypeList;
class UnitTable;

struct ElementCompData;

class	PrismPinPinElement : public PrismElement
{
public:
	
	PrismPinPinElement(NodeImp* startNode, NodeImp* endNode, PropertyImp* property);
	PrismPinPinElement(StrInputStream& inp, const frame_data& data);
	
	virtual const char* GetType() const { return sElemType; }
	
	//	create an element of this type
	static ElementImp* CreateElement(NodeImp* startNode, NodeImp* endNode,
									 PropertyImp* property);
	
	//	create an element of this type
	static ElementImp* ReadElement(StrInputStream& inp, const frame_data& data);
	
	//	get the properties for elements of this type
//	static PropertyImp* CreateProperties(const char* defaultTitle);
//	
//	//	get all the property types for this element type
	static const PropertyTypeList* GetPropertyTypes();
//    
//    // return the property types for the results.
    static const PropertyTypeList* GetResultTypes();
    
	// return the name of this element type
	static const char* GetElementType() { return sElemType; }
	static const ElementFactory::ElementProcs& GetProcs() { return sElementProcs; }
	
private:
	
	static const PropertyTypeList* CreatePropertyTypes();
	
private:
	
	const static ElementFactory::ElementProcs sElementProcs;
	
	static const char*		sElemType;
};

#endif // DlDebugging

#endif /* defined(__StructLib__PrismPinPinElement__) */
