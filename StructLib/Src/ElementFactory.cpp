/*+
 *
 *  ElementFactory.cpp
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

#include "DlPlatform.h"
#include "ElementFactory.h"
#include "PrismElement.h"
#include "PrismPinPinElement.h"
#include "PropertyFactory.h"

ElementFactory ElementFactory::sElementFactory;

//----------------------------------------------------------------------------------------
//  ElementFactory::ElementFactory                                            constructor
//
//      construct the factory.
//
//  returns nothing
//----------------------------------------------------------------------------------------
ElementFactory::ElementFactory()
{
	// register elements here.
	RegisterCreator(PrismElement::GetElementType(), PrismElement::GetProcs());
#if DlDebugging
	// only register this element for debugging.
	RegisterCreator(PrismPinPinElement::GetElementType(), PrismPinPinElement::GetProcs());
#endif
	PropertyFactory::sPropertyFactory.UpdateCompatibilityMap();
}

//----------------------------------------------------------------------------------------
//  ElementFactory::RegisterCreator
//
//      registor an element factory.
//
//  const char* elemType           -> the element type
//  const ElementFactory & func    -> the creator function.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementFactory::RegisterCreator(const char* elemType, const ElementProcs & func)
{
	theMap[elemType] = func;
	PropertyFactory::sPropertyFactory.RegisterProperties(elemType,
				(*func.getPropTypes)(), (*func.getResultTypes)());
}

//----------------------------------------------------------------------------------------
//  ElementFactory::GetElementTypes
//
//      return the types of elements registered.
//
//  returns StringEnumerator   <- list of element type names.
//----------------------------------------------------------------------------------------
StringEnumerator
ElementFactory::GetElementTypes() const
{
	StringEnumerator	types;

	for (auto& i : theMap) {
		types.Add(i.first);
    }
	
	return types;
}

//----------------------------------------------------------------------------------------
//  ElementFactory::CreateElement
//
//      create a new element of the specified type.
//
//  const char* elemType   -> the type
//  NodeImp * startNode    -> the first node
//  NodeImp * endNode      -> the second node.
//  PropertyImp * property -> the element property.
//
//  returns ElementImp *   <- the new element.
//----------------------------------------------------------------------------------------
ElementImp *
ElementFactory::CreateElement(const char* elemType, NodeImp * startNode,
						  NodeImp * endNode, PropertyImp * property) const
{
	auto i = theMap.find(elemType);
	if (i != theMap.end())
		return i->second.create(startNode, endNode, property);

	return nullptr;
}

//----------------------------------------------------------------------------------------
//  ElementFactory::CreateElement
//
//      create a new element from the stream.
//
//  const char* elemType   -> the element type
//  StrInputStream& inp    -> the input stream
//  const frame_data& data -> the frame data object
//
//  returns ElementImp *   <- the new element.
//----------------------------------------------------------------------------------------
ElementImp *
ElementFactory::CreateElement(const char* elemType, StrInputStream& inp,
						  const frame_data& data) const
{
	auto i = theMap.find(elemType);
	if (i != theMap.end())
		return i->second.read(inp, data);

	return NULL;
}

//----------------------------------------------------------------------------------------
//  ElementFactory::GetDefaultElementType
//
//      return the default element type.
//
//  returns const char*    <- 
//----------------------------------------------------------------------------------------
const char*
ElementFactory::GetDefaultElementType() const
{
	return PrismElement::GetElementType();
}


