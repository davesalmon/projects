/*+++++++++++++++++++++++++++++++
**	random.h
**
**		Purpose:	Random number generators
**
 *	Copyright:	COPYRIGHT (C) 1989-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
**		Author:		David C. Salmon
**		Created:	Thu, Sep 14, 1989
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

#if !defined(_H_random)
#define	_H_random

#include	"recipes.h"

/*	Prototypes 
	*/

#ifdef __cplusplus
extern "C" {
#endif

DlFloat		ran0( long * );			/* improved randx */
DlFloat		ran1( long * );			/* good for all compilers */
DlFloat		randx( long * );		/* c library random numbers */
DlFloat		randy( long * );		/* MAC OS random numbers */
DlFloat		randz( long * );		/* SANE random numbers */

#ifdef __cplusplus
}
#endif

#endif
