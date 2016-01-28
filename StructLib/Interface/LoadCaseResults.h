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

#ifndef _H_LoadCaseResults
#define _H_LoadCaseResults

//+--------------------------------- Includes ----------------------------------

#include <valarray>
#include "StrDefines.h"
#include "WorldPoint.h"
#include "DlAssert.h"
#include "LoadCaseCombination.h"

class NodeList;
class NodeImp;
class ElementList;
class StrInputStream;
class StrOutputStream;
class frame_data;

//+--------------------------------- Class -------------------------------------

class ElementForce
{
public:
	ElementForce();
	ElementForce(const DlFloat64 vals[DOF_PER_NODE]);
	
	DlFloat64	operator[](int i) const;
//	DlFloat64	Shear() const;
//	DlFloat64	Moment() const;
	
	static std::size_t	ElemForceSize();
	
	void Read(StrInputStream& inp);
	void Write(StrOutputStream& out) const;
    
    void Add(const ElementForce& frc, DlFloat64 scale);
	
private:
	DlFloat64	_components[DOF_PER_NODE];
};

//+--------------------------------- Class -------------------------------------

class LoadCaseResults
{
public:
	LoadCaseResults(LoadCase lc, const NodeList& nodes, const ElementList& elems);
	LoadCaseResults(LoadCase lc, const LoadCaseCombination& combo, const frame_data& data);
	
	void SetEndForces(DlInt32 index, const ElementForce& force);
	void SetReaction(DlInt32 reactIndex, DlFloat64 value);
	
	void GetDisplacement(const NodeImp* node, DlFloat64 disp[DOF_PER_NODE]) const;
    
    // return the element forces in local element coordinates
	const ElementForce& GetElementForce(DlInt32 index) const;
	
	LoadCase GetLoadCase() const;
	
	// return the number of load cases combined here.
	DlInt32	  GetCombinationCount() const;
	
	// return the factor for the specified load case.
	DlFloat64 GetCombinationFactor(LoadCase which) const;
	
	std::valarray<DlFloat64>& GetDisplacements();
	std::valarray<ElementForce>& GetElementForces();
	std::valarray<DlFloat64>& GetReactions();

	const std::valarray<DlFloat64>& GetDisplacements() const;
//	const std::valarray<ElementForce>& GetElementForces() const;
	const std::valarray<DlFloat64>& GetReactions() const;

	std::size_t	LoadCaseSize() const;
		
	void Read(StrInputStream& inp);
	void Write(StrOutputStream& out) const;

	void InitRatios();
	void SetDispRatio(DlFloat64 dispPerLength);
	void SetShearRatio(DlFloat64 frcPerLength);
	void SetAxialRatio(DlFloat64 frcPerLength);
	void SetMomentRatio(DlFloat64 frcPerLength);

	void SetDisplacementMagnifier(DlFloat64 magnifier);
	DlFloat64 GetDisplacementMagnifier() const;
	
	DlFloat64	DisplacementScale() const;
	DlFloat64	AxialScale() const;
	DlFloat64	ShearScale() const;
	DlFloat64	MomentScale() const;
	
	void 	UpdateDisplacementScale(DlFloat64 s);
	void 	UpdateMomentScale(DlFloat64 s);
	void 	UpdateShearScale(DlFloat64 s);
	void 	UpdateAxialScale(DlFloat64 s);
	

private:

	void addLoadCase(const LoadCaseResults* res, DlFloat64 factor);

	LoadCase					_loadCase;
	
	const LoadCaseCombination*	_combination; // factors for combination
	
	std::valarray<DlFloat64> 	_displacement;
	std::valarray<DlFloat64> 	_reactions;
	std::valarray<ElementForce> _elemForces;
	
	DlFloat64					_dispScale;
	DlFloat64					_momScale;
	DlFloat64					_shearScale;
	DlFloat64					_axialScale;
	DlFloat64					_displacementMagnifier;
};

inline void
LoadCaseResults::UpdateDisplacementScale(DlFloat64 s)
{
	if (_dispScale == 0 || s < _dispScale)
		_dispScale = s;
}

