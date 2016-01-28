/*+
 *	File:		ElementImp.h
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

#ifndef _H_ElementImp
#define _H_ElementImp

//+--------------------------------- Includes ----------------------------------

#include "Element.h"
#include "DlString.h"
//#include <map>

#include "StringEnumerator.h"
#include "PropertyEnumerator.h"
#include "StrDefines.h"
#include "ElementLoad.h"

#include "DlMatrix.h"

class NodeImp;
class PropertyImp;
class StrInputStream;
class StrOutputStream;
class frame_data;
class PropertyTypeList;
class ElemLoadImp;
class WorldRect;
class PointEnumeratorImp;
class LoadCaseResults;

//+--------------------------------- Class -------------------------------------

class	ElementImp
{
	typedef std::pair<DlInt16, ElemLoadImp*> LoadPair;
	typedef std::vector<LoadPair> LoadList;
public:
    
	ElementImp(StrInputStream& inp, const frame_data& data);
	ElementImp(NodeImp* startNode, NodeImp* endNode, PropertyImp* prop);

	virtual ~ElementImp(){
#if DlDebugging
//		printf("destroy element %d\n", _id);
#endif
	};

	//	return the starting node for this element
	NodeImp* 		StartNode() const;
	void			SetStartNode(const NodeImp* n);
	
	//	return the ending node for this element
	NodeImp* 		EndNode() const;
	void			SetEndNode(const NodeImp* n);

    // return the direction cosines for the element
	WorldPoint		Cosines() const;

	//	get the propery object assigned to this type
	PropertyImp* 	GetProperty() const;
	void			SetProperty(PropertyImp* imp);
	
	ElemLoadImp*	GetLoad(LoadCase which) const;
	void			AssignLoad(LoadCase which, ElemLoadImp* load);
	bool			HasLoad(const ElemLoadImp* l) const;
	DlUInt32		LoadCount() const;
	
	DlFloat64		Length() const;

	void			GetDOF(DlInt32 dof[2*DOF_PER_NODE]) const;

	bool 			Hit(const WorldRect& r) const;
	bool			Settlement(LoadCase lc, DlFloat64 disp[2*DOF_PER_NODE]) const;

	//
	//	privileged
	//
	
	// return the maximum load case with assigned loads.
	//void		UpdateLoadCase(bool* lc) const;

	// return the applied load for the specified results
	DlFloat64		GetAppliedLoad(ElementLoadType which, const LoadCaseResults& res) const;
	
	//	update the specified load case using the factors. the response is
	//	first: t = add, f = remove
	//	second: loadptr or null
	std::pair<bool, ElemLoadImp*>
					UpdateLoadCombination(LoadCase lc, const std::vector<DlFloat32>& factors);
	
	//
	//	virtual
	//
	//	subclasses must provide this
	virtual const char* GetType() const = 0;

	virtual void Stiffness(DlArray::DlMatrix<DlFloat64>& m, bool fullMatrix) const = 0;
	virtual bool FixedEndForces(DlFloat64 loads[2*DOF_PER_NODE], LoadCase loadCase) const = 0;

	//
	//	element info
	//
	//	return the number of coords and the coords in imp
	virtual DlInt32	Coords(PointEnumeratorImp* pts) const;
	
	virtual void Write(StrOutputStream& out, const frame_data& data) const = 0;

	//	post analysis
	//
	//	Element forces
	//
	virtual const PropertyType* GetForceTypeForDOF(DlInt32 dof) const = 0;
	virtual void GetElementForce(double where, const ElementForce& feForce, LoadCase lc, ElementForce& force) const = 0;
	virtual ElementForce GetLocalForce(const DlFloat64 globalForces[DOF_PER_NODE]) const = 0;
    
	// returns a diagram for the force for DOF dof
	virtual DlInt32 ForceDOFCoords(DlInt32 dof, DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& res) const = 0;
	virtual DlFloat64 MaxForcePerLength(DlInt32 dof, DlInt32 index, const LoadCaseResults& res) const = 0;
	virtual DlFloat64 DOFForceAt(DlInt32 dof, DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const = 0;

	//
	//	Element displacement
	//
	virtual const PropertyType* GetDispTypeForDOF(DlInt32 dof) const = 0;
	
	// return coordinate lists for the results suitable for plotting on the structure.
	virtual DlInt32 DisplacedCoords(DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& results) const = 0;

    // return the maximum absolute values for result values.
	virtual DlFloat64 MaxDisplacementPerLength(DlInt32 index, const LoadCaseResults& res) const = 0;
	
	// the displacement in the specified dof and location along the beam.
	virtual DlFloat64 DOFDispAt(DlInt32 dof, DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const = 0;

	//
	// Element results for table
	//
	
    // return one of the result values. these values are defined per element type.
    //	the list of values is returned using GetResultType().
	virtual DlInt32 CountResultTypes() const = 0;
	virtual const PropertyType* GetResultType(DlInt32 whichType) const = 0;
	virtual DlFloat64 GetResultValue(int which, const ElementForce& feForce, const LoadCaseResults& res) const = 0;
	
#if DlDebugging
	static int		_idGen;
	int				_id;
#endif

protected:
        
	NodeImp* _nodes[2];
	PropertyImp* _property;
	
	LoadList	_loads;
	
public:

private:
    ElementImp(const ElementImp& e);
    ElementImp& operator=(const ElementImp& e);

};

//+--------------------------------- Inlines -----------------------------------

//----------------------------------------------------------------------------------------
//  ElementImp::ElementImp                                             constructor inline
//
//      Construct element from nodes.
//
//  NodeImp* startNode -> the starting node.
//  NodeImp* endNode   -> the ending node.
//  PropertyImp* prop  -> the property description.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline 
ElementImp::ElementImp(NodeImp* startNode, NodeImp* endNode, PropertyImp* prop)
	: _property(prop)
#if DlDebugging
	, _id(_idGen++)
#endif
{
	_nodes[0] = startNode;
	_nodes[1] = endNode;
}

//----------------------------------------------------------------------------------------
//  ElementImp::StartNode                                                          inline
//
//      return the start node.
//
//  returns NodeImp*   <- the starting node.
//----------------------------------------------------------------------------------------
inline NodeImp*
ElementImp::StartNode() const 
{
	return _nodes[0];
}

//----------------------------------------------------------------------------------------
//  ElementImp::SetStartNode                                                       inline
//
//      Set the start node.
//
//  const NodeImp* n   -> the new start node.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
ElementImp::SetStartNode(const NodeImp* n) 
{
	_nodes[0] = const_cast<NodeImp*>(n);
}

//----------------------------------------------------------------------------------------
//  ElementImp::EndNode                                                            inline
//
//      return the end node.
//
//  returns NodeImp*   <- the ending node.
//----------------------------------------------------------------------------------------
inline NodeImp*
ElementImp::EndNode() const 
{ 
	return _nodes[1]; 
}

//----------------------------------------------------------------------------------------
//  ElementImp::SetEndNode                                                         inline
//
//      set the end node.
//
//  const NodeImp* n   -> the new end node.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
ElementImp::SetEndNode(const NodeImp* n)
{
	_nodes[1] = const_cast<NodeImp*>(n);
}

//----------------------------------------------------------------------------------------
//  ElementImp::GetProperty                                                        inline
//
//      return the property description.
//
//  returns PropertyImp*   <- the property
//----------------------------------------------------------------------------------------
inline PropertyImp*
ElementImp::GetProperty() const
{
	return _property;
}

//----------------------------------------------------------------------------------------
//  ElementImp::SetProperty                                                        inline
//
//      set the property description.
//
//  PropertyImp* imp   -> the new property
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
ElementImp::SetProperty(PropertyImp* imp)
{ 
	_property = imp; 
}

inline DlUInt32
ElementImp::LoadCount() const {
	return _loads.size();
}

#endif

//	eof
