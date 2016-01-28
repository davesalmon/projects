/*+
 *	File:		PrismElement.h
 *
 *	Contains:	Interface for prismatic element implementation.
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

#ifndef _H_PrismElement
#define _H_PrismElement

#include "ElementImp.h"
#include "DlString.h"
#include "ElementFactory.h"

//---------------------------------- Class -------------------------------------

class NodeImp;
class PropertyImp;
class PropertyTypeList;
class UnitTable;

struct ElementCompData;

class	PrismElement : public ElementImp
{

	const int AXIAL_COMPONENT = 0;
	const int SHEAR_COMPONENT = 1;
	const int MOMENT_COMPONENT = 2;

public:
	
	PrismElement(NodeImp* startNode, NodeImp* endNode, PropertyImp* property);
	PrismElement(StrInputStream& inp, const frame_data& data);
			
	virtual const char* GetType() const { return sElemType; }
	virtual void Stiffness(DlArray::DlMatrix<DlFloat64>& m, bool fullMatrix) const;
	virtual bool FixedEndForces(DlFloat64 loads[2*DOF_PER_NODE], LoadCase loadCase) const;

//	virtual void Read(StrInputStream& inp, const frame_data& data);
	virtual void Write(StrOutputStream& out, const frame_data& data) const;

	//
	//	Element forces
	//
	virtual const PropertyType* GetForceTypeForDOF(DlInt32 dof) const;
    // return the element forces at where in load case lc
	virtual void GetElementForce(double where, const ElementForce& feForce, LoadCase lc, ElementForce& force) const;
	virtual ElementForce GetLocalForce(const DlFloat64 globalForces[DOF_PER_NODE]) const;
    
	// returns a diagram for the force for DOF dof
	virtual DlInt32 ForceDOFCoords(DlInt32 dof, DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& res) const;
	virtual DlFloat64 MaxForcePerLength(DlInt32 dof, DlInt32 index, const LoadCaseResults& res) const;
	
	virtual DlFloat64 DOFForceAt(DlInt32 dof, DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const;
	
	//
	//	Element displacement
	//
	// return coordinate lists for the results suitable for plotting on the structure.
	virtual const PropertyType* GetDispTypeForDOF(DlInt32 dof) const;
	virtual DlInt32 DisplacedCoords(DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& results) const;
    
	// return the maximum absolute values for result values.
	virtual DlFloat64 MaxDisplacementPerLength(DlInt32 index, const LoadCaseResults& res) const;
	
	// currently only called by structure-test.
	virtual DlFloat64 DOFDispAt(DlInt32 dof, DlInt32 index, DlFloat64 u, const LoadCaseResults& res) const;

	//
	// Element results for table
	//
	
    // return one of the result values. these values are defined per element type.
    //	the list of values is returned using GetResultType().
	virtual DlInt32 CountResultTypes() const;
	virtual const PropertyType* GetResultType(DlInt32 whichType) const;
	virtual DlFloat64 GetResultValue(int which, const ElementForce& feForce, const LoadCaseResults& res) const;
	
public:
	//
	// static methods for element creation
	//
	
	//	create an element of this type
	static ElementImp* CreateElement(NodeImp* startNode, NodeImp* endNode, 
							PropertyImp* property);

	//	create an element of this type
	static ElementImp* ReadElement(StrInputStream& inp, const frame_data& data);

	//	get the properties for elements of this type
	//static PropertyImp* CreateProperties(const char* defaultTitle);
	
	//	get all the property types for this element type
	static const PropertyTypeList* GetPropertyTypes();
    
    // return the property types for the results.
    static const PropertyTypeList* GetResultTypes();
    
	// return the name of this element type
	static const char* GetElementType() { return sElemType; }

	static const ElementFactory::ElementProcs& GetProcs() { return sElementProcs; }
	
protected:

	static const PropertyTypeList* CreatePropertyTypes();
	static const PropertyTypeList* CreateResultTypes();
	
private:
    

//	class Registrar {
//	public:
//		Registrar();
//	};
	
	const static ElementFactory::ElementProcs sElementProcs;

	DlFloat64 computePinnedEndSlope(const ElementCompData& data, const LoadCaseResults& results) const;

	void computeElementData(const ElementForce& frc, const LoadCaseResults& results, ElementCompData& data) const;

	DlFloat64 LateralDisplacementAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const;
	DlFloat64 AxialDisplacementAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const;
	DlFloat64 SlopeDisplacementAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const;

	virtual DlInt32	AxialCoords(DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& res) const;
	virtual DlInt32	MomentCoords(DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& res) const;
	virtual DlInt32	ShearCoords(DlInt32 index, PointEnumeratorImp* pts, const LoadCaseResults& res) const;

	DlFloat64 MaxAxialPerLength(DlInt32 index, const LoadCaseResults& res) const;
	DlFloat64 MaxMomentPerLength(DlInt32 index, const LoadCaseResults& res) const;
	DlFloat64 MaxShearPerLength(DlInt32 index, const LoadCaseResults& res) const;	 

    // return the axial force in the elem index at u (0->1) relative to node 1.
    DlFloat64 AxialForceAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const;
    DlFloat64 ShearForceAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const;
    DlFloat64 MomentAt(const ElementForce& frc, DlFloat64 u, const LoadCaseResults& res) const;
    DlFloat64 MaxMoment(const ElementForce& frc, DlFloat64& x, const LoadCaseResults& res) const;
	
    DlFloat64 maxAxial(const ElementForce& frc, const LoadCaseResults& res) const;
    DlFloat64 maxShear(const ElementForce& frc, const LoadCaseResults& res) const;
    DlFloat64 maxMoment(const ElementForce& frc, const LoadCaseResults& res) const;

	static const char*		sElemType;
//	static Registrar		sRegistrar;

	
//	elementImplementation * 
};

#endif

