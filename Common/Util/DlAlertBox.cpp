/*+
 *
 *  DlAlertBox.cpp
 *
 *  Copyright © 2006 David C. Salmon. All Rights Reserved.
 *
 *  Handle construction of an Alert dialog box.
-*/
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
#include "DlTypes.h"
#include "DlAlertBox.h"
#include "DlStringUtil.h"
#include "DlUnicode.h"

#include "CFStringTracker.h"

#if TARG_OS_MAC 

//#	if !TARG_API_OSX
//#include "<HIToolbox/Dialogs.h>"
//#	endif

//----------------------------------------------------------------------------------------
//  DlAlertBox::DlAlertBox                                                    constructor
//
//      construct an alert dialog box.
//
//  DlAlertType theType            -> the alert type.
//  const char* okButtonText       -> the text for the ok button. NULL gives "ok".
//  const char* cancelButtonText   -> optional text for the cancel button.
//  const char* leftButtonText     -> optional text for the left button
//  const char* errorText          -> the error text
//  const char* explanation        -> the explanation for the error.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlAlertBox::DlAlertBox(DlAlertType theType, const char* okButtonText,
		const char* cancelButtonText, const char* leftButtonText,
		const char* errorText, const char* explanation) : _itemHit(DlItemCancel)
{
#if !__LP64__
	Str255 actErr;
	Str255 explain;
	Str255 button1;
	Str255 button2;
	Str255 button3;
	SInt16	itemHit;
	
	DlStringToPString(errorText ? errorText : "Error occured", actErr);
	DlStringToPString(explanation ? explanation : "no explanation", explain);
	
	AlertStdAlertParamRec	p;
	
	p.movable = false;
	p.helpButton = false;
	
	p.filterProc = NULL;
	p.defaultText = okButtonText ? 
	DlStringToPString(okButtonText, button1) : 
	(ConstStringPtr)kAlertDefaultOKText;
	p.cancelText = cancelButtonText ? DlStringToPString(cancelButtonText, button2) : NULL;
	p.otherText = leftButtonText ? DlStringToPString(leftButtonText, button3) : NULL;
	p.defaultButton = kAlertStdAlertOKButton;
	p.cancelButton = cancelButtonText ? kAlertStdAlertCancelButton : 0;
	p.position = kWindowDefaultPosition;
	
	AlertType alertType;
	switch (theType) {
		case DlAlertCaution:	alertType = kAlertCautionAlert; break;
		case DlAlertNote:		alertType = kAlertNoteAlert; break;
		case DlAlertStop:		alertType = kAlertStopAlert; break;
		default:				alertType = kAlertPlainAlert; break;
	}
	
	OSErr anErr = StandardAlert(alertType, actErr, explain, &p, &itemHit);
	
	if (anErr == noErr)
		_itemHit = (DlResultType)itemHit;
#else
	
	NSRunAlertPanel(
					(NSString*)(CFStringTracker(errorText ? errorText : "Error occured").get()),
					@"%s",
					(NSString*)(okButtonText ? CFStringTracker(okButtonText).get() : nullptr),
					(NSString*)(cancelButtonText ? CFStringTracker(cancelButtonText).get() : nullptr),
					(NSString*)(leftButtonText ? CFStringTracker(leftButtonText).get() : nullptr),
					explanation ? explanation : "no explanation"
					);
	
//	DlC
	
//	NSRunAlertPanel(<#NSString *title#>, <#NSString *msgFormat#>, <#NSString *defaultButton#>,
//					<#NSString *alternateButton#>, <#NSString *otherButton, ...#>)
	
#endif
}

#else
//----------------------------------------------------------------------------------------
//  DlAlertBox::DlAlertBox                                                    constructor
//
//      construct an alert dialog box.
//
//  DlAlertType theType            -> the alert type.
//  const char* okButtonText       -> the text for the ok button. NULL gives "ok".
//  const char* cancelButtonText   -> optional text for the cancel button.
//  const char* leftButtonText     -> optional text for the left button
//  const char* errorText          -> the error text
//  const char* explanation        -> the explanation for the error.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlAlertBox::DlAlertBox(DlAlertType theType, const char* okButtonText,
			const char* cancelButtonText, const char* leftButtonText,
			const char* errorText, const char* explanation) : _itemHit(DlItemCancel)
	{
#error Not implemented

#	if 0
		int result = 0;
		UINT	msgType = MB_OK;
		switch(alertID)
		{
			case kSmallOKAlrt:
			case kLargeOKAlrt:
				msgType = MB_OK;
				break;
			case kSmallOKCancelAlrt:
			case kLargeOKCancelAlrt:
				msgType = MB_OKCANCEL;
				break;
			case kSmallYesNoCancelAlrt:
			case kLargeYesNoCancelAlrt:
				msgType = MB_YESNOCANCEL;
				break;
		}
		
		switch(alertType)
		{
			case DlException::DlAlertCaution:
				msgType |= MB_ICONEXCLAMATION;
				break;
			case DlException::DlAlertNote:
				msgType |= MB_ICONASTERISK;
				break;
			case DlException::DlAlertStop:
			default:
				msgType |= MB_ICONHAND;
				break;
		}
		
		//	some message box thing
		
		DlException::DlResultType response = DlException::DlDisplayYes;
		
		return static_cast<DlException::DlResultType>(
					MessageBox(0, str, "Structure Library Error", msgType));
#	endif
	}

#endif

