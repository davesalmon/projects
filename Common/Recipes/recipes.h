/*+
 *	File:		recipes.h
 *
 *	Contains:	Interface for numerical recipes routines. Defines errors
 *				and some constants
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 by David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#ifndef _H_recipes
#define _H_recipes

#ifndef _H_DlTypes
#include "DlTypes.h"
#endif

#if	DlFloatOption == DlUseBestDouble

#define	MAX_PRECISION	LDBL_EPSILON

#elif DlFloatOption == DlUseANSIDouble

#define	MAX_PRECISION	DBL_EPSILON

#elif DlFloatOption == DlUseSingleFloat

#define	MAX_PRECISION	FLT_EPSILON

#endif	/*	USE_DOUBLE */

#define	ROOT_MAXPREC	sqrt(MAX_PRECISION)

#include <exception>
#include <string>

class RecipesError : std::exception {
public:
	//		explicit MatrixError(const char* reason) : s(reason) {}
	RecipesError(const char* reason, ...);// : s(reason) {}
	
	const char* what() const throw() { return s.c_str(); }
	
private:
	std::string s;
};

#define _RecipesAssert assert

#endif

//	eof
