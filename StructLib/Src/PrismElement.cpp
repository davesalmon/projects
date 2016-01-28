/*+
 *	File:		PrismElement.cpp
 *
 *	Contains:	Implementation for prismatic element.
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "DlMacros.h"

#include "PrismElement.h"
#include "PropertyTypeList.h"
#include "PropertyImp.h"
#include "NodeImp.h"
#include "ElemLoadImp.h"
#include "LoadCaseResults.h"
#include "PointEnumeratorImp.h"
#include "PropertyFactory.h"

using namespace DlArray;

//---------------------------------- Definitions -------------------------------

const char* PrismElement::sElemType = ("Prism");

// The element properties for Prism elements.
const PropertyType	kProperties[] = {
	  {kPropertyModulus,			UnitsModulus, 	PropDataFloat,	true}
	, {kPropertyMomentOfInertia,	UnitsMOI, 		PropDataFloat,	true}
	, {kPropertyArea,				UnitsArea, 		PropDataFloat,	true}
	, {kPropertyStartPinned, 		UnitsNone, 		PropDataBool,	true}
	, {kPropertyEndPinned,			UnitsNone, 		PropDataBool,	true}
};

// The results for prism elements.
//	These are displayed in results.
const PropertyType kResults[] = {
      {"A(0)",	UnitsForce,			PropDataFloat, false}
    , {"V(0)",	UnitsForce,			PropDataFloat, false}
    , {"M(0)",	UnitsMoment,		PropDataFloat, false}
    , {"A(L)",	UnitsForce,			PropDataFloat, false}
    , {"V(L)",	UnitsForce,			PropDataFloat, false}
    , {"M(L)",	UnitsMoment,		PropDataFloat, false}
    , {"A Max", UnitsForce,			PropDataFloat, false}
    , {"V Max", UnitsForce,			PropDataFloat, false}
    , {"M Max", UnitsMoment,		PropDataFloat, false}
	, {"D(0)", UnitsDisplacement,	PropDataFloat, false}
	, {"D(L/2)", UnitsDisplacement,	PropDataFloat, false}
	, {"D(L)", UnitsDisplacement,	PropDataFloat, false}
};

const PropertyType kDOFForceProperties[] = {
	  {"Axial Force, A", 	UnitsForce,		PropDataFloat, false}
	, {"Shear Force, V",	UnitsForce,		PropDataFloat, false}
	, {"Bending Moment, M",	UnitsMoment,	PropDataFloat, false}
};

const PropertyType kDOFDispProperties[] = {
	  {"Axial",		UnitsDisplacement,	PropDataFloat, false}
	, {"Lateral",	UnitsDisplacement,	PropDataFloat, false}
	, {"Slope",		UnitsRotation,		PropDataFloat, false}
};

// element factory methods.
const ElementFactory::ElementProcs PrismElement::sElementProcs = {
	PrismElement::CreateElement,
	PrismElement::ReadElement,
	PrismElement::CreatePropertyTypes,
	PrismElement::CreateResultTypes
};

static void	
trussKernal(DlFloat64 e, DlFloat64 a, DlFloat64 l, DlFloat64 c, DlFloat64 s, 
	DlMatrix<DlFloat64>& m, bool fullMatrix);

static void	
leftKernal(DlFloat64 e, DlFloat64 a, DlFloat64 i, DlFloat64 l, DlFloat64 c, DlFloat64 s, 
	DlMatrix<DlFloat64>& m, bool fullMatrix);

static void	
rightKernal(DlFloat64 e, DlFloat64 a, DlFloat64 i, DlFloat64 l, DlFloat64 c, DlFloat64 s, 
	DlMatrix<DlFloat64>& m, bool fullMatrix);

static void	
frameKernal(DlFloat64 e, DlFloat64 a, DlFloat64 i, DlFloat64 l, DlFloat64 c, DlFloat64 s, 
	DlMatrix<DlFloat64>& m, bool fullMatrix);

//---------------------------------- Methods -----------------------------------

//----------------------------------------------------------------------------------------
//  PrismElement::PrismElement                                                constructor
//
//      Construct a prism element.
//
//  NodeImp* startNode     -> the start node
//  NodeImp* endNode       -> the end node
//  PropertyImp* property  -> the properties for this element.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PrismElement::PrismElement(NodeImp* startNode, NodeImp* endNode, 
						PropertyImp* property)
	: ElementImp(startNode, endNode, property == NULL ?
				 PropertyFactory::sPropertyFactory.CreateProperty("", sElemType) : property)
{
}

//----------------------------------------------------------------------------------------
//  PrismElement::PrismElement                                                constructor
//
//      Construct a prism element from the input stream
//
//  StrInputStream& inp    -> the data stream
//  const frame_data& data -> the frame.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PrismElement::PrismElement(StrInputStream& inp, const frame_data& data)
	: ElementImp(inp, data)
{
}

//----------------------------------------------------------------------------------------
//  PrismElement::Stiffness
//
//      Compute the stiffness for this element.
//
//  DlMatrix<DlFloat64>& m -> the matrix
//  bool fullMatrix        -> true if full matrix, otherwise only upper diagonal.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
PrismElement::Stiffness(DlMatrix<DlFloat64>& m, bool fullMatrix) const
{
	const WorldPoint& n1 = _nodes[0]->GetCoords();
	const WorldPoint& n2 = _nodes[1]->GetCoords();
	DlFloat64	len = n1.dist(n2);
	DlFloat64	c = (n2.x() - n1.x()) / len;
	DlFloat64	s = (n2.y() - n1.y()) / len;
	DlFloat64	e = _property->GetFloatValue(0);
	DlFloat64	i = _property->GetFloatValue(1);
	DlFloat64	a = _property->GetFloatValue(2);

	bool pinnedLeft = _property->GetBoolValue(3);
	bool pinnedRight = _property->GetBoolValue(4);
	
	if (!(pinnedLeft || pinnedRight))
		frameKernal(e, a, i, len, c, s, m, fullMatrix);
	else if (pinnedLeft) {
		if (pinnedRight) {
			trussKernal(e, a, len, c, s, m, fullMatrix);
		} else {
			rightKernal(e, a, i, len, c, s, m, fullMatrix);
		}
	} else {
		leftKernal(e, a, i, len, c, s, m, fullMatrix);
	}
}

//----------------------------------------------------------------------------------------
//  PrismElement::FixedEndForces
//
//      Compute the end force for this element.
//
//  DlFloat64 frcs[2*DOF_PER_NODE] <-> returned fixed end forces
//  LoadCase loadCase              -> the current load case
//
//  returns bool                   <- return true if there are loads
//----------------------------------------------------------------------------------------
bool
PrismElement::FixedEndForces(DlFloat64 frcs[2*DOF_PER_NODE], LoadCase loadCase) const
{
	const ElemLoadImp* load = GetLoad(loadCase);
	if (load == NULL) {
		
		return false;
	} else {
		const WorldPoint& n1 = _nodes[0]->GetCoords();
		const WorldPoint& n2 = _nodes[1]->GetCoords();
		DlFloat64	len = n1.dist(n2);
		DlFloat64	cost = (n2.x() - n1.x()) / len;
		DlFloat64	sint = (n2.y() - n1.y()) / len;

		// lateral force is towards element...
		DlFloat64	lateral = load->GetValue(ElementLoadLateral);
		DlFloat64	axial = load->GetValue(ElementLoadAxial);
		DlFloat64	preforce = load->GetValue(ElementLoadPreforce);
		DlFloat64	premoment = load->GetValue(ElementLoadPremoment);

		DlFloat64	v = 0.5 * lateral * len;
		DlFloat64	m = v * len / 6.0;

		bool		isPinnedLeft = _property->GetBoolValue(3);
		bool		isPinnedRight = _property->GetBoolValue(4);

		//	Element elongation (e is elongation require to fit bar)
		//	For deltaT: e = - alpha * deltaT

		DlFloat	f = -0.5 * axial * len;
		frcs[0] = (f - preforce) * cost;
		frcs[1] = (f - preforce) * sint;
		frcs[2] = premoment;
		frcs[3] = (f + preforce) * cost;
		frcs[4] = (f + preforce) * sint;
		frcs[5] = -premoment;
		
		DlFloat	vx = -v * sint;
		DlFloat	vy = v * cost;

		if (!(isPinnedLeft || isPinnedRight)) {			//	frame
			frcs[0] += vx;
			frcs[1] += vy;
			frcs[2] -= m;
			frcs[3] += vx;
			frcs[4] += vy;
			frcs[5] +=  m;
		} else if (isPinnedLeft) {
			if (isPinnedRight) {	//	truss
				frcs[0] += vx;
				frcs[1] += vy;
				frcs[3] += vx;
				frcs[4] += vy;
			} else {					//	fix right
				frcs[0] += 0.75 * vx;
				frcs[1] += 0.75 * vy;
				frcs[3] += 1.25 * vx;
				frcs[4] += 1.25 * vy;
				frcs[5] += 1.5 * m;
			}
		} else {						//	fix left
			frcs[0] += 1.25 * vx;
			frcs[1] += 1.25 * vy;
			frcs[3] += 0.75 * vx;
			frcs[4] += 0.75 * vy;
			frcs[2] -= 1.5 * m;
		}
		return true;
	}
}

//----------------------------------------------------------------------------------------
//  PrismElement::Write
//
//      write this element to the given stream
//
//  StrOutputStream& out   -> the stream
//  const frame_data& data -> the frame.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
PrismElement::Write(StrOutputStream& out, const frame_data& data) const
{
	ElementImp::Write(out, data);
}

//----------------------------------------------------------------------------------------
//  computeLateralDisp                                                             static
//
//      Compute the lateral displacement for this element
//		-t0 x + m0 x^2/(2ei) + v0 x^3/(6ei) - w x^4/(24ei)
//
//  DlFloat64 t0       -> clockwise rotation at 0
//  DlFloat64 v0       -> shear force at 0
//  DlFloat64 m0       -> moment at 0
//  DlFloat64 load     -> lateral load
//  DlFloat64 x        -> position along element from 0
//  DlFloat64 ei       -> stiffness.
//
//  returns DlFloat64  <- lateral displacment
//----------------------------------------------------------------------------------------
static DlFloat64
computeLateralDisp(DlFloat64 t0, DlFloat64 v0, DlFloat64 m0, DlFloat64 load, DlFloat64 x, DlFloat64 ei) 
{
	return (-t0 + (m0 + (v0 - load * x / 4.0) * x / 3.0) * x / (2.0 * ei)) * x;
}


//----------------------------------------------------------------------------------------
//  computeSlope                                                                   static
//
//      compute the slope at x for pin-ended members.
//
//  DlFloat64 t0       -> clockwise rotation at 0
//  DlFloat64 v0       -> shear force at 0
//  DlFloat64 m0       -> moment at 0
//  DlFloat64 load     -> lateral load
//  DlFloat64 x        -> position along element from 0
//  DlFloat64 ei       -> stiffness.
//
//  returns DlFloat64  <- slope
//----------------------------------------------------------------------------------------
static DlFloat64
computeSlope(DlFloat64 t0, DlFloat64 v0, DlFloat64 m0, DlFloat64 load, DlFloat64 x, DlFloat64 ei) 
{
	return (t0 - (m0 + (v0 - load * x / 3.0) * x / 2.0) * x / (ei));
}

//----------------------------------------------------------------------------------------
//  computeAxialDisp                                                               static
//
//      compute the axial displacement.
//
//  DlFloat64 a0       -> the axial force at 0
//  DlFloat64 load     -> the axial load
//  DlFloat64 x        -> the position from 0
//  DlFloat64 ea       -> the stiffness
//
//  returns DlFloat64  <- the axial displacement.
//----------------------------------------------------------------------------------------
static DlFloat64
computeAxialDisp(DlFloat64 a0, DlFloat64 load, DlFloat64 x, DlFloat64 ea)
{
	return (-a0 - (0.5 * load * x)) * x / ea;
}

//----------------------------------------------------------------------------------------
//  PrismElement::GetForceTypeForDOF
//
//      return the force type for the specified dof.
//
//  DlInt32 dof                    -> 
//
//  returns const PropertyType*    <- 
//----------------------------------------------------------------------------------------
const PropertyType* 
PrismElement::GetForceTypeForDOF(DlInt32 dof) const
{
	if (dof >= 0 && dof < DlArrayElements(kDOFForceProperties))
		return kDOFForceProperties + dof;
	return 0;
}

//----------------------------------------------------------------------------------------
//  PrismElement::GetElementForce
//
//      Compute the element force a distance of x from the start.
//
//  double x                       -> position from 0
//  const ElementForce& feForce    -> the element forces at 0
//  LoadCase lc                    -> the load case
//  ElementForce& frc              <-> the elements forces at x
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
PrismElement::GetElementForce(double x, const ElementForce& feForce, LoadCase lc, ElementForce& frc) const 
{
	DlFloat64	lateralLoad = 0;
	DlFloat64	axialLoad = 0;

	const ElemLoadImp* load = GetLoad(lc);
	if (load) {
		lateralLoad = load->GetValue(ElementLoadLateral);
		axialLoad = load->GetValue(ElementLoadAxial);
	}

    DlFloat64 forces[DOF_PER_NODE];
	forces[AXIAL_COMPONENT] = feForce[AXIAL_COMPONENT] - axialLoad * x;
	forces[SHEAR_COMPONENT] = feForce[SHEAR_COMPONENT] - lateralLoad * x;
	forces[MOMENT_COMPONENT] = (feForce[MOMENT_COMPONENT] + 
                                (feForce[SHEAR_COMPONENT] - lateralLoad * x / 2.0) * x);
	frc = ElementForce(forces);
}

// Convert DOF forces into local force list.
//----------------------------------------------------------------------------------------
//  PrismElement::GetLocalForce
//
//      return the local force.
//
//  const DlFloat64 globalForces[DOF_PER_NODE] -> 
//
//  returns ElementForce                       <- 
//----------------------------------------------------------------------------------------
ElementForce
PrismElement::GetLocalForce(const DlFloat64 globalForces[DOF_PER_NODE]) const
{
    DlFloat localForce[DOF_PER_NODE];
    
    WorldPoint cosines(Cosines());
    
    localForce[AXIAL_COMPONENT] = -(globalForces[0] * cosines.x() + globalForces[1] * cosines.y());
    localForce[SHEAR_COMPONENT] = (-globalForces[0] * cosines.y() + globalForces[1] * cosines.x());
    localForce[MOMENT_COMPONENT] = globalForces[2];
    
    return ElementForce(localForce);
}

//----------------------------------------------------------------------------------------
//  PrismElement::ForceDOFCoords
//
//      return the force diagram coordinates for the specified dof.
//
//  DlInt32 dof                -> 
//  DlInt32 index              -> 
//  PointEnumeratorImp* pts    -> 
//  const LoadCaseResults& res -> 
//
//  returns DlInt32            <- 
//----------------------------------------------------------------------------------------
DlInt32
PrismElement::ForceDOFCoords(DlInt32 dof, DlInt32 index, PointEnumeratorImp* pts, 
		const LoadCaseResults& res) const
{
	switch(dof)
	{
		case 0:	return AxialCoords(index, pts, res);
		case 1:	return ShearCoords(index, pts, res);
		case 2: return MomentCoords(index, pts, res);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//  PrismElement::MaxForcePerLength
//
//      return the max force per length for the specified dof.
//
//  DlInt32 dof                -> 
//  DlInt32 index              -> 
//  const LoadCaseResults& res -> 
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64
PrismElement::MaxForcePerLength(DlInt32 dof, DlInt32 index, const LoadCaseResults& res) const
{
	switch(dof)
	{
		case 0:	return MaxAxialPerLength(index, res);
		case 1:	return MaxShearPerLength(index, res);
		case 2: return MaxMomentPerLength(index, res);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//  PrismElement::DOFForceAt
//
//      return the force at the specified point for the specified dof.
//
//  DlInt32 dof                -> 
//  DlInt32 index              -> 
//  DlFloat64 u                -> 
//  const LoadCaseResults& res -> 
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64
PrismElement::DOFForceAt(DlInt32 dof, DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const
{
	ElementForce frc(res.GetElementForce(index));
	switch(dof)
	{
		case 0:	return AxialForceAt(frc, u, res);
		case 1:	return ShearForceAt(frc, u, res);
		case 2: return MomentAt(frc, u, res);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//  PrismElement::GetDispTypeForDOF
//
//      return the displacement type (axial, lateral, slope) for the specified dof index.
//
//  DlInt32 dof                    -> 
//
//  returns const PropertyType*    <- 
//----------------------------------------------------------------------------------------
const PropertyType* 
PrismElement::GetDispTypeForDOF(DlInt32 dof) const
{
	if (dof >= 0 && dof < DlArrayElements(kDOFDispProperties))
		return kDOFDispProperties + dof;
	return 0;
}

//----------------------------------------------------------------------------------------
//  PrismElement::DOFDispAt
//
//      return the displacement for the specified dof index.
//
//  DlInt32 dof                -> 
//  DlInt32 index              -> 
//  DlFloat64 u                -> 
//  const LoadCaseResults& res -> 
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64
PrismElement::DOFDispAt(DlInt32 dof, DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const
{
	ElementForce frc(res.GetElementForce(index));
	switch(dof)
	{
		case 0:	return AxialDisplacementAt(frc, u, res);
		case 1:	return LateralDisplacementAt(frc, u, res);
		case 2: return SlopeDisplacementAt(frc, u, res);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//  PrismElement::CountResultTypes
//
//      return the result types.
//
//  returns DlInt32    <- 
//----------------------------------------------------------------------------------------
DlInt32 
PrismElement::CountResultTypes() const
{
	return DlArrayElements(kResults);
}

//----------------------------------------------------------------------------------------
//  PrismElement::GetResultType
//
//      return the specified result type.
//
//  DlInt32 whichType              -> 
//
//  returns const PropertyType*    <- 
//----------------------------------------------------------------------------------------
const PropertyType* 
PrismElement::GetResultType(DlInt32 whichType) const
{
	if (whichType >= 0 && whichType < DlArrayElements(kResults))
		return kResults + whichType;
	return 0;
}

//----------------------------------------------------------------------------------------
//  PrismElement::GetResultValue
//
//      Get the result corresponding to the kResults results types.
//
//  int which                      -> the result type
//  const ElementForce& feForce    -> the element force at 0
//  const LoadCaseResults& res     -> the analysis results
//
//  returns DlFloat64              <- the value
//----------------------------------------------------------------------------------------
DlFloat64
PrismElement::GetResultValue(int which, const ElementForce& feForce, const LoadCaseResults& res) const
{
    switch(which)
    {
        case 0:
        case 1:
        case 2:
            return feForce[which];
            
        case 3:
        case 4:
        case 5:
        {
            ElementForce endFrc;
            GetElementForce(Length(), feForce, res.GetLoadCase(), endFrc);
            return endFrc[which - 3];
        }
        case 6:
            return maxAxial(feForce, res);
            
        case 7:
            return maxShear(feForce, res);
            
        case 8:
            return maxMoment(feForce, res);
		
		case 9:
			return LateralDisplacementAt(feForce, 0, res);
		case 10:
			return LateralDisplacementAt(feForce, 0.5, res);
		case 11:
			return LateralDisplacementAt(feForce, 1.0, res);
    }
    
    return 0;
}

struct ElementCompData
{	
	DlInt32		index;
	
	DlFloat64	lateralLoad;
	DlFloat64	axialLoad;
	DlFloat64	preforce;
	DlFloat64	premoment;
	
	DlFloat64	cost;
	DlFloat64	sint;
	DlFloat64	len;
	
	DlFloat64	ei;
	DlFloat64	ea;
	
	DlFloat64	axialForce;
	DlFloat64	shearForce;
	DlFloat64	moment;
	
	bool		isPinnedLeft;
	bool		isPinnedRight;
};

//----------------------------------------------------------------------------------------
//  PrismElement::computeElementData
//
//      compute the element data.
//
//  const ElementForce& frc        -> the end forces
//  const LoadCaseResults& results -> the results
//  ElementCompData& data          <-> the data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
PrismElement::computeElementData(const ElementForce& frc, const LoadCaseResults& results, ElementCompData& data) const
{
	data.lateralLoad = GetAppliedLoad(ElementLoadLateral, results);
	data.axialLoad = GetAppliedLoad(ElementLoadAxial, results);
	data.premoment = GetAppliedLoad(ElementLoadPremoment, results);
	data.preforce = GetAppliedLoad(ElementLoadPreforce, results);
	
//	const ElemLoadImp* load = _load[results.GetLoadCase()];
//	if (load) 
//	{
//		data.lateralLoad = load->GetValue(ElementLoadLateral);
//		data.axialLoad = load->GetValue(ElementLoadAxial);
//		data.premoment = load->GetValue(ElementLoadPremoment);
//		data.preforce = load->GetValue(ElementLoadPreforce);
//	}
//	else
//	{
//		data.lateralLoad = 0;
//		data.axialLoad = 0;
//		data.premoment = 0;
//		data.preforce = 0;
//	}

	const WorldPoint& n1 = _nodes[0]->GetCoords();
	const WorldPoint& n2 = _nodes[1]->GetCoords();
	
	WorldTransform xform(n1, n2);
	data.cost = xform.cosines.x();
	data.sint = xform.cosines.y();
	data.len = xform.len;

	DlFloat64	e = _property->GetFloatValue(0);
	DlFloat64	i = _property->GetFloatValue(1);
	DlFloat64	a = _property->GetFloatValue(2);
	data.ei = e * i;
	data.ea = e * a;
	
	data.axialForce =  -frc[AXIAL_COMPONENT] + data.preforce;
	data.shearForce = frc[SHEAR_COMPONENT];
	data.moment = frc[MOMENT_COMPONENT];
	
	data.isPinnedLeft = _property->GetBoolValue(3);
	data.isPinnedRight = _property->GetBoolValue(4);
}

//----------------------------------------------------------------------------------------
//  PrismElement::computePinnedEndSlope
//
//      computed the inital slope for a pinned end member.
//
//  const ElementCompData& data    -> the element data.
//  const LoadCaseResults& results -> the results.
//
//  returns DlFloat64              <- the slope at start node.
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::computePinnedEndSlope(const ElementCompData& data,const LoadCaseResults& results) const
{
	DlFloat64 slope;
	DlFloat64 displacements[DOF_PER_NODE];
	results.GetDisplacement(StartNode(), displacements);
	
	DlFloat64 endDisp[DOF_PER_NODE];
	results.GetDisplacement(EndNode(), endDisp);
	if (data.isPinnedRight)
	{
		slope = -((endDisp[1] - displacements[1]) * data.cost - (endDisp[0]-displacements[0]) * data.sint) / data.len;
		slope += data.lateralLoad * data.len * data.len * data.len / (24.0 * data.ei);
	}
	else
	{
		double m = (data.moment + 
							(data.shearForce - data.lateralLoad * data.len / 2.0) * data.len);
		// compute the slope from the right
		double v = data.shearForce - data.lateralLoad * data.len;
		slope = computeSlope(endDisp[2], v, -m, -data.lateralLoad, data.len, data.ei);
	}

	return slope;
}

//----------------------------------------------------------------------------------------
//  PrismElement::DisplacedCoords
//
//      compute the displacements along this element
//
//  DlInt32 index              -> the element index
//  PointEnumeratorImp* pts    -> the point list to fill
//  const LoadCaseResults& res -> the results
//
//  returns DlInt32            <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32 
PrismElement::DisplacedCoords(DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& results) const 
{
	const DlInt32 kDispPoints = 11;

	ElementCompData data;
	computeElementData(results.GetElementForce(index), results, data);
	
	DlFloat64 displacements[DOF_PER_NODE];
	results.GetDisplacement(StartNode(), displacements);
	DlFloat64 slope = displacements[2];

    if (data.isPinnedLeft)
    {
		slope = computePinnedEndSlope(data, results);
    }

	DlFloat64	x = 0;
	DlFloat64	inc = data.len / (kDispPoints - 1);
	DlFloat64	scale = results.DisplacementScale();

	DlFloat64 x0 = scale * displacements[0];
	DlFloat64 y0 = scale * displacements[1];

	const WorldPoint& n1 = _nodes[0]->GetCoords();

	for (int i = 0; i < kDispPoints; i++) {
		DlFloat64 lateralDisp = scale * computeLateralDisp(slope, 
											data.shearForce, data.moment - data.premoment, 
											data.lateralLoad, x, data.ei);
		DlFloat64 axialDisp = x + scale * computeAxialDisp(data.axialForce, data.axialLoad, x, data.ea);
		
		WorldPoint ofs(
			x0 + axialDisp * data.cost - lateralDisp * data.sint, 
			y0 + axialDisp * data.sint + lateralDisp * data.cost);
		ofs += n1;
		pts->set(i, ofs);
		
		x += inc; 
	}

	return kDispPoints;
}


//----------------------------------------------------------------------------------------
//  PrismElement::AxialCoords
//
//      return the axial force coords.
//
//  DlInt32 index              -> the element index
//  PointEnumeratorImp* pts    -> the point list to fill
//  const LoadCaseResults& res -> the results
//
//  returns DlInt32            <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32	
PrismElement::AxialCoords(DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& res) const
{
	DlFloat64	axialLoad = GetAppliedLoad(ElementLoadAxial, res);
//	const ElemLoadImp* load = _load[res.GetLoadCase()];
//	if (load) {
//		axialLoad = load->GetValue(ElementLoadAxial);
//	}

    const WorldPoint& n1 = _nodes[0]->GetCoords();
    const WorldPoint& n2 = _nodes[1]->GetCoords();
	WorldTransform xform(n1, n2);
    DlFloat64	cost = xform.cosines.x();
	DlFloat64	sint = xform.cosines.y();

	ElementForce frc = res.GetElementForce(index);
	DlFloat64 scale = res.AxialScale();
	DlFloat64 axialForce = scale * frc[AXIAL_COMPONENT];

	pts->set(0, n1);
	pts->set(1, n1 + WorldPoint(-axialForce * sint, axialForce * cost));
	
	axialForce -= axialLoad * xform.len * scale;
	pts->set(2, n2 + WorldPoint(-axialForce * sint, axialForce * cost));
	pts->set(3, n2);

	return 4;
}

//----------------------------------------------------------------------------------------
//  PrismElement::MomentCoords
//
//      return the moment coords.
//
//  DlInt32 index              -> the element index
//  PointEnumeratorImp* pts    -> the point list to fill
//  const LoadCaseResults& res -> the results
//
//  returns DlInt32            <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32	
PrismElement::MomentCoords(DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& res) const 
{
	const DlInt32 kDistMomentPts = 15;
	const DlInt32 kNoLoadMomentPts = 4;

	DlInt32		momentPoints = kDistMomentPts;
	DlFloat64	lateralLoad = GetAppliedLoad(ElementLoadLateral, res);
//	DlFloat64	premoment = GetAppliedLoad(ElementLoadPremoment, res);
	
	if (lateralLoad == 0) {
		momentPoints = kNoLoadMomentPts;
	}
	
//	const ElemLoadImp* load = _load[res.GetLoadCase()];
//	if (load) {
//		lateralLoad = load->GetValue(ElementLoadLateral);
//	} else {
//		momentPoints = kNoLoadMomentPts;
//	}

	const WorldPoint& n1 = _nodes[0]->GetCoords();
	const WorldPoint& n2 = _nodes[1]->GetCoords();
	WorldTransform xform(n1, n2);
    DlFloat64	cost = xform.cosines.x();
	DlFloat64	sint = xform.cosines.y();
	DlFloat64	len = xform.len;
    
	DlFloat64	x = 0;
	DlFloat64	inc = len / (momentPoints - 3);
	
	ElementForce frc = res.GetElementForce(index);
	DlFloat64 shearForce = frc[SHEAR_COMPONENT];

	DlFloat64 scale = res.MomentScale();

	pts->set(0, n1);
	for (int i = 1; i < momentPoints - 1; i++) {
	
		double m = scale * (frc[MOMENT_COMPONENT] + (shearForce -lateralLoad * x / 2.0) * x);
		WorldPoint ofs(x * cost - m * sint, x * sint + m * cost);
		
		ofs += n1;
		pts->set(i, ofs);
		
		x += inc; 
	}
	
	pts->set(momentPoints - 1, n2);
	
	return momentPoints;
}

//----------------------------------------------------------------------------------------
//  PrismElement::ShearCoords
//
//      return the shear diagram coords.
//
//  DlInt32 index              -> the element index
//  PointEnumeratorImp* pts    -> the point list to fill
//  const LoadCaseResults& res -> the results
//
//  returns DlInt32            <- the number of points.
//----------------------------------------------------------------------------------------
DlInt32	
PrismElement::ShearCoords(DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& res) const 
{
	DlFloat64	lateralLoad = GetAppliedLoad(ElementLoadLateral, res);
//	const ElemLoadImp* load = _load[res.GetLoadCase()];
//	if (load) {
//		lateralLoad = load->GetValue(ElementLoadLateral);
//	}

	const WorldPoint& n1 = _nodes[0]->GetCoords();
	const WorldPoint& n2 = _nodes[1]->GetCoords();
    
	WorldTransform xform(n1, n2);
    DlFloat64	cost = xform.cosines.x();
	DlFloat64	sint = xform.cosines.y();
	DlFloat64	len = xform.len;
	
	ElementForce frc = res.GetElementForce(index);
	DlFloat64 scale = res.ShearScale();
	DlFloat64 shearForce = scale * frc[SHEAR_COMPONENT];

	pts->set(0, n1);
	pts->set(1, n1 + WorldPoint(-shearForce * sint, shearForce * cost));
	
	shearForce -= lateralLoad * len * scale;
	pts->set(2, n2 + WorldPoint(-shearForce * sint, shearForce * cost));
	pts->set(3, n2);
		
	return 4;
}

//----------------------------------------------------------------------------------------
//  PrismElement::LateralDisplacementAt
//
//      return the lateral displacement at the specified point.
//
//  const ElementForce& frc    -> the element forces
//  DlFloat64 u                -> the location [0, 1]
//  const LoadCaseResults& res -> the results.
//
//  returns DlFloat64          <- lateral displacement
//----------------------------------------------------------------------------------------
DlFloat64
PrismElement::LateralDisplacementAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const
{
	ElementCompData data;
	computeElementData(frc, res, data);
	
	DlFloat64 displacements[DOF_PER_NODE];
	res.GetDisplacement(StartNode(), displacements);
	DlFloat64 slope = displacements[2];

    if (data.isPinnedLeft)
    {
		slope = computePinnedEndSlope(data, res);
    }

	DlFloat64 d0 = data.cost * displacements[1] - data.sint * displacements[0];

	return d0 + computeLateralDisp(slope, 
					data.shearForce, data.moment - data.premoment, 
					data.lateralLoad, u * data.len, data.ei);
}

//----------------------------------------------------------------------------------------
//  PrismElement::SlopeDisplacementAt
//
//      return the slope at the specified point.
//
//  const ElementForce& frc    -> the element forces
//  DlFloat64 u                -> the location [0, 1]
//  const LoadCaseResults& res -> the results.
//
//  returns DlFloat64          <- the slope
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::SlopeDisplacementAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const
{
	ElementCompData data;
	computeElementData(frc, res, data);
	
	DlFloat64 displacements[DOF_PER_NODE];
	res.GetDisplacement(StartNode(), displacements);
	DlFloat64 slope = displacements[2];

    if (data.isPinnedLeft)
    {
		slope = computePinnedEndSlope(data, res);
    }

	return computeSlope(slope, 
					data.shearForce, data.moment - data.premoment, 
					data.lateralLoad, u * data.len, data.ei);
}

//----------------------------------------------------------------------------------------
//  PrismElement::AxialDisplacementAt
//
//      return the axial displacement at the specified point.
//
//  const ElementForce& frc    -> the element forces
//  DlFloat64 u                -> the location [0, 1]
//  const LoadCaseResults& res -> the results.
//
//  returns DlFloat64          <- the axial displacement.
//----------------------------------------------------------------------------------------
DlFloat64
PrismElement::AxialDisplacementAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const
{
	const WorldPoint& n1 = _nodes[0]->GetCoords();
	const WorldPoint& n2 = _nodes[1]->GetCoords();
	
	WorldTransform xform(n1, n2);
	DlFloat64 cost = xform.cosines.x();
	DlFloat64 sint = xform.cosines.y();
	DlFloat64 len = xform.len;

	DlFloat64 axialLoad = GetAppliedLoad(ElementLoadAxial, res);
	DlFloat64 preforce = GetAppliedLoad(ElementLoadPreforce, res);
//	const ElemLoadImp* load = _load[res.GetLoadCase()];
//	if (load != 0)
//	{
//		axialLoad = load->GetValue(ElementLoadAxial);
//		preforce = load->GetValue(ElementLoadPreforce);
//	}
	
	DlFloat64	e = _property->GetFloatValue(0);
	DlFloat64	a = _property->GetFloatValue(2);
	DlFloat64	ea = e * a;

	
//	ElementForce frc = res.GetElementForce(index);
	DlFloat64 axialForce =  -frc[AXIAL_COMPONENT] + preforce;

	DlFloat64 displacements[DOF_PER_NODE];
	res.GetDisplacement(StartNode(), displacements);
	
	DlFloat64 d0 = cost * displacements[0] + sint * displacements[1];

	return d0 + computeAxialDisp(axialForce, axialLoad, u * len, ea);
}

//----------------------------------------------------------------------------------------
//  PrismElement::AxialForceAt
//
//      return the axial force at the specified location from the start of the element.
//
//  const ElementForce& frc    -> the element forces
//  DlFloat64 u                -> the location [0, 1]
//  const LoadCaseResults& res -> the results.
//
//  returns DlFloat64          <- the axial force at this location.
//----------------------------------------------------------------------------------------
DlFloat64
PrismElement::AxialForceAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const
{
	DlFloat64 axialLoad = GetAppliedLoad(ElementLoadAxial, res);
//	DlFloat64 preforce = GetAppliedLoad(ElementLoadAxial, res);
//	const ElemLoadImp* load = _load[res.GetLoadCase()];
//	if (load) {
//		axialLoad = load->GetValue(ElementLoadAxial);
//	}
	
	//ElementForce frc = res.GetElementForce(index);
	return frc[AXIAL_COMPONENT] - axialLoad * u * Length();
}

//----------------------------------------------------------------------------------------
//  PrismElement::ShearForceAt
//
//      return the shear force at the specified point.
//
//  const ElementForce& frc    -> the element forces
//  DlFloat64 u                -> the location [0, 1]
//  const LoadCaseResults& res -> the results.
//
//  returns DlFloat64          <- the shear force
//----------------------------------------------------------------------------------------
DlFloat64
PrismElement::ShearForceAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const
{
	DlFloat64	lateralLoad = GetAppliedLoad(ElementLoadLateral, res);
//	const ElemLoadImp* load = _load[res.GetLoadCase()];
//	if (load) {
//		lateralLoad = load->GetValue(ElementLoadLateral);
//	}
	
	//ElementForce frc = res.GetElementForce(index);
	return frc[SHEAR_COMPONENT] - lateralLoad * Length() * u;
}

//----------------------------------------------------------------------------------------
//  PrismElement::MomentAt
//
//      return the bending moment at the specified point.
//
//  const ElementForce& frc    -> the element forces
//  DlFloat64 u                -> the location [0, 1]
//  const LoadCaseResults& res -> the results.
//
//  returns DlFloat64          <- the moment.
//----------------------------------------------------------------------------------------
DlFloat64
PrismElement::MomentAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const
{
	DlFloat64	lateralLoad = GetAppliedLoad(ElementLoadLateral, res);
	
//	const ElemLoadImp* load = _load[res.GetLoadCase()];
//	if (load) {
//		lateralLoad = load->GetValue(ElementLoadLateral);
//	}
	
	//ElementForce frc = res.GetElementForce(index);
    u *= Length();
    
    return (frc[MOMENT_COMPONENT] + (frc[SHEAR_COMPONENT] - lateralLoad * u / 2.0) * u);
}

//----------------------------------------------------------------------------------------
//  setMaxVal                                                                      inline
//
//      set the max value for something.
//
//  DlFloat64 val      -> the value to test
//  DlFloat64 maxVal   -> the current max
//
//  returns DlFloat64  <- the new maximum.
//----------------------------------------------------------------------------------------
inline DlFloat64
setMaxVal(DlFloat64 val, DlFloat64 maxVal) {
	DlFloat64 absVal = fabs(val);
	return (absVal > maxVal) ? absVal : maxVal;
}

//----------------------------------------------------------------------------------------
//  PrismElement::MaxDisplacementPerLength
//
//      return the maximum displacement per length of element.
//
//  DlInt32 index              -> 
//  const LoadCaseResults& res -> 
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::MaxDisplacementPerLength(DlInt32 index, const LoadCaseResults& res) const
{
	const DlInt32 kMaxDispPts = 3;	// should be odd
	DlFloat64	lateralLoad = GetAppliedLoad(ElementLoadLateral, res);
	DlFloat64	axialLoad = GetAppliedLoad(ElementLoadAxial, res);
	DlFloat64	premoment = GetAppliedLoad(ElementLoadPremoment, res);
	DlFloat64	preforce = GetAppliedLoad(ElementLoadPreforce, res);
//	const ElemLoadImp* load = _load[res.GetLoadCase()];
//	if (load) {
//		lateralLoad = load->GetValue(ElementLoadLateral);
//		axialLoad = load->GetValue(ElementLoadAxial);
//		premoment = load->GetValue(ElementLoadPremoment);
//		preforce = load->GetValue(ElementLoadPreforce);
//	}

//	DlFloat64 fixEndForce[2*DOF_PER_NODE];
	DlFloat64 displacements[2*DOF_PER_NODE];

	res.GetDisplacement(StartNode(), displacements);
	res.GetDisplacement(EndNode(), displacements+DOF_PER_NODE);

	DlFloat64 maxVal = fabs(displacements[0]);
	if (fabs(displacements[1]) > maxVal)
		maxVal = fabs(displacements[1]);
	if (fabs(displacements[3]) > maxVal)
		maxVal = fabs(displacements[3]);
	if (fabs(displacements[4]) > maxVal)
		maxVal = fabs(displacements[4]);
	
	DlFloat64 elemLen = Length();
	
	if (lateralLoad != 0 || axialLoad != 0 || displacements[2] != 0 || displacements[5] != 0) {
		DlFloat64	inc = elemLen / (kMaxDispPts + 1);
		DlFloat64	x = inc;
		DlFloat64	e = _property->GetFloatValue(0);
		DlFloat64	i = _property->GetFloatValue(1);
		DlFloat64	a = _property->GetFloatValue(2);
		DlFloat64	ei = e * i;
		DlFloat64	ea = e * a;

		ElementForce frc = res.GetElementForce(index);
		DlFloat64 axialForce =  -frc[AXIAL_COMPONENT];
		DlFloat64 shearForce = frc[SHEAR_COMPONENT];

		for (int i = 0; i < kMaxDispPts; i++) {
			maxVal = setMaxVal(computeLateralDisp(displacements[2], 
												shearForce, frc[MOMENT_COMPONENT] - premoment, 
												lateralLoad, x, ei), maxVal);
			maxVal = setMaxVal(computeAxialDisp(axialForce + preforce, axialLoad, x, ea), maxVal);
			
			x += inc;
		}
	}
	
	return maxVal / elemLen;
}

//----------------------------------------------------------------------------------------
//  PrismElement::maxAxial
//
//      return the max axial force.
//
//  LoadCase lc                -> 
//  const ElementForce& frc    -> 
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::maxAxial(const ElementForce& frc, const LoadCaseResults& res) const
{
	DlFloat64	axialLoad = GetAppliedLoad(ElementLoadAxial, res);
//	DlFloat64	preforce = GetAppliedLoad(ElementLoadPreforce, res);
	DlFloat64 	maxForce = 0;
//	const ElemLoadImp* load = _load[lc];
//	if (load) {
//		axialLoad = load->GetValue(ElementLoadAxial);
//	}
	
	DlFloat64 elemLen = Length();    
	maxForce = setMaxVal(frc[AXIAL_COMPONENT], maxForce);
    
	if (axialLoad != 0 )
		maxForce = setMaxVal(frc[AXIAL_COMPONENT] - axialLoad * elemLen, maxForce);
    return maxForce;
}

//----------------------------------------------------------------------------------------
//  PrismElement::MaxAxialPerLength
//
//      return the max axial per length
//
//  DlInt32 index              -> 
//  const LoadCaseResults& res -> 
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::MaxAxialPerLength(DlInt32 index, const LoadCaseResults& res) const
{
    return maxAxial(res.GetElementForce(index), res) / Length();
}

//----------------------------------------------------------------------------------------
//  PrismElement::maxShear
//
//      return the max shear.
//
//  LoadCase lc                -> 
//  const ElementForce& frc    -> 
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::maxShear(const ElementForce& frc, const LoadCaseResults& res) const
{
    DlFloat64	lateralLoad = GetAppliedLoad(ElementLoadLateral, res);
	DlFloat64 	maxForce = 0;
//	const ElemLoadImp* load = _load[lc];
//	if (load)
//		lateralLoad = load->GetValue(ElementLoadLateral);
	
	DlFloat64 elemLen = Length();
	maxForce = setMaxVal(frc[SHEAR_COMPONENT], maxForce);		
    
	if (lateralLoad != 0)
		maxForce = setMaxVal(frc[SHEAR_COMPONENT] - lateralLoad * elemLen, maxForce);		
    
	return maxForce;
}

//----------------------------------------------------------------------------------------
//  PrismElement::MaxShearPerLength
//
//      return the max shear per length
//
//  DlInt32 index              -> 
//  const LoadCaseResults& res -> 
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::MaxShearPerLength(DlInt32 index, const LoadCaseResults& res) const
{
    return maxShear(res.GetElementForce(index), res) / Length();
}

//----------------------------------------------------------------------------------------
//  PrismElement::MaxMoment
//
//      compute the maximum moment and the location where the maximum occurs.
//
//  const ElementForce& frc    -> the end forces.
//  DlFloat64& loc             <- the location along the element
//  const LoadCaseResults& res -> the results.
//
//  returns DlFloat64          <- the maximum
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::MaxMoment(const ElementForce& frc, DlFloat64& loc, const LoadCaseResults& res) const
{
	//ElementForce frc = res.GetElementForce(index);
	
	DlFloat64	lateralLoad = GetAppliedLoad(ElementLoadLateral, res);
//	DlFloat64	premoment = GetAppliedLoad(ElementLoadPremoment, res);
	
//	const ElemLoadImp* load = _load[res.GetLoadCase()];
//	if (load) {
//		lateralLoad = load->GetValue(ElementLoadLateral);
//		premoment = load->GetValue(ElementLoadPremoment);
//	}
	
	DlFloat64 	maxMoment = frc[MOMENT_COMPONENT];// - premoment;
	loc = 0;

	DlFloat64 len = Length();	
	DlFloat64 shearForce = frc[SHEAR_COMPONENT]; //-frc.X() * sint + frc.Y() * cost;
//	bool hasRelativeMax = false;
    
	if (lateralLoad != 0) {
		
		DlFloat64 x = shearForce / lateralLoad;
		if (x > 0 && x < len)
		{
			maxMoment = frc[MOMENT_COMPONENT] + (shearForce - lateralLoad * x / 2.0) * x;
//			hasRelativeMax =  true;
			loc = x / len;
		}
	}
    else
	{
		DlFloat64 test = frc[MOMENT_COMPONENT] + (shearForce - lateralLoad * len / 2.0) * len;
		if (fabs(test) > fabs(maxMoment))
		{
			maxMoment = test;
			loc = 1;
		}
	}
	
	return maxMoment;

}

//----------------------------------------------------------------------------------------
//  PrismElement::maxMoment
//
//      compute the maximum moment in this element.
//
//  LoadCase lc                -> the load case.
//  const ElementForce& frc    -> the end forces for this element.
//
//  returns DlFloat64          <- the maximum moment value
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::maxMoment(const ElementForce& frc, const LoadCaseResults& res) const
{
	DlFloat64	lateralLoad = GetAppliedLoad(ElementLoadLateral, res);
	DlFloat64 	maxMoment = 0;
//	const ElemLoadImp* load = _load[lc];
//	if (load) {
//		lateralLoad = load->GetValue(ElementLoadLateral);
//	}
	
	DlFloat64	len = Length();	
	DlFloat64 shearForce = frc[SHEAR_COMPONENT]; //-frc.X() * sint + frc.Y() * cost;
	maxMoment = setMaxVal(frc[MOMENT_COMPONENT], maxMoment);
	maxMoment = setMaxVal(frc[MOMENT_COMPONENT] + (shearForce - lateralLoad * len / 2.0) * len, maxMoment);
    
	if (lateralLoad != 0) {
		
		DlFloat64 x = shearForce / lateralLoad;
		if (x > 0 && x < len)
			maxMoment = setMaxVal(frc[MOMENT_COMPONENT] + (shearForce - lateralLoad * x / 2.0) * x, maxMoment);
	}
    
	return maxMoment;
}

//----------------------------------------------------------------------------------------
//  PrismElement::MaxMomentPerLength
//
//      return the maximum bending moment for this element divided by the length.
//
//  DlInt32 index              -> the element index.
//  const LoadCaseResults& res -> the results
//
//  returns DlFloat64          <- 
//----------------------------------------------------------------------------------------
DlFloat64 
PrismElement::MaxMomentPerLength(DlInt32 index, const LoadCaseResults& res) const
{
    return maxMoment(res.GetElementForce(index), res) / Length();
}

//----------------------------------------------------------------------------------------
//  PrismElement::CreateElement
//
//      create a new prism element.
//
//  NodeImp* startNode     -> the start node.
//  NodeImp* endNode       -> the end node.
//  PropertyImp* property  -> the property set.
//
//  returns ElementImp*    <- the element.
//----------------------------------------------------------------------------------------
ElementImp* 
PrismElement::CreateElement(NodeImp* startNode, NodeImp* endNode, PropertyImp* property)
{
	return NEW PrismElement(startNode, endNode, property);
}

//----------------------------------------------------------------------------------------
//  PrismElement::ReadElement
//
//      Create a new element of this type from the data in the file.
//
//  StrInputStream& inp    -> the input stream
//  const frame_data& data -> the fram data.
//
//  returns ElementImp*    <- the new element.
//----------------------------------------------------------------------------------------
ElementImp* 
PrismElement::ReadElement(StrInputStream& inp, const frame_data& data)
{
	return NEW PrismElement(inp, data);
}

const PropertyTypeList*
PrismElement::CreatePropertyTypes()
{
	return NEW PropertyTypeList(kProperties, DlArrayElements(kProperties));
}

const PropertyTypeList*
PrismElement::CreateResultTypes()
{
    return NEW PropertyTypeList(kResults, DlArrayElements(kResults));
}

//----------------------------------------------------------------------------------------
//  PrismElement::CreateProperties
//
//      create a new properties set.
//
//  returns PropertyImp*   <- the new properties set.
//----------------------------------------------------------------------------------------
//PropertyImp* 
//PrismElement::CreateProperties(const char* defaultTitle)
//{
//	PropertyImp* p = NEW PropertyImp(defaultTitle, &sPropertyTypes);
//	p->SetFloatValue(0L, 1);
//	p->SetFloatValue(1L, 1);
//	p->SetFloatValue(2L, 1);
//	return p;
//}

//	get all registered element types
//----------------------------------------------------------------------------------------
//  PrismElement::GetPropertyTypes
//
//      return the property type names for the prism element.
//
//  returns const PropertyTypeList*    <- the type names.
//----------------------------------------------------------------------------------------
//const PropertyTypeList* 
//PrismElement::GetPropertyTypes()
//{
//	return &sPropertyTypes;
//}

//----------------------------------------------------------------------------------------
//  PrismElement::GetResultTypes
//
//      return the list of result type names for the results view.
//
//  returns const PropertyTypeList*    <- the result names.
//----------------------------------------------------------------------------------------
//const PropertyTypeList*
//PrismElement::GetResultTypes()
//{
//    return &sResultTypes;
//}

//----------------------------------------------------------------------------------------
//  fillMatrix                                                                     static
//
//      fill the symmetric elements in the given symmetric matrix
//
//  DlMatrix<DlFloat64>& m -> the matrix
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void
fillMatrix(DlMatrix<DlFloat64>& m)
{
	for (DlInt32 j = 1; j <= 6; j++) {
		DlSliceIter<DlFloat64> r(m.row(j));
		DlSliceIter<DlFloat64> c(m.col(j));
		for (DlInt32 k = j+1; k <= 6; k++) {
			c[k] = r[k];
		}
	}
}

//----------------------------------------------------------------------------------------
//  trussKernal                                                                    static
//
//      Compute the kernal stiffness matrix for a truss element.
//
//  DlFloat64 e            -> the modulus of elasticity
//  DlFloat64 a            -> the cross section area
//  DlFloat64 l            -> the length
//  DlFloat64 c            -> the cosine
//  DlFloat64 s            -> the sine
//  DlMatrix<DlFloat64>& m <-> the stiffness matrix
//  bool fullMatrix        -> true to compute the lower diagonal too.
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void	
trussKernal(DlFloat64 e, DlFloat64 a, DlFloat64 l, DlFloat64 c, DlFloat64 s, 
	DlMatrix<DlFloat64>& m, bool fullMatrix)
{	
	DlFloat64 eal = e * a / l;
	DlFloat64 csq = c*c;
	DlFloat64 ssq = s*s;
	DlFloat64 cs = c*s;
	
/*	Assemble kernal matrix
	*/

	m[1][1] = csq*eal;
	m[1][2] = cs*eal;
	m[2][2] = eal*ssq;
	m[1][3] = 0;
	m[2][3] = 0;
	m[3][3] = 0;
	m[1][4] = -(csq*eal);
	m[2][4] = -(cs*eal);
	m[3][4] = 0;
	m[4][4] = csq*eal;
	m[1][5] = -(cs*eal);
	m[2][5] = -(eal*ssq);
	m[3][5] = 0;
	m[4][5] = cs*eal;
	m[5][5] = eal*ssq;
	m[1][6] = 0;
	m[2][6] = 0;
	m[3][6] = 0;
	m[4][6] = 0;
	m[5][6] = 0;
	m[6][6] = 0;

	/*	Fill in the symmetric terms */

	if (fullMatrix)
		fillMatrix(m);
}

