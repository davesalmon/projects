/*
 *  DlStream.cpp
 *
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 *	Abstract base class for byte-stream behavior.
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
#include "DlStream.h"
#include "DlException.h"

DlStream::~DlStream()
{
	_DecReference();
}

void 
DlStream::Seek(DlInt32 offset, DlFilePosition fromWhere)
{
	throw DlException("Stream is not seekable.");
}

DlInt64 
DlStream::Tell() const
{
	throw DlException("Stream is not tellable.");
}

