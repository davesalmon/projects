/*+
 *	File:		NodeImp.cpp
 *
 *	Contains:	Frame Node internal interface
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "NodeImp.h"
#include "ElementList.h"
//#include "graphics.h"
#include <valarray>
#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "frame_data.h"

//---------------------------------- Methods -----------------------------------

#if DlDebugging
int NodeImp::_idGen = 0;
#endif

class FindAttachedElement
{
public:

	FindAttachedElement(const NodeImp * theNode) 
		: itsNode(theNode) {}
//	~FindAttachedElement() { decReference(itsList); }
	
	void operator() (const ElementImp* elem, DlUInt32) const
	{
		if (elem->StartNode() == itsNode || elem->EndNode() == itsNode)
			ElementList::GetList(const_cast<ElementEnumerator&>(itsList))->Add(const_cast<ElementImp *>(elem));
	}

	const NodeImp* itsNode;
	ElementEnumerator itsList;
private:
    FindAttachedElement(const FindAttachedElement& a);
    FindAttachedElement& operator=(const FindAttachedElement& a);
};

//
//	public interface
//
//----------------------------------------------------------------------------------------
//  NodeImp::NodeImp                                                          constructor
//
//      construct a node a the specified location.
//
//  const WorldPoint & c   -> the location.
//
//  returns nothing
//----------------------------------------------------------------------------------------
NodeImp::NodeImp(const WorldPoint & c) 
	: _coords(c)
	, _restraint(0)
#if DlDebugging
	, _id(_idGen++)
#endif
{
	memset(_equations, 0, sizeof(_equations));
//	memset(_loads, 0, sizeof(_loads));
}

//----------------------------------------------------------------------------------------
//  NodeImp::NodeImp                                                          constructor
//
//      construct a node from the input stream.
//
//  StrInputStream & input -> the input stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
NodeImp::NodeImp(StrInputStream & input, const frame_data &data)
#if DlDebugging
	: _id(_idGen++)
#endif
{
	DlInt32 i;
	DlFloat64 xc = input.GetDouble();
	DlFloat64 yc = input.GetDouble();
	
	_coords.set(xc, yc);
	_restraint = input.GetInt();
	for (i = 0; i < DOF_PER_NODE; i++)
		_equations[i] = input.GetInt();
	
	if (data.GetVersion() >= kFrameVersionDynamicLoadCounts) {
		int count = input.GetInt();
		if (count) {
			_loads.reserve(count);
			for (DlInt32 i = 0; i < count; i++) {
				LoadCase lc = input.GetInt();
				NodeLoadImp* imp = data.IndexToNodeLoad(lc, input.GetInt());
				if (imp != nullptr)
					AssignLoad(lc, imp);
			}
		}
	} else {
		for (i = 0; i < kMaxLoadCases; i++) {
			NodeLoadImp* imp = data.IndexToNodeLoad(i, input.GetInt());
			if (imp != nullptr)
				AssignLoad(i, imp);
		}
	}
}

//----------------------------------------------------------------------------------------
//  NodeImp::NodeImp                                                          constructor
//
//      copy a node.
//
//  const NodeImp& n   -> the node to copy.
//
//  returns nothing
//----------------------------------------------------------------------------------------
NodeImp::NodeImp(const NodeImp& n)
	: _coords(n._coords)
	, _restraint(n._restraint)
#if DlDebugging
, _id(_idGen++)
#endif
{
	memset(_equations, 0, sizeof(_equations));
//	memset(_loads, 0, sizeof(_loads));
}

//----------------------------------------------------------------------------------------
//  NodeImp::Write
//
//      write this node to the file.
//
//  StrOutputStream & output   -> the output file.
//  const frame_data & data    -> the data object.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeImp::Write(StrOutputStream & output, const frame_data & data) const
{
	DlInt32 i;
	output.PutDouble(_coords.x());
	output.PutDouble(_coords.y());
	output.PutInt(_restraint);
	for (i = 0; i < DOF_PER_NODE; i++)
		output.PutInt(_equations[i]);
	
	output.PutInt(_loads.size());
	for (auto p : _loads) {
		output.PutInt(p.first);
		output.PutInt(data.NodeLoadToIndex(p.first, p.second));
	}
}

//------------------------------------------------------------------------------
//	NodeImp::findAttached
//
//		return list of elements attached to this node.
//
//	elems				->	list to search.
//	return				<-	list of elements.
//------------------------------------------------------------------------------
ElementEnumerator 
NodeImp::FindAttached(const ElementList& elems)
{
	FindAttachedElement	finder(this);
	elems.Foreach(finder);
	return finder.itsList;
}

//------------------------------------------------------------------------------
//	NodeImp::minEquation
//
//		return the minimum equation number for this node.
//
//	return				<-	lowest equation number.
//------------------------------------------------------------------------------
DlInt32
NodeImp::MinEquation(DlInt32 currMin) const
{
	if (_equations[0] > 0 && _equations[0] < currMin)
		currMin = _equations[0];
	if (_equations[1] > 0 && _equations[1] < currMin)
		currMin = _equations[1];
	if (_equations[2] > 0 && _equations[2] < currMin)
		currMin = _equations[2];
	return currMin;
}

//------------------------------------------------------------------------------
//	NodeImp::setEquationNumbers
//
//		set the equation numbers for this node.
//
//	total				<->	current assigned equation.
//	react				<->	current assigned reaction.
//------------------------------------------------------------------------------
void
NodeImp::SetEquationNumbers(DlInt32 * total, DlInt32 * react)
{
	for (int i = 0; i < DOF_PER_NODE; i++) {
		if (IsFree(i))
			_equations[i] = ++(*total);
		else
			_equations[i] = -(++(*react));
	}
}

//----------------------------------------------------------------------------------------
//  NodeImp::AssignLoad
//
//      assign the specified load to this node.
//
//  LoadCase lc        -> the load case.
//  NodeLoadImp* load  -> the load to assign.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
NodeImp::AssignLoad(LoadCase lc, NodeLoadImp* load)
{
	auto iter = std::lower_bound(_loads.begin(), _loads.end(),
								 LoadPair(lc, nullptr),
								 [] (const LoadPair& p1, const LoadPair& p2) -> bool {
									 return p1.first < p2.first; });
	if (load) {
		if (iter != _loads.end() && iter->first == lc)
			iter->second = load;
		else
			_loads.insert(iter, LoadPair(lc, load));
	} else if (iter != _loads.end() && iter->first == lc) {
		_loads.erase(iter);
	} else {
		throw DlException("AssignLoad: Attempt to remove non-existant node load from lc %u.", lc);
	}
}

//----------------------------------------------------------------------------------------
//  NodeImp::GetLoad
//
//      return the load assigned.
//
//  LoadCase lc            -> the load case.
//
//  returns NodeLoadImp*   <- the load assigned or null.
//----------------------------------------------------------------------------------------
NodeLoadImp*
NodeImp::GetLoad(LoadCase lc) const
{
	auto iter = std::lower_bound(_loads.begin(), _loads.end(),
								 LoadPair(lc, nullptr), [] (const LoadPair& p1, const LoadPair& p2) {
									 return p1.first < p2.first; });
	
	if (iter != _loads.end() && iter->first == lc)
		return iter->second;
	
	return nullptr;
}

bool
NodeImp::HasLoad(const NodeLoadImp* l) const
{
	for (const auto& p : _loads) {
		if (p.second == l)
			return true;
	}
	return false;
}

std::pair<bool, NodeLoadImp*>
NodeImp::UpdateLoadCombination(LoadCase lc, const std::vector<DlFloat32>& factors)
{
	// check to see if a load is needed.
	bool needsLoad = false;
	for (auto p : _loads) {
		if (p.first >= factors.size())
			break;
		if (factors[p.first] != 0) {
			needsLoad = true;
			break;
		}
	}
	
	// and get the existing load
	NodeLoadImp* l = GetLoad(lc);

	if (!needsLoad) {
		// no load is needed, remove it.
		if (l)
			AssignLoad(lc, nullptr);
		return std::pair<bool, NodeLoadImp*>(false, l);
		
	} else {

		// now update the load
		bool doCreate = l == nullptr;
		if (doCreate) {
			l = NEW NodeLoadImp();
			AssignLoad(lc, l);
		} else {
			l->Clear();
		}
		
		for (auto p : _loads) {
			if (p.first >= factors.size())
				break;
			
			l->Add(p.second, factors[p.first]);
		}
		
		return std::pair<bool, NodeLoadImp*>(true, doCreate ? l : nullptr);
	}
}

//	eof