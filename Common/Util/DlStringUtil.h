/*+
 *	File:		DlStringUtil.h
 *
 *	Contains:	String functions
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT © 1990-96 by David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#ifndef _H_DlStringUtil
#define _H_DlStringUtil

/* ---------------------------- Includes ----------------------------------- */

#include "DlMacros.h"
#include <string>
#include <vector>

#define DlStrCmp	strcmp
#define DlStrNCmp	strncmp

/* ---------------------------- Declares ----------------------------------- */

class DlCStringKeyLess {
public:
	bool operator() (const char* s1, const char* s2) const
	{
		if (s1 == s2)
			return false;
		return DlStrCmp(s1, s2) < 0;
	}
};

/// Trim whitespace from the start of the string.
/// @param s The string to trim.
/// @return The trimmed string.
std::string			DlStringTrimLeft(const std::string& s);

/// Trim whitespace from the end of the string.
/// @param s The string to trim.
/// @return The trimmed string.
std::string			DlStringTrimRight(const std::string& s);

/// Trim whitespace from the start and of the string.
/// @param s The string to trim.
/// @return The trimmed string.
std::string			DlStringTrimBoth(const std::string& s);

/// Create a vector of strings by splitting the specifed string at any character in the 
/// specified set.
/// @param list Strings found are appended to this list.
/// @param str The input string to split.
/// @param delimits The set of characters to use for split.
/// @return The number of string added to list.
DlInt32				DlStringSplit(std::vector<std::string>& list, const char* str, 
						const char* delimits);

#ifndef DlStrICmp
/// Compare strings case-insensitively.
/// @param a First string.
/// @param b Second string.
/// @return value is < 0 if a < b, > 0 if a > b, and == 0 if a == b.
int			DlStrICmp(const char* a, const char* b);
#endif

#ifndef DlStrNICmp
/// Compare strings case-insensitively. the size indicates the maximum number of 
///	characters to compare.
/// @param a First string.
/// @param b Second string.
/// @param n The maximum number of characters to compare.
/// @return value is < 0 if a < b, > 0 if a > b, and == 0 if a == b.
int			DlStrNICmp(const char* a, const char* b, size_t n);
#endif

/// Compare strings case-insensitively.
/// @param a First string.
/// @param b Second string.
/// @return value is < 0 if a < b, > 0 if a > b, and == 0 if a == b.
inline int	
DlStrICmp(const std::string& a, const std::string& b) { return DlStrICmp(a.c_str(), b.c_str()); }


/// Return true if the last chars in s correspond to end.
/// @param s the string to check.
/// @param sLen the length of s.
/// @param end the end we want.
/// @param eLen the lenth of end.
/// @return true if the end of s is end.
bool DlEndsWith(const char* s, DlSizeT sLen, const char* end, DlSizeT eLen);

/// Return true if the last chars in s correspond to end without regard to case.
/// @param s the string to check.
/// @param sLen the length of s.
/// @param end the end we want.
/// @param eLen the lenth of end.
/// @return true if the end of s is end.
bool DlEndsWithI(const char* s, DlSizeT sLen, const char* end, DlSizeT eLen);

/// Return true if the last chars in s correspond to end.
/// @param s the string to check.
/// @param end the end we want.
/// @return true if the end of s is end.
bool DlEndsWith(const char* s, const char* end);

/// Return true if the last chars in s correspond to end without regard to case.
/// @param s the string to check.
/// @param end the end we want.
/// @return true if the end of s is end.
bool DlEndsWithI(const char* s, const char* end);

#if TARG_OS_MAC

inline std::string	DlPStringToString(ConstStringPtr ps) {
						return std::string((const char*)ps + 1, ps[0]);
					}

inline void			DlPStringToString(ConstStringPtr ps, std::string& s) {
						s.assign((const char*)ps + 1, ps[0]);
					}

inline StringPtr	DlStringToPString(const std::string& s, Str255 ps) {
						ps[0] = s.length(); 
						DlMemCopy(s.c_str(), ps + 1, ps[0]); 
						return ps;
					}

inline StringPtr	DlStringToPString(const char* s, Str255 ps) {
						ps[0] = strlen(s); 
						DlMemCopy(s, ps + 1, ps[0]); 
						return ps;
					}

#endif


#endif

