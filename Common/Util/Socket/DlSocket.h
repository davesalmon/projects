/*
 *  DlSocket.h
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

#ifndef _H_DlSocket
#define _H_DlSocket

#include "DlStream.h"

#include <string>

typedef int SOCKET;

class DlSocket : public DlStream
{
	friend class DlListenerSocket;
	friend class DlConnectorSocket;

public:
	virtual ~DlSocket();

	virtual DlUInt32 ReadBytes(void* buf, DlUInt32 buflen);
	virtual DlUInt32 WriteBytes(const void* buf, DlUInt32 count);

	void			SetWriteTimeout(DlInt32 timeInMillis);
	void			SetReadTimeout(DlInt32 timeInMillis);
	DlUInt32		GetLocalAddress() const;
	DlInt32			GetLocalPort() const;
	bool			Ready() const;
	
	const std::string& GetHostName() const;
	
	static bool		IsIPAddress(const char* hostName);
	static DlInt32	GetError();
	static const char* GetErrorString();
	
	static void		SetDefaultReadTimeout(DlInt32 timeInMillis);
	static void		SetDefaultWriteTimeout(DlInt32 timeInMillis);

protected:
	virtual void 	Close();

private:
					DlSocket(SOCKET s, const char* hostName=NULL);
	SOCKET			GetSocket() const;
	SOCKET			_sock;
	std::string		_hostName;
//	DlInt32			_port;

	static DlInt32	_sDefaultReadTimeout;
	static DlInt32	_sDefaultWriteTimeout;
};

//inlines

//----------------------------------------------------------------------------------------
//  DlSocket::GetHostName                                                          inline
//
//      return the host name connected to this socket.
//
//  returns const std::string& <- the name (or ip)
//----------------------------------------------------------------------------------------
inline
const std::string&
DlSocket::GetHostName() const
{
	return _hostName;
}

//----------------------------------------------------------------------------------------
//  DlSocket::Ready                                                                inline
//
//      return true if the socket is ready for read/write.
//
//  returns bool   <- true if connected.
//----------------------------------------------------------------------------------------
inline bool
DlSocket::Ready() const
{
	return _sock != 0;
}

//----------------------------------------------------------------------------------------
//  DlSocket::GetSocket                                                            inline
//
//      return the socket.
//
//  returns SOCKET <- the socket
//----------------------------------------------------------------------------------------
inline SOCKET
DlSocket::GetSocket() const
{
	return _sock;
}

//----------------------------------------------------------------------------------------
//  DlSocket::SetDefaultReadTimeout                                                inline
//
//      set the default read timeout for all DlSocket.
//
//  DlInt32 timeInMillis   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
DlSocket::SetDefaultReadTimeout(DlInt32 timeInMillis) {
	_sDefaultReadTimeout = timeInMillis;
}

//----------------------------------------------------------------------------------------
//  DlSocket::SetDefaultWriteTimeout                                               inline
//
//      set the default write timeout for all DlSocket.
//
//  DlInt32 timeInMillis   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
DlSocket::SetDefaultWriteTimeout(DlInt32 timeInMillis) {
	_sDefaultWriteTimeout = timeInMillis;
}

//----------------------------------------------------------------------------------------
//  DlSocket::GetError                                                             inline
//
//      return the last error.
//
//  returns DlInt32    <- 
//----------------------------------------------------------------------------------------
inline DlInt32
DlSocket::GetError()
{
    return errno;
}

//----------------------------------------------------------------------------------------
//  DlSocket::GetErrorString                                                       inline
//
//      return a string for the last error.
//
//  returns const char*    <- 
//----------------------------------------------------------------------------------------
inline const char*
DlSocket::GetErrorString()
{
	return strerror(errno);
}


#endif // _H_DlSocket
