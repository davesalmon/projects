/*+
 *	File:		NodeImp.h
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

#ifndef _H_NodeImp
#define _H_NodeImp

//---------------------------------- Includes ----------------------------------

//#include "Node.h"
#include "WorldRect.h"
#include "NodeLoadList.h"
#include "DlAssert.h"
#include "Node.h"
#include "ElementEnumerator.h"

#include <valarray>

class frame_data;
//class graphics;
class NodeLoad;
class StrInputStream;
class StrOutputStream;

class ElementList;

//---------------------------------- Class -------------------------------------

class NodeImp
{
	
	typedef std::pair<DlInt16, NodeLoadImp*> LoadPair;
	typedef std::vector<LoadPair> LoadList;

public:

	NodeImp(const WorldPoint & c);
	NodeImp(StrInputStream & input, const frame_data &data);
	NodeImp(const NodeImp& n);

	NodeImp& operator=(const NodeImp& i) = delete;

//
//	public interface
//

	//	coordinate access
	const WorldPoint&	GetCoords() const;
	WorldPoint&			GetCoords();
	void				SetCoords(const WorldPoint& newLoc);

	//	restraint access
	DlUInt32			GetRestraint() const;
	bool				GetRestrained(DlInt32 whichDof) const;
	void				SetRestrained(DlInt32 whichDof, bool fixed);
	DlUInt32			SetRestraint(DlUInt32 restCode);

	bool				IsFree(DlInt32 dof) const;
	void				SetFixed(DlInt32 dof);
	void				SetFree(DlInt32 dof);

//
//	Internal interface
//
	//	find the elements attached to this node
	ElementEnumerator	FindAttached(const ElementList& elems);

	//	find the minimum eq for node
	DlInt32				MinEquation(DlInt32 currMin) const;
	void				SetEquationNumbers(DlInt32* total, DlInt32* react);
	DlInt32				GetEquationNumber(DlInt32 i) const;

	//	user interface
	bool				Hit(const WorldRect& r) const;

	//	assign loads
	void				AssignLoad(LoadCase lc, NodeLoadImp* load);
	NodeLoadImp*		GetLoad(LoadCase lc) const;
	bool				HasLoad(const NodeLoadImp* imp) const;
	DlUInt32			LoadCount() const;
	std::pair<bool, NodeLoadImp*>
						UpdateLoadCombination(LoadCase lc, const std::vector<DlFloat32>& factors);

	void				Write(StrOutputStream & output, const frame_data & data) const;
	
#if DlDebugging
	static int		_idGen;
	int				_id;
#endif

private:

	WorldPoint				_coords;
	DlUInt32				_restraint;
	DlInt32					_equations[DOF_PER_NODE];
	
	LoadList	_loads;

};

//---------------------------------- Inlines -----------------------------------

//----------------------------------------------------------------------------------------
//  NodeImp::GetEquationNumber                                                     inline
//
//      return the equation number associated with this dof.
//		Negative equation numbers refer to the index into the reaction list.
//		Equation and reaction numbers are 1 based.
//
//  DlInt32 dof    -> the dof
//
//  returns DlInt32   <- equation number for specified dof.
//----------------------------------------------------------------------------------------
inline DlInt32
NodeImp::GetEquationNumber(DlInt32 dof) const
{
	return _equations[dof];
}

//----------------------------------------------------------------------------------------
//  NodeImp::IsFree                                                                inline
//
//      return true if the specified dof is free.
//
//  DlInt32 dof    -> the dof
//
//  returns bool   <- true if free.
//----------------------------------------------------------------------------------------
inline bool
NodeImp::IsFree(DlInt32 dof) const 
{
	return (_restraint & (1<<dof)) == 0;
}

//----------------------------------------------------------------------------------------
//  NodeImp::SetFixed                                                              inline
//
//      set the specified dof to fixed.
//
//  DlInt32 dof    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
NodeImp::SetFixed(DlInt32 dof) 
{
	_restraint |= (1<<dof);
}

//----------------------------------------------------------------------------------------
//  NodeImp::SetFree                                                               inline
//
//      set the specifed dof to free.
//
//  DlInt32 dof    -> the degree of freedom.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
NodeImp::SetFree(DlInt32 dof) 
{
	_restraint &= ~(1<<dof);
}

//------------------------------------------------------------------------------
//	NodeImp::GetRestraint
//
//		Get the restraint code for this node
//
//	returns				<-	the restraint code.
//------------------------------------------------------------------------------
inline DlUInt32
NodeImp::GetRestraint() const
{
	return _restraint;
}

//------------------------------------------------------------------------------
//	NodeImp::GetRestrained
//
//		Return true if the specified dof is restrained.
//
//	whichDof			->	dof to test.
//
//	returns				<-	true if whichDof is constrained.
//------------------------------------------------------------------------------
inline bool
NodeImp::GetRestrained(DlInt32 whichDof) const
{
	return !IsFree(whichDof);
}

//------------------------------------------------------------------------------
//	NodeImp::SetRestraint
//
//		Set the restraint for this node.
//
//	whichDof			->	dof to set.
//	fixed				->	true if whichDof is constrained.
//
//	returns nothing.
//------------------------------------------------------------------------------
inline void
NodeImp::SetRestrained(DlInt32 whichDof, bool fixed)
{
	if (fixed)
		SetFixed(whichDof);
	else
		SetFree(whichDof);
}

//----------------------------------------------------------------------------------------
//  NodeImp::SetRestraint                                                          inline
//
//      set the restraint code for the specified dof.
//
//  DlUInt32 restCode  -> the restraint code.
//
//  returns DlUInt32   <- the old restraint.
//----------------------------------------------------------------------------------------
inline DlUInt32
NodeImp::SetRestraint(DlUInt32 restCode) {
	DlUInt32 old = _restraint;
	_restraint = restCode;
	return old;
}

//----------------------------------------------------------------------------------------
//  NodeImp::GetCoords                                                             inline
//
//      return the coordinates of the node.
//
//  returns const WorldPoint & <- 
//----------------------------------------------------------------------------------------
inline const WorldPoint & 
NodeImp::GetCoords() const 
{
	return _coords;
}

//------------------------------------------------------------------------------
//	NodeImp::GetCoords
//
//		external coordinate access.
//
//	returns				<-	node coordinates.
//------------------------------------------------------------------------------
inline WorldPoint & 
NodeImp::GetCoords()  
{
	return _coords;
}

//------------------------------------------------------------------------------
//	NodeImp::SetCoords
//
//		change node coordinates.
//
//	newLoc				->	node coordinates.
//------------------------------------------------------------------------------
inline void 
NodeImp::SetCoords(const WorldPoint& newLoc) 
{
	_coords = newLoc;
}

//	user interface
//----------------------------------------------------------------------------------------
//  NodeImp::Hit                                                                   inline
//
//      return true if the specified location is a hit on this node.
//
//  const WorldRect& r -> the hit box.
//
//  returns bool       <- true if node in the box.
//----------------------------------------------------------------------------------------
inline bool 
NodeImp::Hit(const WorldRect& r) const 
{
	return r.contains(_coords);
}

inline DlUInt32
NodeImp::LoadCount() const {
	return _loads.size();
}

#endif