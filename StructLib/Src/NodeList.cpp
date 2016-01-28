/*+
 *	File:		NodeList.cpp
 *
 *	Contains:	Frame Node List interface
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2003 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
-*/
/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//--------------------------------------- Includes ---------------------------------------

#include "DlPlatform.h"
#include "NodeList.h"
#include "NodeImp.h"
#include "Node.h"
#include "StrMessage.h"

#include "ElementList.h"

//--------------------------------------- Class ------------------------------------------
//
//	NodeSelector
//
//		add all nodes within r to the list.
//
class	NodeSelector 
{
public:
	NodeSelector(const WorldRect& r) 
		: _rect(r) {}

	void operator () (const NodeImp* n, DlInt32)
	{
		if (n->Hit(_rect))
			NodeList::GetList(_list)->Add(const_cast<NodeImp*>(n));
	}

	NodeEnumerator _list;
	const WorldRect& _rect;
};

//
//	MaxLoadCaseFinder
//
//		find the maximum load case.
//
//class MaxLoadCaseFinder {
//public:
//	MaxLoadCaseFinder(bool* lcs)
//		: _lcs(lcs) {}
//	
//	void operator () (const NodeImp* n, DlInt32)
//	{
//		n->UpdateLoadCase(_lcs);
//	}
//
//	bool* _lcs;
//};

//
//	NearestNodeFinder
//
//		find the node closest to p.
//
class	NearestNodeFinder 
{
public:
	NearestNodeFinder(const WorldPoint& p) 
		: _point(p)
		, _node(0)
		, _dist(-1) {}
	
	//	operator for foreach
	void operator() (const NodeImp* n, DlInt32) 
	{
		DlFloat64 d = WorldPoint::dist(n->GetCoords(), _point);
		if (d < _dist || _dist == -1)
		{
			_dist = d;
			_node = n;
		}
	}
	
	const WorldPoint& _point;
	const NodeImp* _node;
	DlFloat64	_dist;
};

//
//	LoadAssembler
//
//		assemble loads.
//
class LoadAssembler {
	public:
	LoadAssembler(std::valarray<DlFloat64>& rhs, LoadCase lc) 
		: _rhs(rhs)
		, _lc(lc) {}
	 
	void operator() (const NodeImp* n, DlInt32);
	
	std::valarray<DlFloat64>& _rhs;
	LoadCase _lc;
};

//
//	LoadFinder
//
//		find loads.
//
class LoadFinder {
	public:
	LoadFinder(LoadCase lc, const NodeLoadImp* ld) 
		: _lc(lc)
		, _load(ld) {}
	
	void operator() (const NodeImp* n, DlInt32) {
		if ((_lc == kAnyLoadCase && n->HasLoad(_load))
				|| (_lc != kAnyLoadCase && n->GetLoad(_lc) == _load))
			NodeList::GetList(_list)->Add(const_cast<NodeImp*>(n));
	}
	
	LoadCase _lc;
	const NodeLoadImp* _load;
	NodeEnumerator _list;
};

//
//	LoadChecker
//
//		check loads.
//
class LoadChecker {
public:
	LoadChecker(NodeLoadType theType)
	: _type(theType) {}
	
	void operator() (const NodeImp* n, DlInt32);
	
	NodeLoadType _type;
	NodeEnumerator _list;
};

//--------------------------------------- Methods ----------------------------------------

//----------------------------------------------------------------------------------------
//  NodeList::NodeList                                                        constructor
//
//      construct node list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
NodeList::NodeList() 
	: itsNumEquations(0)
	, itsNumReactions(0)
	, itsNumMatrixElems(0)
{
}

//----------------------------------------------------------------------------------------
//  NodeList::GetListID
//
//      return the id for the list.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
DlUInt32
NodeList::GetListID() const
{
	return NodeListID;
}

//----------------------------------------------------------------------------------------
//  NodeList::Select
//
//      return list of all nodes enclosed by r.
//
//  const WorldRect& r     -> the rect
//
//  returns NodeEnumerator <- list of enclosed nodes
//----------------------------------------------------------------------------------------
NodeEnumerator 
NodeList::Select(const WorldRect& r) const
{
	NodeSelector s(r);
	Foreach(s);
	return s._list;
}