//----------------------------------------------------------------------------------------
//  leftKernal                                                                     static
//
//      Compute the kernal stiffness matrix for a fix-pinned element.
//
//  DlFloat64 e            -> the modulus of elasticity
//  DlFloat64 a            -> the cross section area
//  DlFloat64 i            -> the moment of inertia
//  DlFloat64 l            -> the length
//  DlFloat64 c            -> the cosine
//  DlFloat64 s            -> the sine
//  DlMatrix<DlFloat64>& m <-> the stiffness matrix
//  bool fullMatrix        -> true to compute the lower diagonal too.
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void	
leftKernal(DlFloat64 e, DlFloat64 a, DlFloat64 i, DlFloat64 l, DlFloat64 c, DlFloat64 s, 
		DlMatrix<DlFloat64>& m, bool fullMatrix)
{	
	DlFloat64 eal = e * a / l;
	DlFloat64 eil = 3.0 * e * i / l;
	DlFloat64 eil2 = eil / l;
	DlFloat64 eil3 = eil2 / l;
	DlFloat64 csq = c*c;
	DlFloat64 ssq = s*s;
	DlFloat64 cs = c*s;

/*	Assemble kernal matrix
	*/

	m[1][1] = csq*eal + eil3*ssq;
	
	m[1][2] = cs*eal - cs*eil3;
	m[2][2] = csq*eil3 + eal*ssq;
	
	m[1][3] = eil2*s;
	m[2][3] = -(c*eil2);
	m[3][3] = eil;
	
	m[1][4] = -(csq*eal) - eil3*ssq;
	m[2][4] = -(cs*eal) + cs*eil3;
	m[3][4] = -(eil2*s);
	m[4][4] = csq*eal + eil3*ssq;
	
	m[1][5] = -(cs*eal) + cs*eil3;
	m[2][5] = -(csq*eil3) - eal*ssq;
	m[3][5] = c*eil2;
	m[4][5] = cs*eal - cs*eil3;
	m[5][5] = csq*eil3 + eal*ssq;
	
	m[1][6] = 0;
	m[2][6] = 0;
	m[3][6] = 0;
	m[4][6] = 0;
	m[5][6] = 0;
	m[6][6] = 0;

	/*	Fill in the symmetric terms */

	if (fullMatrix)
		fillMatrix(m);

}

