/*+
 *	File:		FrameStructure.h
 *
 *	Contains:	Master interface for 2D frame structure
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

#ifndef _H_FrameStructure
#define _H_FrameStructure

//---------------------------------- Includes ----------------------------------

#include "DlException.h"
#include "StrDefines.h"

#include "PointEnumerator.h"
#include "ElementEnumerator.h"
#include "NodeLoadEnumerator.h"
#include "NodeEnumerator.h"
#include "PropertyEnumerator.h"
#include "ElementLoadEnumerator.h"
#include "PropertyTypeEnumerator.h"
#include "NodeLoad.h"
#include "ElementLoad.h"

#include "StringEnumerator.h"
#include <memory>
#include <vector>

class frame_data;

//class graphics;
class WorldPoint;
class WorldRect;
class DlString;

class Node;
class Element;
class Property;
class NodeLoad;
class ElementLoad;
class LoadCaseResults;
class LoadCaseCombination;

class Action;
class DlListener;


class StrInputStream;
class StrOutputStream;

typedef enum {
	AnalysisStageInit,
	AnalysisStageDecomp,
	AnalysisStageBackSub,
	AnalysisStageRecover,
	AnalysisStageDone,
	AnalysisStageFailed
} AnalysisStage;

typedef enum {
	FailureReasonNoFailure = 0,
	FailureReasonSingular,
	FailureReasonNonPositiveDefinite,
	FailureReasonUserCancelled,
	FailureReasonNeedPivot
} FailureReason;

//	structure sent with broadcast
typedef struct AnalysisBroadcast {
	AnalysisStage	stage;		//	current stage
	FailureReason	reason;		//	failure reason
	DlInt32			eqNum;		//	current equation
	DlInt32			total;		//	total equations
	bool			cancel;		//	set true to cancel
} AnalysisBroadcast;

typedef struct AnalysisData {
	DlInt32 nodeCount;
	DlInt32 elemCount;
	DlInt32 eqCount;
	DlInt32	lcCount;
} AnalysisData;

const DlUInt32 kAnalysisMessage = 3000;

//---------------------------------- Class -------------------------------------

class FrameStructure
{
public:
	
	// load case item contains name and LoadCase value.
	typedef std::pair<const char*, LoadCase> LoadCaseItem;
	typedef std::vector<LoadCaseItem> LoadCaseList;
	
	typedef void (*ForEachNodeFunc)(Node n,  DlInt32 index, void* context);
	typedef void (*ForEachElementFunc)(Element n, DlInt32 index, void* context);
	
	//	constructors
	
	FrameStructure(const char* defaultPropName);
	~FrameStructure();
	
    // copies
	FrameStructure(const FrameStructure& fs) = delete;
    FrameStructure& operator=(const FrameStructure& fs) = delete;
	
	//	copy with selected node and elements
	FrameStructure(const NodeEnumerator& nodes, const ElementEnumerator& elems, bool deepCopy=false);

	void DebugPrint() const;
	
	void AddListener(DlListener* l);
	void RemoveListener(DlListener* l);
	
	Action* PasteSame(const FrameStructure* pasteMe, const WorldPoint& ofs);
	Action* PasteNew(const FrameStructure* pasteMe, const WorldPoint& ofs);
//
// Nodes
//

	// construction
	Action* AddNode(const WorldPoint& pt, Node& addedNode);			
	Action* RemoveNodes(const NodeEnumerator& nodes, const ElementEnumerator& elems);
	DlInt32 GetNodeIndex(Node n) const;
	void DoForEachNode(ForEachNodeFunc f, void* ctx);
	
	NodeEnumerator GetNodes() const;
	
	//	do any of the nodes in moved nodes overlap?
	bool	DuplicateNodes(const NodeEnumerator& movedNodes, DlFloat64 tol) const;
	//	stitch the nodes. If stitch all is true, the given list
	//	is checked against itself. Otherwise, the list is checked against the
	//	rest of the structure.
	Action* StitchNodes(const NodeEnumerator& nodes, DlFloat64 tol, bool stitchAll=false);
	Action* MoveNodes(const NodeEnumerator& nodes, const WorldPoint& offset);
	Node FindNode(const WorldPoint& pt) const;
	Node SelectOneNode(const WorldRect& rect) const;
	NodeEnumerator SelectNodes(const WorldRect& rect) const;

	// return the node at the specified index
	Node GetNode(DlInt32 index) const;
	DlUInt32 CountNodes() const;

	//	property assignment
	Action* SetRestraints(const NodeEnumerator& nodes, DlUInt32 restCode);
	Action* ToggleRestraints(const NodeEnumerator& nodes, DlInt32 whichDof);

	// return the bounds of this structure
	WorldRect GetBounds() const;
//
// Elements
//

	const char* GetActiveElementType() const;
	void SetActiveElementType(const char* theType);

	Action* AddElement(const Node& node1, const Node& node2, const char* typeName, 
						Element& addedElement);
	
	//	add an element between the two points, snapping to an existing node if the
	//	endpoint is nearer than snapTol.
	Action* AddElement(const Node& p1, const WorldPoint& p2, 
						const char* typeName, Element& added);

	Action* AddElements(const Node& toNode, const NodeEnumerator& fromNodes,
						const char* typeName, const ElementEnumerator& added);
	
	// split the specified elements into two elements at the hit point, creating node.
	Action* SplitElement(Element& hitElem, Node& addedNode, DlFloat64 eLoc);
	
	// split the specified elements into two elements using the node.
//	Action* SplitElement(const Node& addedNode, Element& hitElem);

	DlInt32 GetElementIndex(Element e) const;
	
	void DoForEachElement(ForEachElementFunc f, void* ctx);
	
	ElementEnumerator GetElements() const;
	
	// return the node at the specified index
	Element GetElement(DlInt32 index) const;
	DlUInt32 CountElements() const;
	
	StringEnumerator GetElementTypes() const;

	//	hit testing
	
	Element SelectOneElement(const WorldRect& rect, DlFloat64& loc) const;
	Element SelectNearest(const WorldPoint& pt) const;
	
	ElementEnumerator SelectElements(const NodeEnumerator& nodes) const;
	ElementEnumerator SelectElements(const WorldRect& r) const;
	ElementEnumerator AttachedElements(Node n) const;
	bool DuplicateElement(Node n1, Node n2) const;

//
//	Properties
//
	//	return list of all properties
	PropertyEnumerator GetProperties() const;
	
	//	get the active property for this element type
	//	this property will be assigned to new elements?
	Property GetActiveProperty() const;
	void SetActiveProperty(Property prop);
	
	//	return list of elements all assigned to prop
	ElementEnumerator GetAssignedElements(const Property& prop) const;

	//	return list of elements that can be assigned to prop
	//ElementEnumerator CanAssignProperty(const Property& prop) const;

	//	get the property object assigned to this element
	Property GetProperty(const Element& elem) const;

	// return the property types for the element, or the master list.
	PropertyTypeEnumerator GetPropertyTypes(const char* elementType) const;
	
	// return the master property list.
	PropertyTypeEnumerator GetMergedPropertyTypes() const;

	//	assign property to the list of elements. The element list
	//	must be a subset of the list returned by CanAssign
	Action* AssignProperties(const ElementEnumerator& elems, const Property& prop);
	Action* ChangeProperty(Property prop, const char* key, const char* newValue);

	//	create new default property for this element type
	Action* CreateProperty(const char* defName, Property& property);

	// remove the specified property from the list.
	Action* RemoveProperty(Property property);

	Action* LoadProperty(StrInputStream& input);
	void	SaveProperty(Property prop, StrOutputStream& output) const;

//
//	Node loads
//

	//	return list of all nodal loads
	NodeLoadEnumerator GetNodeLoads() const;
	
	//	return list of all nodes assigned to load in lc or any load case if lc is -1
	NodeEnumerator GetAssignedNodes(const NodeLoad& load, LoadCase lc = kAnyLoadCase) const;

	// return a list of nodes where the node load assigned is not appropriate
	//	to the restraint.
	NodeEnumerator CheckNodeLoadEquations(NodeLoadType theType) const;
	
	Action* CreateNodeLoad(NodeLoad& load);
	
	Action* AssignNodeLoads(const NodeEnumerator& objs, NodeLoad load);
	Action* ChangeNodeLoad(NodeLoad load, DlInt32 dof, const char* value, NodeLoadType theType);
	
	// remove the specified load from the list.
	Action* RemoveNodeLoad(NodeLoad load);

	Action* LoadNodeLoad(StrInputStream& input);
	void	SaveNodeLoad(NodeLoad l, StrOutputStream& output) const;

	bool GetNodeLoads(DlFloat32 vals[DOF_PER_NODE], NodeLoadType types[DOF_PER_NODE], Node n) const;
	
	// return a string for node load display
	DlString GetNodeLoadString(DlInt32 dof, Node n);
	
//
//	Element Loads
//
	//	return list of all element loads
	ElementLoadEnumerator GetElementLoads() const;
	
	//	return list of all elements assigned to load in lc or any load case if lc is -1
	ElementEnumerator GetAssignedElements(const ElementLoad& load, LoadCase lc = kAnyLoadCase) const;

	Action* CreateElementLoad(ElementLoad& load);

	Action* AssignElementLoads(const ElementEnumerator& elems, const ElementLoad& load);
	Action* ChangeElementLoad(ElementLoad load, ElementLoadType type, const char* value);

	// remove the specified load from the list.
	Action* RemoveElementLoad(ElementLoad load);

	Action* LoadElementLoad(StrInputStream& input);
	void	SaveElementLoad(ElementLoad l, StrOutputStream& output) const;
	
	// return the value of the specified load component for the spcified element
	bool GetElementLoads(DlFloat32 vals[ElementLoadCount], Element e) const;
	
	DlString GetElementLoadString(ElementLoadType component, Element e) const;
	
//
//	state
//
	bool	CanAnalyze() const;
	void	InitAnalysis(AnalysisData* data);
	bool	Analyzed() const;
	
	//	Analysis
	void	Analyze(DlListener* listener);

//
//	io
//
	void	Load(StrInputStream& input);
	void	Save(StrOutputStream& output);


	void	SaveResults(StrOutputStream& output);
	void	LoadResults(StrInputStream& input);
	
//
//	load case
//
	LoadCaseList GetEditableLoadCases() const;
	
	Action*		CreateLoadCase(const char* name);
	Action*		UpdateLoadCase(LoadCase lc, const char* name);
	
	const char* GetLoadCaseName(LoadCase lc);
	// return the count of all cases including combinations.
	DlUInt32	GetLoadCaseCount() const;
	
	void		SetActiveLoadCase(LoadCase lc);
	LoadCase	GetActiveLoadCase() const;
	
	// return true if the active load case is an assigned one, false if combination
	bool		ActiveLoadCaseIsEditable() const;
	
//
// load case combination
//
	
	Action*			AddLoadCaseCombination(const char* name, LoadCaseCombination&& combo);
	Action*			UpdateLoadCaseCombination(LoadCase lc, const char* name, LoadCaseCombination&& val);
	
	const LoadCaseCombination*	GetLoadCaseCombination(LoadCase lc) const;
//	bool 						HasLoadCaseCombination(DlInt32 which) const;
	DlUInt32					GetLoadCaseCombinationCount() const;
	
//
//	results
//
	// return the result types for the specified element.
	PropertyTypeEnumerator GetResultTypes(const char* elementType) const;
	
	// return the master result types.
	PropertyTypeEnumerator GetMergedResultTypes() const;
	
	const LoadCaseResults*	GetResults() const;
	void					ClearResults();

	void SetDisplacementMagnifier(DlFloat64 magnifier);
	DlFloat64 GetDisplacementMagnifier() const;

private:

	frame_data* itsData;

};

#endif

//	eof
