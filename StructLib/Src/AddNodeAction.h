/*+
 *	File:		AddNodeAction.h
 *
 *	Contains:	Action interface for adding nodes
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
-*/

#ifndef _H_AddNodeAction
#define _H_AddNodeAction

//---------------------------------- Includes ----------------------------------

#include "Action.h"
#include "NodeList.h"

class NodeImp;

//---------------------------------- Class -------------------------------------

class	AddNodeAction : public Action
{
public:
	AddNodeAction(NodeImp* theNode, NodeList* theList);

	virtual ~AddNodeAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    AddNodeAction(const AddNodeAction& a);
    AddNodeAction& operator=(const AddNodeAction& a);
        
	NodeList* itsList; 
	NodeImp* itsNode;
};

class SetRestraintsAction : public Action
{
public:
	SetRestraintsAction(const NodeEnumerator& theList, DlUInt32 restCode);
	~SetRestraintsAction();
	
	virtual void Redo();
	virtual void Undo();
private:
    SetRestraintsAction(const SetRestraintsAction& a);
    SetRestraintsAction& operator=(const SetRestraintsAction& a);

	NodeEnumerator 	itsList; 
	DlUInt32	itsRestCode;
	DlUInt8*	itsOldCodes;
};

class ToggleRestraintsAction : public Action
{
public:
	ToggleRestraintsAction(const NodeEnumerator& theList, int whichDof);
	virtual void Redo();
	virtual void Undo();
private:
    ToggleRestraintsAction(const ToggleRestraintsAction& a);
    ToggleRestraintsAction& operator=(const ToggleRestraintsAction& a);

	NodeEnumerator 	itsList; 
	int			itsDof;
};

//---------------------------------- Inlines -----------------------------------

#endif

//	eof

