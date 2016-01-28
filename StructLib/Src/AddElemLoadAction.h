/*+
 *	File:		AddElemLoadAction.h
 *
 *	Contains:	Action interface for adding node loads
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
-*/

#ifndef _H_AddElemLoadAction
#define _H_AddElemLoadAction

//---------------------------------- Includes ----------------------------------

#include "Action.h"
#include "ElementList.h"
#include "ElementLoad.h"

class ElemLoadImp;
class ElemLoadList;
class frame_data;

//---------------------------------- Class -------------------------------------

class	AddElemLoadAction : public Action
{
public:
	AddElemLoadAction(ElemLoadImp* theElem, ElemLoadList* theList);

	virtual ~AddElemLoadAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    AddElemLoadAction(const AddElemLoadAction& a);
    AddElemLoadAction& operator=(const AddElemLoadAction&a);
        
	ElemLoadList* itsList;
	ElemLoadImp* itsElem;
};

class	RemoveElemLoadAction : public Action
{
public:
	RemoveElemLoadAction(ElemLoadImp* theElem, ElemLoadList* theList);
	
	virtual ~RemoveElemLoadAction();
	
	virtual void Redo();
	virtual void Undo();
	
private:
	RemoveElemLoadAction(const RemoveElemLoadAction& a);
	RemoveElemLoadAction& operator=(const RemoveElemLoadAction&a);
	
	ElemLoadList* itsList;
	ElemLoadImp* itsElem;
};

class AssignElemLoadAction : public Action
{
public:
	AssignElemLoadAction(ElemLoadImp* theLoad, const ElementEnumerator& theList, LoadCase lc);

	virtual ~AssignElemLoadAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    AssignElemLoadAction(const AssignElemLoadAction& a);
    AssignElemLoadAction& operator=(const AssignElemLoadAction& a);
        
	ElementEnumerator itsList;
	ElemLoadImp* itsLoad;
	LoadCase itsLoadCase;
	ElemLoadImp** savedLoads;
};

class ChangeElemLoadAction : public Action {
public:
	ChangeElemLoadAction(ElemLoadImp* theLoad, ElementLoadType dof, const char* theValue, frame_data* data);
	virtual void Redo();
	virtual void Undo();
private:
        
    ChangeElemLoadAction(const ChangeElemLoadAction& a);
    ChangeElemLoadAction& operator=(const ChangeElemLoadAction& a);
        
	ElemLoadImp*	itsLoad;
	DlFloat64		itsOldValue;
//	DlInt32			itsOldType;
	ElementLoadType	itsType;
//	bool			itsIsValue;
	frame_data*		itsData;
};

//---------------------------------- Inlines -----------------------------------

#endif

//	eof

