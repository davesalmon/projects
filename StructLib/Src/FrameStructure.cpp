/*+
 *	File:		FrameStructure.cpp
 *
 *	Contains:	Interface for 2D frame structure
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

//+--------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "FrameStructure.h"
#include "frame_data.h"
#include "Node.h"
#include "NodeImp.h"

#include "AddNodeAction.h"
#include "RemoveNodeAction.h"

#include "AddElementAction.h"
#include "RemoveElementAction.h"

#include "AddElemLoadAction.h"
#include "AddNodeLoadAction.h"
#include "MoveNodeAction.h"
#include "PasteSameStructureAction.h"
#include "PasteNewStructureAction.h"
#include "StitchAction.h"
#include "PropertyAction.h"
#include "ElementFactory.h"
#include "PropertyFactory.h"
#include "LoadCaseAction.h"

#include "PropertyImp.h"

//+--------------------------------- Classes -----------------------------------

class CallNodeFunction
{
public:
	CallNodeFunction(FrameStructure::ForEachNodeFunc f, void* ctx) : _f(f), _ctx(ctx) {}
	
	void operator() (NodeImp* i, DlInt32 index) 
	{
		_f(Node(i), index, _ctx);
	}
	
	FrameStructure::ForEachNodeFunc _f;
	void*			_ctx;
};

class CallElementFunction
{
public:
	CallElementFunction(FrameStructure::ForEachElementFunc f, void* ctx) : _f(f), _ctx(ctx) {}
	
	void operator() (ElementImp* i, DlInt32 index) 
	{
		_f(Element(i), index, _ctx);
	}
	
	FrameStructure::ForEachElementFunc _f;
	void*			_ctx;
};

#pragma mark -
#pragma mark ====== Implementation ======
#pragma mark -

/* +--------------------------------- Methods ----------------------------------- */

//----------------------------------------------------------------------------------------
//  FrameStructure::FrameStructure                                            constructor
//
//      construct a new FrameStructure.
//
//  const char* defaultPropName    -> the default property title.
//
//  returns nothing
//----------------------------------------------------------------------------------------
FrameStructure::FrameStructure(const char* defaultPropName)
	: itsData(NEW frame_data(defaultPropName))
{
}

//----------------------------------------------------------------------------------------
//  FrameStructure::~FrameStructure                                            destructor
//
//      destruct a frame structure.
//
//  returns nothing
//----------------------------------------------------------------------------------------
FrameStructure::~FrameStructure()
{
	delete itsData;
}

//----------------------------------------------------------------------------------------
//  FrameStructure::FrameStructure                                            constructor
//
//      copy a frame structure from node and element selections.
//
//  const NodeEnumerator& nodes    -> the selected nodes.
//  const ElementEnumerator& elems -> the selected elements.
//
//  returns nothing
//----------------------------------------------------------------------------------------
FrameStructure::FrameStructure(const NodeEnumerator& nodes,
							   const ElementEnumerator& elems, bool deepCopy)
	: itsData(NEW frame_data(NodeList::GetList(nodes)
							 , ElementList::GetList(elems), deepCopy))
{
}

//----------------------------------------------------------------------------------------
//  FrameStructure::DebugPrint
//
//      print some values.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::DebugPrint() const
{
#if DlDebugging
	printf("elements = %d\n", itsData->Elements().Length());
	printf("nodes = %d\n", itsData->Nodes().Length());
	printf("nodeLoads = %d\n", itsData->NodeLoads().Length());
	printf("elementLoads = %d\n", itsData->ElementLoads().Length());
	printf("properties = %d\n", itsData->Properties().Length());
	
#endif
}

