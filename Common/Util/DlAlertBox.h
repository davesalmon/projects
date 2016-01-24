/*!
 * \file DlAlertBox.h
 * \author David C. Salmon
 * \date 6/1/2004
 *
 *	Copyright (C) 2004 By David C. Salmon. All rights reserved.
 *
 *	DlAlertBox is a class that displays an alert box. Designed to be cross-platform.
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

#ifndef _H_DlAlertBox
#define _H_DlAlertBox

#include "DlException.h"

//------------------------------------------------------------------------------
/// Display a simple alert box.
///	The alert box is can be customized by passing strings (or NULL) into the 
///	constructor. Button strings specified as NULL suppress the display of that
/// button.
//------------------------------------------------------------------------------
class DlAlertBox {

public:

	/// The alert box result type. Returns information about the users choice.
	typedef enum {
		DlItemYes = DlExceptionResultYes,		///<	yes or ok
		DlItemCancel = DlExceptionResultCancel,	///<	cancel
		DlItemNo = DlExceptionResultNo			///<	no
	}	DlResultType;
	
	/// The alert type. Use to define the window icon.
	typedef enum {
		DlAlertCaution = DlExceptionWarning,	///< warn the user.
		DlAlertNote = DlExceptionInformation,	///< provide information.
		DlAlertStop = DlExceptionError			///< error
	}	DlAlertType;

	/// Display an alert box with the specified buttons and text. This is the default
	///	implementation for SetExceptionDisplay() in DlException.h
	/// @param theType The alert box type. Defines the icon.
	/// @param okButtonText The text for the OK button. Pass NULL for "Ok"
	/// @param cancelButtonText The text for the Cancel button. Pass NULL to
	///			suppress cancel button.
	/// @param leftButtonText The text for the left-side button (No). Pass NULL to
	///			suppress left-side button.
	/// @param errorText The heading for the alert. Typically defines the dialog title.
	/// @param explanation The body message.
	/// @result the users button choice.
	static DlResultType ShowAlert(DlAlertType theType, const char* okButtonText,
		const char* cancelButtonText, const char* leftButtonText,
		const char* errorText, const char* explanation);

	// simple alert with title and message
	static void ShowAlert(const char* title, const char* explanation);

private:

	/// Construct and display an alert box.
	DlAlertBox(DlAlertType theType, const char* okButtonText,
		const char* cancelButtonText, const char* leftButtonText,
		const char* errorText, const char* explanation);

	DlResultType	GetItemHit() const {
		return _itemHit;
	}

	DlResultType	_itemHit;
};


//----------------------------------------------------------------------------------------
//  DlAlertBox::ShowAlert                                                          inline
//
//      show the alert.
//
//  DlAlertType theType                -> the alert type
//  const char* okButtonText           -> the text for ok button
//  const char* cancelButtonText       -> the text for cancel button
//  const char* leftButtonText         -> the text for the left opt button
//  const char* errorText              -> the text for the error description.
//  const char* explanation            -> the test for the explanation.
//
//  returns DlAlertBox::DlResultType   <- the button hit.
//----------------------------------------------------------------------------------------
inline DlAlertBox::DlResultType 
DlAlertBox::ShowAlert(DlAlertType theType, const char* okButtonText,
	const char* cancelButtonText, const char* leftButtonText,
	const char* errorText, const char* explanation)
{
	return DlAlertBox(theType, okButtonText, cancelButtonText, leftButtonText,
		errorText, 	explanation).GetItemHit();
}

inline void
DlAlertBox::ShowAlert(const char* title, const char* explanation)
{
	DlAlertBox(DlAlertStop, nullptr, nullptr, nullptr, title, explanation);
}


#endif
