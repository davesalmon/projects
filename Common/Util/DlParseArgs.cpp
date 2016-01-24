//+
//	DlParseArgs.cpp
//
//	Copyright © 2004 David C. Salmon. All Rights Reserved
//
//	Contains an argument parser.
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

#include "DlPlatform.h"
#include "DlParseArgs.h"

#include "DlException.h"

#include <cstring>
#include <cstdlib>

static DlParseArgSpecifier* findArg(const char* argType, DlParseArgSpecifier* spec, int count);
static void processArg(DlParseArgSpecifier* spec);

//----------------------------------------------------------------------------------------
//  GetArgType                                                                     inline
//
//      return the argument type.
//
//  const DlParseArgSpecifier& arg   -> the argument record.
//
//  returns DlUInt32   				  <- the actual type.
//----------------------------------------------------------------------------------------
inline DlUInt32
GetArgType(const DlParseArgSpecifier& arg) {
	return (arg.argType) &~DlParseArgTypeOptional;
}

//----------------------------------------------------------------------------------------
//  IsOptional                                                                     inline
//
//      return true if the argument is optional.
//
//  const DlParseArgSpecifier& arg   -> the argument record
//
//  returns bool       <- true if optional
//----------------------------------------------------------------------------------------
inline bool
IsOptional(const DlParseArgSpecifier& arg) {
	return (arg.argType & DlParseArgTypeOptional) != 0;
}

//----------------------------------------------------------------------------------------
//  DlParseArgs
//
//      parse the command line arguments.
//
//  int argc                   -> the number of arguments.
//  const char* argv[]         -> the argument list.
//  DlParseArgSpecifier* spec  -> the specifier list.
//  int count                  -> the number of specifiers.
//
//  returns int                <- the last argument processed.
//----------------------------------------------------------------------------------------
int 
DlParseArgs(int argc, const char* argv[], DlParseArgSpecifier* spec, int count) 
{
	int i;
	for (i = 0; i < count; i++) {
		spec[i].argFound = false;
		spec[i].value = NULL;
	}
	
	for (i = 1; i < argc; i++) {
	
		DlParseArgSpecifier* curr;
		const char* theArg = argv[i];
		
		if (theArg[0] == '-') {
			curr = findArg(theArg, spec, count);
			if (curr->value == 0 && curr->argType != DlParseArgTypeBool) {
				++i;
				if (i == argc)
					throw DlException("Missing value for %s.", theArg);
				curr->value = argv[i];
			}
		} else {
			curr = findArg(0, spec, count);
			if (curr == 0)
				break;
			curr->value = theArg;
		}
	}
	
	for (int j = 0; j < count; j++) {
		processArg(spec + j);
	}
	
	return i;
}

//----------------------------------------------------------------------------------------
//  DlParseArgsPrintUsage
//
//      print the usage string.
//
//  const char* program        -> the name of the program.
//  const char* message        -> the global message.
//  DlParseArgSpecifier* spec  -> the argument specifiers.
//  int count                  -> the number of specifiers.
//  std::FILE* f               -> the file to write to. default value is stderr.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
DlParseArgsPrintUsage(const char* program, const char* message, DlParseArgSpecifier* spec, int count, std::FILE* f) 
{    
	std::fprintf(f, "%s", program ? program : "");
	
	int lineLen = 0;
	int specLen = strlen("<param>");
	for (int i = 0; i < count; i++) {
		char argBuf[256];
		argBuf[0] = 0;
		
		strcat(argBuf, " ");
		if (IsOptional(spec[i]))
			strcat(argBuf, "[ ");
		
		if (spec[i].specifier) {
			int tempLen = strlen(spec[i].specifier);
			if (tempLen > specLen)
				specLen = tempLen;
		
			strcat(argBuf, spec[i].specifier);
			
			switch(GetArgType(spec[i])) {
			case DlParseArgTypeInt:		strcat(argBuf, " <int>");	break;
			case DlParseArgTypeFloat:	strcat(argBuf," <float>");	break;
			case DlParseArgTypeString:	strcat(argBuf," <string>");	break;
			case 0: strcat(argBuf, " "); break;	//	type bool
			default: strcat(argBuf, " <bad>"); break;
			}
		} else {
			strcat(argBuf, "<param>");
		}
			
		if (IsOptional(spec[i]))
			strcat(argBuf, " ]");
		
		int argLen = strlen(argBuf);
		if (argLen + lineLen > 80) {
			fprintf(f, "\n");
			lineLen = 0;
		}
		::fputs(argBuf, f);
//		fprintf(f, argBuf);
		lineLen += argLen;
	}
	
	fprintf(f, "\n");
	fflush(f);
	
	if (message != 0)
		fprintf(f, "%s\n\n", message);
	
	specLen += 2;
	
	//	now print the descriptions
	for (int i = 0; i < count; i ++) {
		const char* paramName = spec[i].specifier ? spec[i].specifier : "<param>";
		fprintf(f, "%s", paramName);
		int remLen = specLen - strlen(paramName);
		for (int j = 0; j < remLen; j++) fputc(' ', f);
		fprintf(f, "%s.\n", spec[i].description ? spec[i].description : "no description");
	}
}


