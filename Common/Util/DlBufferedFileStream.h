//+
//	DlBufferedFileStream.h
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains the interface for buffered file stream class.
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

#ifndef _H_DlBufferedFileStream
#define _H_DlBufferedFileStream

//---------------------------------- Includes ----------------------------------

#include "DlFileStream.h"
#include "DlBufferedStream.h"

#include <cstdio>
#include <memory>
#include <string>

//----------------------------------- Defines ----------------------------------

//---------------------------------- Includes ----------------------------------

class DlBufferedFileStream : public DlBufferedStream
{
public:

	DlBufferedFileStream(const DlFileSpec& inFileSpec, DlOpenMode mode,
					  DlInt32 bufLen = kDefaultBufferLen);
	DlBufferedFileStream(const DlFileStream& stream, DlInt32 bufLen = kDefaultBufferLen);

	virtual ~DlBufferedFileStream() {
		DlBufferedStream::Flush();
	}

private:
	DlFileStream _file;
};


#endif
