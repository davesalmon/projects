/*
 *  DlSocket.cpp
 *
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 * Socket class for network communication.
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
#include "DlSocket.h"
#include "PrivSocket.h"
#include "DlException.h"

#include <assert.h>

using namespace std;

// Global timeout defaults
DlInt32 DlSocket::_sDefaultReadTimeout = -1;
DlInt32 DlSocket::_sDefaultWriteTimeout = -1;

//----------------------------------------------------------------------------------------
//  DlSocket::~DlSocket                                                        destructor
//
//      destruct socket.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlSocket::~DlSocket()
{
	Close();
}

//----------------------------------------------------------------------------------------
//  DlSocket::DlSocket                                                        constructor
//
//      construct socket.
//
//  SOCKET s               -> the socket.
//  const char* hostName   -> the host name (or ip address string).
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlSocket::DlSocket(SOCKET s, const char* hostName)
	: _sock(s)
	, _hostName(hostName == 0 ? "" : hostName)
{
    if (!_sock || _sock == INVALID_SOCKET) {
        throw DlException("DlSocket: invalid socket in constructor");
    }
    if (_sDefaultReadTimeout >= 0)
        SetReadTimeout(_sDefaultReadTimeout);
    if (_sDefaultWriteTimeout >= 0)
        SetWriteTimeout(_sDefaultWriteTimeout);
}


//----------------------------------------------------------------------------------------
//  DlSocket::ReadBytes
//
//      read data from socket.
//
//  void *buf          -> the buffer to read into.
//  DlUInt32 buflen    -> the size of the buffer.
//
//  returns DlUInt32   <- the size actually read.
//----------------------------------------------------------------------------------------
DlUInt32
DlSocket::ReadBytes(void *buf, DlUInt32 buflen)
{
    assert(_sock);
	
    int bytes = recv(_sock, static_cast<char *>(buf), buflen, 0);
    if (bytes < 0)
        throw DlException("DlSocket: failed to read from socket: %s",
						  DlSocket::GetErrorString());
    return bytes;
}

//----------------------------------------------------------------------------------------
//  DlSocket::WriteBytes
//
//      write data to socket.
//
//  const void *buf    -> the buffer to write.
//  DlUInt32 count     -> the size to write.
//
//  returns DlUInt32   <- the size actually written.
//----------------------------------------------------------------------------------------
DlUInt32 
DlSocket::WriteBytes(const void *buf, DlUInt32 count)
{
    assert(_sock);

    DlInt32 r = send(_sock, static_cast<const char *>(buf), count, 0);

    if (r != count) {
        throw DlException("DlSocket: failed to write to socket: %s",
						  DlSocket::GetErrorString());
    }
	return r;
}

//----------------------------------------------------------------------------------------
//  DlSocket::SetWriteTimeout
//
//      set the write timeout for this socket.
//
//  DlInt32 timeInMillis   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlSocket::SetWriteTimeout(DlInt32 timeInMillis)
{
    assert(_sock);
	
    if (setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO,
        reinterpret_cast<const char *>(&timeInMillis),
        sizeof(timeInMillis)) != 0)
    {
        throw DlException("DlSocket: can\'t set write timeout for socket: %s",
							  DlSocket::GetErrorString());
    }
}

//----------------------------------------------------------------------------------------
//  DlSocket::SetReadTimeout
//
//      set the read timeout for this socket.
//
//  DlInt32 timeInMillis   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlSocket::SetReadTimeout(DlInt32 timeInMillis)
{
    assert(_sock);
	
    if (setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO,
        reinterpret_cast<const char *>(&timeInMillis),
        sizeof(timeInMillis)) != 0)
    {
        throw DlException("DlSocket: can\'t set read timeout for socket: %s",
						  DlSocket::GetErrorString());
    }
}

//----------------------------------------------------------------------------------------
//  DlSocket::GetLocalAddress
//
//      return the connection ip address.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
DlUInt32
DlSocket::GetLocalAddress() const
{
    assert(_sock);

    struct sockaddr_in thisAddress;
    socklen_t thisAddressLength = sizeof(thisAddress);

    if (getsockname(_sock, (struct sockaddr*)&thisAddress, &thisAddressLength) != 0) {
        throw DlException("DlSocket: can\'t get local address for socket: %s",
						  DlSocket::GetErrorString());
    }

    return (DlUInt32)thisAddress.sin_addr.s_addr;
}

//----------------------------------------------------------------------------------------
//  DlSocket::GetLocalPort
//
//      return the connection port.
//
//  returns DlInt32    <- 
//----------------------------------------------------------------------------------------
DlInt32
DlSocket::GetLocalPort() const
{
    assert(_sock);

    struct sockaddr_in thisAddress;
    socklen_t thisAddressLength = sizeof(thisAddress);

    if (getsockname(_sock, (struct sockaddr*)&thisAddress, &thisAddressLength) != 0) {
        throw DlException("DlSocket: can\'t get local port for socket: %s",
						  DlSocket::GetErrorString());
    }

    return (DlInt32)thisAddress.sin_port;
}

//----------------------------------------------------------------------------------------
//  DlSocket::IsIPAddress
//
//      return true if the string is an ip address.
//
//  const char* hostName   -> string with ip in it?
//
//  returns bool           <- true if string is ip address.
//----------------------------------------------------------------------------------------
bool
DlSocket::IsIPAddress(const char* hostName)
{
    int v[4];
    int fields = sscanf(hostName, "%d.%d.%d.%d", v, v+1, v+2, v+3);
    return fields == 4 && v[0] < 256 && v[1] < 256 && v[2] < 256 && v[3] < 256;
}

//----------------------------------------------------------------------------------------
//  DlSocket::Close
//
//      close the socket.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlSocket::Close()
{
    if (_sock && LastReference()) {
        shutdown(_sock, SHUT_RDWR);
        close(_sock);

		_sock = 0;
    }
}

