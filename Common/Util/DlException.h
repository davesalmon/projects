/*!
 * \file DlException.h
 * \author David C. Salmon
 * \date 1995
 *
 *	Copyright © 1999 David C. Salmon. All Rights Reserved
 *
 *	\brief Implement exception handling and display.
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

#ifndef	_H_DlException
#define	_H_DlException

//--------------------------------- Includes -----------------------------------

#include "DlTypes.h"
#include "DlExceptionDefs.h"

#include <exception>

//--------------------------------- Declares -----------------------------------

/// A numeric code specified when the exception was thrown. For resource based
///	Exceptions, this is the code associated used to look up the text in the 
/// resource. See DlExceptionDefs.h
typedef	DlInt32	DlExceptionCode;

/// The exception severity type. This value may be used to differentiate actions 
/// to be taken when the exception is caught.
typedef enum {
	DlExceptionWarning, 	///< the exception is a warning.
	DlExceptionInformation,	///< the exception is informational.
	DlExceptionError		///< a true error ocurred.
}	DlExceptionType;

/// The result of an exception. This value is returned by DlException::Display
typedef enum {
	DlExceptionResultYes = 1,		///< User pressed yes or ok.
	DlExceptionResultCancel = 2,	///< User pressed cancel.
	DlExceptionResultNo = 3			///< User pressed no.
}	DlExceptionResult;

/// The source where resource-based exception strings are acquired.
typedef enum {
	DlExceptionResource = 0,	///< For legacy Mac code, a resource file.
	DlExceptionPList = 1		///< For new code, a plist resource.
}	DlExceptionSource;

//------------------------------------------------------------------------------
///	\typedef DlExceptionDisplay
///	The function used to display exception strings. A custom function can be created for 
///	the display of exception strings. This is primarily to allow DlException::Display()
///	DlException::DisplayChoice() and DlException::DisplayYesNoCancel() to display in
///	a custom fashion.
///
///	@param theType the exception type.
///	@param okButtonText the text to dispay on the OK button.
///	@param cancelButtonText the text for the cancel button.
///	@param leftButtonText the text for the left side button.
///	@param errorText the title for the exception.
///	@param explaination the actual exception text.
///	@return the dialog box result.
typedef DlExceptionResult (*DlExceptionDisplay)(DlExceptionType theType, 
		const char* okButtonText,
		const char* cancelButtonText, const char* leftButtonText,
		const char* errorText, const char* explanation);

//------------------------------------------------------------------------------
/// Set the exception display. The displayer is a #DlExceptionDisplay function
///	that allows the customization of how exceptions are displayed by the Display
///	methods. If the the exception displayer is set to null the text is printed
/// to the terminal.
///
/// @param displayer The function to use for exception display.
/// @return The previous definition of the exception display function.
DlExceptionDisplay	SetExceptionDisplay(DlExceptionDisplay displayer);

//------------------------------------------------------------------------------
/// Set the source type for resource exceptions. If the type is DlExceptionResource
/// then a kErrorResType resource is used to look up string. Otherwise a plist
/// file is created.
/// @param src the exception source type.
/// @return the old type.
DlExceptionSource	SetExceptionSourceType(DlExceptionSource src);

//------------------------------------------------------------------------------
/// Set the exception source name. This is the name of the plist file for
/// DlExceptionPList type exceptions. The defaults is "Exceptions" which
///	causes the exception strings to be looked up from a file called 
/// "Exceptions.strings".
/// @param name the exception source name.
/// @return false if the name is too long.
bool				SetExceptionSourceName(const char* name);
char*				GetExceptionSourceName(char* buffer, DlSizeT* len);

#ifdef __cplusplus

//------------------------------------------------------------------------------
///		Base class for managing exceptions. This class provides an interface
///		for extracting resource strings for display. There is an accompanying
///		complier (DlCompiler) for extracting strings from files and building
///		a resource file.
//------------------------------------------------------------------------------
class DlException : public std::exception {

#if !EXECUTE_DOXYGEN_ONLY
	DlExceptionBegin(DlException)
		Generic
		//	@comment This is a generic string 
		//	@string "An error (%ld) occurred."
		//	@end
	DlExceptionEnd()
#endif

public:
					/// Construct and exception with error code an type. The exception
					/// will lookup a message from the exception message source when
					/// either the what(), Display(), DisplayChoice(), or DisplayYesNoCancel()
					/// methods are called. The err code is used to look up the message.
					///
					/// @param err the error code.
					/// @param t the exception type.
	explicit		DlException(DlExceptionCode err = 0, DlExceptionType t = DlExceptionError);

					/// construct an exception with a string. \note This method is not recommended
					/// because strings embedded in the code cannot be localized.
					///
					///	@param errString the error string.
					/// @param t the error type.
	explicit		DlException(const char* errString, 
							DlExceptionType t);

					/// Construct an exception using errString and parameters specified
					/// by errString using printf style formatting. \note This constructor is
					/// not recommended because error strings generated this way may not 
					/// be localized.
					///
					/// @param errString the error format string
	explicit		DlException(const char* errString, ...);

	virtual			~DlException() throw();

					///	Display an alert with specified title, exception message, and an OK button.
					///
					/// \note The behavior of Display() may be customized by specifying a
					/// #DlExceptionDisplay function with SetExceptionDisplay().
					///
					/// @param title the title of the dialog box.
	void			Display(const char* title = "Error!");

					///	Display an alert with specified title, exception message, an OK button,
					/// and a Cancel button.
					///
					/// \note The behavior of Display() may be customized by specifying a
					/// #DlExceptionDisplay function with SetExceptionDisplay().
					///
					/// @param title the title of the dialog box.
					/// @ return true if user hit OK, otherwise false.
	bool			DisplayChoice(const char* title = "Error! Continue?");

					///	Display an alert with specified title, exception message, an Yes button,
					/// a No button, and a Cancel button.
					///
					/// \note The behavior of Display() may be customized by specifying a
					/// #DlExceptionDisplay function with SetExceptionDisplay().
					///
					/// @param title the title of the dialog box.
					/// @return the result.
	DlExceptionResult	
					DisplayYesNoCancel(const char* title = "Error!");

					///	Mark the exception as having been displayed. Typically, exception
					/// messages should be displayed only once (or not at all). When Display(),
					///	DisplayChoice(), or DisplayYesNoCancel() are called, the exception is
					/// marked as having been displayed. Subsequent calls to these functions
					///	will not display the exception again. If, in an exception handler,
					///	the exception message is displayed, call this method to prevent 
					/// redisplay.
	void			SetDisplayed() { itsDisplayed = true; }

					/// Get the exception message.
	virtual 
	const char*		what() const throw() { return MakeString(); }

protected:

					///	return the error code
					/// @return the error code.
	DlExceptionCode	GetErrorCode() const { return itsError; }

					///	get the exception type.
					/// @return the exception type.
	DlExceptionType GetAlertType() const { return itsType; }

					///	Make the exception string. This method extracts the string from the 
					///	resource if necessary and returns a pointer to it.
					/// @return a pointer to the string or the default string, if the string
					///	could not be found.
	virtual 
	const char*		MakeString() const throw();

					/// Get the default string. The pointer returned by this method can be 
					/// compared to that returned by MakeString() to determine if the 
					/// default string was returned.
					/// @return pointer to the default string.
	static
	const char*	DefaultStr();

					///	Get a buffer to build the string into.
					/// @param sizeNeeded The size required for the returned pointer.
					/// @return a pointer to a buffer of at least sizeNeeded bytes
	char*			GetBuffer( DlInt32 sizeNeeded ) const;
	
					/// Return the size of the unused portion of the buffer.
					/// @return the remaining buffer size.
	DlInt32			GetBufferSize() const;
	
					///	Lookup the string from the resource. 
					/// @param stringName The exception module name.
					/// @param offset The exception index.
					/// @return a pointer to the string.
	const char* 	GetString(const char* stringName, DlInt32 offset) const;

					/// Print a formatted string to the exception buffer. The string defined
					/// by stringName and offset may have printf format specifiers. This 
					/// method formats the string with the arguments specified.
					/// @param stringName The exception module name.
					/// @param offset The exception index.
					/// @return a pointer to the string.
	const char*		PrintToBuffer(const char* stringName, DlInt32 offset, ...) const;

private:

	//	no copies
//	DlException( const DlException& );
	DlException& operator= ( const DlException& );

	DlExceptionCode				itsError;
	DlExceptionType				itsType;
	mutable DlInt32				itsBufUsed;
	bool						itsDisplayed;
};

//--------------------------------- Defines ------------------------------------


//--------------------------------- Declares -----------------------------------
#endif
#endif

// EOF