//----------------------------------------------------------------------------------------
//  rightKernal                                                                    static
//
//      Compute the kernal stiffness matrix for a pinned-fix element.
//
//  DlFloat64 e            -> the modulus of elasticity
//  DlFloat64 a            -> the cross section area
//  DlFloat64 i            -> the moment of inertia
//  DlFloat64 l            -> the length
//  DlFloat64 c            -> the cosine
//  DlFloat64 s            -> the sine
//  DlMatrix<DlFloat64>& m <-> the stiffness matrix
//  bool fullMatrix        -> true to compute the lower diagonal too.
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void	
rightKernal(DlFloat64 e, DlFloat64 a, DlFloat64 i, DlFloat64 l, DlFloat64 c, DlFloat64 s, 
	DlMatrix<DlFloat64>& m, bool fullMatrix)
{	
	DlFloat64 eal = e * a / l;
	DlFloat64 eil = 3.0 * e * i / l;
	DlFloat64 eil2 = eil / l;
	DlFloat64 eil3 = eil2 / l;
	DlFloat64 csq = c*c;
	DlFloat64 ssq = s*s;
	DlFloat64 cs = c*s;

/*	Assemble kernal matrix
	*/

	m[1][1] = csq*eal + eil3*ssq;
	
	m[1][2] = cs*eal - cs*eil3;
	m[2][2] = csq*eil3 + eal*ssq;
	
	m[1][3] = 0;
	m[2][3] = 0;
	m[3][3] = 0;
	
	m[1][4] = -(csq*eal) - eil3*ssq;
	m[2][4] = -(cs*eal) + cs*eil3;
	m[3][4] = 0;
	m[4][4] = csq*eal + eil3*ssq;
	
	m[1][5] = -(cs*eal) + cs*eil3;
	m[2][5] = -(csq*eil3) - eal*ssq;
	m[3][5] = 0;
	m[4][5] = cs*eal - cs*eil3;
	m[5][5] = csq*eil3 + eal*ssq;
	
	m[1][6] = eil2*s;
	m[2][6] = -(c*eil2);
	m[3][6] = 0;
	m[4][6] = -(eil2*s);
	m[5][6] = c*eil2;
	m[6][6] = eil;

	/*	Fill in the symmetric terms */

	if (fullMatrix)
		fillMatrix(m);
}


