/*+
 *
 *  ElementFactory.h
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  Keep track of the types of elements that exist and methods to create them.
 *
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

#ifndef __StructLib__ElementFactory__
#define __StructLib__ElementFactory__

#include "ElementImp.h"
#include "PropertyImp.h"
#include "PropertyTypeList.h"
#include "StringEnumerator.h"

#include <map>

class ElementFactory
{
public:
	typedef ElementImp* 				(*ElementCreator)(NodeImp* startNode,
														  NodeImp* endNode, PropertyImp * p);
	typedef ElementImp* 				(*ElementReader)(StrInputStream& inp,
														 const frame_data& data);
	typedef const PropertyTypeList*		(*ElementPropTypes)();

	struct ElementProcs {
		ElementCreator			create;
		ElementReader			read;
		
		ElementPropTypes		getPropTypes;
		ElementPropTypes		getResultTypes;
	};
	
	ElementFactory();
	
	// register a new element type.
	void 					RegisterCreator(const char* elemType, const ElementProcs & func);
		
	//	get all registered element types
	StringEnumerator 		GetElementTypes() const;
		
	//	create an element of this type
	ElementImp* 		CreateElement(const char* elementType, NodeImp* startNode, NodeImp* endNode,
										  PropertyImp* property) const;
	//	create an element of this type
	ElementImp* 		CreateElement(const char* elementType, StrInputStream& inp,
										  const frame_data& data) const;
	
	//	get the propery names for elements of this type
	//	static StringEnumerator GetPropertyNames(const DlString & elemType);
	const char*		GetDefaultElementType() const;

private:
		
	typedef	std::map<const char*, ElementProcs, DlCStringKeyLess>	CreatorMap;

	CreatorMap theMap;
	
public:
	
	static ElementFactory sElementFactory;
};


#endif /* defined(__StructLib__ElementFactory__) */
