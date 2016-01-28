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

#include "DlPlatform.h"
#include "UnitTable.h"
#include "DlString.h"
#include "DlAssert.h"
#include "StrErrCode.h"
#include <vector>
#include <ctype.h>

//typedef UnitDef	UnitVector[UnitsCount];

// 1MPa = 1 N / mm^2 = 1000 kN / m^2 = 100^2 N / dm^2 = 10.0 kN / dm^2

const UnitVector TheTables[] = {
{
	{1.0, ""},
	{1.0, "L"},			//	to dm
	{1.0, "L"},			//	to dm
	{1.0, "rad"},		//	to rad
	{1.0, "F"},			//	to kN
	{1.0, "F-L"},		//	to kN-dm
	{1.0, "L^2"},		//	to dm^2
	{1.0, "L^4"},		//	to dm^4
	{1.0, "F/L^2"},		//	to kN/dm^2
	{1.0, "F/L"}		// to KN/dm
}, {
	{1.0,								""},
	{(12 * 0.254),						"ft"},		// to dm
	{(0.254),							"in"},		// to dm
	{(1.0),								"rad"},		// to rad
	{(4.44822),							"kip"},		// to kN
	{(12 * 0.254 * 4.44822),			"kip-ft"},	// to kN-dm
	{(2.54 * 0.254),					"in^2"},	// to dm^2
	{(0.254 * 0.254 * 0.254 * 0.254), 	"in^4"},	// to dm^4
	{(4.44822/(0.254 * 0.254)),			"ksi"},		// to kN/dm^2
	{(4.44822/(12 * 0.254)),			"kip/ft"},	// to kN/dm
}, {
	{1.0,							""},
	{(10.0),						"m"},		// to dm
	{(0.01),						"mm"},		// to dm
	{(1.0),							"rad"},		// to rad
	{(1.0),							"kN"},		// to kN
	{(10.0),						"kN-m"},	// to kN-dm
	{(0.0001),						"mm^2"},	// to dm^2
	{(0.00000001),					"mm^4"},	// to dm^4
	{(10.0),						"MPa"},		// to kN/dm^2
	{(0.1),							"kN/m"},	// to kN/dm
}, {
	{1.0,								""},
	{(0.254),							"in"},		// to dm
	{(0.254),							"in"},		// to dm
	{(1.0),								"rad"},		// to rad
	{(4.44822),							"kip"},		// to kN
	{(0.254 * 4.44822),					"kip-in"},	// to kN-dm
	{(0.254 * 0.254),					"in^2"},	// to dm^2
	{(0.254 * 0.254 * 0.254 * 0.254), 	"in^4"},	// to dm^4
	{(4.44822/(0.254 * 0.254)),			"ksi"},		// to kN/dm^2
	{(4.44822/(0.254)),					"kip/in"},	// to kN/dm
}, {
	{1.0,							""},
	{(0.01),						"mm"},		// to dm
	{(0.01),						"mm"},		// to dm
	{(1.0),							"rad"},		// to rad
	{(1.0),							"kN"},		// to kN
	{(0.01),						"kN-mm"},	// to kN-dm
	{(0.0001),						"mm^2"},	// to dm^2
	{(0.00000001),					"mm^4"},	// to dm^4
	{(10.0),						"MPa"},		// to kN/dm^2
	{(100.0),						"kN/mm"},	// to kN/dm
}
};

//typedef std::vector<UnitDef>	UnitVector;
typedef std::vector<const UnitDef*>	UnitsList;

static UnitsList sUnitsList;
UnitTable UnitTable::sCurrentUnits;

//----------------------------------------------------------------------------------------
//  UnitTable::UnitTable                                                      constructor
//
//      Construct the table tracker.
//
//  returns nothing
//----------------------------------------------------------------------------------------
UnitTable::UnitTable() 
	: _unitsDef(0)
{
	for (int i = 0; i < DlArrayElements(TheTables); i++)
	{
		sUnitsList.push_back(TheTables[i]);
//		for (int j = 0; j < UnitsCount; j++)
//			sUnitsList[i].push_back(TheTables[i][j]);
	}
}

