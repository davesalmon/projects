/*+
 *	File:		recipes.c
 *
 *	Contains:	Define recipes error handlers
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

#include "DlPlatform.h"
#include "recipes.h"

const uint kBufSize = 256;

RecipesError::RecipesError(const char* reason, ...)
	: s(reason)
{
	va_list args;
	va_start(args, reason);

	char buf[kBufSize];
	
	vsnprintf(buf, kBufSize - 1, reason, args);

	s.assign(buf);
}

//	eof
