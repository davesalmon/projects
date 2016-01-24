//+
//	DlParseArgs.h
//
//	Copyright © 2004 David C. Salmon. All Rights Reserved
//
//	Contains an argument parser.
//
//	Given a list ofargument specifier records, DlParseArgs checks the command
//	line entries against the specifier records. Entries which are missing or
//	are not the proper type generate an exception.
//
//	4 types of argument are defined:
//		DlParsArgTypeBool:	the argument is expected to be a simple flag with
//			no associated value. like -q
//		DlParseArgTypeInt:	the argument is expected to be an integer value.
//			like -i 4 or -i4
//		DlParseArgTypeFloat:	a floating point value is expected. -q 1.2
//		DlParseArgTypeString:	a string is expected.
//
//		For all types except DlParsArgTypeBool, the DlParseArgTypeOptional
//		bit may be set, marking the value as optional. In this case no 
//		exception will be thrown if the argument is missing.
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

#ifndef _H_DlParseArgs
#define _H_DlParseArgs
//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"
#include <cstdio>

enum {
	 DlParseArgTypeBool			= (1 << 0)
	, DlParseArgTypeOptional	= DlParseArgTypeBool
	, DlParseArgTypeInt			= (1 << 1)
	, DlParseArgTypeFloat		= (1 << 2)
	, DlParseArgTypeString		= (1 << 3)
};

const int DlParseArgPositional = -1;

typedef struct DlParseArgSpecifier {
	const char*		specifier;		// the argument specifier "-i", "-include"
    								// specify 0 for positional params.
	DlUInt32	 	argType;		// the type of argument expected.
	const char*		description;	// the description of the argument for usage.
	
	bool			argFound;		// set to true if the argument was found/
	const char*		value;			// set to the argument string.
	union {
		void*			voidValue;
		DlFloat64*		floatValue;
		DlInt32*		intValue;
		bool*			boolValue;
		const char**	stringValue;
	} argValue;						// pointer to variable to hold result.
    								// this may be NULL.
} DlParseArgSpecifier;

int DlParseArgs(int argc, const char* argv[], DlParseArgSpecifier* spec, 
                			int count);

void DlParseArgsPrintUsage(const char* program, const char* message, 
                           DlParseArgSpecifier* spec, int count, 
                           std::FILE* f = stderr);

#endif