inline void
LoadCaseResults::UpdateMomentScale(DlFloat64 s)
{
	if (_momScale == 0 || s < _momScale)
		_momScale = s;
}

inline void
LoadCaseResults::UpdateShearScale(DlFloat64 s)
{
	if (_shearScale == 0 || s < _shearScale)
		_shearScale = s;
}

inline void
LoadCaseResults::UpdateAxialScale(DlFloat64 s)
{
	if (_axialScale == 0 || s < _axialScale)
		_axialScale = s;
}

inline DlFloat64
LoadCaseResults::GetDisplacementMagnifier() const
{
	return _displacementMagnifier;
}

inline void
LoadCaseResults::SetDisplacementMagnifier(DlFloat64 magnifier)
{
	_displacementMagnifier = magnifier;
}

inline void
LoadCaseResults::SetEndForces(DlInt32 index, const ElementForce& frc)
{
	_DlAssert(index >= 0 && index < _elemForces.size());
	_elemForces[index] = frc;
}

inline void
LoadCaseResults::SetReaction(DlInt32 reactIndex, DlFloat64 value)
{
	_DlAssert(reactIndex > 0 && reactIndex <= _reactions.size());
	_reactions[reactIndex-1] += value;
}

inline const ElementForce&
LoadCaseResults::GetElementForce(DlInt32 index) const
{
	_DlAssert(index >= 0 && index < _elemForces.size());
	return _elemForces[index];
}

inline LoadCase
LoadCaseResults::GetLoadCase() const
{
	return _loadCase;
}

inline DlInt32
LoadCaseResults::GetCombinationCount() const
{
	return _combination ? (DlInt32)_combination->GetFactors().size() : 0;
}

inline DlFloat64
LoadCaseResults::GetCombinationFactor(LoadCase which) const
{
	_DlAssert(_combination && which < _combination->GetFactors().size());
	return _combination->GetFactor(which);
}

inline std::valarray<DlFloat64>&
LoadCaseResults::GetDisplacements() 
{
	return _displacement;
}

inline std::valarray<ElementForce>& 
LoadCaseResults::GetElementForces()
{
	return _elemForces;
}

inline std::valarray<DlFloat64>& 
LoadCaseResults::GetReactions()
{
	return _reactions;
}

inline const std::valarray<DlFloat64>&
LoadCaseResults::GetDisplacements() const
{
	return _displacement;
}

inline const std::valarray<DlFloat64>&
LoadCaseResults::GetReactions() const
{
	return _reactions;
}

inline std::size_t
LoadCaseResults::LoadCaseSize() const {
	return 4 * sizeof(DlInt32) + (_displacement.size() + _reactions.size()) * sizeof(DlFloat64)
			 + _elemForces.size() * ElementForce::ElemForceSize();
}

inline DlFloat64
LoadCaseResults::DisplacementScale() const 
{
	return _dispScale * _displacementMagnifier;
}

inline DlFloat64
LoadCaseResults::AxialScale() const 
{ 
	return _axialScale;
}

inline DlFloat64
LoadCaseResults::ShearScale() const 
{ 
	return _shearScale; 
}

inline DlFloat64
LoadCaseResults::MomentScale() const 
{
	return _momScale; 
}

inline 
ElementForce::ElementForce() 
{
	memset(_components, 0, sizeof(_components));
}

inline 
ElementForce::ElementForce(const DlFloat64 components[DOF_PER_NODE])
{
	memcpy(_components, components, sizeof(_components));
}

inline DlFloat64
ElementForce::operator[](int which) const
{ 
	return _components[which];
}
inline std::size_t
ElementForce::ElemForceSize()
{
	return DOF_PER_NODE * sizeof(DlFloat64);
}

inline void 
ElementForce::Add(const ElementForce& frc, DlFloat64 scale)
{
	if (scale != 0)
    {
		for (int i = 0; i < DOF_PER_NODE; i++)
    		_components[i] += frc._components[i] * scale;
    }
}


#endif