//----------------------------------------------------------------------------------------
//  argComp                                                                        static
//
//      compare arguments.
//
//  const char* s1 -> 
//  const char* s2 -> 
//
//  returns int    <- 
//----------------------------------------------------------------------------------------
static int
argComp(const char* s1, const char* s2) 
{
	const char* saveS = s1;
	while (*s1 && *s2) {
		if (*s1++ != *s2++)
			return -1;
	}
	return s1 - saveS;
}

//----------------------------------------------------------------------------------------
//  findArg                                                                        static
//
//      find the specified argument.
//
//  const char* theValue           -> 
//  DlParseArgSpecifier* spec      -> 
//  int count                      -> 
//
//  returns DlParseArgSpecifier*   <- 
//----------------------------------------------------------------------------------------
static DlParseArgSpecifier* 
findArg(const char* theValue, DlParseArgSpecifier* spec, int count) 
{
	//	run through all the unprocessed args
	for (int i = 0; i < count; i++, spec++) {
		if (!spec->argFound) {
			//	theValue == 0 for positional params. argComp returns -1 for not found.
			//	specifier is also null for positional params
			if (spec->specifier && theValue) {
				int compLen = argComp(spec->specifier, theValue);
				if (compLen != -1) {
					spec->value = theValue[compLen] == 0 ? 0 : theValue + compLen;
					spec->argFound = true;
					return spec;
				}
			} else if (!spec->specifier && !theValue) {
				spec->value = 0;
				spec->argFound = true;
				return spec;
			}
		}
	}
	if (theValue)
		throw DlException("Invalid argument %s.", theValue);
	return 0;
}

//----------------------------------------------------------------------------------------
//  processArg                                                                     static
//
//      process the argument.
//
//  DlParseArgSpecifier* spec  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void 
processArg(DlParseArgSpecifier* spec) 
{
	if (!spec->argFound) {
		//	if we did not find it and its optional, then ok. Otherwise throw.
		if (IsOptional(*spec))
			return;
		if (spec->specifier)
			throw DlException("Required argument %s not found.", spec->specifier);
		else
			throw DlException("Required parameter not provided.");
	} else {
		DlUInt32 theType = GetArgType(*spec);
		//	we found it. parse the value
		if (theType == 0) {
			//	check value provided for none
			if (spec->value)
				throw DlException("Extra characters after valid switch %s(%s)", spec->specifier, spec->value);
			if (spec->argValue.boolValue)
				*spec->argValue.boolValue = true;
				
		} else {
			switch(theType) {
			case DlParseArgTypeString:
				if (spec->argValue.stringValue)
					*spec->argValue.stringValue = spec->value;
				break;
			case DlParseArgTypeInt:
				if (spec->argValue.intValue) {
					char* strEnd = NULL;
					*spec->argValue.intValue = std::strtol(spec->value, &strEnd, 0);
					if (*strEnd != 0)
						throw DlException("Invalid integer value specified %s.", spec->value);
				}
				break;
			case DlParseArgTypeFloat:
				if (spec->argValue.floatValue) {
					char* strEnd = NULL;
					*spec->argValue.floatValue = std::strtod(spec->value, &strEnd);
					if (*strEnd != 0)
						throw DlException("Invalid floating point value specified %s.", spec->value);
				}
				break;
			default:
				throw DlException("Program error, invalid argument type %x.", spec->argType);
				break;
			}
		}
	}
}
