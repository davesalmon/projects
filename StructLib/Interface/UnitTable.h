/*+
 *	File:		UnitTable.h
 *
 *	Contains:	Units for structures
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

#ifndef _H_UnitTable
#define _H_UnitTable

#include "DlTypes.h"

typedef enum {
	UnitsNone			= 0,
	UnitsLength			= 1,
	UnitsDisplacement	= 2,
	UnitsRotation		= 3,
	UnitsForce			= 4,
	UnitsMoment			= 5,
	UnitsArea			= 6,
	UnitsMOI			= 7,
	UnitsModulus		= 8,
	UnitsDistribForce 	= 9,
	UnitsCount
}	UnitType;

//	predefined unit types
enum {
	ConsistentUnits,
	EnglishUnits,
	SIUnits
};

typedef struct UnitDef
{
	DlFloat64	factor;
	const char* title;
}				UnitDef;

typedef const UnitDef	UnitVector[UnitsCount];

//	unit string maximum length
const DlInt32	kMaxUnitLength = 8;

class DlString;
class DlFloatFormat;

class	UnitTable {

public:
	static bool SetUnits(DlInt32 unitType);
	static void AddUnits(UnitVector theTable);

	static const char* 	GetTitle(UnitType theType, DlInt32 theTable = -1) { return sCurrentUnits.GetCurrTitle(theType, theTable);}
	static DlFloat64	GetFactor(UnitType theType, DlInt32 theTable = -1) { return sCurrentUnits.GetCurrFactor(theType, theTable);}

	static DlFloat64	ToConsistent(UnitType theType, DlInt32 theTable, DlFloat64 value);
	static DlFloat64	FromConsistent(UnitType theType, DlInt32 theTable, DlFloat64 value);

	//	convert the consistent units value to units with unit type and title
	static DlFloat64 	ToUnits(const char* title, UnitType theType, DlFloat64 value);
	//	convert the value with unit type and title to consistent
	static DlFloat64 	FromUnits(const char* title, UnitType theType, DlFloat64 value);

	//	return the unit table index and the unit type containing the string.
	static DlInt32 		FindUnits(const char* unitString, UnitType& theType);
	
	static void		AppendUnitString(DlString& str, UnitType theType, DlInt32 table);
	static DlInt32	FindUnitString(const char* str, UnitType theType);
	
	static DlInt32	GetDefaultTable() { return sCurrentUnits._unitsDef; }
	
	static DlString		FormatValue(DlFloat64 value, UnitType units, const DlFloatFormat& fmt,
							DlBoolean wantUnits=true);
	static DlFloat64	ParseValue(const char* value, UnitType units);
	static DlInt32		ParseIntValue(const char* value);
	static DlUInt32		ParseUnsignedValue(const char* value);
	static DlBoolean	ParseBoolValue(const char* value);
	
private:
	UnitTable();

	DlFloat64 		GetCurrFactor(UnitType theType, DlInt32 theTable) const;
	const char* 	GetCurrTitle(UnitType theType, DlInt32 theTable) const;

	DlInt32				_unitsDef;
	
	static UnitTable 	sCurrentUnits;	//	the current units
	
};

#endif

//	eof