/*
 *  DlListenerSocket.h
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 * Socket for incoming to network connection.
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

#ifndef _H_DlListenerSocket
#define _H_DlListenerSocket

#include "DlSocket.h"

#include <stdio.h> // for NULL
#include <limits.h>

class DlListenerSocket
{
public:
				DlListenerSocket(
					const char* ip = NULL,			// any nic
					DlInt32 port = 0,				// any port
					DlInt32 backlog = INT_MAX);		// connection queue
		
	DlSocket	Accept() const;
	
	DlUInt32	GetLocalAddress() const;
	DlInt32		GetLocalPort() const;

private:
	void        create(const char*ip, DlInt32 port, DlInt32 backlog);

	DlSocket	sock;
};

//inlines

//----------------------------------------------------------------------------------------
//  DlListenerSocket::GetLocalAddress                                              inline
//
//      return the local address we are listening on.
//
//  returns DlUInt32   <- the IP address.
//----------------------------------------------------------------------------------------
inline DlUInt32
DlListenerSocket::GetLocalAddress() const
{
	return sock.GetLocalAddress();
}

//----------------------------------------------------------------------------------------
//  DlListenerSocket::GetLocalPort                                                 inline
//
//      return the local port we are listening on.
//
//  returns DlInt32    <- the port
//----------------------------------------------------------------------------------------
inline DlInt32
DlListenerSocket::GetLocalPort() const
{
	return sock.GetLocalPort();
}

#endif // _H_DlListenerSocket
