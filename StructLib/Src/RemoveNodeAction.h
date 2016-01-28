/*+
 *	File:		RemoveNodeAction.h
 *
 *	Contains:	Action interface for removing nodes
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
-*/

#ifndef _H_RemoveNodeAction
#define _H_RemoveNodeAction

//---------------------------------- Includes ----------------------------------

#include "Action.h"
#include "NodeEnumerator.h"
#include "ElementEnumerator.h"

class NodeImp;
class NodeList;
class ElementList;

//---------------------------------- Class -------------------------------------

class	RemoveNodeAction : public Action
{
public:
	RemoveNodeAction(const NodeEnumerator& remNodes, const ElementEnumerator& remElems,
		NodeList * theNodeList,
		ElementList * theElemList);

	virtual ~RemoveNodeAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    RemoveNodeAction(const RemoveNodeAction& a);
    RemoveNodeAction& operator=(const RemoveNodeAction& a);

	NodeList * itsNodeList;
	ElementList * itsElemList;

	NodeEnumerator itsRemovedNodes;
	ElementEnumerator itsRemovedElements;
};

//---------------------------------- Inlines -----------------------------------

#endif

//	eof