//----------------------------------------------------------------------------------------
//  NodeList::Nearest
//
//      return the nearest node to p.
//
//  const WorldPoint& p    -> point
//
//  returns NodeImp*       <- nearest node to point
//----------------------------------------------------------------------------------------
NodeImp* 
NodeList::Nearest(const WorldPoint& p) const
{
	NearestNodeFinder f(p);
	Foreach(f);
	return const_cast<NodeImp*>(f._node);
}

//----------------------------------------------------------------------------------------
//  NodeList::SelectOne
//
//      select first node enclosed by r.
//
//  const WorldRect& r -> rectangle
//
//  returns NodeImp*   <- first node enclosed.
//----------------------------------------------------------------------------------------
NodeImp*
NodeList::SelectOne(const WorldRect& r) const
{
	Reset();
	
	while(HasMore()) {
		const NodeImp* n = Next();
		if (r.contains(n->GetCoords()))
			return const_cast<NodeImp*>(n);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
//  NodeList::Find
//
//      Find the node whose coordinate exactly match point p.
//
//  const WorldPoint& p    -> the point
//
//  returns NodeImp*       <- the node.
//----------------------------------------------------------------------------------------
NodeImp*
NodeList::Find(const WorldPoint& p) const
{
	Reset();
	
	while(HasMore()) {
		const NodeImp* n = Next();
		if (n->GetCoords() == p)
			return const_cast<NodeImp*>(n);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
//  NodeList::FindAttachedLoad
//
//      return any load attached to this node.
//
//  LoadCase lc            -> 
//  NodeLoad theLoad       -> 
//
//  returns NodeEnumerator <- 
//----------------------------------------------------------------------------------------
NodeEnumerator
NodeList::FindAttachedLoad(LoadCase lc, NodeLoad theLoad) const 
{
	LoadFinder finder(lc, theLoad);
	Foreach(finder);
	return finder._list;
}

//----------------------------------------------------------------------------------------
//  NodeList::Duplicates
//
//      check for nodes occupying the same location.
//
//  const NodeList* movedNodes -> nodes that were moved that could overlap
//  DlFloat64 tol              -> how close needed.
//
//  returns bool               <- true if duplicates were found.
//----------------------------------------------------------------------------------------
bool
NodeList::Duplicates(const NodeList* movedNodes, DlFloat64 tol) const 
{
	DlFloat64 mtol = tol * 1.414;
	DlInt32 totalLen = Length();

	if (movedNodes) {
		
		DlInt32 len = movedNodes->Length();
		for(DlInt32 j = 0; j < totalLen; ++j) {
			NodeImp* keepNode = ElementAt(j);
			if (!movedNodes->Contains(keepNode)) {
				const WorldPoint& c = keepNode->GetCoords();
			
				for (DlInt32 i = 0; i < len; ++i) {
					NodeImp* testNode = movedNodes->ElementAt(i);
					const WorldPoint& testCoords(testNode->GetCoords());

					if (testCoords.manhattan(c) < mtol) {
						if (testCoords.dist(c) < tol) {
							return true;
						}
					}
				}
			}
		}
	} else {
		for (DlInt32 i = 0; i < totalLen; i++) {
			const WorldPoint& c1 = ElementAt(i)->GetCoords();
			for (DlInt32 j = i + 1; j < totalLen; j++) {
				const WorldPoint& c2 = ElementAt(j)->GetCoords();
				if (c2.manhattan(c1) < mtol) {
					if (c2.dist(c1) < tol) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
//  NodeList::StitchMap
//
//      Create a Map of nodes to nodes at the same location.
//
//  const NodeList* movedNodes  -> the point
//  NodeCloneMap& nodeMap       -> the replacement map
//
//  returns NodeImp*       <- the node.
//----------------------------------------------------------------------------------------
void
NodeList::StitchMap(const NodeList* movedNodes, DlFloat64 tol, NodeCloneMap& nodeMap) const 
{
	//	we want to check each node in moved Nodes against all the node not in
	//	moved nodes.
	DlFloat64 mtol = tol * 1.414;
	DlInt32 totalLen = Length();
	
	if (movedNodes) {
		DlInt32 len = movedNodes->Length();
			
		for(DlInt32 j = 0; j < totalLen; ++j) {
			NodeImp* keepNode = ElementAt(j);
			if (!movedNodes->Contains(keepNode)) {
				const WorldPoint& c = keepNode->GetCoords();
			
				for (DlInt32 i = 0; i < len; ++i) {
					NodeImp* testNode = movedNodes->ElementAt(i);
					if (nodeMap.find(testNode) == nodeMap.end()) {
						
						const WorldPoint& testCoords(testNode->GetCoords());
				
						if (testCoords.manhattan(c) < mtol) {
							if (testCoords.dist(c) < tol) {
								nodeMap[testNode] = keepNode;
							}
						}
					}
				}
			}
		} 
	} else {
		
		for (DlInt32 i = 0; i < totalLen; i++) {
			NodeImp* n1 = ElementAt(i);
			const WorldPoint& c1 = n1->GetCoords();
			for (DlInt32 j = i + 1; j < totalLen; j++) {
				NodeImp* n2 = ElementAt(j);
				const WorldPoint& c2 = n2->GetCoords();
				if (c2.manhattan(c1) < mtol) {
					if (c2.dist(c1) < tol) {
						nodeMap[n2] = n1;
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  NodeList::Clone
//
//      clone this list for copy.
//
//  NodeList* newNodes     -> the new node list.
//  NodeLoadList* newLoads -> the node loads.
//  NodeCloneMap& nodeMap  -> map created between old and new nodes for elements.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeList::Clone(NodeList* newNodes, NodeLoadList* newLoads, NodeCloneMap& nodeMap) const
{
	// clone the nodes
	Reset();
	
	while(HasMore()) {
		const NodeImp* nextNode = Next();
		NodeImp* copyNode = NEW NodeImp(*nextNode);
		newNodes->Add(copyNode);
				
		for (LoadCase lc = 0; lc < nextNode->LoadCount(); lc++) {
			NodeLoadImp* ll = nextNode->GetLoad(lc);
			if (ll) {
				NodeLoadImp* load = ll->IsCombo() ? nullptr : newLoads->FindMatchingLoad(ll);
				if (load == nullptr) {
					load = NEW NodeLoadImp(*ll);
					newLoads->Add(load);
				}
				
				copyNode->AssignLoad(lc, load);
			}
		}
		
		nodeMap[nextNode] = copyNode;
	}
	
}

//----------------------------------------------------------------------------------------
//  NodeList::ShallowClone
//
//      clone this list for copy.
//
//  NodeList* newNodes     ->
//  NodeCloneMap& nodeMap  ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeList::ShallowClone(NodeList* newNodes, NodeCloneMap& nodeMap) const
{
	Reset();

	while(HasMore()) {
		const NodeImp* nextNode = Next();
		NodeImp* copyNode = NEW NodeImp(*nextNode);
		newNodes->Add(copyNode);

		for (LoadCase lc = 0; lc < nextNode->LoadCount(); lc++) {
			NodeLoadImp* ll = nextNode->GetLoad(lc);
			if (ll) {
				copyNode->AssignLoad(lc, ll);
			}
		}

		nodeMap[nextNode] = copyNode;
	}
}

//----------------------------------------------------------------------------------------
//  NodeList::SetStarts
//
//      Set the starting matrix positions for each column.
//
//  const ElementList* elems   -> the element list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeList::SetStarts(const ElementList* elems)
{
	itsStarts.resize(itsNumEquations + 1, itsNumEquations + 1);
	elems->SetStarts(itsStarts, itsNumEquations + 1);
	
	//	The connect list has the minimum dof for each column. 
	//	Figure the off-diagonal distance to assemble the column start indices.
	itsNumMatrixElems = 0;
	for (DlInt32 i = 1; i <= itsNumEquations; i++) {
		DlInt32	itemp = itsStarts[i-1];
		itsStarts[i-1] = itsNumMatrixElems + 1;
		//	this adds the equation - the minimum dof to the number of equations
		//	i is the distance from the top to the diagonal
		//	itemp is the number of unused element in the column
		itsNumMatrixElems += i - itemp + 1;
	}

	itsStarts[itsNumEquations+1-1] = itsNumMatrixElems+1;
}

//----------------------------------------------------------------------------------------
//  NodeList::PrepareToAnalyze
//
//      Assign equation numbers and compute the columns starts.
//
//  const ElementList* elems   -> the list of elements.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeList::PrepareToAnalyze(const ElementList* elems)
{
	//	first compute all the equation numbers
	Reset();
	
	itsNumEquations = 0;
	itsNumReactions = 0;
	
	while (HasMore()) {
		Next()->SetEquationNumbers(&itsNumEquations, &itsNumReactions);
	}
	
	//	here, if desired we can minimize the profile by calling
	//  elems->GetConnectivity(connect, starts)
	//	GPSRenum...
	
	//	then map the current equation numbers to the new ones.
	
	//	compute the number of equations and set the columns
	SetStarts(elems);
}

////----------------------------------------------------------------------------------------
////  NodeList::GetMaxLoadCase
////
////      return the maximum load case for node loads.
////
////  LoadCase currMax   -> 
////
////  returns LoadCase   <- 
////----------------------------------------------------------------------------------------
//void
//NodeList::UpdateLoadCase(bool* isAssigned) const
//{
//	MaxLoadCaseFinder m(isAssigned);
//	Foreach(m);
//}

//----------------------------------------------------------------------------------------
//  NodeList::AssembleLoads
//
//      assemble the node loads.
//
//  s  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeList::AssembleLoads(std::valarray<DlFloat64>& rhs, LoadCase lc) {
	LoadAssembler assembler(rhs, lc);
	Foreach(assembler);
}

//----------------------------------------------------------------------------------------
//  NodeList::FindAttachedLoad
//
//      return any load attached to this node.
//
//  LoadCase lc            ->
//  NodeLoad theLoad       ->
//
//  returns NodeEnumerator <-
//----------------------------------------------------------------------------------------
NodeEnumerator
NodeList::CheckLoadEquations(NodeLoadType theType) const
{
	LoadChecker finder(theType);
	Foreach(finder);
	return finder._list;
}

//----------------------------------------------------------------------------------------
//  NodeList::ComputeBounds
//
//      Compute the enclosing bounds for the structure.
//
//  returns WorldRect  <- 
//----------------------------------------------------------------------------------------
WorldRect 
NodeList::ComputeBounds() const
{
	WorldRect	wr;

	if (HasMore()) {
		wr.bottomLeft() = {Next()->GetCoords()};
		wr.topRight() = {Next()->GetCoords()};

		while (HasMore()) {
			wr.addBounds(Next()->GetCoords());
		}
	}

	return wr;
}

//----------------------------------------------------------------------------------------
//  LoadAssembler::operator()
//
//      invoke load assembler function.
//
//  const NodeImp* n   -> the node
//  DlInt32            -> ignored offset.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
LoadAssembler::operator() (const NodeImp* n, DlInt32) 
{
	NodeLoadImp* ld = n->GetLoad(_lc);
	if (ld != 0) {
		for (int i = 0; i < DOF_PER_NODE; i++) {
			DlInt32 eq = n->GetEquationNumber(i);
			if (eq > 0) {
				if (ld->GetType(i) == NodeLoadIsForce) {
					_rhs[eq-1] += ld->GetValue(i);
				}
			}
		}
	}
}

void
LoadChecker::operator() (const NodeImp* n, DlInt32) {
	
	for (LoadCase lc = 0; lc < n->LoadCount(); lc++) {
		NodeLoad l = n->GetLoad(lc);
		if (l) {
			// if there is a load, check each dof
			DlFloat32 vals[DOF_PER_NODE];
			NodeLoadType types[DOF_PER_NODE];
			l.GetValues(vals, types);
			
			for (DlInt32 dof = 0; dof < DOF_PER_NODE; dof++) {
				// if the type matches our type and there is a value
				//	then make sure there either is, or is not an equation there.
				// For forces, there should be an equation, and for disp there should not.
				if (types[dof] == _type && vals[dof] != 0) {
					DlInt32 eq = n->GetEquationNumber(dof);
					if ((eq > 0 && _type != NodeLoadIsForce)
						|| (eq < 0 && _type != NodeLoadIsDisp)) {
						NodeList::GetList(_list)->Add(const_cast<NodeImp*>(n));
					}
				}
			}
		}
	}
}


// eof