/*+
 *
 *  PropertyFactory.h
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  track the properties used by different element types and return property list values
 *  for each type as well as the list for the structure as a whole.
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

#ifndef __StructLib__PropertyFactory__
#define __StructLib__PropertyFactory__

#include <map>
#include <set>
#include <array>

//#include "PropertyTypeList.h"
#include "DlStringUtil.h"
#include "PropertyTypeEnumerator.h"
#include "StringEnumerator.h"

class PropertyImp;
class PropertyTypeList;

class PropertyFactory
{
	PropertyFactory();
	
public:
	
	// register property types for the specified element types.
	void RegisterProperties(const char* elementType,
							const PropertyTypeList* propTypes,
							const PropertyTypeList* resultTypes);
	
	// return the list of property names for the specified element type.
	const PropertyTypeList* GetPropertyTypes(const char* elemType) const;

	// return union of all element property types.
	const PropertyTypeList* GetMergedPropertyTypes() const;

	// return the list of result type names for the specified element type.
	const PropertyTypeList* GetResultTypes(const char* elemType) const;
	
	// return union of all element result types.
	const PropertyTypeList* GetMergedResultTypes() const;
	
	//	create a new property object for elements of this type
	PropertyImp*			CreateProperty(const char* title, const char* elemType) const;

	// return true if the specified property type list is assignable to elements of
	//	type elementType.
	bool					IsPropertyAssignable(const char* elementType,
												 const PropertyTypeList* properties);

	// return the element type name for the specified property type list.
	const char*				GetElementForPropertyTypes(const PropertyTypeList* propertyTypes) const;
	
	// update the compatibility map showing which properties are assignable.
	void					UpdateCompatibilityMap();
	
	// the type factory singleton.
	static PropertyFactory sPropertyFactory;
		
private:
	
	using PropertyTypePair	= std::pair<PropertyTypeEnumerator, PropertyTypeEnumerator>;
	using PropertyMap		= std::map<const char*, PropertyTypePair, DlCStringKeyLess>;
	using ElementSet		= std::set<const char*, DlCStringKeyLess>;
	
	using TypeMap			= std::map<const PropertyTypeList*, ElementSet>;
	
	PropertyMap			_propertyMap;
	ElementSet			_elementTypesInUse;
	TypeMap				_compatibleTypes;
	
	PropertyTypeEnumerator	_currentPropertyTypes;
	PropertyTypeEnumerator	_currentResultTypes;
};

#endif /* defined(__StructLib__PropertyFactory__) */
