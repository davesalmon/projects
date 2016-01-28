/*+
 *	File:		LoadCaseResults.h
 *
 *	Contains:	class to track results for a load case
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#include "DlPlatform.h"
#include "LoadCaseResults.h"
#include "DlException.h"
#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "NodeList.h"
#include "NodeImp.h"
#include "ElementList.h"
#include "frame_data.h"

//----------------------------------------------------------------------------------------
//  LoadCaseResults::LoadCaseResults                                          constructor
//
//      Construct results object for a load case.
//
//  LoadCase lc                -> the load case.
//  const NodeList& nodes      -> the nodes.
//  const ElementList& elems   -> the elements.
//
//  returns nothing
//----------------------------------------------------------------------------------------
LoadCaseResults::LoadCaseResults(LoadCase lc, const NodeList& nodes, const ElementList& elems)
	: _loadCase(lc)
	, _displacement(0.0, nodes.GetEquationCount())
	, _reactions(0.0, nodes.GetReactionCount())
	, _elemForces(ElementForce(), elems.Length())
	, _dispScale(0)
	, _momScale(0)
	, _axialScale(0)
	, _shearScale(0)
	, _displacementMagnifier(1.0)
{
}

LoadCaseResults::LoadCaseResults(LoadCase lc, const LoadCaseCombination& combo, const frame_data& data)
	: _loadCase(lc)
	, _dispScale(0)
	, _momScale(0)
	, _axialScale(0)
	, _shearScale(0)
	, _displacementMagnifier(1.0)
	, _combination(&combo)
{
	const LoadCaseResults* base = nullptr;
	for (int i = 0; i < data.CountResults(); i++) {
		if ((base = data.GetResults(i)) != nullptr) {
			break;
		}
	}
	
	_displacement.resize(base->_displacement.size());
	_reactions.resize(base->_reactions.size());
	_elemForces.resize(base->_elemForces.size());

	const vector<DlFloat32>& factors = combo.GetFactors();
		 
	for (int i = 0; i < factors.size(); i++) {
		if (fabs(factors[i]) > 1.0e-6) {
			addLoadCase(data.GetResults(i), factors[i]);
		}
	}
}

//----------------------------------------------------------------------------------------
//  LoadCaseResults::GetDisplacement
//
//      Return displacements for this result at the specified node.
//
//  const NodeImp* nd              -> the node
//  DlFloat64 disp[DOF_PER_NODE]   -> array of displacement values.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
LoadCaseResults::GetDisplacement(const NodeImp* nd, DlFloat64 disp[DOF_PER_NODE]) const
{
	for (DlInt32 i = 0; i < DOF_PER_NODE; i++)
	{
		DlInt32 eq = nd->GetEquationNumber(i);
		if (eq > 0)
			disp[i] = _displacement[eq-1];
		else {
			NodeLoadImp* imp = nd->GetLoad(_loadCase);
			if (imp && imp->GetType(i) == NodeLoadIsDisp)
				disp[i] = imp->GetValue(i);
			else
				disp[i] = 0;
		}
	}
}

//----------------------------------------------------------------------------------------
//  LoadCaseResults::AddLoadCase
//
//      Add the given load case multiplied by factor to this one
//
//  const LoadCaseResults& res -> the load case to add
//  DlFloat64 factor           -> the factor.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
LoadCaseResults::addLoadCase(const LoadCaseResults* res, DlFloat64 factor)
{
	_displacement += res->_displacement * factor;
	_reactions += res->_reactions * factor;
	
	for (int i = 0; i < _elemForces.size(); i++)
    	_elemForces[i].Add(res->_elemForces[i], factor);
	
	UpdateDisplacementScale(fabs(res->DisplacementScale() / factor));
	UpdateMomentScale(fabs(res->MomentScale() / factor));
	UpdateShearScale(fabs(res->ShearScale() / factor));
	UpdateAxialScale(fabs(res->AxialScale() / factor));
}

//----------------------------------------------------------------------------------------
//  LoadCaseResults::Read
//
//      Read the result from a file.
//
//  StrInputStream& inp    -> the input file stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
LoadCaseResults::Read(StrInputStream& inp)
{
	_loadCase = inp.GetInt();
	DlInt32 numElems = _displacement.size();
	if (numElems != inp.GetInt())
		throw DlException("Results file does not match current structure.");
	
	for (DlInt32 i = 0; i < numElems; i++)
		_displacement[i] = inp.GetDouble();
	
	numElems = _reactions.size();
	if (numElems != inp.GetInt())
		throw DlException("Results file does not match current structure.");
	
	for (DlInt32 i = 0; i < numElems; i++)
		_reactions[i] = inp.GetDouble();
	
	numElems = _elemForces.size();
	if (numElems != inp.GetInt())
		throw DlException("Results file does not match current structure.");
	
	for (DlInt32 i = 0; i < numElems; i++)
		_elemForces[i].Read(inp);
}

//----------------------------------------------------------------------------------------
//  LoadCaseResults::Write
//
//      Write the result to a file.
//
//  StrOutputStream& out   -> output stream
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
LoadCaseResults::Write(StrOutputStream& out) const
{
	out.PutInt(_loadCase);
	DlInt32 numElems = _displacement.size();
	out.PutInt(numElems);
	for (DlInt32 i = 0; i < numElems; i++)
		out.PutDouble(_displacement[i]);
	numElems = _reactions.size();
	out.PutInt(numElems);
	for (DlInt32 i = 0; i < numElems; i++)
		out.PutDouble(_reactions[i]);
	numElems = _elemForces.size();
	out.PutInt(numElems);
	for (DlInt32 i = 0; i < numElems; i++)
		_elemForces[i].Write(out);
}

//----------------------------------------------------------------------------------------
//  setAnyScale                                                                    inline
//
//      set the scale factor, if the new scale factor is smaller than the current.
//
//  DlFloat64 valPerLength -> the computed value per length.
//  DlFloat64 ratio        -> the target value per length. 
//  DlFloat64& val         -> the current scale value
//
//  returns DlFloat64      <- the new scale value.
//----------------------------------------------------------------------------------------
inline DlFloat64 
setAnyScale(DlFloat64 valPerLength, DlFloat64 ratio, const DlFloat64& val)
{
	if (valPerLength > 0) {
		DlFloat64 scaleFact = ratio / valPerLength;
		return scaleFact < val || val == 0 ? scaleFact : val;
	}
	return val;
}

//----------------------------------------------------------------------------------------
//  LoadCaseResults::InitRatios
//
//      Intialize scale factor ratios.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
LoadCaseResults::InitRatios() 
{
	_dispScale = _axialScale = _shearScale = _momScale = 0.0;
}

//----------------------------------------------------------------------------------------
//  LoadCaseResults::SetDispRatio
//
//      Set the scale factor for displacements.
//
//  DlFloat64 dispPerLength    -> the computed displacement per element length.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
LoadCaseResults::SetDispRatio(DlFloat64 dispPerLength) 
{
	_dispScale = setAnyScale(dispPerLength, 0.25, _dispScale);
}

//----------------------------------------------------------------------------------------
//  LoadCaseResults::SetAxialRatio
//
//      Set the scale factor for forces.
//
//  DlFloat64 frcPerLength -> the computed force per element length.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
LoadCaseResults::SetAxialRatio(DlFloat64 frcPerLength) 
{
	_axialScale = setAnyScale(frcPerLength, 0.3, _axialScale);
}

//----------------------------------------------------------------------------------------
//  LoadCaseResults::SetShearRatio
//
//      Set the scale factor for forces.
//
//  DlFloat64 frcPerLength -> the computed force per element length.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
LoadCaseResults::SetShearRatio(DlFloat64 frcPerLength) 
{
	_shearScale = setAnyScale(frcPerLength, 0.3, _shearScale);
}

//----------------------------------------------------------------------------------------
//  LoadCaseResults::SetMomentRatio
//
//      Set the scale factor for Moment.
//
//  DlFloat64 frcPerLength -> the computed moment per element length.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
LoadCaseResults::SetMomentRatio(DlFloat64 frcPerLength) 
{
	_momScale = setAnyScale(frcPerLength, 0.3, _momScale);
}

//----------------------------------------------------------------------------------------
//  ElementForce::Read
//
//      Read an element force.
//
//  StrInputStream& inp    -> the input stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementForce::Read(StrInputStream& inp)
{
    for (int i = 0; i < DOF_PER_NODE; i++)
		_components[i] = inp.GetDouble();
}

//----------------------------------------------------------------------------------------
//  ElementForce::Write
//
//      Write an element force.
//
//  StrOutputStream& out   -> the output stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ElementForce::Write(StrOutputStream& out) const
{
    for (int i = 0; i < DOF_PER_NODE; i++)
		out.PutDouble(_components[i]);
}

// eof

