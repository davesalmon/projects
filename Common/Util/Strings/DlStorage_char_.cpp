//+
//	DlStorage_char_.cpp
//
//	©1998 David C. Salmon. All Rights Reserved
//
//	Define a template class for managing memory. It manages both 
//	stack and heap memory, depending on constructor.
//-
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "DlStorage.tem"

//---------------------------------- Declares ----------------------------------

#if defined( DlDebugging )
template<> 
DlStorage<char>::DlBlock::_BlockList	DlStorage<char>::DlBlock::sBlockList(0);
#endif

//---------------------------------- Methods -----------------------------------

//------------------------------------------------------------------------------
//	DlStorage<char>::set
//
//		specialization of set for character data. Note: the allocation for 
//		the data must already exists.
//
//	p					->	the data to set
//	len					->	the length of data
//	ofs					->	the offset into the buffer to start setting.
//------------------------------------------------------------------------------
template <> void
DlStorage<char>::pset( const char * p, size_t len, size_t ofs )
{
	_DlAssert( ofs >= 0 && len + ofs <= numelems() );
	if ( len != 0 ) {
		memcpy( ptr() + ofs, p, len );
	}
}

//------------------------------------------------------------------------------
//	DlStorage<char>::move
//
//		specialization of move for character arrays. Data of length elemsToMove
//		is moved from offset from to offset to. There must be sufficient space
//		in the buffer to move the data.
//
//	elemsToMove			->	the number of elements (chars) to move
//	from				->	where to start moving
//	to					->	where to move to
//------------------------------------------------------------------------------
template <> void
DlStorage<char>::pmove( size_t elemsToMove, size_t from, size_t to )
{
	_DlAssert( from + elemsToMove < numelems() && 
			to + elemsToMove < numelems() );
	memmove( ptr() + to, ptr() + from, elemsToMove );
}

//------------------------------------------------------------------------------
//	DlStorage<char>::get
//
//		specialization of get for character arrays. Data of length l is moved to
//		buf from offset within the array ofs.
//
//	ofs					->	the offset to get characters from
//	len					->	number of characters to get
//	buf					<-	destination
//------------------------------------------------------------------------------
template <> void		
DlStorage<char>::get( size_t ofs, size_t len, char * buf ) const
{
	_DlAssert( ofs >= 0 && ofs + len < numelems() );
	if ( len != 0 ) {
		memcpy( buf, ptr() + ofs, len );
	}
}

//	instantiate the rest of the template.
template class DlStorage<char>;
