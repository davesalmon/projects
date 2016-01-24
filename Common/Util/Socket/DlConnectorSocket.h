/*
 *  DlConnectorSocket.h
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 * Socket for connecting to network resource.
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

#ifndef _H_DlConnectorSocket
#define _H_DlConnectorSocket

#include "DlSocket.h"

struct sockaddr_in;

class DlConnectorSocket
{
public:
	// construct connector
	DlConnectorSocket();
	
	// establish connection and return socket.
	DlSocket Connect(const char* host, DlInt32 port) const;
	
protected:

private:
	DlSocket	Connect(struct sockaddr_in& remoteAddress,
						const char* host) const;
};

#endif // _H_DlConnectorSocket
