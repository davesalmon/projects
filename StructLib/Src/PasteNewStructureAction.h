//
//  PasteNewStructureAction.h
//  StructLib
//
//	Paste the given nodes and elements into the structure.
//
//  Created by David Salmon on 4/2/14.
//
//	Copyright (c) 2004, David C. Salmon. All rights reserved.
//
//

#ifndef __StructLib__PasteNewStructureAction__
#define __StructLib__PasteNewStructureAction__

#include "Action.h"
#include "frame_data.h"

class	PasteNewStructureAction : public Action
{
public:
	PasteNewStructureAction(const frame_data* src, frame_data* dest, const WorldPoint& ofs);

	virtual ~PasteNewStructureAction();

	virtual void Redo();
	virtual void Undo();

private:
	PasteNewStructureAction(const PasteNewStructureAction& a);
    PasteNewStructureAction& operator=(const PasteNewStructureAction& a);

	ElementEnumerator 		_pastedElements;
	NodeEnumerator 			_pastedNodes;
	
	DlInt32				_propsCount;
	DlInt32				_nodeLoadCount;
	DlInt32				_elemLoadCount;
	
	PropertyEnumerator		_addedProps;
	NodeLoadEnumerator		_addedNodeLoads;
	ElementLoadEnumerator	_addedElementLoads;

	//	const FrameStructure*	_source;
	frame_data* 			_dest;
};

#endif /* defined(__StructLib__PasteNewStructureAction__) */
