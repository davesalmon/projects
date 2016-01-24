/*
 *  DlBufferedStream.h
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright (c) 2002 __MyCompanyName__. All rights reserved.
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

#ifndef _H_DlBufferedStream
#define _H_DlBufferedStream

//---------------------------------- Includes ----------------------------------

//#include "DlFileStream.h"

#include <cstdio>
#include <memory>
#include <string>

//----------------------------------- Defines ----------------------------------


//---------------------------------- Includes ----------------------------------
class DlStream;

class DlBufferedStream
{
public:
	enum { kDefaultBufferLen = 16384 };

	DlBufferedStream(DlStream& stream, DlInt32 bufSize = kDefaultBufferLen);
	virtual ~DlBufferedStream();
	
	void	UnreadChar();
	int		ReadChar();
	
	// read the next line
	bool	ReadLine(std::string & s);
	
	//	read the next non-blank, non-comment line
	bool	ReadNextLine(std::string &s, const char* comment = "#");
	
	// read a token from the stream, delimited by delim
	bool	ReadToken(std::string& s, char delim=',');
	
	// return the current line count read (for diagnostic messages)
	DlInt32	GetLineCount() const {return readLineCount;}

	void	WriteChar(int c);
	void	Write(const char * str, char delim = 0);
	void	Write(DlUInt8 num, char delim = 0) {Write(static_cast<DlInt32>(num), delim);}
	void	Write(DlInt32 num, char delim = 0);
	void	Write(DlUInt32 num, char delim = 0){Write(static_cast<DlInt32>(num), delim);}
	void	Write(DlFloat num, char delim = 0);
	void	Write(ConstStringPtr p, char delim = 0);
	void	Write(const std::string & s, char delim = 0) { Write(s.c_str(), delim); }

	void	WriteLine(const char * s);
	void	WriteLine(const std::string & s) { WriteLine(s.c_str()); }
	void	Flush();

	void	WriteBlock(const void* buf, int size);
	DlInt32	ReadBlock(void* buf, int size);

	virtual void Seek(DlInt32 offset, DlFilePosition fromWhere);
	virtual DlInt64 Tell() const;
	
private:
    DlBufferedStream(const DlBufferedStream& d);
    DlBufferedStream& operator=(const DlBufferedStream& d);
        
	long	bufferRemaining() const { return writePos - buffer.get(); }

	int	fillBuffer();

	DlStream&			_stream;
	std::unique_ptr<char[]> buffer;
	DlInt32				bufferLen;

	const char *		readPos;
	const char * 		readEnd;

	char *				writePos;
	DlInt32				readLineCount;
};

//----------------------------------------------------------------------------------------
//  DlBufferedStream::ReadChar                                                     inline
//
//      read a single character.
//
//  returns int    <- 
//----------------------------------------------------------------------------------------
inline int
DlBufferedStream::ReadChar()
{
	return (readPos == readEnd) ? fillBuffer() : *readPos++;
}

//----------------------------------------------------------------------------------------
//  DlBufferedStream::WriteChar                                                    inline
//
//      write a single character.
//
//  int c  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
DlBufferedStream::WriteChar(int c)
{
	if (writePos - buffer.get() == bufferLen)
		Flush();
	*writePos++ = c;
}

//----------------------------------------------------------------------------------------
//  DlBufferedStream::UnreadChar                                                   inline
//
//      unread a character.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
DlBufferedStream::UnreadChar()
{
	readPos--;
}

#endif
