//+
//	PasteSameStructureAction
//
//	Paste the given nodes and elements into the same structure.
//
//	Copyright (c) 2004, David C. Salmon. All rights reserved.
//-

#ifndef _H_PasteSameStructureAction
#define _H_PasteSameStructureAction

#include "Action.h"
#include "frame_data.h"
//#include "StitchAction.h"

class StitchAction;

class	PasteSameStructureAction : public Action
{
public:
	PasteSameStructureAction(const frame_data* src, frame_data* dest, const WorldPoint& ofs);

	virtual ~PasteSameStructureAction();
	
	virtual void Redo();
	virtual void Undo();

private:
	PasteSameStructureAction(const PasteSameStructureAction& a);
    PasteSameStructureAction& operator=(const PasteSameStructureAction& a);

	ElementEnumerator 		_pastedElements;
	NodeEnumerator 			_pastedNodes;

	frame_data* 			_dest;
};



#endif // _H_PasteSameStructureAction
