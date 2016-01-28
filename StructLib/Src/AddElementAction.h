/*+
 *	File:		AddElementAction.h
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

#ifndef _H_AddElementAction
#define _H_AddElementAction

//---------------------------------- Includes ----------------------------------

#include "Action.h"
#include <vector>

class ElementImp;
class ElementList;
class NodeList;

//---------------------------------- Class -------------------------------------

class	AddElementAction : public Action
{
public:
	AddElementAction(ElementImp* theElem, ElementList* theList, 
		Action* node1Action);

	AddElementAction(ElementImp* e1, ElementImp* e2, ElementList* theList,
					 Action* rmElem, Action* node1Action);
	
	virtual ~AddElementAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    AddElementAction(const AddElementAction& a);
    AddElementAction& operator=(const AddElementAction&a);
        
	ElementList* itsList;
	std::vector<ElementImp*> itsElems;
	std::vector<Action*> itsOtherActions;
};

//---------------------------------- Inlines -----------------------------------

#endif

//	eof

