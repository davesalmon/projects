/*+
 *	File:		DlStringUtil.cpp
 *
 *	Contains:	String Utilities
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 by David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

// ---------------------------- includes --------------------------------------

#include "DlPlatform.h"
#include "DlStringUtil.h"
#include "CFStringTracker.h"
#include "DlCFUtilities.h"

#include <cctype>
#include <vector>

using namespace std;

// ---------------------------- Routines --------------------------------------

//----------------------------------------------------------------------------------------
//  DlStringTrimLeft
//
//      Trim whitespace from the left edge of a string.
//
//  const string& s    -> the string to trim.
//
//  returns string     <- the new string.
//----------------------------------------------------------------------------------------
string
DlStringTrimLeft(const string& s)
{
	string::const_iterator i;
	for (i = s.begin(); i != s.end(); ++i)
		if (!isspace(*i))
			break;
	
	return string(i, s.end());
}

//----------------------------------------------------------------------------------------
//  DlStringTrimRight
//
//      Trim whitespace from the right edge of a string.
//
//  const string& s    -> the string to trim.
//
//  returns string     <- the new string.
//----------------------------------------------------------------------------------------
string
DlStringTrimRight(const string& s)
{
	string::const_iterator i;
	for (i = s.end() - 1; i != s.begin(); --i) {
		if (!isspace(*i)) {
			++i;
			break;
		}
	}
		
	return string(s.begin(), i);
}

//----------------------------------------------------------------------------------------
//  DlStringTrimBoth
//
//      Trim whitespace from both edges of a string.
//
//  const string& s    -> the string to trim.
//
//  returns string     <- the new string.
//----------------------------------------------------------------------------------------
string
DlStringTrimBoth(const string& s)
{
	string::const_iterator i;
	string::const_iterator j = s.end();
	
	for (i = s.begin(); i != j; ++i)
		if (!isspace(*i))
			break;
	
	if (i != j) {
		--j;
		for (; j != i; --j)
			if (!isspace(*j)) {
				++j;
				break;
			}
	}

	return string(i, j);
}

//----------------------------------------------------------------------------------------
//  DlStringSplit
//
//      Split a string using delimits into a vector of parts.
//
//  vector<string>& list    <-> the vector to add strings to.
//  const char* str         -> the string to split.
//  const char* delimits    -> the delimiters used to split the string.
//
//  returns DlInt32         <- number of strings appened to list.
//----------------------------------------------------------------------------------------
DlInt32
DlStringSplit(std::vector<std::string>& list, const char* str, const char* delimits)
{
	DlInt32 count = 0;
	while (true) {
		// find the delimit character
		int len = strcspn(str, delimits);
		if (len > 0) {
			list.push_back(string(str, len));
			count++;
		}
		if (str[len] == 0)
			break;
		str += len + 1;
	}
	return count;
}


#ifndef DlStrICmp
//------------------------------------------------------------------------------
//	DlStrICmp
//
//		case insensitive compare.
//
//	a		-> first string
//	b		-> second string
//	returns	<- (<0) if a < b, 0 if a == b and (>0) if a > b
//------------------------------------------------------------------------------
int
DlStrICmp(const char *a, const char *b)
{
	while(true) {
		unsigned char la = tolower(*a++);
		unsigned char lb = tolower(*b++);
		
		if (la == 0 || lb == 0 || la != lb)
			return la - lb;
	}
}
//------------------------------------------------------------------------------
//	DlStrICmp
//
//		case insensitive compare.
//
//	a		-> first string
//	b		-> second string
//	returns	<- (<0) if a < b, 0 if a == b and (>0) if a > b
//------------------------------------------------------------------------------
int
DlStrNICmp(const char *a, const char *b, size_t n)
{
	while(n--) {
		unsigned char la = tolower(*a++);
		unsigned char lb = tolower(*b++);
		
		if (la == 0 || lb == 0 || la != lb)
			return la - lb;
	}
	return 0;
}

#endif

bool DlEndsWith(const char* s, DlSizeT sLen, const char* end, DlSizeT eLen) {
	if (sLen < eLen)
		return false;
	const char* cPtr = s + sLen - eLen;
	return DlStrNCmp(cPtr, end, eLen) == 0;
}

bool DlEndsWith(const char* s, const char* end) {
	return DlEndsWith(s, strlen(s), end, strlen(end));
}

bool DlEndsWithI(const char* s, DlSizeT sLen, const char* end, DlSizeT eLen) {
	if (sLen < eLen)
		return false;
	const char* cPtr = s + sLen - eLen;
	return DlStrNICmp(cPtr, end, eLen) == 0;
}

bool DlEndsWithI(const char* s, const char* end) {
	return DlEndsWithI(s, strlen(s), end, strlen(end));
}

// eof

