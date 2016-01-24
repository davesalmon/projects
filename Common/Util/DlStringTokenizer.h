/*
 *  DlStringTokenizer.h
 *
 *  Copyright (c) 2001 David C. Salmon. All rights reserved.
 *
 *	class to return tokens from string.
 */
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

#ifndef _H_DlStringTokenizer
#define _H_DlStringTokenizer

#include <string>

using namespace std;

class	DlStringTokenizer
{
public:
	DlStringTokenizer(const char * str, const char * delims)
		: theString(str)
		, theDelims(delims) {skipDelims();}
	
	bool	hasMoreTokens() const { return *theString != 0; }
	
	string	nextToken();
	
	bool nextToken(string& str);
	
	double nextTokenAsDouble(bool* status = NULL);
	
	int	nextTokenAsInt(bool* status = NULL);
	
	bool nextTokenAsBool(bool* status = NULL);

private:
	
	void skipDelims() {
		//	skip over tokens
		while( *theString && strchr(theDelims, *theString) != 0)
			theString++;
	}
	
	const char * 	theString;
	const char *	theDelims;
};


#endif
