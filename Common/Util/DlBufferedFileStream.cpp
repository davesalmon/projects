//+
//	DlBufferedFileStream.cpp
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains the buffered file stream class.
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
#include "DlBufferedFileStream.h"

#if !TARG_OS_MAC
#include <errno.h>
#endif

#include <cctype>

//----------------------------------- Methods ----------------------------------

using namespace std;

//----------------------------------------------------------------------------------------
//  DlBufferedFileStream::DlBufferedFileStream                                constructor
//
//      Construct a buffered file stream.
//
//  const DlFileSpec& inFileSpec   -> the file.
//  DlOpenMode mode                -> the open mode (e.g., read/write)
//  DlInt32 bufLen                 -> the buffer size to use
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlBufferedFileStream::DlBufferedFileStream(const DlFileSpec& inFileSpec,
										   DlOpenMode mode,
										   DlInt32 bufLen)
: DlBufferedStream(_file, bufLen)
, _file(inFileSpec, mode)
{
}

//----------------------------------------------------------------------------------------
//  DlBufferedFileStream::DlBufferedFileStream                                constructor
//
//      Implementation of a buffered text file stream.
//
//  const DlFileStream& stream -> the file stream
//  DlInt32 bufLen             -> the buffer length to use.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlBufferedFileStream::DlBufferedFileStream(const DlFileStream& stream, DlInt32 bufLen)
: DlBufferedStream(_file, bufLen)
, _file(stream)
{
}

//	 eof
