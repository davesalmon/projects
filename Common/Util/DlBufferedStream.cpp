//+
//	DlBufferedStream.cpp
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
#include "DlBufferedStream.h"
#include "DlStream.h"
//#include "DlFileException.h"
//#include "DlNumberUtils.h"

#if !TARG_OS_MAC
#include <errno.h>
#endif

#include <cctype>

using namespace std;

//----------------------------------- Methods ----------------------------------

//------------------------------------------------------------------------------
//	DlBufferedStream::DlBufferedStream
//
//		create the stream. 
//
//------------------------------------------------------------------------------
DlBufferedStream::DlBufferedStream(DlStream& stream, DlInt32 bufSize) 
	: _stream(stream)
	, buffer(NEW char[bufSize]) //(char *)OPERATORNEW(bufSize*sizeof(char)))
	, bufferLen(bufSize)
	, readPos(0)
	, readEnd(0)
	, writePos(0)
	, readLineCount(0)
{
	writePos = buffer.get();
}

//------------------------------------------------------------------------------
//	DlBufferedStream::~DlBufferedStream
//
//		destructor: Cannot flush stream here. Must call flush in the subclass
//		destructor 
//
//------------------------------------------------------------------------------
DlBufferedStream::~DlBufferedStream()
{
	if (writePos != buffer.get()) {
		Flush();
	}
}

//------------------------------------------------------------------------------
//	DlBufferedStream::ReadLine
//
//		read the line and return true if eol found.. 
//
//  string &s              <-> the string to fill with the line
//
//  returns bool           <- true if not eof
//------------------------------------------------------------------------------
bool
DlBufferedStream::ReadLine(string & s)
{
	bool	rValue = false;
	s.erase();
	while(true)
	{
		int c = ReadChar();
		if (c < 0) {
			rValue = s.length() != 0;
			break;
		} else if (c == '\r') {
			c = ReadChar();
			if (c > 0 && c != '\n')
				UnreadChar();
			rValue = true;
			break;
		} else if (c == '\n') {
			rValue = true;
			break;
		}
		s += (char)c;
	}
	
	if (rValue)
		readLineCount++;
	return rValue;
}

//------------------------------------------------------------------------------
//	DlBufferedStream::ReadLine
//
//		read the line and return true if eol found.
//
//  string &s              ->
//
//  returns bool           <-
//------------------------------------------------------------------------------
bool
DlBufferedStream::ReadToken(string & s, char delim)
{
	bool	rValue = false;
	
	s.erase();
	while(true)
	{
		int c = ReadChar();
		if (c < 0) {
			rValue = s.length() != 0;
			break;
		} else if (c == delim) {
			rValue = true;
			break;
		}
		
		s += (char)c;
	}
	
	return rValue;
}


