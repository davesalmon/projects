/*+
 *	File:		MoveNodeAction.h
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

#ifndef _H_MoveNodeAction
#define _H_MoveNodeAction

//---------------------------------- Includes ----------------------------------

#include "Action.h"
#include "WorldPoint.h"
#include "NodeList.h"

//---------------------------------- Class -------------------------------------

class	MoveNodeAction : public Action
{
public:
	MoveNodeAction(const NodeEnumerator& nodes, const WorldPoint& offset);

	virtual ~MoveNodeAction();
	
	virtual void Redo();
	virtual void Undo();

private:

	void moveBy(const WorldPoint& offset);

	NodeEnumerator 	_list;
	WorldPoint 		_offset;
};

//---------------------------------- Inlines -----------------------------------

#endif

//	eof

