//+
//	DlString.cp
//
//	©1998 David C. Salmon. All Rights Reserved
//
//	Implement a string class
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
#include	"DlString.h"
//#include 	"DlMacrosMac.h"

//---------------------------------- Routines ----------------------------------

DlFloatFormat	DlString::sFloatFormat;
DlIntFormat		DlString::sIntFormat;

//---------------------------------- Routines ----------------------------------

//------------------------------------------------------------------------------
//	DlString::AssignFromPtr
//
//		assign the string from memory
//
//	mem				->	pointer to memory
//	len				->	size
//	returns			<-	string reference
//------------------------------------------------------------------------------
const DlString &
DlString::AssignFromPtr( const void * mem, size_t len )
{
	itsBuf.allocate( len + 1 );
	itsBuf.pset( (const char *)mem, len, 0 );
	itsLength = len;
	itsBuf.pptr()[itsLength] = '\0';
	return *this;
}

//------------------------------------------------------------------------------
//	DlString::InsertFromPtr
//
//		Insert the characters at mem with length len into the string at offset
//		where.
//
//	mem						->	char array to insert
//	value					->	length of array to append
//	value					->	offset from start
//	returns					<-	string reference
//------------------------------------------------------------------------------
const DlString &
DlString::InsertFromPtr( const void * mem, size_t len, size_t where )
{
	if ( where > itsLength )
		where = itsLength;
	
	//	add len to the size
	itsBuf.allocate( itsLength + len + 1 );
	
	if ( where != itsLength ) {
		itsBuf.pmove( itsLength - where, where, where + len );
	}
	itsBuf.pset( (const char *)mem, len, where );
	itsLength += len;
	itsBuf.pptr()[itsLength] = '\0';
	
	return *this;
}

//------------------------------------------------------------------------------
//	DlString::AppendFromPtr
//
//		append memory to the string
//
//	mem				->	pointer to memory
//	len				->	size
//	returns			<-	string reference
//------------------------------------------------------------------------------
const DlString &
DlString::AppendFromPtr( const void *mem, size_t len )
{
	itsBuf.allocate( itsLength + len + 1 );
	itsBuf.pset( (const char *)mem, len, itsLength );
	itsLength += len;
	itsBuf.pptr()[itsLength] = '\0';
	return *this;
}

//------------------------------------------------------------------------------
//	DlString::Fill
//
//		fill the string with fillchar
//
//	fillchar		->	character to fill with
//------------------------------------------------------------------------------
void
DlString::Fill( char fillchar )
{
	size_t	i;
	for ( i = 0; i < itsLength; i++ ) 
		itsBuf.pptr()[i] = fillchar;
	itsBuf.pptr()[i] = 0;
}

#if USE_STREAMS

//------------------------------------------------------------------------------
//	operator >>		-	inserter from stream
//
//	s				->	stream to insert from
//	str				<-	string to insert to
//	returns			<-	stream
//------------------------------------------------------------------------------
istream & 
operator >> ( istream & s, DlString & str )
{
	str.Assign( "" );

	while( 1 ) {
		char	buf[100];
		s.getline( buf, 100 );
		str.Append( buf );

		if ( !s.fail() ) 
			break;
	}

	return s;
}

//------------------------------------------------------------------------------
//	operator >>		-	extractor to stream
//
//	s				->	stream to extract to
//	str				<-	string to extract from
//	returns			<-	stream
//------------------------------------------------------------------------------
ostream & 
operator << ( ostream & s, const DlString & str )
{
	s.write(str, static_cast<int>( str.itsLength )) << endl;
	return s;
}

#endif
//	eof
