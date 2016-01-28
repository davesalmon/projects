// PropertyAction.h

#ifndef _H_PropertyAction
#define _H_PropertyAction

#include "Action.h"
#include "ElementEnumerator.h"

class PropertyImp;
class PropertyList;
class ElementList;
class frame_data;

class AddPropertyAction : public Action 
{
public:
	AddPropertyAction(PropertyImp* prop, PropertyList* theList);
	
	virtual ~AddPropertyAction();
	
	virtual void Redo();
	virtual void Undo();

private:
	AddPropertyAction(const AddPropertyAction& a);
    AddPropertyAction& operator=(const AddPropertyAction& a);

	PropertyList* itsList;
	PropertyImp* itsElem;
};

class RemovePropertyAction : public Action
{
public:
	RemovePropertyAction(PropertyImp* prop, PropertyList* theList, frame_data* data);
	
	virtual ~RemovePropertyAction();
	
	virtual void Redo();
	virtual void Undo();
	
private:
	RemovePropertyAction(const RemovePropertyAction& a);
	RemovePropertyAction& operator=(const RemovePropertyAction& a);
	
	PropertyList* itsList;
	PropertyImp* itsElem;
	frame_data* itsData;
};

class AssignPropertyAction : public Action 
{
public:
	AssignPropertyAction(PropertyImp* prop, const ElementEnumerator& theList);
	
	virtual ~AssignPropertyAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    AssignPropertyAction(const AssignPropertyAction& a);
    AssignPropertyAction& operator=(const AssignPropertyAction& a);

	ElementEnumerator itsList;
	PropertyImp* itsProp;
	PropertyImp** savedProps;
};

class ChangePropertyAction : public Action 
{
public:
	ChangePropertyAction(PropertyImp* prop, const char* key, const char* newValue);
	
	virtual ~ChangePropertyAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    ChangePropertyAction(const ChangePropertyAction& a);
    ChangePropertyAction& operator=(const ChangePropertyAction& a);

	PropertyImp* itsProp;
	const char* itsKey;
	DlString itsNewValue;
	DlString itsOldValue;
};


#endif // _H_PropertyAction

// eof
