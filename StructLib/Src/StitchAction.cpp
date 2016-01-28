//+
//	StitchAction.h
//
//	Stitch the given nodes into the existing structure.
//
//	Copyright (c) 2004, David C. Salmon. All rights reserved.
//-

#include "DlPlatform.h"
#include "StitchAction.h"


StitchAction::StitchAction(frame_data* structure, const NodeList* stitchNodes, DlFloat64 tol, bool stitchAll) 
	: Action("Remove Duplicate Nodes")
	, _dest(structure) 
{
	if (tol == 0)
		tol = kDefaultTol;
	if (stitchAll) {
		stitchNodes->StitchMap(NULL, tol, _dupNodes);
	} else {
		_dest->Nodes().StitchMap(stitchNodes, tol, _dupNodes);
	}
}

StitchAction::~StitchAction() {
	if (PerformIsUndo()) {
		NodeCloneMapIter i = _dupNodes.begin();
		for ( ; i != _dupNodes.end(); i++) {
			delete i->first;
		}
	}
}
	
void 
StitchAction::Redo() 
{
	// swap out the duplicate nodes
	_dest->Elements().MapNodes(_dupNodes, _changedElements, _startNodes, _endNodes);

	// remove the nodes.
	NodeCloneMapCIter i = _dupNodes.begin();
	for (; i != _dupNodes.end(); i++) {
		NodeImp* n = const_cast<NodeImp*>(i->first);
#if DlDebugging
		printf("stitch removing node %d\n", n->_id);
#endif
		_dest->Nodes().Remove(n);
	}

	// find the elements that may be exact covers of the existing.
	_dupElements = _dest->Elements().FindDuplicates(_changedElements.GetList());

	ElementList& elems = _dest->Elements();
	for (const ElementImp* i : _dupElements) {
#if DlDebugging
		printf("stitch removing element %d\n", elems.IndexOf(i));
#endif
		elems.Remove(const_cast<ElementImp*>(i));
	}
}

void
StitchAction::Undo() 
{
	// restore elements that were removed.
	ElementList& elems = _dest->Elements();
	for (const ElementImp* e : _dupElements) {
		elems.Add(const_cast<ElementImp*>(e));
	}

	// and the nodes that were removed.
	NodeCloneMapCIter i = _dupNodes.begin();
	for (; i != _dupNodes.end(); i++) {
		NodeImp* n = const_cast<NodeImp*>(i->first);
#if DlDebugging
		printf("adding node %d\n", n->_id);
#endif
		_dest->Nodes().Add(n);
	}

	// restore the old nodes to the elements.
	ElementList* theList = ElementList::GetList(_changedElements);
	for (DlUInt32 j = 0; j < theList->Length(); j++) {

		ElementImp* elem = theList->ElementAt(j);
		const NodeImp* start = _startNodes[j];
		const NodeImp* end = _endNodes[j];

#if DlDebugging
		printf("swapping start node %d for node %d\n", elem->StartNode()->_id, start->_id);
		printf("swapping end node %d for node %d\n", elem->EndNode()->_id, end->_id);
#endif
		elem->SetStartNode(start);
		elem->SetEndNode(end);
	}
	
	_changedElements = ElementEnumerator();
	_startNodes = ElementList::NodeVector();
	_endNodes = ElementList::NodeVector();
}
 
