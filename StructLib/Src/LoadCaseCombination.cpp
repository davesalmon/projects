/*+
 *
 *  LoadCaseCombination.cpp
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  track load case combinations.
 *
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

#include "LoadCaseCombination.h"
#include "StrInputStream.h"
#include "StrOutputStream.h"

//----------------------------------------------------------------------------------------
//  LoadCaseCombination::LoadCaseCombination                                  constructor
//
//      construct load case combinations.
//
//  returns nothing
//----------------------------------------------------------------------------------------
LoadCaseCombination::LoadCaseCombination()
{
}

//----------------------------------------------------------------------------------------
//  LoadCaseCombination::LoadCaseCombination                                  constructor
//
//      construct load case combinations from stream.
//
//  StrInputStream& s  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
LoadCaseCombination::LoadCaseCombination(StrInputStream& s)
{
	DlUInt32 count = s.GetInt();
	
	for (int i = 0; i < count; i++) {
		_factors.push_back(s.GetFloat());
	}
}

//----------------------------------------------------------------------------------------
//  LoadCaseCombination::Write
//
//      write load case combinations.
//
//  StrOutputStream& output    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
LoadCaseCombination::Write(StrOutputStream& output) const
{
	output.PutInt(_factors.size());
	for (DlFloat32 f : _factors) {
		output.PutFloat(f);
	}
}
