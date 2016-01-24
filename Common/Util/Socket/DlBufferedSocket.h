/*
 *  DlBufferedSocket.h
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 * Socket implementation of buffered stream. all inline.
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

#ifndef _H_DlBufferedSocket
#define _H_DlBufferedSocket

#include "DlBufferedStream.h"
#include "DlSocket.h"

class DlBufferedSocket : public DlBufferedStream {
public:
	DlBufferedSocket(const DlSocket& sock, DlInt32 bufSize = kDefaultBufferLen);

	const DlSocket& GetSocket() const;
	
	virtual ~DlBufferedSocket();
	
private:
	DlSocket _sock;
};

//----------------------------------------------------------------------------------------
//  DlBufferedSocket::DlBufferedSocket                                 constructor inline
//
//      Construct socket buffer.
//
//  const DlSocket& sock   -> the socket.
//  DlInt32 bufSize        -> the size of the buffer.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
DlBufferedSocket::DlBufferedSocket(const DlSocket& sock, DlInt32 bufSize)
	: DlBufferedStream(_sock, bufSize), _sock(sock)
{
}

//----------------------------------------------------------------------------------------
//  DlBufferedSocket::GetSocket                                                    inline
//
//      return the socket.
//
//  returns const DlSocket&    <- the socket
//----------------------------------------------------------------------------------------
inline
const DlSocket&
DlBufferedSocket::GetSocket() const
{
	return _sock;
}

//----------------------------------------------------------------------------------------
//  DlBufferedSocket::~DlBufferedSocket                                 destructor inline
//
//      destruct the socket buffer ... flush the underlying socket.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
DlBufferedSocket::~DlBufferedSocket()
{
	DlBufferedStream::Flush();
}

#endif
