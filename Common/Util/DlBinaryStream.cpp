/*
 *  DlBinaryStream.cpp
 *  Common_Util
 *
 *  Created by David Salmon on 12/29/07.
 *  Copyright 2007 David C. Salmon. All rights reserved.
 *
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
#include "DlBinaryStream.h"
#include "DlStream.h"
#include "DlException.h"

//----------------------------------------------------------------------------------------
//  DlBinaryStream::DlBinaryStream                                            constructor
//
//      Construct a binary file wrapper around a stream.
//
//  DlStream& stream   -> the stream
//  DlUInt32 bufSize   -> the buffer size
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlBinaryStream::DlBinaryStream(DlStream& stream, DlUInt32 bufSize)
	: _stream(stream)
	, _buffer((DlUInt8*)operator new (bufSize))
	, _bufferLen(bufSize)
	, _readPos(_buffer.get())
	, _readEnd(_buffer.get())
	, _writePos(_buffer.get())
{
}
 
//----------------------------------------------------------------------------------------
//  DlBinaryStream::~DlBinaryStream                                            destructor
//
//      Destruct the binary wrapper.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlBinaryStream::~DlBinaryStream()
{
	Flush();
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::Flush
//
//      Flush the buffer to the underlying stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlBinaryStream::Flush()
{
	DlUInt32 bytes = _writePos - _buffer.get();
	if (bytes > 0)
		_stream.WriteBytes(_buffer.get(), bytes);
	_writePos = _buffer.get();
	
	_readEnd = _readPos = _buffer.get();
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::WriteBlock
//
//      Write a buffer to the underlying stream.
//
//  const void* buf    -> the buffer.
//  DlUInt32 size      -> the size.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlBinaryStream::WriteBlock(const void* buf, DlUInt32 size)
{
	if (size >= _bufferLen)
	{
		Flush();
		_stream.WriteBytes(buf, size);
	}
	else
	{
		size_t rem = _writePos - _buffer.get();
		if (rem + size > _bufferLen)
			Flush();
		memcpy(_writePos, buf, size);
		_writePos += size;
	}
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::ReadBlock
//
//      read a block from the underlying stream.
//		throws DlException if there is not enough data to satisfy request.
//
//  void* buf          -> the buffer to fill.
//  DlUInt32 size      -> the request size.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlBinaryStream::ReadBlock(void* buf, DlUInt32 size)
{
	size_t rem = _readEnd - _readPos;
	if (rem < size)
	{
		memcpy(buf, _readPos, rem);
		size -= rem;

		if (size > _bufferLen)
		{
			DlUInt32 nRead = _stream.ReadBytes((DlUInt8*)buf+rem, size);
			if (nRead < size)
				throw DlException("File is too short");
			_readPos = _readEnd;
			return;
		}
		else
		{
			size_t n = fillBuffer();
			if (n < size)
				throw DlException("File is too short");

			memcpy((DlUInt8*)buf + rem, _readPos, size);
		}
	}
	else
	{
		memcpy(buf, _readPos, size);
	}
	
	_readPos += size;
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::fillBuffer
//
//      fill the buffer.
//
//  returns DlUInt32   <- the number of bytes available.
//----------------------------------------------------------------------------------------
DlUInt32	
DlBinaryStream::fillBuffer()
{
	_readPos = _buffer.get();
	_readEnd = _readPos + _bufferLen;
	return _stream.ReadBytes(_buffer.get(), _bufferLen);
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::Seek
//
//      Seek the underlying stream to the specified position.
//
//  DlInt32 offset             -> the offset.
//  DlFilePosition fromWhere   -> the seek location.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlBinaryStream::Seek(DlInt32 offset, DlFilePosition fromWhere)
{
	Flush();
	_stream.Seek(offset, fromWhere);
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::Tell
//
//      return the current file pointer.
//
//  returns DlInt64    <- 
//----------------------------------------------------------------------------------------
DlInt64 
DlBinaryStream::Tell() const
{
	DlInt64 ofs = _stream.Tell();
	ofs -= (_readEnd - _readPos);
	return ofs;
}


#if 1

#include "DlStrStream.h"

static void test()
{
	DlStrStream ss("thisisa stream");
	DlBinaryStream bs(ss);
	DlFloat32 val;
	
	bs.ReadLE(val);
	
	bool val2;
	bs.ReadBE(val2);
	
	bs.WriteLE(val2);
	
	fprintf(stderr, "value is %lf\n", val);
}

#endif
