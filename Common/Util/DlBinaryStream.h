/*
 *  DlBinaryStream.h
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

#ifndef _H_DlBinaryStream
#define _H_DlBinaryStream

#include <memory>
#include "DlSwap.h"

//----------------------------------- Defines ----------------------------------


//---------------------------------- Includes ----------------------------------
class DlStream;

// class DlBinaryStream
//
//	Adapter class for stream that allows reading and writing of binary values.
//	There are four basic readers and writers:
//
//	ReadChars/WriteChars - reads/writes a given number of characters to the file.
//	ReadLE/WriteLE - reads/writes value in little-ending format.
//	ReadBE/WriteBE - reads/writes value in big-endian format.
//	Read/Write - reads/writes value in native format.
//
//	Each T used for ReadXX and WriteXX must have a swap function. All the basic
//	data-type swap functions are defined in DlSwap.h
//
class DlBinaryStream
{
public:
	enum { kDefaultBufferLen = 16384 };

	// constuct from stream
	DlBinaryStream(DlStream& stream, DlUInt32 bufSize = kDefaultBufferLen);
	virtual ~DlBinaryStream();

	// flush the buffer. throws DlException
	void	Flush();

	// low level read and write. throws DlException
	void	WriteBlock(const void* buf, DlUInt32 size);
	void	ReadBlock(void* buf, DlUInt32 size);
	
	// read stuff
	void ReadChars(char* s, DlUInt32 len);
	
	template <class T> void ReadLE(T& val);
	template <class T> void ReadBE(T& val);
	template <class T> void Read(T& val);
	
	//	write stuff
	void WriteChars(const char* s, DlUInt32 len);
	
	template <class T> void WriteLE(const T& val);
	template <class T> void WriteBE(const T& val);
	template <class T> void Write(const T& val);
	
	// seek and tell. support depends on the underlying stream
	void Seek(DlInt32 offset, DlFilePosition fromWhere);
	DlInt64 Tell() const;
	
private:
	
	DlUInt32	fillBuffer();

	DlStream&				_stream;
	std::auto_ptr<DlUInt8>	_buffer;
	DlUInt32				_bufferLen;

	const DlUInt8 *			_readPos;
	const DlUInt8 *			_readEnd;

	DlUInt8 *				_writePos;

};

//----------------------------------------------------------------------------------------
//  DlBinaryStream::ReadChars                                                      inline
//
//      read a buffer of characters.
//
//  char* b        <-> the buffer
//  DlUInt32 len   -> number of chars to read.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
DlBinaryStream::ReadChars(char* b, DlUInt32 len)
{
	ReadBlock(b, len);
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::WriteChars                                                     inline
//
//      Write a buffer of characters.
//
//  const char* b  -> the buffer to write.
//  DlUInt32 len   -> the number of chars.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
DlBinaryStream::WriteChars(const char* b, DlUInt32 len)
{
	WriteBlock(b, len);
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::ReadLE
//
//      Read a little endian value of type T from the stream. Each T must have a DlSwapLE
//		function defined.
//
//  T& val                 <-> the value to read
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T> 
inline void 
DlBinaryStream::ReadLE(T& val)
{
	ReadBlock(&val, sizeof(val));
	DlSwapLE(val);
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::ReadBE
//
//      Read a big-endian value from the stream. Each T must have a DlSwapBE function.
//
//  T& val                 <-> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T> 
inline void 
DlBinaryStream::ReadBE(T& val)
{
	ReadBlock(&val, sizeof(val));
	DlSwapBE(val);
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::Read
//
//      Read a value in native format.
//
//  T& val                 <-> the value
//
//  returns nothing 
//----------------------------------------------------------------------------------------
template <class T> 
inline void 
DlBinaryStream::Read(T& val)
{
	ReadBlock(&val, sizeof(val));
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::Read
//
//      specialization for bool.
//
//  bool& val               <-> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <> 
inline void 
DlBinaryStream::Read<bool>(bool& b)
{
	DlUInt32 val;
	Read(val);
	b = val != 0;
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::ReadLE
//
//      specialization for bool.
//
//  bool& val               <-> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <> 
inline void 
DlBinaryStream::ReadLE<bool>(bool& b)
{
	Read(b);
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::ReadBE
//
//      specialization for bool.
//
//  bool& val               <-> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <> 
inline void 
DlBinaryStream::ReadBE<bool>(bool& b)
{
	Read(b);
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::WriteLE
//
//      Write the value in little-endian format.
//
//  const T& t             -> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T> 
inline void 
DlBinaryStream::WriteLE(const T& t)
{
	T val = t;
	DlSwapLE(val);
	WriteBlock(&val, sizeof(val));
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::WriteBE
//
//      Write the value in big-endian format.
//
//  const T& t             -> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T> 
inline void 
DlBinaryStream::WriteBE(const T& t)
{
	T val = t;
	DlSwapBE(val);
	WriteBlock(&val, sizeof(val));
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::Write
//
//      Write the value in native format.
//
//  const T& t             -> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T> 
inline void 
DlBinaryStream::Write(const T& t)
{
	WriteBlock(&t, sizeof(t));
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::Write
//
//      specialization for bool.
//
//  const bool& t           -> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <> 
inline void 
DlBinaryStream::Write<bool>(const bool& b)
{
	DlUInt32 val = b ? -1 : 0;
	WriteBlock(&val, sizeof(val));
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::WriteLE
//
//      specialization for bool.
//
//  const bool& t           -> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <> 
inline void 
DlBinaryStream::WriteLE<bool>(const bool& b)
{
	Write(b);
}

//----------------------------------------------------------------------------------------
//  DlBinaryStream::WriteBE
//
//      specialization for bool.
//
//  const bool& t           -> the value
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <> 
inline void 
DlBinaryStream::WriteBE<bool>(const bool& b)
{
	Write(b);
}

#endif

