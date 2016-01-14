/*!
 * \file DlAssert.h
 * \author David C. Salmon
 * \date 1999
 *
 *	Copyright © 1999 David C. Salmon. All Rights Reserved
 *
 *	Implement DlAssert for debugging. This assert macro uses the DlException::Display()
 *	method for displaying the assert. This makes it useful in GUI applications where 
 *  there is not typically a console. It can also be used for console applications by
 *	changing the alert display definiton (SetExceptionDisplay()).
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

#ifndef _H_DlAssert
#define	_H_DlAssert

#if defined(DlDebugging)

//--------------------------------- Includes -----------------------------------

#ifndef _H_DlException
#include "DlException.h"
#endif

//--------------------------------- Declares -----------------------------------


//------------------------------------------------------------------------------
///	Define the assert exception. This exception is used to display alerts and
/// is thrown if the user chooses to cancel after an assert.
//------------------------------------------------------------------------------

class	DlAssertException : public DlException {

#if !EXECUTE_DOXYGEN_ONLY
	DlExceptionBegin( DlAssertException )
		Generic
		//	@comment This is a generic string 
		//	@string "Assert %s failed on line %ld of file %s."
		//	@end
	DlExceptionEnd()
#endif

public:
	/// Construct the assert exception.
	/// @param cond	The condition that failed.
	/// @param file The file where the condition is defined.
	/// @param line The line within the file.
	DlAssertException( const char* cond, const char* file, DlInt32 line ) :
		DlException("Assert %s failed on line %ld of file %s.", cond, line, file) {}
};	

/// The base assert function
void	DlAssert( const char* cond, const char* file, DlInt32 line );

/// The assert macro. cond is the condition to test.
#define _DlAssert( cond ) if ( !(cond) ) { DlAssert( #cond, __FILE__, __LINE__ ); }

#else
//	here DlDebugging is not defined

#	if !defined(NDEBUG)
#		include <assert.h>
#		define _DlAssert	assert
#	else
#		define _DlAssert(cond)
#	endif
#endif	//	DlDebugging

#endif	//	_H_DlAssert
//	eof
