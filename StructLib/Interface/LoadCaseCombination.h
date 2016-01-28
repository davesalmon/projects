/*+
 *	File:		LoadCaseCombination.h
 *
 *	Contains:	Interface for combining load case results.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2015 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#ifndef StructLib_LoadCaseCombination_h
#define StructLib_LoadCaseCombination_h

#include "DlString.h"
#include "StrDefines.h"
#include "DlAssert.h"

class StrInputStream;
class StrOutputStream;

class LoadCaseCombination
{
public:
	LoadCaseCombination();
	LoadCaseCombination(std::vector<DlFloat32>&& f);
	LoadCaseCombination(StrInputStream& s);
	
	const std::vector<DlFloat32>& GetFactors() const;
	
	float GetFactor(LoadCase lc) const;
	void SetFactor(LoadCase lc, float factor);
	
//	void 		Read(StrInputStream& input);
	void		Write(StrOutputStream& output) const;

private:
	
	vector<DlFloat32>		_factors;
};

inline
LoadCaseCombination::LoadCaseCombination(std::vector<DlFloat32>&& f)
	: _factors(f)
{
}

inline const std::vector<DlFloat32>&
LoadCaseCombination::GetFactors() const
{
	return _factors;
}

inline float
LoadCaseCombination::GetFactor(LoadCase lc) const
{
	_DlAssert(lc < _factors.size());
	return _factors[lc];
}

inline void
LoadCaseCombination::SetFactor(LoadCase lc, float factor)
{
	// expand the factor array to accomodate size elements
	while (lc >= _factors.size()) {
		_factors.push_back(0.0);
	}
	
	_factors[lc] = factor;
}

#endif
