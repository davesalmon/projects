/*+
 *
 *  PropertyAction.cpp
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  Handle actions for changing properties.
 *
-*/

#include "DlPlatform.h"
#include "PropertyAction.h"
#include "PropertyList.h"
#include "ElementList.h"
#include "frame_data.h"

//----------------------------------------------------------------------------------------
//  AddPropertyAction::AddPropertyAction                                      constructor
//
//      Construct property add action.
//
//  PropertyImp* prop      -> the property being added.
//  PropertyList* theList  -> the list to add to.
//
//  returns nothing
//----------------------------------------------------------------------------------------
AddPropertyAction::AddPropertyAction(PropertyImp* prop, PropertyList* theList) 
	: Action("Create Property")
	, itsElem(prop)
	, itsList(theList) 
{
}

//----------------------------------------------------------------------------------------
//  AddPropertyAction::~AddPropertyAction                                      destructor
//
//      desctruct property add action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
AddPropertyAction::~AddPropertyAction() 
{
	if (!PerformIsUndo())
		delete itsElem;
}

//----------------------------------------------------------------------------------------
//  AddPropertyAction::Redo
//
//      redo the add action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AddPropertyAction::Redo() 
{
	itsList->Add(itsElem);
}

//----------------------------------------------------------------------------------------
//  AddPropertyAction::Undo
//
//      undo the add action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AddPropertyAction::Undo() 
{
	itsList->Remove(itsElem);
}

//----------------------------------------------------------------------------------------
//  RemovePropertyAction::RemovePropertyAction                                constructor
//
//      construct remove action.
//
//  PropertyImp* prop      -> the property being removed.
//  PropertyList* theList  -> the property list.
//  frame_data* data       -> the data object.
//
//  returns nothing
//----------------------------------------------------------------------------------------
RemovePropertyAction::RemovePropertyAction(PropertyImp* prop, PropertyList* theList, frame_data* data)
: Action("Create Property")
, itsElem(prop)
, itsList(theList)
, itsData(data)
{
}

//----------------------------------------------------------------------------------------
//  RemovePropertyAction::~RemovePropertyAction                                destructor
//
//      destruct the remove action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
RemovePropertyAction::~RemovePropertyAction()
{
	if (PerformIsUndo())
		delete itsElem;
}

//----------------------------------------------------------------------------------------
//  RemovePropertyAction::Undo
//
//      undo the remove action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemovePropertyAction::Undo()
{
	itsList->Add(itsElem);
	itsData->SetActiveProperty(itsElem);
}

//----------------------------------------------------------------------------------------
//  RemovePropertyAction::Redo
//
//      redo the remove action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemovePropertyAction::Redo()
{
	itsList->Remove(itsElem);
	itsData->SetActiveProperty(Property()); // force new active property
}

//----------------------------------------------------------------------------------------
//  AssignPropertyAction::AssignPropertyAction                                constructor
//
//      construct the assign action.
//
//  PropertyImp* prop                  -> the property being assigned.
//  const ElementEnumerator& theList   -> the list of elements to assign.
//
//  returns nothing
//----------------------------------------------------------------------------------------
AssignPropertyAction::AssignPropertyAction(PropertyImp* prop, const ElementEnumerator& theList)
	:Action("Assign Property")
	, itsList(theList)
	, itsProp(prop)
	, savedProps(0)
{
	int numProps = itsList.Length();
	ElementList* l = ElementList::GetList(itsList);
//	savedProps = (PropertyImp**)OPERATORNEW(numProps * sizeof(PropertyImp*));
	
	savedProps = NEW PropertyImp*[numProps];
	
//	const char* elementType = Property(prop).GetElementType();
	
	for (int i = 0; i < numProps; i++) {
		ElementImp* elem = l->ElementAt(i);
		if (itsProp->CanAssignToElement(elem->GetType())) {
			savedProps[i] = elem->GetProperty();
		}
	}
}

//----------------------------------------------------------------------------------------
//  AssignPropertyAction::~AssignPropertyAction                                destructor
//
//      destruct the assign action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
AssignPropertyAction::~AssignPropertyAction() 
{
	delete[] savedProps;
}

//----------------------------------------------------------------------------------------
//  AssignPropertyAction::Redo
//
//      redo the assign action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AssignPropertyAction::Redo()
{
	int numProps = itsList.Length();
	ElementList* l = ElementList::GetList(itsList);

	for (int i = 0; i < numProps; i++) {
		ElementImp* elem = l->ElementAt(i);
		if (itsProp->CanAssignToElement(elem->GetType())) {
			elem->SetProperty(itsProp);
		}
	}
}

//----------------------------------------------------------------------------------------
//  AssignPropertyAction::Undo
//
//      undo the assign action/
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AssignPropertyAction::Undo()
{
	int numProps = itsList.Length();
	ElementList* l = ElementList::GetList(itsList);

	for (int i = 0; i < numProps; i++) {
		ElementImp* elem = l->ElementAt(i);
		if (itsProp->CanAssignToElement(elem->GetType())) {
			elem->SetProperty(savedProps[i]);
		}
	}
}


//----------------------------------------------------------------------------------------
//  ChangePropertyAction::ChangePropertyAction                                constructor
//
//      construct the change property value action.
//
//  PropertyImp* prop      -> the property being changed.
//  const char* key        -> the value key.
//  const char* newValue   -> the new value to set.
//
//  returns nothing
//----------------------------------------------------------------------------------------
ChangePropertyAction::ChangePropertyAction(PropertyImp* prop, const char* key, const char* newValue)
	: Action("Change Property Value")
	, itsProp(prop)
	, itsKey(key)
	, itsNewValue(newValue)
	, itsOldValue(Property(prop).GetValue(key, false))
{
}
	
//----------------------------------------------------------------------------------------
//  ChangePropertyAction::~ChangePropertyAction                                destructor
//
//      destruct change property value action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
ChangePropertyAction::~ChangePropertyAction()
{
}

//----------------------------------------------------------------------------------------
//  ChangePropertyAction::Redo
//
//      redo change property value.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
ChangePropertyAction::Redo()
{
	itsProp->SetValue(itsKey, itsNewValue);
}

//----------------------------------------------------------------------------------------
//  ChangePropertyAction::Undo
//
//      undo change property value.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
ChangePropertyAction::Undo()
{
	itsProp->SetValue(itsKey, itsOldValue);
}
