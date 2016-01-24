/*
 *  DlListenerSocket.cpp
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

#include "DlPlatform.h"
#include "DlListenerSocket.h"
#include "DlSimpException.h"
#include "PrivSocket.h"

#include <assert.h>

using namespace std;

//----------------------------------------------------------------------------------------
//  DlListenerSocket::DlListenerSocket                                        constructor
//
//      construct socket listener.
//
//  const char* ip     -> the ip or host name.
//  DlInt32 port       -> the port to listen on.
//  DlInt32 backlog    -> the connection queue size.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlListenerSocket::DlListenerSocket(const char* ip, DlInt32 port, DlInt32 backlog)
    : sock(socket(AF_INET, SOCK_STREAM, 0))
{
    create(ip, port, backlog);
}

//----------------------------------------------------------------------------------------
//  DlListenerSocket::create
//
//      create the listener and listen for connections.
//
//  const char* ip     -> the ip or host name.
//  DlInt32 port       -> the port to listen on.
//  DlInt32 backlog    -> the connection queue size.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlListenerSocket::create(const char* ip, DlInt32 port, DlInt32 backlog)
{
	struct sockaddr_in thisAddress;

	thisAddress.sin_len = sizeof(thisAddress);
	thisAddress.sin_family		= AF_INET;
	thisAddress.sin_port		= htons((u_short)port);

	if(ip == NULL)
		thisAddress.sin_addr.s_addr	= htonl(INADDR_ANY);
	else {
		if(DlSocket::IsIPAddress(ip)) {
			thisAddress.sin_addr.s_addr	= inet_addr(ip);
        } else {
		    struct hostent* hostinfo;
		    if ((hostinfo = gethostbyname(ip)) == NULL) {
			    throw DlSimpException("DlListenerSocket: Unable to get host by name (%d)", DlSocket::GetError());
		    }
		    thisAddress.sin_addr.s_addr = *(long*)hostinfo->h_addr_list[0];
		}
	}

#if TARG_OS_DARWIN
	DlInt32 yes = -1;
	setsockopt(sock.GetSocket(), SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes));
#endif

	if (bind(sock.GetSocket(), (struct sockaddr*)&thisAddress, sizeof(thisAddress)) != 0) {
		throw DlSimpException("DlListenerSocket: Unable to bind socket (%d)", DlSocket::GetError());
	}

	listen(sock.GetSocket(), backlog);
}

//----------------------------------------------------------------------------------------
//  DlListenerSocket::Accept
//
//      accept incoming connection.
//
//  returns DlSocket   <- the socket accepted.
//----------------------------------------------------------------------------------------
DlSocket
DlListenerSocket::Accept() const
{
	struct sockaddr_in remoteAddress;
    socklen_t addrlen = sizeof(remoteAddress);

    DlInt32 tmp = accept(sock.GetSocket(), (struct sockaddr*)&remoteAddress, &addrlen);
    if(!tmp || tmp == INVALID_SOCKET)
		throw DlSimpException("DlListenerSocket: Accept failed (%d)", DlSocket::GetError());

	return DlSocket(tmp);
}