//----------------------------------------------------------------------------------------
//  UnitTable::SetUnits
//
//      Set the current default units.
//
//  DlInt32 unitType   -> the unit type
//
//  returns bool       <- true if the specified units exist.
//----------------------------------------------------------------------------------------
bool
UnitTable::SetUnits(DlInt32 unitType)
{
	if (unitType < sUnitsList.size()) {
		sCurrentUnits._unitsDef = unitType;
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
//  UnitTable::AddUnits
//
//      Add new unit definition to the table.
//
//  UnitVector theTable    -> new table.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
UnitTable::AddUnits(UnitVector theTable)
{
//	int index = sUnitsList.size();
	sUnitsList.push_back(theTable);
}

//----------------------------------------------------------------------------------------
//  UnitTable::ToUnits
//
//      Convert from consistent to the specified units.
//
//  const char* title  -> the unit string.
//  UnitType theType   -> the unit type.
//  DlFloat64 value    -> the value to convert.
//
//  returns DlFloat64  <- the converted value.
//----------------------------------------------------------------------------------------
DlFloat64 
UnitTable::ToUnits(const char* title, UnitType theType, DlFloat64 value)
{
	//	search for the correct units
	for (int i = 0; i < sUnitsList.size(); i++)
	{
		if (strcmp(title, sUnitsList[i][theType].title) == 0) {
			return value / sUnitsList[i][theType].factor;
		}
	}

	return value;
}

//----------------------------------------------------------------------------------------
//  UnitTable::FromUnits
//
//      Convert from the specified units to consistent.
//
//  const char* title  -> the unit string.
//  UnitType theType   -> the unit type.
//  DlFloat64 value    -> the value.
//
//  returns DlFloat64  <- converted value.
//----------------------------------------------------------------------------------------
DlFloat64 
UnitTable::FromUnits(const char* title, UnitType theType, DlFloat64 value)
{
	//	search for the correct units
	for (int i = 0; i < sUnitsList.size(); i++)
	{
		if (strcmp(title, sUnitsList[i][theType].title) == 0) {
			return value * sUnitsList[i][theType].factor;
		}
	}

	return value;
}

//----------------------------------------------------------------------------------------
//  UnitTable::ToConsistent
//
//      Convert to consistent units.
//
//  UnitType theType   -> the unit type.
//  DlInt32 theTable   -> the unit table.
//  DlFloat64 value    -> the value.
//
//  returns DlFloat64  <- converted value.
//----------------------------------------------------------------------------------------
DlFloat64 
UnitTable::ToConsistent(UnitType theType, DlInt32 theTable, DlFloat64 value)
{
	return value * GetFactor(theType, theTable);
}

//----------------------------------------------------------------------------------------
//  UnitTable::FromConsistent
//
//      Convert from consistent units.
//
//  UnitType theType   -> the unit type.
//  DlInt32 theTable   -> the unit table.
//  DlFloat64 value    -> the value.
//
//  returns DlFloat64  <- converted value.
//----------------------------------------------------------------------------------------
DlFloat64
UnitTable::FromConsistent(UnitType theType, DlInt32 theTable, DlFloat64 value)
{
	return value / GetFactor(theType, theTable);
}

//	return the unit table index and the unit type containing the string.
//----------------------------------------------------------------------------------------
//  UnitTable::FindUnits
//
//      Find the unit type and table from the string.
//
//  const char* str    -> the string containing the unit string.
//  UnitType& theType  <-> the type (returned).
//
//  returns DlInt32    <- the table or -1 if not found.
//----------------------------------------------------------------------------------------
DlInt32
UnitTable::FindUnits(const char* str, UnitType& theType) 
{
	const char* unitString = strchr(str, ' ');
	if (unitString != NULL) {
		while(*unitString == ' ')
			unitString++;
	} else {
		if (isalpha(*str))
			unitString = str;
	}
	
	if (unitString != NULL && *unitString != 0) {
//		DlInt32 currTable = sCurrentUnits._unitsDef;
		DlInt32 tableSize = sUnitsList.size();
		for (DlInt32 i = 0; i < tableSize; i++) {
			for (DlInt32 j = 0; j < UnitsCount; j++) {
				if (strcmp(unitString, sUnitsList[i][j].title) == 0) {
					theType = (UnitType)j;
					return i;				
				}
			}
		}
//		currTable++;
//		if (currTable == tableSize)
//			currTable = 0;
	}
	return -1;
}

//----------------------------------------------------------------------------------------
//  UnitTable::AppendUnitString
//
//      Append the unit string to the value.
//
//  DlString& str      <-> the string.
//  UnitType theType   -> the unit type.
//  DlInt32 tableId    -> the unit table.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
UnitTable::AppendUnitString(DlString& str, UnitType theType, DlInt32 tableId)
{
	if (tableId == -1)
		tableId = sCurrentUnits._unitsDef;
	_DlAssert(tableId >= 0 && tableId < sUnitsList.size());
	str.Append(' ');
	const char* val = sUnitsList[tableId][theType].title;
	str.Append(val);
}

//----------------------------------------------------------------------------------------
//  UnitTable::FindUnitString
//
//      Find unit table from type and string.
//
//  const char* str    -> the string
//  UnitType theType   -> the type.
//
//  returns DlInt32    <- the table.
//----------------------------------------------------------------------------------------
DlInt32 
UnitTable::FindUnitString(const char* str, UnitType theType)
{
	const char* unitString = strchr(str, ' ');
	if (unitString != NULL) {
		while(*unitString == ' ')
			unitString++;
	} else {
		if (isalpha(*str))
			unitString = str;
	}
	
	if (unitString == NULL || *unitString == 0)
		return ConsistentUnits;

	//	search the tables
	for (int i = 0; i < sUnitsList.size(); i++)
		if (strcmp(unitString, sUnitsList[i][theType].title) == 0) {
			return i;
		}
	
	return -1;
}

//----------------------------------------------------------------------------------------
//  UnitTable::FormatValue
//
//      Format the specified value, optionally including the unit string.
//
//  DlFloat64 value            -> the value to format
//  UnitType units             -> the units
//  const DlFloatFormat& fmt   -> the floating point format.
//  bool wantUnits             -> true to append units.
//
//  returns DlString           <- formatted string.
//----------------------------------------------------------------------------------------
DlString
UnitTable::FormatValue(DlFloat64 value, UnitType units, const DlFloatFormat& fmt, 
		bool wantUnits)
{
	double convValue = FromConsistent(units, sCurrentUnits._unitsDef, value);
	DlString result(convValue, fmt);
	if (wantUnits)
		UnitTable::AppendUnitString(result, units, -1);
	return result;
}

//----------------------------------------------------------------------------------------
//  UnitTable::ParseValue
//
//      Parse the specified value, including units, returning the value in consistent
//		units.
//
//  const char* value  -> the string to parse.
//  UnitType units     -> the units.
//
//  returns DlFloat64  <- 
//----------------------------------------------------------------------------------------
DlFloat64
UnitTable::ParseValue(const char* value, UnitType units)
{
	DlInt32 currTable = GetDefaultTable();
	char* send;
	double dval = strtod(value, &send);
	if (*send != 0)
		currTable = FindUnitString(send, units);
	if (currTable < 0)
		throw DlException("Unrecognized or invalid units in value %s", value);
	return ToConsistent(units, currTable, dval);
}

//----------------------------------------------------------------------------------------
//  UnitTable::ParseIntValue
//
//      Parse the integer value.
//
//  const char* val    -> the string to parse.
//
//  returns DlInt32    <- the int.
//----------------------------------------------------------------------------------------
DlInt32 
UnitTable::ParseIntValue(const char* val)
{
	char* send;
	DlInt32 ival = strtol(val, &send, 0);
	if (*send  != 0)
		throw DlException("Value \"%s\" must be an integer.", val);
	return ival;
}

//----------------------------------------------------------------------------------------
//  UnitTable::ParseUnsignedValue
//
//      Parse the unsigned value.
//
//  const char* val    -> the string to parse.
//
//  returns DlUInt32   <- the unisigned int.
//----------------------------------------------------------------------------------------
DlUInt32 
UnitTable::ParseUnsignedValue(const char* val)
{
	char* send;
	DlUInt32 ival = strtoul(val, &send, 0);
	if (*send  != 0)
		throw DlException("Value \"%s\" must be an unsigned integer.", val);
	return ival;
}

//----------------------------------------------------------------------------------------
//  UnitTable::ParseBoolValue
//
//      Parse the boolean value.
//
//  const char* val    -> the boolean valued string to parse.
//
//  returns DlBoolean  <- the value.
//----------------------------------------------------------------------------------------
DlBoolean 
UnitTable::ParseBoolValue(const char* val)
{
	if (DlStrICmp(val, "t") == 0 || DlStrICmp(val, "y") == 0 || DlStrICmp(val, "true") == 0)
		return true;
	else if (DlStrICmp(val, "f") == 0 || DlStrICmp(val, "n") == 0 || DlStrICmp(val, "false") == 0)
		return false;
	else {
		char* send;
		DlInt32 bval = strtol(val, &send, 0);
		if (*send  != 0)
			throw DlException("Value \"%s\" must be true or false.", val);
		return bval != 0;
	}
}

//----------------------------------------------------------------------------------------
//  UnitTable::GetCurrFactor
//
//      Get the current conversion factor.
//
//  UnitType theType   -> the unit type.
//  DlInt32 theTable   -> the unit table.
//
//  returns DlFloat64  <- the conversion factor to consistent units.
//----------------------------------------------------------------------------------------
DlFloat64
UnitTable::GetCurrFactor(UnitType theType, DlInt32 theTable) const
{
	if (theTable == -1)
		theTable = _unitsDef;
	_DlAssert(theTable >= 0 && theTable < sUnitsList.size());
	return sUnitsList[theTable][theType].factor;
}

//----------------------------------------------------------------------------------------
//  UnitTable::GetCurrTitle
//
//      Get the current unit title.
//
//  UnitType theType       -> the unit type.
//  DlInt32 theTable       -> the table.
//
//  returns const char *   <- the units for the table
//----------------------------------------------------------------------------------------
const char *
UnitTable::GetCurrTitle(UnitType theType, DlInt32 theTable) const
{
	if (theTable == -1)
		theTable = _unitsDef;
	_DlAssert(theTable >= 0 && theTable < sUnitsList.size());
	return sUnitsList[theTable][theType].title;
}



//	eof