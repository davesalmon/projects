/*+
 *	File:		AddNodeLoadAction.h
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

#ifndef _H_AddNodeLoadAction
#define _H_AddNodeLoadAction

//---------------------------------- Includes ----------------------------------

#include "Action.h"
#include "NodeList.h"

class NodeLoadImp;
class NodeLoadList;
class frame_data;

//---------------------------------- Class -------------------------------------

/* ----------------------------------------------------------
 *	AddNodeLoadAction
 *
 *		handle redo and undo for adding a load.
 *
 */
class	AddNodeLoadAction : public Action
{
public:
	AddNodeLoadAction(NodeLoadImp* theElem, NodeLoadList* theList);

	virtual ~AddNodeLoadAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    AddNodeLoadAction(const AddNodeLoadAction& a);
    AddNodeLoadAction& operator=(const AddNodeLoadAction& a);

	NodeLoadList* itsList;
	NodeLoadImp* itsElem;
};

/* ----------------------------------------------------------
 *	RemoveNodeLoadAction
 *
 *		handle redo and undo for removing a load.
 *
 */
class	RemoveNodeLoadAction : public Action
{
public:
	RemoveNodeLoadAction(NodeLoadImp* theElem, NodeLoadList* theList);
	
	virtual ~RemoveNodeLoadAction();
	
	virtual void Redo();
	virtual void Undo();
	
private:
	RemoveNodeLoadAction(const RemoveNodeLoadAction& a);
	RemoveNodeLoadAction& operator=(const RemoveNodeLoadAction& a);
	
	NodeLoadList* itsList;
	NodeLoadImp* itsElem;
};

/* ----------------------------------------------------------
 *	AssignNodeLoadAction
 *
 *		handle redo and undo for assigning a load.
 *
 */
class AssignNodeLoadAction : public Action
{
public:
	AssignNodeLoadAction(NodeLoadImp* theLoad, const NodeEnumerator& theList, LoadCase lc);

	virtual ~AssignNodeLoadAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    AssignNodeLoadAction(const AssignNodeLoadAction& a);
    AssignNodeLoadAction& operator=(const AssignNodeLoadAction& a);

	NodeEnumerator itsList;
	NodeLoadImp* itsLoad;
	LoadCase itsLoadCase;
	NodeLoadImp** savedLoads;
};

/* ----------------------------------------------------------
 *	ChangeNodeLoadAction
 *
 *		handle redo and undo for changing a load.
 *
 */
class ChangeNodeLoadAction : public Action {
public:
	ChangeNodeLoadAction(NodeLoadImp* theLoad, DlInt32 dof, const char* theValue, NodeLoadType theType,
						 frame_data* data);
	virtual void Redo();
	virtual void Undo();
private:
	ChangeNodeLoadAction(const ChangeNodeLoadAction& a);
    ChangeNodeLoadAction& operator=(const ChangeNodeLoadAction& a);
	
    NodeLoadImp*	itsLoad;
	DlFloat64		itsOldValue;
	NodeLoadType	itsOldType;
	DlInt32			itsDof;
	bool			itsIsValue;
	frame_data*		itsData;
};

//---------------------------------- Inlines -----------------------------------

#endif

//	eof

