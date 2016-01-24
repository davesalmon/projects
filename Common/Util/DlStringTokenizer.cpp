//+
//	DlStringTokenizer.cpp
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains the string tokenizer class.
//-
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
#include "DlStringUtil.h"
#include "DlStringTokenizer.h"
#include <string.h>
using namespace std;

//---------------------------------- Routines ----------------------------------

//------------------------------------------------------------------------------
//	DlStringTokenizer::nextToken
//
//		return the next token as a string.
//
//	returns				<-	next token
//------------------------------------------------------------------------------
string	
DlStringTokenizer::nextToken()
{
	string	str;
	nextToken(str);
	return str;
}

//------------------------------------------------------------------------------
//	DlStringTokenizer::nextToken
//
//		copy the next token into str and return true if the length was not zero.
//
//	str					<-	copy of next token
//	returns				<-	boolean
//------------------------------------------------------------------------------
bool 
DlStringTokenizer::nextToken(string& str)
{
	str.assign("");
	while (*theString && strchr(theDelims, *theString) == 0)
		str += *theString++;
	skipDelims();
	
	return !str.empty();
}

//------------------------------------------------------------------------------
//	DlStringTokenizer::nextTokenAsDouble
//
//		return the next token as double.
//
//	returns				<-	double
//------------------------------------------------------------------------------
double
DlStringTokenizer::nextTokenAsDouble(bool* status)
{
	string	str;
	if (nextToken(str)) {
		char* endPtr = NULL;
		double d = strtod(str.c_str(), &endPtr);
		if (status)
			*status = *endPtr == 0;
		return d;
	}
	if (status)
		*status = false;
	return 0;
}

//------------------------------------------------------------------------------
//	DlStringTokenizer::nextTokenAsInt
//
//		return the next token as an integer.
//
//	returns				<-	integer
//------------------------------------------------------------------------------
int
DlStringTokenizer::nextTokenAsInt(bool* status)
{
	string	str;
	if (nextToken(str)) {
		char* endPtr = NULL;
		long i = strtol(str.c_str(), &endPtr, 0);
		if (status)
			*status = *endPtr == 0;
		return i;
	}
	if (status)
		*status = false;
	return 0;
}

//------------------------------------------------------------------------------
//	DlStringTokenizer::nextTokenAsBool
//
//		return the next token as a boolean. If the string is "true", or the
//		numeric value is not zero, return true. otherwise false. "true" is not
//		case sensitive.
//
//	returns				<-	boolean
//------------------------------------------------------------------------------
bool
DlStringTokenizer::nextTokenAsBool(bool* status)
{
	string	str;
	if (nextToken(str)) {
		if (status)
			*status = true;
		if (DlStrICmp(str.c_str(), "true") == 0)
			return true;
		
		return strtol(str.c_str(), NULL, 0) != 0;
	}
	if (status)
		*status = false;
	return false;
}