//----------------------------------------------------------------------------------------
//  frameKernal                                                                    static
//
//      Compute the kernal stiffness matrix for a fix-fix element. Coordinates are
//		clockwise + for rotation and positive cartesian.
//
//  DlFloat64 e            -> the modulus of elasticity
//  DlFloat64 a            -> the cross section area
//  DlFloat64 i            -> the moment of inertia
//  DlFloat64 l            -> the length
//  DlFloat64 c            -> the cosine
//  DlFloat64 s            -> the sine
//  DlMatrix<DlFloat64>& m <-> the stiffness matrix
//  bool fullMatrix        -> true to compute the lower diagonal too.
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void	
frameKernal(DlFloat64 e, DlFloat64 a, DlFloat64 i, DlFloat64 l, DlFloat64 c, DlFloat64 s, 
	DlMatrix<DlFloat64>& m, bool fullMatrix)
{	
	DlFloat64 eal = e * a / l;
	DlFloat64 eil = 2.0 * e * i / l;
	DlFloat64 eil2 = 3.0 * eil / l;
	DlFloat64 eil3 = 2.0 * eil2 / l;
	DlFloat64 csq = c * c;
	DlFloat64 ssq = s * s;
	DlFloat64 cs = c * s;
	
/*	Assemble kernal matrix
	*/

	m[1][1] = csq*eal + eil3*ssq;
	
	m[1][2] = cs*eal - cs*eil3;
	m[2][2] = csq*eil3 + eal*ssq;
	
	m[1][3] = eil2*s;
	m[2][3] = -(c*eil2);
	m[3][3] = 2*eil;
	
	m[1][4] = -(csq*eal) - eil3*ssq;
	m[2][4] = -(cs*eal) + cs*eil3;
	m[3][4] = -(eil2*s);
	m[4][4] = csq*eal + eil3*ssq;
	
	m[1][5] = -(cs*eal) + cs*eil3;
	m[2][5] = -(csq*eil3) - eal*ssq;
	m[3][5] = c*eil2;
	m[4][5] = cs*eal - cs*eil3;
	m[5][5] = csq*eil3 + eal*ssq;
	
	m[1][6] = eil2*s;
	m[2][6] = -(c*eil2);
	m[3][6] = eil;
	m[4][6] = -(eil2*s);
	m[5][6] = c*eil2;
	m[6][6] = 2*eil;

	/*	Fill in the symmetric terms */

	if (fullMatrix)
		fillMatrix(m);

}
