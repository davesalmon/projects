/*
 *  DlSimpException.cpp
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 *  Simple string-based exception.
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


#include "DlPlatform.h"
#include "DlSimpException.h"

/*------------------------------------------------------------------------------
 * DlSimpException::DlSimpException                                  constructor
 *
 * Describe function
 *
 * const std::string& format	-> argument description here
 * ...	-> argument description here
 *----------------------------------------------------------------------------*/
DlSimpException::DlSimpException(const char* format, ...)
{
	char	buf[2048];
	va_list	args;
	
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);

	_data.assign(buf);
}

/*------------------------------------------------------------------------------
 * DlSimpException::DlSimpException                                  constructor
 *
 * Describe function
 *
 * const std::string& format	-> argument description here
 * va_list args	-> argument description here
 *----------------------------------------------------------------------------*/
DlSimpException::DlSimpException(const char* format, va_list args)
{
	char	buf[2048];
	vsnprintf(buf, sizeof(buf), format, args);
	_data.assign(buf);
}

DlSimpException::~DlSimpException() throw()
{
}

