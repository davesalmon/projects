//+
//	StitchAction.h
//
//	Stitch the given nodes into the existing structure.
//
//	Copyright (c) 2004, David C. Salmon. All rights reserved.
//-

#ifndef _H_StitchAction
#define _H_StitchAction

#include "Action.h"
#include "frame_data.h"

const DlFloat64 kDefaultTol = 1.0e-6;

class	StitchAction : public Action
{
public:
	StitchAction(frame_data* structure, const NodeList* stitchNodes, DlFloat64 tol, bool stitchAll);

	virtual ~StitchAction();
	
	virtual void Redo();
	virtual void Undo();

private:
    StitchAction(const StitchAction& a);
    StitchAction& operator=(const StitchAction& a);

	ElementEnumerator 		_changedElements;
	ElementList::NodeVector	_startNodes;
	ElementList::NodeVector	_endNodes;
	NodeCloneMap 			_dupNodes;
	ElementList::ElementSet	_dupElements;

	frame_data* 			_dest;
};



#endif // _H_StitchAction
 
