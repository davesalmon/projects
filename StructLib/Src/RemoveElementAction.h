/*+
 *
 *  RemoveElementAction.h
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  Class to track the removal of an element from the structure.
 *
 -*/

#ifndef __StructLib__RemoveElementAction__
#define __StructLib__RemoveElementAction__

//---------------------------------- Includes ----------------------------------

#include "Action.h"
#include <vector>

class ElementImp;
class ElementList;
class NodeList;

//---------------------------------- Class -------------------------------------

class	RemoveElementAction : public Action
{
public:
	RemoveElementAction(ElementImp* theElem, ElementList* theList);
	
	RemoveElementAction(const RemoveElementAction& a) = delete;
    RemoveElementAction& operator=(const RemoveElementAction&a) = delete;

	virtual ~RemoveElementAction();
	
	virtual void Redo();
	virtual void Undo();
	
private:
	
	ElementList* itsList;
	ElementImp* itsElem;
};

//---------------------------------- Inlines -----------------------------------


#endif /* defined(__StructLib__RemoveElementAction__) */
