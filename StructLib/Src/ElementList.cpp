/*+
 *	File:		ElementList.cpp
 *
 *	Contains:	Frame Element List implementation
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
#include "ElementList.h"
#include "NodeList.h"
#include "NodeImp.h"
#include "LoadCaseResults.h"
#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "DlException.h"
#include "StrMessage.h"
#include "ElemLoadList.h"
#include "PropertyList.h"
#include "ElementFactory.h"

using namespace std;
using namespace DlArray;

//--------------------------------------- Class ------------------------------------------

//----------------------------------------------------------------------------------------
// class GetAttachedNodes
//
//      class to assemble list of elements attached to the nodes in the list.
//
//----------------------------------------------------------------------------------------
class GetAttachedNodes
{
public:
	GetAttachedNodes(const NodeList* list)
		: _nodeList(list)
	{
	}
			
	void operator() (const ElementImp* elem, DlInt32)
	{
		if (_nodeList->Contains(elem->StartNode()) ||
			_nodeList->Contains(elem->EndNode()))
			_elemList.Add(const_cast<ElementImp *>(elem));
	}
	
	const NodeList* _nodeList;
	ElementEnumerator _elemList;
private:
    GetAttachedNodes(const GetAttachedNodes& a);
    GetAttachedNodes& operator=(const GetAttachedNodes& a);
};

//----------------------------------------------------------------------------------------
// class GetSelection
//
//      get the selected elements, based on the selected nodes.
//
//----------------------------------------------------------------------------------------
class GetSelection
{
public:
	GetSelection(const NodeList* list)
		: _nodeList(list)
	{
	}

	void operator() (const ElementImp* elem, DlInt32)
	{
		if (_nodeList->Contains(elem->StartNode()) && _nodeList->Contains(elem->EndNode()))
			_elemList.Add(const_cast<ElementImp *>(elem));
	}

	const NodeList* _nodeList;
	ElementEnumerator _elemList;
private:
    GetSelection(const GetSelection& a);
    GetSelection& operator=(const GetSelection& a);
};

//----------------------------------------------------------------------------------------
// class ElementSelector
//
//      get the selected elements that are enclosed by the rectangle.
//
//----------------------------------------------------------------------------------------
class ElementSelector 
{
public:
	ElementSelector(const WorldRect & r) : _rect(r) {
	}

	void operator () (const ElementImp* e, DlInt32)
	{
		// if both the nodes are in the rect
		if ((e->StartNode()->Hit(_rect) && e->EndNode()->Hit(_rect)) || e->Hit(_rect)) {
			ElementList::GetList(_list)->Add(const_cast<ElementImp*>(e));
		}
	}

	ElementEnumerator _list;
	const WorldRect& _rect;
private:
    ElementSelector(const ElementSelector& a);
    ElementSelector& operator=(const ElementSelector& a);
};

//----------------------------------------------------------------------------------------
// class NodeMapper
//
//      get the list of elements and and corresponding nodes that changed due to the
//		mapping of new nodes to old nodes (NodeCloneMap).
//
//	basically construct three lists:.
//	1) modified element.
//	2) old start node for that element.
//	3) old end node for that element.
//
//	The indexes into these lists are all the same.
//
//----------------------------------------------------------------------------------------
class NodeMapper 
{
public:
	NodeMapper(const NodeCloneMap& theNodeMap) 
		: _map(theNodeMap) 
	{
	}

	void operator () (ElementImp* e, DlInt32) 
	{
//		ElementImp* ee = const_cast<ElementImp*>(e);
		// look up the new nodes that are at the same location
		// as the existing ones.
		NodeCloneMapCIter i = _map.find(e->StartNode());
		NodeCloneMapCIter j = _map.find(e->EndNode());
		if (i != _map.end() || j != _map.end()) {
			// add this element as modified
			_list.Add(e);

			// and add the old nodes to the start and end lists.
			_startNodes.push_back(e->StartNode());
			_endNodes.push_back(e->EndNode());

			// and update the elements
			if (i != _map.end())
				e->SetStartNode(i->second);
			if (j != _map.end())
				e->SetEndNode(j->second);
		}
	}
	
	ElementEnumerator _list;
	ElementList::NodeVector _startNodes;
	ElementList::NodeVector _endNodes;

	const NodeCloneMap& _map;
private:
    NodeMapper(const NodeMapper& a);
    NodeMapper& operator=(const NodeMapper& a);
};

//----------------------------------------------------------------------------------------
// class ElementDupFinder
//
//	Build a set of the elements in incoming that are duplicates of the existing
//	elements.
//----------------------------------------------------------------------------------------
class ElementDupFinder
{
public:
	ElementDupFinder(const ElementList* incoming)
		: _list(incoming) {}

	void operator () (const ElementImp* e, DlInt32)
	{
		const NodeImp* start = e->StartNode();
		const NodeImp* end = e->EndNode();

//#if DlDebugging
//		printf("checking %d -> %d\n", start->_id, end->_id);
//#endif

		while (_list->HasMore()) {
			const ElementImp* test = _list->Next();
			// skip over same element.
			if (test != e) {

				const NodeImp* tstart = test->StartNode();
				const NodeImp* tend = test->EndNode();

				if ((start==tstart && end==tend) || (start==tend && end==tstart)) {
					elemSet.insert(test);
//#if DlDebugging
//					printf("\tadd\n");
//#endif
				}
			}
		}

		_list->Reset();
	}

	ElementList::ElementSet elemSet;

private:
	const ElementList* _list;
};

////----------------------------------------------------------------------------------------
////	MaxLoadCaseFinder
////
////		find the maximum load case, based on element loads.
////
////----------------------------------------------------------------------------------------
//class MaxLoadCaseFinder {
//public:
//	MaxLoadCaseFinder(bool* lcs)
//		: _lcs(lcs) {}
//	
//	void operator () (const ElementImp* e, DlInt32)
//	{
//		e->UpdateLoadCase(_lcs);
//	}
//
//	bool* _lcs;
//private:
//    MaxLoadCaseFinder(const MaxLoadCaseFinder& a);
//    MaxLoadCaseFinder& operator=(const MaxLoadCaseFinder& a);
//};

//----------------------------------------------------------------------------------------
// class CountAttachedNodes
//
//      class to count elements attached to the nodes in the list.
//
//----------------------------------------------------------------------------------------
class CountAttachedNodes
{
public:
	CountAttachedNodes(const NodeImp* n)
		: itsNode(n)
		, itsCount(0)
	{
	}
	
	void operator() (const ElementImp* elem, DlInt32)
	{
		if (itsNode == elem->StartNode() || itsNode == elem->EndNode())
			itsCount++;
	}
	
	const NodeImp*	itsNode;
	DlInt32 		itsCount;
private:
    CountAttachedNodes(const CountAttachedNodes& a);
    CountAttachedNodes& operator=(const CountAttachedNodes& a);
};

//----------------------------------------------------------------------------------------
// class GetElemConnect
//
//      class to build connectivity list for the current structure.
//
//----------------------------------------------------------------------------------------
class GetElemConnect
{
	typedef vector<DlInt32> dofList;
	typedef vector<DlInt32>::iterator dofIter;
public:
	GetElemConnect(DlInt32 numDof) : _connect(numDof), _inserted(0) {}
	
	void insert(DlInt32 dof, DlInt32 connected);
	void assemble(const ElementImp* elem);
	void operator() (const ElementImp* elem, DlInt32)
	{
		assemble(elem);
	}
	
	vector<dofList>	_connect;
	DlInt32 _inserted;
private:
    GetElemConnect(const GetElemConnect& a);
    GetElemConnect& operator=(const GetElemConnect& a);
};

//----------------------------------------------------------------------------------------
// class DoAssembleStiffness
//
//      class to assemble the stiffness matrix.
//
//----------------------------------------------------------------------------------------
class DoAssembleStiffness
{
public:
	DoAssembleStiffness(valarray<DlFloat64>& stiffness, const valarray<DlInt32>& maxa)
		: _stiffness(stiffness), _maxa(maxa) {}

	void assembleStiffness(const ElementImp* elem);
	void operator() (const ElementImp* elem, DlInt32)
	{
		assembleStiffness(elem);
	}

	valarray<DlFloat64>& _stiffness;
	const valarray<DlInt32>& _maxa;
private:
    DoAssembleStiffness(const DoAssembleStiffness& a);
    DoAssembleStiffness& operator=(const DoAssembleStiffness& a);
};

//----------------------------------------------------------------------------------------
// class DoAssembleLoads
//
//      class to assemble the loads.
//
//----------------------------------------------------------------------------------------
class DoAssembleLoads
{
public:
	DoAssembleLoads(valarray<DlFloat64>& rhs, LoadCase lc)
		: _rhs(rhs), _lc(lc) {}

	void assembleLoads(const ElementImp* elem);
	void operator() (const ElementImp* elem, DlInt32)
	{
		assembleLoads(elem);
	}

	valarray<DlFloat64>& 	_rhs;
	LoadCase				_lc;
    
private:
    DoAssembleLoads(const DoAssembleLoads& a);
    DoAssembleLoads& operator=(const DoAssembleLoads& a);
};

//----------------------------------------------------------------------------------------
// class DoRecoverForces
//
//      class to recover forces.
//
//----------------------------------------------------------------------------------------
class DoRecoverForces
{
public:
	DoRecoverForces(LoadCaseResults& results)
		: _results(results)
	{
		_results.InitRatios();
	}

	void recoverForces(const ElementImp* elem, DlInt32 index);
	void operator() (const ElementImp* elem, DlInt32 index)
	{
		recoverForces(elem, index);
	}

	LoadCaseResults&	_results;
private:
    DoRecoverForces(const DoRecoverForces& a);
    DoRecoverForces& operator=(const DoRecoverForces& a);
};

//----------------------------------------------------------------------------------------
// class DoGetAttachedProperties
//
//      get the list of all Property objects that are attached to some element in the 
//		list. Used for Clone
//
//----------------------------------------------------------------------------------------
class DoGetAttachedProperties 
{
public:
	DoGetAttachedProperties()
		: _propList(NEW PropertyList) {}

	~DoGetAttachedProperties() {
		delete _propList;
	}

	void operator() (const ElementImp* elem, DlInt32 index)
	{
		_propList->Add(elem->GetProperty());
	}

	PropertyList*	_propList;
private:
    DoGetAttachedProperties(const DoGetAttachedProperties& a);
    DoGetAttachedProperties& operator=(const DoGetAttachedProperties& a);
};


//--------------------------------------- Methods ----------------------------------------


//----------------------------------------------------------------------------------------
//  ElementList::ElementList                                                  constructor
//
//      Default constructor for element list. 
//
//----------------------------------------------------------------------------------------
ElementList::ElementList()
{
}

//----------------------------------------------------------------------------------------
//  ElementList::GetListID
//
//      return the identifier for element lists. Used to identify the list when sending
//      list changed messages.
//
//  returns DlUInt32   <- the identifier for element lists.
//----------------------------------------------------------------------------------------
DlUInt32
ElementList::GetListID() const
{
	return ElementListID;
}

//----------------------------------------------------------------------------------------
//  ElementList::Attached
//
//      Return a list of elements attached to any of the nodes specified. 
//
//  const NodeList* nodes  -> The list of nodes
//
//  returns ElementList*   <- The list of connected elements.
//----------------------------------------------------------------------------------------
ElementEnumerator 
ElementList::Attached(const NodeList* nodes) const
{
	GetAttachedNodes	a(nodes);
	Foreach(a);
	
	return a._elemList;
}


//----------------------------------------------------------------------------------------
//  ElementList::Select
//
//      return the list of selected elements determined by the joint selection.
//
//  const NodeList* nodes      -> the selected nodes
//
//  returns ElementEnumerator  <- the attached elements.
//----------------------------------------------------------------------------------------
ElementEnumerator
ElementList::Select(const NodeList* nodes) const
{
	GetSelection	a(nodes);
	Foreach(a);
	return a._elemList;
}

//----------------------------------------------------------------------------------------
//  ElementList::Select
//
//      return list of all elements enclosed by r.
//
//  const WorldRect& r     -> the rect
//
//  returns NodeEnumerator <- list of enclosed nodes
//----------------------------------------------------------------------------------------
ElementEnumerator 
ElementList::Select(const WorldRect& r) const
{
	ElementSelector s(r);
	Foreach(s);
	return s._list;
}

//----------------------------------------------------------------------------------------
//  ElementList::SelectOne
//
//      select a single element within a small rectangle.
//
//  const WorldRect & r    -> the rectangle around the desired point.
//  const WorldRect & r    <-> the location along the element.
//
//  returns ElementImp*    <- the element selected or NULL
//----------------------------------------------------------------------------------------
ElementImp*
ElementList::SelectOne(const WorldRect & r, DlFloat64& loc) const
{
	WorldPoint c(r.center());
	DlFloat64 tol(r.width()/2.0);

	Reset();
	
	while(HasMore()) {
		const ElementImp* e = Next();
		WorldPoint p = c.transform(e->StartNode()->GetCoords(), e->EndNode()->GetCoords());
		if (p.x() > 0 && p.x() < e->Length() && fabs(p.y()) < tol)
		{
			loc = p.x() / e->Length();
			return const_cast<ElementImp*>(e);
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//  ElementList::CountAttached
//
//      Return the number of elements attached to the specified node.
//
//  const NodeImp* n   -> The node
//
//  returns DlInt32    <- the number of attached elements.
//----------------------------------------------------------------------------------------
DlInt32
ElementList::CountAttached(const NodeImp* n) const
{
	CountAttachedNodes c(n);
	Foreach(c);
	return c.itsCount;
}

//----------------------------------------------------------------------------------------
//  ElementList::IsDuplicate
//
//      Return true if an element is already attached to both of the specified nodes.
//
//  const NodeImp* n1  -> The first node
//  const NodeImp* n2  -> The second node
//
//  returns bool    	<- true if duplicate.
//----------------------------------------------------------------------------------------
bool
ElementList::IsDuplicate(const NodeImp* n1, const NodeImp* n2) const
{
	Reset();
	while(HasMore()) {
		const ElementImp* e = Next();

		const NodeImp* startNode = e->StartNode();
		if (startNode == n1 || startNode == n2) {
			const NodeImp* endNode = e->EndNode();
			if (endNode == n1 || endNode == n2)
				return true;
		}
	}
	return false;
}

//
//	PropFinder
//
//		find properties.
//
class PropFinder {
public:
	PropFinder(const PropertyImp* prop)
	: _prop(prop) {}
	
	void operator() (const ElementImp* e, DlInt32) {
		if (e->GetProperty() == _prop)
			ElementList::GetList(_list)->Add(const_cast<ElementImp*>(e));
	}
	
	const PropertyImp* _prop;
	ElementEnumerator _list;
};

ElementEnumerator
ElementList::FindAttachedProperty(const Property& prop) const
{
	PropFinder finder(prop);
	Foreach(finder);
	return finder._list;
}

//
//	LoadFinder
//
//		find loads.
//
class LoadFinder {
public:
	LoadFinder(LoadCase lc, const ElemLoadImp* ld)
	: _lc(lc)
	, _load(ld) {}
	
	void operator() (const ElementImp* e, DlInt32) {
		if ((_lc == kAnyLoadCase && e->HasLoad(_load))
			|| (_lc != kAnyLoadCase && e->GetLoad(_lc) == _load))
			ElementList::GetList(_list)->Add(const_cast<ElementImp*>(e));
	}
	
	LoadCase _lc;
	const ElemLoadImp* _load;
	ElementEnumerator _list;
};

ElementEnumerator
ElementList::FindAttachedLoad(LoadCase lc, const ElementLoad& l) const
{
	LoadFinder finder(lc, l);
	Foreach(finder);
	return finder._list;
}


typedef std::map<const ElemLoadImp*, ElemLoadImp*> ElemLoadMap;
typedef std::map<const ElemLoadImp*, ElemLoadImp*>::const_iterator ElemLoadMapIter;

typedef std::map<const PropertyImp*, PropertyImp*> PropertyMap;
typedef std::map<const PropertyImp*, PropertyImp*>::const_iterator PropertyMapIter;

//----------------------------------------------------------------------------------------
//  lookupNode                                                                     static
//
//      find a node in the map. If the node is not in the map, clone the node, add it to
//      nodes, and return the new node. Otherwise, return the node we found.
//
//  const NodeCloneMap& nodeMap    -> the map of old to new nodes.
//  const NodeImp* theImp          -> the target nodes.
//  NodeList* nodes                -> the list of created nodes.
//
//  returns NodeImp*               <- the new node to use.
//----------------------------------------------------------------------------------------
static 
NodeImp*
lookupNode(const NodeCloneMap& nodeMap, const NodeImp* theImp, NodeList* nodes) {
	NodeCloneMapCIter i = nodeMap.find(theImp);
	if (i == nodeMap.end()) {
		NodeImp* newImp = NEW NodeImp(theImp->GetCoords());
		nodes->Add(newImp);
		return newImp;
	}
	return i->second;
}

//----------------------------------------------------------------------------------------
//  lookupProp                                                                     static
//
//      find the property or fail with exception.
//
//  const PropertyMap& propMap -> the map of old to new properties.
//  const PropertyImp* theImp  -> the target property.
//
//  returns PropertyImp*       <- the new property.
//----------------------------------------------------------------------------------------
static 
PropertyImp*
lookupProp(const PropertyMap& propMap, const PropertyImp* theImp) {
	PropertyMapIter i = propMap.find(theImp);
	if (i == propMap.end())
		throw DlException("Failed to find property.");
	return i->second;
}

//----------------------------------------------------------------------------------------
//  lookupLoad                                                                     static
//
//      find the load or fail with exception.
//
//  const ElemLoadMap& elemMap -> the map of old to new loads.
//  const ElemLoadImp* theImp  -> the target load.
//
//  returns ElemLoadImp*       <- the new load.
//----------------------------------------------------------------------------------------
static 
ElemLoadImp*
lookupLoad(const ElemLoadMap& elemMap, const ElemLoadImp* theImp) {
	ElemLoadMapIter i = elemMap.find(theImp);
	if (i == elemMap.end())
		throw DlException("Failed to find property.");
	return i->second;
}


//----------------------------------------------------------------------------------------
//  ElementList::Clone
//
//      Clone the current list. nodeMap contains the mapping between existing nodes and
//      cloned nodes. newList, newProps, newLoads, and newNodes are returned as the cloned
//      data.
//
//  ElementList* newList           -> the cloned elements
//  const NodeCloneMap& nodeMap    -> the map of old to new nodes.
//  PropertyList* newProps         -> the cloned properties.
//  ElemLoadList* newLoads         -> the cloned loads.
//  NodeList* newNodes             -> the cloned nodes.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::Clone(ElementList* newList, const NodeCloneMap& nodeMap, 
						PropertyList* newProps, ElemLoadList* newLoads,
						NodeList* newNodes) const
{
	//	clone the elements
	Reset();

	while(HasMore()) {
		const ElementImp* elem = Next();
		NodeImp* startNode = lookupNode(nodeMap, elem->StartNode(), newNodes);
		NodeImp* endNode = lookupNode(nodeMap, elem->EndNode(), newNodes);
		
		// find or create a property for this element
		PropertyImp* prop = newProps->FindMatchingProperty(elem->GetProperty());
		if (prop == nullptr) {
			prop = NEW PropertyImp(*elem->GetProperty());
			newProps->Add(prop);
		}
		
		ElementImp* copyElem = ElementFactory::sElementFactory.CreateElement(
													elem->GetType(), startNode, endNode, prop);
		newList->Add(copyElem);
		
		for (LoadCase lc = 0; lc < elem->LoadCount(); lc++) {
			ElemLoadImp* ll = elem->GetLoad(lc);
			if (ll) {
				
				// find or create an load for this element
				ElemLoadImp* load = ll->IsCombo() ? nullptr : newLoads->FindMatchingLoad(ll);
				if (load == nullptr) {
					load = NEW ElemLoadImp(*ll);
					newLoads->Add(load);
				}
				
				copyElem->AssignLoad(lc, load);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  ElementList::ShallowClone
//
//      Clone the current list. nodeMap contains the mapping between existing nodes and
//      cloned nodes. Only the elements are copied (along with updated node references)
//		while any load or property data refers back to the original structure.
//
//  ElementList* newList           -> the cloned elements
//  const NodeCloneMap& nodeMap    -> the map of old to new nodes.
//  NodeList* newNodes             -> the cloned nodes.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::ShallowClone(ElementList* newList, const NodeCloneMap& nodeMap,
				   NodeList* newNodes) const
{
	Reset();

	while(HasMore()) {
		const ElementImp* elem = Next();
		NodeImp* startNode = lookupNode(nodeMap, elem->StartNode(), newNodes);
		NodeImp* endNode = lookupNode(nodeMap, elem->EndNode(), newNodes);
//		PropertyImp* prop = lookupProp(thePropMap, elem->GetProperty());

		ElementImp* copyElem = ElementFactory::sElementFactory.CreateElement(elem->GetType(),
														 startNode, endNode, elem->GetProperty());
		newList->Add(copyElem);

		for (LoadCase lc = 0; lc < elem->LoadCount(); lc++) {
			ElemLoadImp* ll = elem->GetLoad(lc);
			if (ll) {
				copyElem->AssignLoad(lc, ll);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  ElementList::MapNodes
//
//      determine the list of elements affected by a clone operation.
//
//  const NodeCloneMap& nodeMap        -> the map of old to new nodes.
//  ElementEnumerator& changedElems    -> the elements affected by this mapping.
//  NodeEnumerator& startNodes         -> the starting nodes affected.
//  NodeEnumerator& endNodes           -> the ending nodes affected.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::MapNodes(const NodeCloneMap& nodeMap, ElementEnumerator& changedElems, 
				NodeVector& startNodes, NodeVector& endNodes)
{
	NodeMapper mapper(nodeMap);
	Foreach(mapper);
	
	changedElems = mapper._list;
	startNodes = std::move(mapper._startNodes);
	endNodes = std::move(mapper._endNodes);
}

ElementList::ElementSet
ElementList::FindDuplicates(const ElementList* incomingElements) const
{
	ElementDupFinder dupFinder(incomingElements);
	Foreach(dupFinder);

	return std::move(dupFinder.elemSet);
}

////----------------------------------------------------------------------------------------
////  ElementList::GetMaxLoadCase
////
////      return the maximum used load case, based on element loads.
////
////  LoadCase currMax   -> the current maximum load case.
////
////  returns LoadCase   <- the new maximum
////----------------------------------------------------------------------------------------
//void
//ElementList::UpdateLoadCase(bool* lcs) const
//{
////	MaxLoadCaseFinder m(lcs);
////	Foreach(m);
//}

//----------------------------------------------------------------------------------------
//  ElementList::GetConnectivity
//
//      create a vector mapping degrees of freedom connected by each element. Also, the
//      size of the connection is returned in starts.
//
//  DlInt32 numEqs                     -> the total number of equations
//  std::valarray<DlInt32>& connect    <-> a list of which dofs are connected.
//  std::valarray<DlInt32>& starts     <-> the start positions in connect for each dof.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::GetConnectivity(DlInt32 numEqs, std::valarray<DlInt32>& connect, 
			std::valarray<DlInt32>& starts) const
{
	//	create a vector of connected degrees of freedom. The vector contains an entry
	//	for each degree of freedom which contains a sorted list of the attached dofs. 
	GetElemConnect c(numEqs);
	Foreach(c);
	
	//	now use the vector to assemble the connectivity list. This list is basically
	//	a flattened version of c._connect. starts contains the starting index for each
	//	dof.
	connect.resize(c._inserted);
	starts.resize(numEqs+1);
	
	starts[0] = 1;
	DlInt32 count = 0;
	for (DlInt32 i = 0; i < numEqs; ++i) {
		vector<DlInt32>& v(c._connect[i]);
		DlInt32 n = v.size();
		for (DlInt32 j = 0; j < n; ++j, ++count) {
			connect[count] = v[j];
		}
		starts[i+1] = count + 1;
	}
}

//----------------------------------------------------------------------------------------
//  ElementList::AssembleStiffness
//
//      Assemble the stiffness matrix for the structure.
//
//  std::valarray<DlFloat64>& mat      <-> the matrix to build.
//  const std::valarray<DlInt32>& maxa -> the offsets to the diagonal.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::AssembleStiffness(std::valarray<DlFloat64>& mat, 
					const std::valarray<DlInt32>& maxa) const
{
	DoAssembleStiffness a(mat, maxa);
	Foreach(a);
}

//----------------------------------------------------------------------------------------
//  ElementList::AssembleLoads
//
//      Assemble the load vector.
//
//  std::valarray<DlFloat64>& rhs  <-> the vector of loads.
//  LoadCase lc                    -> the current load case.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::AssembleLoads(std::valarray<DlFloat64>& rhs, LoadCase lc) const
{
	DoAssembleLoads a(rhs, lc);
	Foreach(a);
}

//----------------------------------------------------------------------------------------
//  ElementList::RecoverResults
//
//      Recover the reactions and forces at StartNode within each element.
//
//  LoadCase lc                -> the current load case
//  LoadCaseResults& results   <-> the results.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::RecoverResults(LoadCaseResults& results) const
{
	DoRecoverForces a(results);
	Foreach(a);
}

//----------------------------------------------------------------------------------------
//  ElementList::SetStarts
//
//      determine the minimum dof attached to each node. For each element, look at the
//		connected nodes and set the minimum dof if less than that set by some other
//		element.
//
//  std::valarray<DlInt32>& starts  -> the starts vector, initialized to maxDof
//  DlInt32 maxDof					-> the number of equations + 1.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::SetStarts(std::valarray<DlInt32>& starts, DlInt32 maxDof) const
{
	Reset();
	while (HasMore()) {
		const ElementImp* e = Next();
		
		const NodeImp* n1 = e->StartNode();
		const NodeImp* n2 = e->EndNode();
		
		DlInt32 minDof = n1->MinEquation(maxDof);
		DlInt32 minDofN2 = n2->MinEquation(maxDof);
		
		if (minDofN2 > 0 && minDofN2 < minDof)
			minDof = minDofN2;
		
		for (int i = 0; i < DOF_PER_NODE; i++)
		{
			if (minDof > 0)
			{
				DlInt32 eq1 = n1->GetEquationNumber(i);
				if (eq1 > 0)
				{
					if (minDof < starts[eq1-1])
						starts[eq1-1] = minDof;
				}
				
				DlInt32 eq2 = n2->GetEquationNumber(i);
				if (eq2 > 0)
				{
					if (minDof < starts[eq2-1])
						starts[eq2-1] = minDof;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  ElementList::Read
//
//      read in the list.
//
//  StrInputStream& inp    -> the input stream
//  DlInt32 count          -> the number of elements to read
//  const frame_data& data -> the data object
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::Read(StrInputStream& inp, DlInt32 count, const frame_data& data)
{
	reserve(count);
	for (DlInt32 i = 0; i < count; i++) {
		DlString elemType = inp.GetString();
		ElementImp* elem = ElementFactory::sElementFactory.CreateElement(elemType, inp, data);
		if (elem == NULL)
			throw DlException("Element of type %s could not be created.", (const char*)elemType);
		Add(elem);
		//	and construct the element
		
	}
}

//----------------------------------------------------------------------------------------
//  ElementList::Write
//
//      write out the list.
//
//  StrOutputStream& out   -> the output stream
//  const frame_data& data -> the data object.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementList::Write(StrOutputStream& out, const frame_data& data) const
{
	Reset();
	while (HasMore()) {
		const ElementImp* elem = Next();
		out.PutString(elem->GetType());
		elem->Write(out, data);
	}
}


//----------------------------------------------------------------------------------------
//  GetElemConnect::insert
//
//      insert a dof into the connectivity list.
//
//  DlInt32 dof        -> the first dof
//  DlInt32 connected  -> the connected dof
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
GetElemConnect::insert(DlInt32 dof, DlInt32 connected)
{
	// the target is always the smaller equation.
	if (dof > connected) {
		DlInt32 tmp = dof;
		dof = connected;
		connected = tmp;
	}
	
	// look it up in the list.
	dofList& l = _connect[dof-1];
	dofIter e = l.end();
	dofIter i = lower_bound(l.begin(), e, connected);
	
	// if not in the list, insert it
	if (i == e || *i != connected) {
		l.insert(i, connected);
		_inserted++;
	}
}

//----------------------------------------------------------------------------------------
//  GetElemConnect::assemble
//
//      assemble the element connectivity list.
//
//  const ElementImp* elem -> the element to assemble
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
GetElemConnect::assemble(const ElementImp* elem)
{
	DlInt32 dof[2*DOF_PER_NODE];
	elem->GetDOF(dof);
	
	for (DlInt32 i = 0; i < 2*DOF_PER_NODE; ++i) {
		if (dof[i] > 0) {
			for (DlInt32 j = i; j < 2*DOF_PER_NODE; ++j) {
				if (dof[j] > 0)
					insert(dof[i], dof[j]);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  DoAssembleStiffness::assembleStiffness
//
//      assemble the stiffness for this element.
//
//  const ElementImp* elem -> the element
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
DoAssembleStiffness::assembleStiffness(const ElementImp* elem)
{
	DlInt32 dof[2*DOF_PER_NODE];
	DlMatrix<DlFloat64> m(2*DOF_PER_NODE,2*DOF_PER_NODE);
	elem->Stiffness(m, false);
	
	DlOneBasedConstIter<DlInt32>	maxa(_maxa);
	DlOneBasedIter<DlFloat64>		a(_stiffness);
	
	elem->GetDOF(dof);
	
	for (DlInt32 i = 0; i < 2*DOF_PER_NODE; ++i) {
		DlInt32 r = dof[i];
		if (r > 0) {
			for (DlInt32 j = i; j < 2*DOF_PER_NODE; j++) {
				DlInt32 c = dof[j];
				if (c > 0) {
					DlFloat64 kVal = m[i+1][j+1];
					if (dof[i] < dof[j]) {
						r = dof[i];
						c = dof[j];
					} else {
						r = dof[j];
						c = dof[i];
					}
					//	now the diagonal of the column (c) is maxa[c]
					//	and the offset (c - r)
					DlInt32 loc = maxa[c] + c - r;
					a[loc] += kVal;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  DoAssembleLoads::assembleLoads
//
//      assemble the load vector for this element.
//
//  const ElementImp* elem -> the element
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
DoAssembleLoads::assembleLoads(const ElementImp* elem)
{
	DlFloat64 fixEndForce[2*DOF_PER_NODE];
	DlFloat64 disp[2*DOF_PER_NODE];
	bool hasFEForce = elem->FixedEndForces(fixEndForce, _lc);
	bool hasDisp = elem->Settlement(_lc, disp);
	
	if (hasFEForce || hasDisp) {
		DlInt32 dof[2*DOF_PER_NODE];
		elem->GetDOF(dof);
				
		DlOneBasedIter<DlFloat64>	rhs(_rhs);
		
		//	sum in the forces if any
		if (hasFEForce) {
			for (DlInt32 i = 0; i < 2*DOF_PER_NODE; ++i) {
				if (dof[i] > 0)
					rhs[dof[i]] -= fixEndForce[i];
			}
		}
		
		//	and compute the FEM due to settlement if any
		if (hasDisp) {
			
			hasDisp = false;
			//	dont apply displacements to dofs that are not
			//	fixed, since this just cancels out.
			for (DlInt32 i = 0; i < 2*DOF_PER_NODE; i++) {
				if (dof[i] > 0)
					disp[i] = 0;
				else if (disp[i] != 0)
					hasDisp |= true;
			}
			
			if (hasDisp) {
				DlMatrix<DlFloat64> m(2*DOF_PER_NODE,2*DOF_PER_NODE);
				elem->Stiffness(m, true);
				DlInt32* dofPtr = dof;
				//	compute the fixed end forces 
				for (DlInt32 i = 1; i <= 2*DOF_PER_NODE; ++i, ++dofPtr) {
					if (*dofPtr > 0) {
						
						DlFloat* dPtr = disp;
						DlFloat64 sum = 0;
						DlSliceIter<DlFloat64> row(m.row(i));
						
						for (DlInt32 j = 1; j <= 2*DOF_PER_NODE; ++j, ++dPtr) {
							sum += row[j] * (*dPtr);
						}
						
						rhs[*dofPtr] -= sum;
					}
				}
			}
		}
	}
	
}

//----------------------------------------------------------------------------------------
//  DoRecoverForces::recoverForces
//
//      recover the forces for this element.
//
//  const ElementImp* elem -> the element
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DoRecoverForces::recoverForces(const ElementImp* elem, DlInt32 index)
{
	DlInt32 dof[2*DOF_PER_NODE];
	DlMatrix<DlFloat64> m(2*DOF_PER_NODE,2*DOF_PER_NODE);
	DlFloat64 fixEndForce[2*DOF_PER_NODE];
	DlFloat64 displacements[2*DOF_PER_NODE];

	elem->GetDOF(dof);

	_results.GetDisplacement(elem->StartNode(), displacements);
	_results.GetDisplacement(elem->EndNode(), displacements+DOF_PER_NODE);

	elem->Stiffness(m, true);
	if (!elem->FixedEndForces(fixEndForce, _results.GetLoadCase()))
		std::memset(fixEndForce, 0, sizeof(fixEndForce));
		
	DlFloat64* outPtr = fixEndForce;
	for (DlInt32 i = 1; i <= 2*DOF_PER_NODE; ++i, ++outPtr) {
		
		DlFloat64 sum = *outPtr;
		DlFloat64* inPtr = displacements;
		DlSliceIter<DlFloat64> row(m.row(i));
		for (DlInt32 j = 1; j <= 2*DOF_PER_NODE; ++j, ++inPtr)
			sum += row[j] * (*inPtr);
		
		 *outPtr = DlChop(sum, kFloatTolerance);
		 
		 if (dof[i-1] < 0)
		 	_results.SetReaction(-dof[i-1], *outPtr);
	}
	
	//	ok now we have in residuals, the node forces due to the displacement.
    //	convert to element local forces    
	_results.SetEndForces(index, elem->GetLocalForce(fixEndForce));	

	//	compute the scale factors
	_results.SetDispRatio(elem->MaxDisplacementPerLength(index, _results));
	_results.SetShearRatio(elem->MaxForcePerLength(1, index, _results));
	_results.SetAxialRatio(elem->MaxForcePerLength(0, index, _results));
	_results.SetMomentRatio(elem->MaxForcePerLength(2, index, _results));
}

//	eof
