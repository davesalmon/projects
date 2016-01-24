/*+
 *
 *  DlStrStream.cpp
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  Implement stream with string as storage. String-based strings act like files opened
 *	for append. All writes go to the end of the string. If a seek is attempted beyond
 *	the end of the string it is an error.
 *
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
#include "DlStrStream.h"
#include "DlException.h"

using namespace std;

//----------------------------------------------------------------------------------------
//  DlStrStream::~DlStrStream                                                  destructor
//
//      destruct stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlStrStream::~DlStrStream()
{
	if (LastReference()) {
		delete _s;
		_s = NULL;
	}
}

// assign the stream
DlStrStream& 
DlStrStream::operator=(const DlStrStream& s)
{
    if (this != &s)
    {
        // if this is the only reference to _s then delete it.
        if (LastReference())
            delete _s;
        
        // copy reference count and variables
        DlStream::operator=(s);
        _s = s._s;
        _readPos = s._readPos;
    }
    
    return *this;
}

//----------------------------------------------------------------------------------------
//  DlStrStream::ReadBytes
//
//      read a block of bytes from the stream.
//
//  void* buf          -> 
//  DlUInt32 size      -> 
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
DlUInt32
DlStrStream::ReadBytes(void* buf, DlUInt32 size)
{
	if (_s) {
		if ((DlUInt32)size > _s->length() - _readPos)
			size = _s->length() - _readPos;
			
		memcpy(buf, _s->c_str() + _readPos, size);
		_readPos += size;
		
		return size;
	} else {
		throw DlException("DlStrStream: Attempt to read from empty string");
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//  DlStrStream::WriteBytes
//
//      Write a block of bytes to the stream.
//
//  const void* buf    -> 
//  DlUInt32 size      -> 
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
DlUInt32
DlStrStream::WriteBytes(const void* buf, DlUInt32 size)
{
	if (!_s)
		_s = NEW string;

	if (!_s)
		throw DlException("DlStrStream: Failed to create string\n");

	if (_s) {
		_s->append(reinterpret_cast<const char*>(buf), size);
		return size;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//  DlStrStream::Seek
//
//      seek to the specified position in the stream. StrStream implicitly acts like it
//      was opened for append, with writes always going to the end of the string.
//
//  DlInt32 offset             -> 
//  DlFilePosition fromWhere   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlStrStream::Seek(DlInt32 offset, DlFilePosition fromWhere)
{
	DlInt64 newLoc = offset;
	string::size_type currLen = _s ? _s->length() : 0;
	
	switch (fromWhere) {
		case DlFromStart: break;
		case DlFromCurrent:
			newLoc += _readPos;
			break;
		case DlFromEnd:
			newLoc += currLen;
		default:
			break;
	}
	
	if (newLoc > currLen || newLoc < 0) {
		throw DlException("Seek ouside the bounds of the stream.");
	}
	
	if (newLoc >= 0) {
		_readPos = newLoc;
	}
}

//----------------------------------------------------------------------------------------
//  DlStrStream::Tell
//
//      return the current read pointer.
//
//  returns DlInt64    <- 
//----------------------------------------------------------------------------------------
DlInt64
DlStrStream::Tell() const
{
	return _readPos;
}



