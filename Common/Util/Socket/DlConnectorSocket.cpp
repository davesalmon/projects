/*
 *  DlConnectorSocket.cpp
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 * Implement Socket for connecting to network resource.
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
#include "DlConnectorSocket.h"
#include "PrivSocket.h"
#include "DlSimpException.h"

#include <assert.h>

using namespace std;

//----------------------------------------------------------------------------------------
//  DlConnectorSocket::DlConnectorSocket                                      constructor
//
//      construct connector.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlConnectorSocket::DlConnectorSocket()
{
}

//----------------------------------------------------------------------------------------
//  DlConnectorSocket::Connect
//
//      connect to network resource on the specified port.
//
//  const char* host   -> the host name.
//  DlInt32 port       -> the port.
//
//  returns DlSocket   <- the socket
//----------------------------------------------------------------------------------------
DlSocket
DlConnectorSocket::Connect(const char* host, DlInt32 port) const
{
	struct sockaddr_in remoteAddress;

	remoteAddress.sin_family	= AF_INET;
	remoteAddress.sin_port		= htons((u_short)port);

	if (DlSocket::IsIPAddress(host)) {
		remoteAddress.sin_addr.s_addr	= inet_addr(host);
	} else {
		struct hostent* hostinfo;
		if ((hostinfo = gethostbyname(host)) == NULL) {
			throw DlSimpException("DlConnectorSocket: Unable to get host by name (%s)",
					 DlSocket::GetErrorString());
		}
		remoteAddress.sin_addr.s_addr	= *(long*)hostinfo->h_addr_list[0];
	}

	return Connect(remoteAddress, host);
}

//----------------------------------------------------------------------------------------
//  DlConnectorSocket::Connect
//
//      low level connect.
//
//  struct sockaddr_in& remoteAddress  -> the IP address.
//  const char* host                   -> the host name.
//
//  returns DlSocket                   <- the socket.
//----------------------------------------------------------------------------------------
DlSocket
DlConnectorSocket::Connect(struct sockaddr_in& remoteAddress, const char* host) const
{
	assert(remoteAddress.sin_family == AF_INET);
	
	SOCKET sock = 0;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		throw DlSimpException("DlConnectorSocket: socket initialization failed (%s)",
					DlSocket::GetErrorString());
	}

#if TARG_OS_UNIX
	DlInt32 yes = -1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes));
#endif

	if (connect(sock, (struct sockaddr*)&remoteAddress, sizeof(remoteAddress)) != 0) {
		throw DlSimpException("DlConnectorSocket: socket connect failed (%s)",
					DlSocket::GetErrorString());
	}

	return DlSocket(sock, host);
}