#pragma mark -
#pragma mark ====== Listener ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::AddListener
//
//      add a listener to the structure. The listener is attached to the element, node
//		node load, element load and properties lists.
//
//  DlListener* l  -> the listener to add.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::AddListener(DlListener* l)
{
	itsData->AddListener(l);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::RemoveListener
//
//      remove a listener from the list elements.
//
//  DlListener* l  -> the listener to remove.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::RemoveListener(DlListener* l)
{
	itsData->RemoveListener(l);
}

#pragma mark -
#pragma mark ====== Paste ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::PasteSame
//
//      Create an action to paste the given structure into this structure. This method
//		assumes that pasteMe was copied from the same structure as this one, so a
//		shallow copy is used in the paste. This keeps the load and property assignments
//		the same in the pasted elements.
//
//  const FrameStructure* pasteMe  -> the structure to paste.
//  const WorldPoint& ofs          -> the offset to paste at.
//
//  returns Action*                <- the action to perform the paste.
//----------------------------------------------------------------------------------------
Action*
FrameStructure::PasteSame(const FrameStructure* pasteMe, const WorldPoint& ofs)
{
	return NEW PasteSameStructureAction(pasteMe->itsData, itsData, ofs);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::PasteNew
//
//      Create an action to paste the given structure into this structure. This method
//		assumes that pasteMe was copied from a different structure than this one. This
//		clones all the load and property types that are associated with pasteMe.
//
//  const FrameStructure* pasteMe  -> the structure to paste.
//  const WorldPoint& ofs          -> the offset to paste at.
//
//  returns Action*                <- the action to perform the paste.
//----------------------------------------------------------------------------------------
Action*
FrameStructure::PasteNew(const FrameStructure* pasteMe, const WorldPoint& ofs)
{
	return NEW PasteNewStructureAction(pasteMe->itsData, itsData, ofs);
}

#pragma mark -
#pragma mark ====== Nodes ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::AddNode
//
//      add a new node to the structure.
//
//  const WorldPoint& pt   -> the loaction of the new node.
//  Node& addedNode        <-> the added node object.
//
//  returns Action*        <- an action to add the node.
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::AddNode(const WorldPoint& pt, Node& addedNode)
{
	NodeImp* theNode = NEW NodeImp(pt);
	addedNode = Node(theNode);
	return NEW AddNodeAction(theNode, &itsData->Nodes());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::RemoveNodes
//
//      remove the specified nodes and elements from the structure. This also removes
//		all elements attached to the nodes, even if they are not in the element list.
//
//  const NodeEnumerator& nodes    -> the nodes.
//  const ElementEnumerator& elems -> the elements.
//
//  returns Action*                <- an action to perform the task.
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::RemoveNodes(const NodeEnumerator& nodes, const ElementEnumerator& elems)
{
	return NEW RemoveNodeAction(nodes, elems, &itsData->Nodes(), &itsData->Elements());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetNodeIndex
//
//      return the offset of the node in the list.
//
//  Node n             -> the node
//
//  returns DlInt32    <- the index in the node list.
//----------------------------------------------------------------------------------------
DlInt32 
FrameStructure::GetNodeIndex(Node n) const 
{
	return itsData->NodeToIndex(n);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::DoForEachNode
//
//      perform the specified task for each node.
//
//  ForEachNodeFunc f  -> function to apply
//  void* ctx          -> context for function.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
FrameStructure::DoForEachNode(ForEachNodeFunc f, void* ctx)
{
	CallNodeFunction c(f, ctx);
	itsData->Nodes().Foreach(c);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetNodes
//
//      return the nodes.
//
//  returns NodeEnumerator <- the node list.
//----------------------------------------------------------------------------------------
NodeEnumerator
FrameStructure::GetNodes() const
{
	//	return NodeEnumerator(NEW NodeList(itsData->Nodes()));
	return NodeEnumerator(&itsData->Nodes());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::DuplicateNodes
//
//      return true if any of moved nodes occupy the same location as existing nodes.
//
//  const NodeEnumerator& movedNodes   -> the list of moved nodes.
//	DlFloat64 tol					   -> the tolerance.
//
//  returns bool                       <- true if duplicated.
//----------------------------------------------------------------------------------------
bool
FrameStructure::DuplicateNodes(const NodeEnumerator& movedNodes, DlFloat64 tol) const
{
	return itsData->Nodes().Duplicates(NodeList::GetList(movedNodes), tol);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::StitchNodes
//
//      stitch duplicate nodes together.
//
//  const NodeEnumerator& nodes    -> the nodes
//	DlFloat64 tol			       -> the tolerance.
//  bool stitchAll                 -> true to stitch all nodes in the structure.
//
//  returns Action*                <- action to perform stitch.
//----------------------------------------------------------------------------------------
Action*
FrameStructure::StitchNodes(const NodeEnumerator& nodes, DlFloat64 tol, bool stitchAll)
{
	return NEW StitchAction(itsData, NodeList::GetList(nodes), tol, stitchAll);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::MoveNodes
//
//      move the specified node set.
//
//  const NodeEnumerator& nodes    -> the nodes to move
//  const WorldPoint& offset       -> the offset to move them
//
//  returns Action*                <- action to move em.
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::MoveNodes(const NodeEnumerator& nodes, const WorldPoint& offset)
{
	return new MoveNodeAction(nodes, offset);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::FindNode
//
//      return the node whose coordinates exactly match pt.
//
//  const WorldPoint& pt   -> find a node at the specified point.
//
//  returns Node           <- the node.
//----------------------------------------------------------------------------------------
Node
FrameStructure::FindNode(const WorldPoint& pt) const
{
	return itsData->Nodes().Find(pt);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SelectOneNode
//
//      return the first node found inside the bounds of rect.
//
//  const WorldRect& rect  -> where to look
//
//  returns Node           <- first node found.
//----------------------------------------------------------------------------------------
Node
FrameStructure::SelectOneNode(const WorldRect& rect) const
{
	return itsData->Nodes().SelectOne(rect);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SelectNodes
//
//      return all the nodes within rect.
//
//  const WorldRect& rect  -> the rect.
//
//  returns NodeEnumerator <- the nodes.
//----------------------------------------------------------------------------------------
NodeEnumerator
FrameStructure::SelectNodes(const WorldRect& rect) const
{
	return itsData->Nodes().Select(rect);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetNode
//
//      return the node at the specified index in the list.
//
//  DlInt32 index  -> the index.
//
//  returns Node   <- the node.
//----------------------------------------------------------------------------------------
Node
FrameStructure::GetNode(DlInt32 index) const
{
	return itsData->Nodes().ElementAt(index);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::CountNodes
//
//      return the number of nodes.
//
//  returns DlUInt32   <- the number of nodes.
//----------------------------------------------------------------------------------------
DlUInt32
FrameStructure::CountNodes() const
{
	return itsData->Nodes().Length();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SetRestraints
//
//      set the restraint code for nodes. The restraint is a bit vector where 1 represents
//		restrained and 0 represents free in each dof.
//
//  const NodeEnumerator& nodes    -> the nodes.
//  DlUInt32 restCode              -> the restraint code.
//
//  returns Action*                <- 
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::SetRestraints(const NodeEnumerator& nodes, DlUInt32 restCode)
{
	return NEW SetRestraintsAction(nodes, restCode);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::ToggleRestraints
//
//      toggle the restraints for the specified nodes in the given dof.
//
//  const NodeEnumerator& nodes    -> the nodes.
//  DlInt32 whichDof               -> the dof.
//
//  returns Action*                <- and action to toggle restraints.
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::ToggleRestraints(const NodeEnumerator& nodes, DlInt32 whichDof)
{
	return NEW ToggleRestraintsAction(nodes, whichDof);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetBounds
//
//      return the bounds for this structure.
//
//  returns WorldRect     <- the bounds.
//----------------------------------------------------------------------------------------
WorldRect
FrameStructure::GetBounds() const
{
	return itsData->Nodes().ComputeBounds();
}

#pragma mark -
#pragma mark ====== Elements ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::GetActiveElementType
//
//      return the current elment type.
//
//  returns const char*    <- the name of the active element.
//----------------------------------------------------------------------------------------
const char* 
FrameStructure::GetActiveElementType() const 
{
	return itsData->GetActiveElementType();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SetActiveElementType
//
//      specify the active element type.
//
//  const char* theType    -> set the active element type.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
FrameStructure::SetActiveElementType(const char* theType) 
{
	itsData->SetActiveElementType(theType);	
}

//----------------------------------------------------------------------------------------
//  FrameStructure::AddElement
//
//      add a new element.
//
//  const Node& node1      -> the start node.
//  const Node& node2      -> the end node.
//  const char* typeName   -> the element type name.
//  Element& added         <-> the element object.
//
//  returns Action*        <- action to add the element.
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::AddElement(const Node& node1, const Node& node2, 
					const char* typeName, Element& added)
{
	ElementImp* e = ElementFactory::sElementFactory.CreateElement(typeName, node1, node2, itsData->GetActiveProperty());
	added = Element(e);
	return NEW AddElementAction(e, &itsData->Elements(), 0);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::AddElement
//
//      add an element, creating the end node.
//
//  const Node& n1         -> the start node
//  const WorldPoint& p2   -> the location of the attached node to create.
//  const char* typeName   -> the element type.
//  Element& added         <-> the element object.
//
//  returns Action*        <- action to add element.
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::AddElement(const Node& n1, const WorldPoint& p2, 
						const char* typeName, Element& added)
{
	_DlAssert(!n1.Empty());
	NodeImp* n2 = NEW NodeImp(p2);
	Action* n2Action = NEW AddNodeAction(n2, &itsData->Nodes());

	ElementImp* e = ElementFactory::sElementFactory.CreateElement(typeName, n1, n2, itsData->GetActiveProperty());
	added = e;
	return NEW AddElementAction(e, &itsData->Elements(), n2Action);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetElementIndex
//
//      return the index of the element.
//
//  Element e          -> the element
//
//  returns DlInt32    <- its index in the list.
//----------------------------------------------------------------------------------------
DlInt32
FrameStructure::GetElementIndex(Element e) const 
{
	return itsData->ElemToIndex(e);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::DoForEachElement
//
//      perform task for each element.
//
//  ForEachElementFunc f   -> the function
//  void* ctx              -> the context
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
FrameStructure::DoForEachElement(ForEachElementFunc f, void* ctx)
{
	CallElementFunction c(f, ctx);
	itsData->Elements().Foreach(c);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetElements
//
//      return the elements.
//
//  returns ElementEnumerator  <- the elements
//----------------------------------------------------------------------------------------
ElementEnumerator
FrameStructure::GetElements() const
{
	//	return ElementEnumerator(NEW ElementList(itsData->Elements()));
	return ElementEnumerator(&itsData->Elements());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetElement
//
//      return the element at the specified index in the list.
//
//  DlInt32 index      -> the index
//
//  returns Element    <- the element at that index
//----------------------------------------------------------------------------------------
Element
FrameStructure::GetElement(DlInt32 index) const
{
	return itsData->Elements().ElementAt(index);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::CountElements
//
//      count the elements.
//
//  returns DlUInt32   <- the total element count.
//----------------------------------------------------------------------------------------
DlUInt32
FrameStructure::CountElements() const
{
	return itsData->Elements().Length();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetElementTypes
//
//      return list of element types in use.
//
//  returns StringEnumerator   <- list of element types available.
//----------------------------------------------------------------------------------------
StringEnumerator
FrameStructure::GetElementTypes() const 
{
	return ElementFactory::sElementFactory.GetElementTypes();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SelectOneElement
//
//      select an element that passes through the specified rectangle.
//
//  const WorldRect& rect  -> the location
//
//  returns Element        <- the element
//----------------------------------------------------------------------------------------
Element
FrameStructure::SelectOneElement(const WorldRect& rect, DlFloat64& loc) const
{
	return itsData->Elements().SelectOne(rect, loc);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SelectElements
//
//      select all elements attached to the specified nodes.
//
//  const NodeEnumerator& nodes    -> 
//
//  returns ElementEnumerator      <- 
//----------------------------------------------------------------------------------------
ElementEnumerator 
FrameStructure::SelectElements(const NodeEnumerator& nodes) const
{
	return itsData->Elements().Select(NodeList::GetList(nodes));
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SelectElements
//
//      select all elements passing through rect.
//
//  const WorldRect& r         -> the rectangle.
//
//  returns ElementEnumerator  <- the elements.
//----------------------------------------------------------------------------------------
ElementEnumerator 
FrameStructure::SelectElements(const WorldRect& r) const 
{
	return itsData->Elements().Select(r);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::AttachedElements
//
//      return the elements attached to the node.
//
//  Node n                     -> the node
//
//  returns ElementEnumerator  <- the attached elements.
//----------------------------------------------------------------------------------------
ElementEnumerator
FrameStructure::AttachedElements(Node n) const
{
	return ((NodeImp*)n)->FindAttached(itsData->Elements());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::DuplicateElement
//
//      return true if there is an element that contains the specified nodal points.
//
//  Node n1        -> start or end node.
//  Node n2        -> start or end node.
//
//  returns bool   <- true if there is an element between the specified points.
//----------------------------------------------------------------------------------------
bool 
FrameStructure::DuplicateElement(Node n1, Node n2) const 
{
	return itsData->Elements().IsDuplicate(n1, n2);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SplitElement
//
//      split the specified element by inserting added node in the middle.
//
//  const Node& addedNode   -> the added node.
//  Element& hitElem        -> the element to split.
//
//  returns Action*			<- an action to perform the split.
//----------------------------------------------------------------------------------------
//Action*
//FrameStructure::SplitElement(const Node& addedNode, Element& hitElem)
//{
//	ElementImp* e1 = ElementFactory::sElementFactory.CreateElement(
//				hitElem.GetType(), hitElem.StartNode(), addedNode, itsData->GetActiveProperty());
//	
//	ElementImp* e2 = ElementFactory::sElementFactory.CreateElement(
//				hitElem.GetType(), addedNode, hitElem.EndNode(), itsData->GetActiveProperty());
//
//	return NEW AddElementAction(e1, e2, &itsData->Elements(),
//								NEW RemoveElementAction(hitElem, &itsData->Elements()),
//								nullptr);
//}

//----------------------------------------------------------------------------------------
//  FrameStructure::SplitElement
//
//      split the specified element by creating a new node at loc and inserting it in the
//		middle.
//
//  const WorldPoint& loc   -> the new node location.
//  Element& hitElem        -> the element to split.
//	Node& newNode			<-> the new node.
//
//  returns Action*			<- an action to perform the split.
//----------------------------------------------------------------------------------------
Action*
FrameStructure::SplitElement(Element& hitElem, Node& newNode, DlFloat64 eLoc)
{
	WorldPoint start = hitElem.StartNode().GetCoords();
	WorldPoint end = hitElem.EndNode().GetCoords();
	
	WorldPoint loc(start.x() + (end.x() - start.x()) * eLoc, start.y() + (end.y() - start.y()) * eLoc);
	
	NodeImp* addedNode = NEW NodeImp(loc);
	Action* n2Action = NEW AddNodeAction(addedNode, &itsData->Nodes());
	
	newNode = addedNode;
	
	ElementImp* e1 = ElementFactory::sElementFactory.CreateElement(
				hitElem.GetType(), hitElem.StartNode(), addedNode, itsData->GetActiveProperty());
	
	ElementImp* e2 = ElementFactory::sElementFactory.CreateElement(
				hitElem.GetType(), addedNode, hitElem.EndNode(), itsData->GetActiveProperty());
	
	return NEW AddElementAction(e1, e2, &itsData->Elements(),
								NEW RemoveElementAction(hitElem, &itsData->Elements()),
								n2Action);
}

#pragma mark -
#pragma mark ====== Properties ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::GetProperties
//
//      return properties.
//
//  returns PropertyEnumerator <- 
//----------------------------------------------------------------------------------------
PropertyEnumerator 
FrameStructure::GetProperties() const
{
	return PropertyEnumerator(&itsData->Properties());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetActiveProperty
//
//      return the active property
//
//  returns Property   <- 
//----------------------------------------------------------------------------------------
Property
FrameStructure::GetActiveProperty() const
{
	return itsData->GetActiveProperty();	
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SetActiveProperty
//
//      set the active property.
//
//  Property prop  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::SetActiveProperty(Property prop) 
{
	itsData->SetActiveProperty(prop);	
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetAssignedElements
//
//      return the elements assigned to the property.
//
//  Property prop  -> the property.
//
//  returns ElementEnumerator <- list of elements containing property.
//----------------------------------------------------------------------------------------
ElementEnumerator
FrameStructure::GetAssignedElements(const Property& prop) const
{
	if (!prop.Empty())
		return itsData->Elements().FindAttachedProperty(prop);
	return ElementEnumerator();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetPropertyTypes
//
//      return the property types for the specified element.
//
//  const char* elementType        -> the element type.
//
//  returns PropertyTypeEnumerator <- the property types.
//----------------------------------------------------------------------------------------
PropertyTypeEnumerator
FrameStructure::GetPropertyTypes(const char* elementType) const
{
	return PropertyFactory::sPropertyFactory.GetPropertyTypes(elementType);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetMergedPropertyTypes
//
//      return the master property type list. This list contains all defined property
//		types.
//
//  returns PropertyTypeEnumerator <- the master property type list.
//----------------------------------------------------------------------------------------
PropertyTypeEnumerator
FrameStructure::GetMergedPropertyTypes() const
{
	return PropertyFactory::sPropertyFactory.GetMergedPropertyTypes();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::AssignProperties
//
//      assign properties to elements.
//
//  const ElementEnumerator& elems -> 
//  const Property& prop           -> 
//
//  returns Action*                <- 
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::AssignProperties(const ElementEnumerator& elems, const Property& prop)
{
	return NEW AssignPropertyAction(prop, elems);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::ChangeProperty
//
//      update the property.
//
//  Property prop          -> 
//  const char* key        -> 
//  const char* newValue   -> 
//
//  returns Action*        <- 
//----------------------------------------------------------------------------------------
Action*
FrameStructure::ChangeProperty(Property prop, const char* key, const char* newValue) 
{
	return NEW ChangePropertyAction(prop, key, newValue);	
}

//----------------------------------------------------------------------------------------
//  FrameStructure::CreateProperty
//
//      create a new property.
//
//  Property& property -> 
//
//  returns Action*    <- 
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::CreateProperty(const char* defName, Property& property)
{
	PropertyImp* p = PropertyFactory::sPropertyFactory
						.CreateProperty(defName, itsData->GetActiveElementType());
	property = p;
	return NEW AddPropertyAction(p, &itsData->Properties());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::RemoveProperty
//
//      remove an new property.
//
//  Property property  -> the property to remove
//
//  returns Action*    <- the action to perform operation
//----------------------------------------------------------------------------------------
Action* FrameStructure::RemoveProperty(Property property)
{
	_DlAssert(GetAssignedElements(property).Length() == 0);
	return NEW RemovePropertyAction(property, &itsData->Properties(), itsData);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::LoadProperty
//
//      read a property from the stream
//
//  StrInputStream& input  -> the stream.
//
//  returns                <- Property
//----------------------------------------------------------------------------------------
Action*
FrameStructure::LoadProperty(StrInputStream& input)
{
	PropertyImp* p = itsData->Properties().ReadOne(input, *itsData);
	return NEW AddPropertyAction(p, &itsData->Properties());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SaveProperty
//
//      write the property to the stream.
//
//  Property prop              -> the property.
//  StrOutputStream& output    -> the stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::SaveProperty(Property prop, StrOutputStream& output) const
{
	itsData->Properties().WriteOne(output, *itsData, prop);
}

#pragma mark -
#pragma mark ====== Node Loads ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::GetNodeLoads
//
//      return the node loads.
//
//  returns NodeLoadEnumerator <- 
//----------------------------------------------------------------------------------------
NodeLoadEnumerator 
FrameStructure::GetNodeLoads() const
{
	//	return NodeLoadEnumerator(NEW NodeLoadList(itsData->NodeLoads()));
	return NodeLoadEnumerator(&itsData->NodeLoads());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetAssignedNodes
//
//      return the nodes assigned to a load.
//
//  LoadCase lc            -> 
//  const NodeLoad& load   -> 
//
//  returns NodeEnumerator <- 
//----------------------------------------------------------------------------------------
NodeEnumerator 
FrameStructure::GetAssignedNodes(const NodeLoad& load, LoadCase lc) const
{
	if (!load.Empty())
		return itsData->Nodes().FindAttachedLoad(lc, load);
	return NodeEnumerator();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::CheckNodeLoadEquations
//
//      return the nodes assigned to a load.
//
//  LoadCase lc            ->
//  const NodeLoad& load   ->
//
//  returns NodeEnumerator <-
//----------------------------------------------------------------------------------------
NodeEnumerator
FrameStructure::CheckNodeLoadEquations(NodeLoadType theType) const
{
	return itsData->Nodes().CheckLoadEquations(theType);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::CreateNodeLoad
//
//      create a new node load.
//
//  NodeLoad& load     -> 
//
//  returns Action*    <- 
//----------------------------------------------------------------------------------------
Action*
FrameStructure::CreateNodeLoad(NodeLoad& load)
{
	NodeLoadImp* theLoad = NEW NodeLoadImp();
	load = theLoad;
	return NEW AddNodeLoadAction(theLoad, &itsData->NodeLoads());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::AssignNodeLoads
//
//      assign node loads.
//
//  const NodeEnumerator& objs -> 
//  LoadCase lc                -> 
//  NodeLoad load              -> 
//
//  returns Action*            <- 
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::AssignNodeLoads(const NodeEnumerator& objs, NodeLoad load)
{
	return NEW AssignNodeLoadAction(load, objs, itsData->GetActiveLoadCase());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::ChangeNodeLoad
//
//      change the loads.
//
//  NodeLoad load      -> 
//  DlInt32 dof        -> 
//  const char* value  -> 
//  DlInt32 theType    -> 
//
//  returns Action*    <- 
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::ChangeNodeLoad(NodeLoad load, DlInt32 dof, const char* value, NodeLoadType theType)
{
	return NEW ChangeNodeLoadAction(load, dof, value, theType, itsData);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::RemoveNodeLoad
//
//      remove the node load.
//
//  NodeLoad load      -> the load to remove.
//
//  returns Action*    <- an action to perform the change.
//----------------------------------------------------------------------------------------
Action* FrameStructure::RemoveNodeLoad(NodeLoad load)
{
	return NEW RemoveNodeLoadAction(load, &itsData->NodeLoads());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::LoadNodeLoad
//
//      read a NodeLoad from the stream
//
//  StrInputStream& input  -> the stream.
//
//  returns                <- NodeLoad
//----------------------------------------------------------------------------------------
Action*
FrameStructure::LoadNodeLoad(StrInputStream& input)
{
	NodeLoadImp* theLoad = NEW NodeLoadImp(input, *itsData);
	return NEW AddNodeLoadAction(theLoad, &itsData->NodeLoads());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SaveNodeLoad
//
//      write the node load to the stream.
//
//  NodeLoad l                 -> the load.
//  StrOutputStream& output    -> the stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::SaveNodeLoad(NodeLoad l, StrOutputStream& output) const
{
	const NodeLoadImp* imp = l;
	imp->Write(output, *itsData);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetNodeLoads
//
//      get the loads and types applied to the specified node. This applies to load case
//		combinations in addition to defined loads.
//
//  DlFloat32 vals[DOF_PER_NODE]       <-> the values.
//  NodeLoadType types[DOF_PER_NODE]   <-> the types.
//  Node n                             -> the node.
//
//  returns bool                       <- true if any loads are applied to that node.
//----------------------------------------------------------------------------------------
bool
FrameStructure::GetNodeLoads(DlFloat32 vals[DOF_PER_NODE], NodeLoadType types[DOF_PER_NODE], Node n) const
{
	LoadCase lc = GetActiveLoadCase();
	NodeLoad l = n.GetLoad(lc);
	if (l) {
		l.GetValues(vals, types);
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetNodeLoadString
//
//      get a string representing the load at the specified node. This works in the case
//		of combined loads.
//
//  DlInt32 dof        -> the dof.
//  Node n             -> the node.
//
//  returns DlString   <- string representing the load.
//----------------------------------------------------------------------------------------
DlString
FrameStructure::GetNodeLoadString(DlInt32 dof, Node n)
{
	LoadCase lc = GetActiveLoadCase();
	NodeLoad l = n.GetLoad(lc);
	if (l) {
		NodeLoadType theType =  n.GetRestrained(dof) ? NodeLoadIsDisp : NodeLoadIsForce;
		if (theType == l.GetType(dof))
			return l.GetValue(dof, true);
	}
	
	return "--";
}

#pragma mark -
#pragma mark ====== Element Loads ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::GetElementLoads
//
//      return the element loads.
//
//  returns ElementLoadEnumerator  <- 
//----------------------------------------------------------------------------------------
ElementLoadEnumerator 
FrameStructure::GetElementLoads() const
{
	//	return ElementLoadEnumerator(NEW ElemLoadList(itsData->ElementLoads()));
	return ElementLoadEnumerator(&itsData->ElementLoads());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetAssignedElements
//
//      return a list of elements that are assiged to the specified load.
//
//  const ElementLoad& load    -> the load.
//  LoadCase lc                -> the load case.
//
//  returns ElementEnumerator  <- list of elements assigned that load in that load case.
//----------------------------------------------------------------------------------------
ElementEnumerator
FrameStructure::GetAssignedElements(const ElementLoad& load, LoadCase lc) const
{
	if (!load.Empty())
		return itsData->Elements().FindAttachedLoad(lc, load);
	return ElementEnumerator();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::CreateElementLoad
//
//      create new element load.
//
//  ElementLoad& load  <-> the created load.
//
//  returns Action*    <- an action to perform the change.
//----------------------------------------------------------------------------------------
Action*
FrameStructure::CreateElementLoad(ElementLoad& load)
{
	ElemLoadImp* theLoad = NEW ElemLoadImp();
	load = theLoad;
	return NEW AddElemLoadAction(theLoad, &itsData->ElementLoads());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::AssignElementLoads
//
//      assign element loads.
//
//  const ElementEnumerator& elems -> the elements to assign.
//  LoadCase lc                    -> the load case.
//  const ElementLoad& load        -> the load to assign.
//
//  returns Action*                <- an action to perform the change.
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::AssignElementLoads(const ElementEnumerator& elems, const ElementLoad& load)
{
	return NEW AssignElemLoadAction(load, elems, itsData->GetActiveLoadCase());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::ChangeElementLoad
//
//      update the loads.
//
//  ElementLoad load   -> the load to change
//  DlInt32 dof        -> the load index
//  const char* value  -> the new value.
//
//  returns Action*    <- an action to perform the change.
//----------------------------------------------------------------------------------------
Action* 
FrameStructure::ChangeElementLoad(ElementLoad load, ElementLoadType type, const char* value)
{
	return NEW ChangeElemLoadAction(load, type, value, itsData);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::RemoveElementLoad
//
//      remove the element load.
//
//  ElementLoad load   -> the load to remove.
//
//  returns Action*    <- an action to perform the change.
//----------------------------------------------------------------------------------------
Action* FrameStructure::RemoveElementLoad(ElementLoad load)
{
	return NEW RemoveElemLoadAction(load, &itsData->ElementLoads());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::LoadElementLoad
//
//      read an element load from the stream
//
//  StrInputStream& input  -> the stream.
//
//  returns                <- ElementLoad
//----------------------------------------------------------------------------------------
Action*
FrameStructure::LoadElementLoad(StrInputStream& input)
{
	ElemLoadImp* theLoad = NEW ElemLoadImp(input, *itsData);
	return NEW AddElemLoadAction(theLoad, &itsData->ElementLoads());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SaveElementLoad
//
//      write the element load to the stream.
//
//  ElementLoad l              -> the load.
//  StrOutputStream& output    -> the stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::SaveElementLoad(ElementLoad l, StrOutputStream& output) const
{
	const ElemLoadImp* imp = l;
	imp->Write(output, *itsData);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetElementLoads
//
//      return the loads applied to the element. This works for both applied and combined
//		loads.
//
//  DlFloat32 vals[ElementLoadCount]   <-> the values.
//  Element e                          -> the element.
//
//  returns bool                       <- true if there are any loads.
//----------------------------------------------------------------------------------------
bool
FrameStructure::GetElementLoads(DlFloat32 vals[ElementLoadCount], Element e) const
{
	LoadCase lc = GetActiveLoadCase();
	ElementLoad l = e.GetLoad(lc);
		
	if (l) {
		l.GetValues(vals);
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetElementLoadString
//
//      return a string representing the value for the specified component for the
//      element. This works for both applied and combined loads.
//
//  ElementLoadType component  -> the load component.
//  Element e                  -> the element.
//
//  returns DlString           <- a string representing the applied load.
//----------------------------------------------------------------------------------------
DlString
FrameStructure::GetElementLoadString(ElementLoadType component, Element e) const
{
	LoadCase lc = GetActiveLoadCase();
	ElementLoad l = e.GetLoad(lc);
	if (l)
		return l.GetValue(component, true);
	
	return "--";
}


#pragma mark -
#pragma mark ====== Analysis ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::CanAnalyze
//
//      return true if the structure can be analyzed.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
bool
FrameStructure::CanAnalyze() const {
	return itsData->CanAnalyze();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::Analyzed
//
//      return true if the structure has analysis results.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
bool
FrameStructure::Analyzed() const
{
	return itsData->Analyzed();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::InitAnalysis
//
//      initialize an analysis.
//
//  AnalysisData* data -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
FrameStructure::InitAnalysis(AnalysisData* data)
{
	itsData->InitAnalysis(data);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::Analyze
//
//      perform the analysis.
//
//  DlListener* listener   -> the listener to report status.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::Analyze(DlListener* listener)
{
	itsData->Analyze(listener);
}

#pragma mark -
#pragma mark ======== IO =======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::Load
//
//      read this structure from the input streams
//
//  StrInputStream& input  -> the stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::Load(StrInputStream& input)
{
	itsData->read(input);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::Save
//
//      write this structure to the output stream.
//
//  StrOutputStream& output    -> the output.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::Save(StrOutputStream& output)
{
	itsData->write(output);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::LoadResults
//
//      TODO: read in the results.
//
//  StrInputStream& input  -> the input stream
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::LoadResults(StrInputStream& input)
{
}

#pragma mark -
#pragma mark ====== Results ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::GetActiveResultTypes
//
//      return the names and units for the table elements contained in the results.
//
//  returns PropertyTypeEnumerator <- the types.
//----------------------------------------------------------------------------------------
PropertyTypeEnumerator
FrameStructure::GetResultTypes(const char* elementType) const
{
	return PropertyFactory::sPropertyFactory.GetResultTypes(elementType);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetMergedResultTypes
//
//      return the master result type list.
//
//  returns PropertyTypeEnumerator <- the mater result type list.
//----------------------------------------------------------------------------------------
PropertyTypeEnumerator
FrameStructure::GetMergedResultTypes() const
{
	return PropertyFactory::sPropertyFactory.GetMergedResultTypes();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetResults
//
//      return the analysis results for the specified load case.
//
//  LoadCase lc                    -> the load case.
//
//  returns const LoadCaseResults* <- the results.
//----------------------------------------------------------------------------------------
const LoadCaseResults*
FrameStructure::GetResults() const
{
	return itsData->GetResults(itsData->GetActiveLoadCase());
}

//----------------------------------------------------------------------------------------
//  FrameStructure::ClearResults
//
//      clear the analysis results.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::ClearResults() 
{
	itsData->ClearAnalysis(0);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetDisplacementMagnifier
//
//      return the magnification factor for the displacement diagram.
//
//  LoadCase lc        -> the current load case.
//
//  returns DlFloat64  <- the magnifier.
//----------------------------------------------------------------------------------------
DlFloat64
FrameStructure::GetDisplacementMagnifier() const
{
	return itsData->GetResults(itsData->GetActiveLoadCase())->GetDisplacementMagnifier();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SetDisplacementMagnifier
//
//      set the magnification factor for the displacement diagram.
//
//  LoadCase lc            -> the current load case.
//  DlFloat64 magnifier    -> the new magnifier.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::SetDisplacementMagnifier(DlFloat64 magnifier)
{
	itsData->SetDisplacementMagnifier(itsData->GetActiveLoadCase(), magnifier);
}


#pragma mark -
#pragma mark ====== Load Cases ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::GetEditableLoadCases
//
//      return a list of editable load case name/number pairs.
//
//  returns FrameStructure::LoadCaseList   <- the list.
//----------------------------------------------------------------------------------------
FrameStructure::LoadCaseList
FrameStructure::GetEditableLoadCases() const
{
	LoadCaseList l;
	l.resize(itsData->GetLoadCaseCount() - itsData->GetLoadCaseCombinationCount());
	
	for (auto i = 0; i < itsData->GetLoadCaseCount(); i++) {
		if (itsData->IsDefinedLoadCase(i)) {
			l[i] = LoadCaseItem(itsData->GetLoadCaseName(i), i);
		}
	}
	
	return l;
}

//----------------------------------------------------------------------------------------
//  FrameStructure::CreateLoadCase
//
//      create a new load case.
//
//  const char* name   -> the name
//
//  returns Action*    <- action to create load case.
//----------------------------------------------------------------------------------------
Action*
FrameStructure::CreateLoadCase(const char* name)
{
	return NEW AddLoadCaseAction(name, *itsData);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::UpdateLoadCase
//
//      update the specified load case.
//
//  LoadCase lc        -> 
//  const char* name   -> 
//
//  returns Action*    <- 
//----------------------------------------------------------------------------------------
Action*
FrameStructure::UpdateLoadCase(LoadCase lc, const char* name)
{
	return NEW UpdateLoadCaseAction(lc, name, *itsData);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetLoadCaseName
//
//      return the name for the specified case.
//
//  LoadCase lc            -> the load case.
//
//  returns const char*    <- the name.
//----------------------------------------------------------------------------------------
const char*
FrameStructure::GetLoadCaseName(LoadCase lc)
{
	return itsData->GetLoadCaseName(lc);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetLoadCaseCount
//
//      return the count of load cases.
//
//  returns DlUInt32   <- the total number of cases.
//----------------------------------------------------------------------------------------
DlUInt32
FrameStructure::GetLoadCaseCount() const
{
	return itsData->GetLoadCaseCount();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SetActiveLoadCase
//
//      set the active load case.
//
//  LoadCase lc    -> the new active load case.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
FrameStructure::SetActiveLoadCase(LoadCase lc)
{
	itsData->SetActiveLoadCase(lc);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetActiveLoadCase
//
//      return the active load case.
//
//  returns LoadCase   <- the active load case.
//----------------------------------------------------------------------------------------
LoadCase
FrameStructure::GetActiveLoadCase() const
{
	return itsData->GetActiveLoadCase();
}

//----------------------------------------------------------------------------------------
//  FrameStructure::ActiveLoadCaseIsEditable
//
//      return true if the active load case is editable.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
bool
FrameStructure::ActiveLoadCaseIsEditable() const
{
	return itsData->IsDefinedLoadCase(itsData->GetActiveLoadCase());
}

#pragma mark -
#pragma mark ====== Load Case combination ======
#pragma mark -

//----------------------------------------------------------------------------------------
//  FrameStructure::AddLoadCaseCombination
//
//      add the specified load case combination.
//
//  const LoadCaseCombination& combo   -> the new combination.
//
//  returns nothing
//----------------------------------------------------------------------------------------
Action*
FrameStructure::AddLoadCaseCombination(const char* name, LoadCaseCombination&& combo)
{
	return NEW AddLoadCaseComboAction(name, std::move(combo), *itsData);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::SetLoadCaseCombination
//
//      update the load case combination.
//
//  DlInt32 which                  -> the index.
//  const LoadCaseCombination& val -> the updated combination.
//
//  returns nothing
//----------------------------------------------------------------------------------------
Action*
FrameStructure::UpdateLoadCaseCombination(LoadCase lc, const char* name, LoadCaseCombination&& combo)
{
	return NEW UpdateLoadCaseComboAction(lc, name, std::move(combo), *itsData);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetLoadCaseCombination
//
//      return the load case combination.
//
//  DlInt32 which                      -> the index.
//
//  returns const LoadCaseCombination& <- the load case at that index.
//----------------------------------------------------------------------------------------
const LoadCaseCombination*
FrameStructure::GetLoadCaseCombination(LoadCase lc) const
{
	return itsData->GetLoadCaseCombination(lc);
}

//----------------------------------------------------------------------------------------
//  FrameStructure::GetLoadCaseCombinationCount
//
//      return the number of defined combinations.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
DlUInt32
FrameStructure::GetLoadCaseCombinationCount() const
{
	return itsData->GetLoadCaseCombinationCount();
}

//	eof
