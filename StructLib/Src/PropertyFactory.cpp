/*+
 *
 *  PropertyFactory.cpp
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

#include "DlPlatform.h"
#include "PropertyFactory.h"
#include "PropertyImp.h"
#include "PropertyTypeList.h"

PropertyFactory PropertyFactory::sPropertyFactory {};

//----------------------------------------------------------------------------------------
//  PropertyFactory::PropertyFactory                                          constructor
//
//      construct the property factory.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PropertyFactory::PropertyFactory()
	: _currentPropertyTypes(NEW PropertyTypeList({}))
	, _currentResultTypes(NEW PropertyTypeList({}))
{
}

//----------------------------------------------------------------------------------------
//  PropertyFactory::RegisterProperties
//
//      register the property type set for the specified element.
//
//  const char* elementType                -> the element type id.
//  const PropertyTypeList* propTypes      -> the property type list.
//  const PropertyTypeList* resultTypes    -> the result type list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
PropertyFactory::RegisterProperties(const char* elementType,
										 const PropertyTypeList* propTypes,
										 const PropertyTypeList* resultTypes)
{
	_propertyMap[elementType] = { PropertyTypeEnumerator(propTypes),
		PropertyTypeEnumerator(resultTypes)};

	_currentPropertyTypes.GetList()->Merge(*propTypes);
	_currentResultTypes.GetList()->Merge(*resultTypes);
}

//----------------------------------------------------------------------------------------
//  PropertyFactory::GetPropertyTypes
//
//      return the property types for the specified element. If no element is specified
//		return the master list.
//
//  const char* elemType               -> the element type id.
//
//  returns const PropertyTypeList*    <- the associated property type list.
//----------------------------------------------------------------------------------------
const PropertyTypeList*
PropertyFactory::GetPropertyTypes(const char* elemType) const
{
	_DlAssert(elemType != nullptr);
	return _propertyMap.find(elemType)->second.first.GetList();
}

const PropertyTypeList*
PropertyFactory::GetMergedPropertyTypes() const
{
	return _currentPropertyTypes.GetList();
}


//----------------------------------------------------------------------------------------
//  PropertyFactory::GetResultTypes
//
//      return the result types for the specified element. If no element is specified
//		return the master list.
//
//  const char* elemType               -> the element type id
//
//  returns const PropertyTypeList*    <- the associated result type list.
//----------------------------------------------------------------------------------------
const PropertyTypeList*
PropertyFactory::GetResultTypes(const char* elemType) const
{
	_DlAssert(elemType != nullptr);
	return _propertyMap.find(elemType)->second.second.GetList();
}

const PropertyTypeList*
PropertyFactory::GetMergedResultTypes() const
{
	return _currentResultTypes.GetList();
}

//----------------------------------------------------------------------------------------
//  PropertyFactory::CreateProperty
//
//      create a new property set for the specified element type.
//
//  const char* title      -> the title for the properties.
//  const char* elemType   -> the element type id.
//
//  returns PropertyImp*   <- the new property set.
//----------------------------------------------------------------------------------------
PropertyImp*
PropertyFactory::CreateProperty(const char* title, const char* elemType) const
{
	auto loc = _propertyMap.find(elemType);
	
	if (loc != _propertyMap.end()) {
		return new	PropertyImp(title, loc->second.first.GetList());
	}
	
	return nullptr;
}

//----------------------------------------------------------------------------------------
//  PropertyFactory::IsPropertyAssignable
//
//      return true if the specified property types are assignable to the specified
//      element.
//
//  const char* elementType                -> the element type id.
//  const PropertyTypeList* propertyTypes  -> the property types to assign.
//
//  returns bool                           <- 
//----------------------------------------------------------------------------------------
bool
PropertyFactory::IsPropertyAssignable(const char* elementType,
											 const PropertyTypeList* propertyTypes)
{
	// find the element set for the specified property type and return true
	// if it contains the specified element.
	auto setValue = _compatibleTypes.find(propertyTypes);
	
	_DlAssert(setValue != _compatibleTypes.end());
	
	return setValue->second.find(elementType) != setValue->second.end();
}

//----------------------------------------------------------------------------------------
//  PropertyFactory::GetElementForPropertyTypes
//
//      return the element type associated with the specified property types.
//
//  const PropertyTypeList* propertyTypes  -> the property types.
//
//  returns const char*                    <- the element.
//----------------------------------------------------------------------------------------
const char*
PropertyFactory::GetElementForPropertyTypes(const PropertyTypeList* propertyTypes) const
{
	for (auto t : _propertyMap) {
		if (t.second.first.GetList() == propertyTypes)
			return t.first;
	}
	
	return nullptr;
}

//----------------------------------------------------------------------------------------
//  PropertyFactory::UpdateCompatibilityMap
//
//      update the compatibility map. This map maps each property type list to the set
//		of element types to which that property may be assigned.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
PropertyFactory::UpdateCompatibilityMap()
{
	// rebuild the compatible types list.
	for (auto propPair : _propertyMap) {
		
		const PropertyTypeList* props = propPair.second.first.GetList();
		
		ElementSet& elems = _compatibleTypes[props] = { propPair.first };
		
		for (auto otherPropPair : _propertyMap) {
			
			const PropertyTypeList* other = otherPropPair.second.first.GetList();
			
			if (other == props)
				continue;
			
			// is assignable as return true if other could be used in place of props.
			if (other->IsAssignableAs(props)) {
				elems.insert(otherPropPair.first);
			}
#if DlDebugging
			else {
				printf("%s is not compatible with %s\n", propPair.first, otherPropPair.first);
			}
#endif
		}
	}

}
