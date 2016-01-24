/*
 *  PrivSocket.h
 *
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 * low level socket includes and definitions.
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

#ifndef _H_PrivSocket
#define _H_PrivSocket

#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <errno.h>
#	include <netdb.h>
#	if TARG_OS_DARWIN
#		include <unistd.h>
#	endif

#	define INVALID_SOCKET  -1

#endif  // _H_PrivSocket