//----------------------------------------------------------------------------------------
//  DlBufferedStream::ReadNextLine
//
//      read and return the next non-empty, non-comment line of text.
//
//  string &s              -> 
//  const char* comment    -> 
//
//  returns bool           <- 
//----------------------------------------------------------------------------------------
bool
DlBufferedStream::ReadNextLine(string &s, const char* comment)
{
	while (ReadLine(s)) {
		int len = s.length();
		if (len > 0) {
			const char* sPtr = s.c_str();
			while (*sPtr && isspace(*sPtr++))
				len--;
			
			if (len > 0) {
				if (!comment || !strchr(comment, s[0])) {
					return true;
				}
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------
//	DlBufferedStream::WriteLine
//
//		write the string to stream and append eol char(s). 
//
//------------------------------------------------------------------------------
void
DlBufferedStream::WriteLine(const char * s)
{
	Write(s);
	WriteChar('\n');
}

//------------------------------------------------------------------------------
//	DlBufferedStream::Write
//
//		write the string to the stream. 
//
//	s		-> string to write
//------------------------------------------------------------------------------
void
DlBufferedStream::Write(const char * s, char delim)
{
	if (delim)
		WriteChar(delim);
	while (*s)
		WriteChar(*s++);
}

//------------------------------------------------------------------------------
//	DlBufferedStream::WriteInt
//
//		write the integer to the stream. 
//
//	num		-> number to write
//------------------------------------------------------------------------------
void
DlBufferedStream::Write(DlInt32 num, char delim)
{
	char buf[256];
	::snprintf(buf, sizeof(buf), "%d", num);
//	::NumToString(num, s);
	Write(buf, delim);
}

//------------------------------------------------------------------------------
//	DlBufferedStream::WriteFloat
//
//		write the floating point number. 
//
//	num		-> number to write
//------------------------------------------------------------------------------
void
DlBufferedStream::Write(DlFloat num, char delim)
{
	char s[16];
	sprintf(s, "%f", num);
	Write(s, delim);
}

//------------------------------------------------------------------------------
//	DlBufferedStream::WritePString
//
//		write length delimited string. 
//
//	p		-> string to write
//------------------------------------------------------------------------------
void
DlBufferedStream::Write(ConstStringPtr p, char delim)
{
	DlInt32 len = p[0];
	if (delim)
		WriteChar(delim);
	for (DlInt32 i = 1; i <= len; i++)
		WriteChar(static_cast<char>(p[i]));
}

//------------------------------------------------------------------------------
//	DlBufferedStream::WriteBlock
//
//		write the string to the stream. 
//
//	buf		-> block to write
//	len		-> length of block
//------------------------------------------------------------------------------
void
DlBufferedStream::WriteBlock(const void* buf, int len)
{
	if (len > bufferLen) {
		Flush();
		_stream.WriteBytes(buf, len);
		return;
	}

	long remSize = len;
	while (true) {
		if (len > bufferRemaining()) {
			len = bufferRemaining();
		}
		
		memcpy(writePos, buf, len);
		writePos += len;
		
		if (bufferRemaining() == 0)
			Flush();
		
		remSize -= len;
		
		if (remSize == 0)
			break;
		
		buf = (const char *)buf + len;
		len = remSize;
	}		
}

//----------------------------------------------------------------------------------------
//  DlBufferedStream::ReadBlock
//
//      read a block of data.
//
//  void* buf          -> 
//  int len            -> 
//
//  returns DlInt32    <- 
//----------------------------------------------------------------------------------------
DlInt32
DlBufferedStream::ReadBlock(void* buf, int len)
{
	DlInt32 remLen = len;
	
	while(remLen) {
	
		//	determine bytes in buffer
		DlInt32 bufRem = readEnd - readPos;
		if (bufRem == 0) {
			//	fill the buffer
			DlInt32 bytesRead = _stream.ReadBytes(buffer.get(), bufferLen);
			readPos = buffer.get();
			readEnd = readPos + bytesRead;
			bufRem = bytesRead;
			if (bufRem == 0)
				return len - remLen;
		}
	
		//	figure how much to read
		DlInt32 count = remLen;
		if (count > bufRem)
			count = bufRem;
		
		//	copy data
		memcpy(buf, readPos, count);
		
		//	update pointers
		readPos += count;
		remLen -= count;
		buf = (char *)buf + count;
	}

	return len;
}

//----------------------------------------------------------------------------------------
//  DlBufferedStream::Seek
//
//      move the read pointer in the underlying stream.
//
//  DlInt32 offset             -> 
//  DlFilePosition fromWhere   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlBufferedStream::Seek(DlInt32 offset, DlFilePosition fromWhere)
{
	Flush();
	readPos = readEnd = 0;
	_stream.Seek(offset, fromWhere);
}

//----------------------------------------------------------------------------------------
//  DlBufferedStream::Tell
//
//      return the location for subsequent reads.
//
//  returns DlInt64    <- 
//----------------------------------------------------------------------------------------
DlInt64
DlBufferedStream::Tell() const
{
	DlInt64 loc = _stream.Tell();
	if (readPos)
		loc += readPos - buffer.get();
	
	return loc;
}

//------------------------------------------------------------------------------
//	DlBufferedStream::fillBuffer
//
//		fill the buffer. 
//
//------------------------------------------------------------------------------
int
DlBufferedStream::fillBuffer()
{
	DlInt32	readCount = _stream.ReadBytes(buffer.get(), bufferLen);
	if (readCount == 0)
		return -1;

	readPos = buffer.get();
	readEnd = readPos + readCount;
	return *readPos++;
}

//------------------------------------------------------------------------------
//	DlBufferedStream::Flush
//
//		flush the stream. 
//
//------------------------------------------------------------------------------
void
DlBufferedStream::Flush()
{
	SInt32	len = writePos - buffer.get();
	if (len > 0)
	{
		_stream.WriteBytes(buffer.get(), len);
		writePos = buffer.get();
	}
}


//	 eof
