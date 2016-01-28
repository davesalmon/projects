/*+
 *	File:		ElementImp.cpp
 *
 *	Contains:	Interface for element
 *	
 *	Elements are created through the factory method. An element implementation
 *	must provide static methods for making an element
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

//+--------------------------------- Includes -----------------------------------

#include "DlPlatform.h"
#include "ElementImp.h"
#include "NodeImp.h"
#include "PropertyImp.h"
#include "frame_data.h"
#include "PointEnumeratorImp.h"

#include "PrismElement.h"
#include "PrismPinPinElement.h"

#include "StrInputStream.h"
#include "StrOutputStream.h"

//ElementImp::CreatorMap* ElementImp::theMap = 0;

#if DlDebugging
int ElementImp::_idGen = 0;
#endif

//+--------------------------------- Methods -----------------------------------


//----------------------------------------------------------------------------------------
//  ElementImp::Cosines
//
//      compute and return the direction cosines for the element.
//
//  returns WorldPoint <- 
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//  ElementImp::Cosines
//
//      return the direction cosines for the element.
//
//  returns WorldPoint <- 
//----------------------------------------------------------------------------------------
WorldPoint
ElementImp::Cosines() const 
{
	return WorldPoint::norm(StartNode()->GetCoords(), EndNode()->GetCoords());
}

//----------------------------------------------------------------------------------------
//  ElementImp::ElementImp                                                    constructor
//
//      Construct the implementation.
//
//  StrInputStream& inp    -> 
//  const frame_data& data -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
ElementImp::ElementImp(StrInputStream& inp, const frame_data& data)
#if DlDebugging
: _id(_idGen++)
#endif

{
	_nodes[0] = data.IndexToNode(inp.GetInt());
	_nodes[1] = data.IndexToNode(inp.GetInt());
	_property = data.IndexToProp(inp.GetInt());
	
	if (data.GetVersion() >= kFrameVersionDynamicLoadCounts) {
		int count = inp.GetInt();
		
		if (count) {
			_loads.reserve(count);
			for (DlInt32 i = 0; i < count; i++) {
				LoadCase lc = inp.GetInt();
				ElemLoadImp* imp = data.IndexToElemLoad(lc, inp.GetInt());
				if (imp != nullptr)
					AssignLoad(lc, imp);
			}
		}
	} else {
		for (DlInt32 i = 0; i < kMaxLoadCases; i++) {
			ElemLoadImp* imp = data.IndexToElemLoad(i, inp.GetInt());
			if (imp != nullptr) {
				AssignLoad(i, imp);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  ElementImp::Write
//
//      Write element to stream.
//
//  StrOutputStream& out   -> 
//  const frame_data& data -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
ElementImp::Write(StrOutputStream& out, const frame_data& data) const
{
	out.PutInt(data.NodeToIndex(_nodes[0]));
	out.PutInt(data.NodeToIndex(_nodes[1]));
	out.PutInt(data.PropToIndex(_property));
	
	out.PutInt(_loads.size());
	for (auto p : _loads) {
		out.PutInt(p.first);
		out.PutInt(data.ElemLoadToIndex(p.first, p.second));
	}
}

//----------------------------------------------------------------------------------------
//  ElementImp::MaxLoadCase
//
//      Determine the maximum load case with attached load for this element.
//
//  LoadCase currMax   -> 
//
//  returns LoadCase   <- 
//----------------------------------------------------------------------------------------
//void
//ElementImp::UpdateLoadCase(bool* lcs) const
//{
//	for (DlInt32 i = 0; i < kMaxLoadCases; i++) {
//		if (_load[i] != 0) {
//			lcs[i] = true;
//		}
//	}
//}

//----------------------------------------------------------------------------------------
//  ElementImp::GetDOF
//
//      return the dofs for this element.
//
//  DlInt32 dof[6] <-> array to hold 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementImp::GetDOF(DlInt32 dof[6]) const
{
	const NodeImp * nPtr = _nodes[0];
	for (DlInt32 i = 0; i < DOF_PER_NODE; i++)
		*dof++ = nPtr->GetEquationNumber(i);
	nPtr = _nodes[1];
	for (DlInt32 i = 0; i < DOF_PER_NODE; i++)
		*dof++ = nPtr->GetEquationNumber(i);
}

//----------------------------------------------------------------------------------------
//  ElementImp::Hit
//
//      hit test the element.
//
//  const WorldRect& r -> the hit rectangle.
//
//  returns bool       <- true if element intersects hit rectangle.
//----------------------------------------------------------------------------------------
bool
ElementImp::Hit(const WorldRect& r) const
{
	return r.intersects(_nodes[0]->GetCoords(), _nodes[1]->GetCoords());
}

//----------------------------------------------------------------------------------------
//  ElementImp::Settlement
//
//      returnt the settlement.
//
//  LoadCase lc                    -> the load case.
//  DlFloat64 disp[2*DOF_PER_NODE] -> array to hold displacement.
//
//  returns bool                   <- true if settlement attached.
//----------------------------------------------------------------------------------------
bool
ElementImp::Settlement(LoadCase lc, DlFloat64 disp[2*DOF_PER_NODE]) const 
{
	bool gotDisp = false;
	NodeLoadImp* ld = _nodes[0]->GetLoad(lc);
	std::memset(disp, 0, 2*DOF_PER_NODE*sizeof(DlFloat64));
	if (ld) {
		for (DlInt32 i = 0; i < DOF_PER_NODE; i++) {
			if (ld->GetType(i) == NodeLoadIsDisp) {
				gotDisp = true;
				disp[i] = ld->GetValue(i);
			}
		}
	}
	
	ld = _nodes[1]->GetLoad(lc);
	if (ld) {
		for (DlInt32 i = 0; i < DOF_PER_NODE; i++) {
			if (ld->GetType(i) == NodeLoadIsDisp) {
				gotDisp = true;
				disp[i+3] = ld->GetValue(i);
			}
		}
	}
		
	return gotDisp;
}

//----------------------------------------------------------------------------------------
//  ElementImp::Length
//
//      return the length.
//
//  returns DlFloat64  <- the length.
//----------------------------------------------------------------------------------------
DlFloat64
ElementImp::Length() const 
{
	return (_nodes[1]->GetCoords() - _nodes[0]->GetCoords()).length();
}

//----------------------------------------------------------------------------------------
//  ElementImp::Coords
//
//      return the coordinates.
//
//  PointEnumeratorImp* pts    -> the point enumerator to add to.
//
//  returns DlInt32            <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32
ElementImp::Coords(PointEnumeratorImp* pts) const 
{
	pts->set(0, _nodes[0]->GetCoords());
	pts->set(1, _nodes[1]->GetCoords());
	return 2;
}

//----------------------------------------------------------------------------------------
//  ElementImp::GetLoad
//
//      return the load.
//
//  LoadCase which         -> the load case
//
//  returns ElemLoadImp*   <- the attached load.
//----------------------------------------------------------------------------------------
ElemLoadImp*
ElementImp::GetLoad(LoadCase which) const
{
	auto iter = std::lower_bound(_loads.begin(), _loads.end(), LoadPair(which, nullptr),
								 [] (const LoadPair& p1, const LoadPair& p2) -> bool {
									 return p1.first < p2.first; });
	
	if (iter != _loads.end() && iter->first == which)
		return iter->second;
	
	return nullptr;
	
	//	return _load[which];
}

//----------------------------------------------------------------------------------------
//  ElementImp::AssignLoad                                                         
//
//      set the load.
//
//  LoadCase which     -> the load case.
//  ElemLoadImp* load  -> the load to attach.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementImp::AssignLoad(LoadCase which, ElemLoadImp* load)
{
	auto iter = std::lower_bound(_loads.begin(), _loads.end(), LoadPair(which, nullptr),
								 [] (const LoadPair& p1, const LoadPair& p2) -> bool {
									 return p1.first < p2.first; });
	
	if (load) {
		// if it is already there, replace it, otherwise insert it.
		if (iter != _loads.end() && iter->first == which)
			iter->second = load;
		else
			_loads.insert(iter, LoadPair(which, load));
	} else if (iter != _loads.end() && iter->first == which) {
		_loads.erase(iter);
	} else {
		throw DlException("AssignLoad: Attempt to remove non-existant element load from load case %u.", which);
	}
}

//----------------------------------------------------------------------------------------
//  ElementImp::HasLoad
//
//      return true if the element has assigned load.
//
//  const ElemLoadImp* l   -> 
//
//  returns bool           <- 
//----------------------------------------------------------------------------------------
bool
ElementImp::HasLoad(const ElemLoadImp* l) const
{
	for (const auto& p : _loads) {
		if (p.second == l)
			return true;
	}
	return false;
}

// return the applied load for the specified results
//----------------------------------------------------------------------------------------
//  ElementImp::GetAppliedLoad
//
//      return the applied load.
//
//  ElementLoadType which      -> 
//  const LoadCaseResults& res -> 
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64
ElementImp::GetAppliedLoad(ElementLoadType which, const LoadCaseResults& res) const
{
	const ElemLoadImp* l = GetLoad(res.GetLoadCase());
	if (l) {
		return l->GetValue(which);
	}
	
	return 0;
}

//----------------------------------------------------------------------------------------
//  ElementImp::UpdateLoadCombination
//
//      update loads for the specified combination. If a new load is needed, it
//		is returned in the pair.
//
//  LoadCase lc                            -> the load case.
//  const s                                -> the factors for that case.
//
//  returns std::pair<bool, ElemLoadImp*>  <- true if load needed for this case, along
//												with any new load created.
//----------------------------------------------------------------------------------------
std::pair<bool, ElemLoadImp*>
ElementImp::UpdateLoadCombination(LoadCase lc, const std::vector<DlFloat32>& factors)
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
	ElemLoadImp* l = GetLoad(lc);
	
	if (!needsLoad) {
		// no load is needed, remove it.
		if (l)
			AssignLoad(lc, nullptr);
		return std::pair<bool, ElemLoadImp*>(false, l);
		
	} else {
	
		// now update the load
		bool doCreate = l == nullptr;
		if (doCreate) {
			l = NEW ElemLoadImp();
			AssignLoad(lc, l);
		} else {
			l->Clear();
		}
		
		for (auto p : _loads) {
			if (p.first >= factors.size())
				break;
		
			l->Add(p.second, factors[p.first]);
		}
		
		return std::pair<bool, ElemLoadImp*>(true, doCreate ? l : nullptr);
	}
}
