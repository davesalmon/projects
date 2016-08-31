//+
//	DlException.cpp
//
//	Copyright © 1999 David C. Salmon. All Rights Reserved
//
//	Implement exception handling and display
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
#include "DlException.h"
#include "DlAssert.h"

#include "CFStringTracker.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "DlMacros.h"

using namespace std;

static DlExceptionDisplay sExceptionDisplay = NULL;
static DlExceptionSource  sExceptionSource = DlExceptionPList;
static char sExceptionSourceName[256] = "Exceptions";

//---------------------------------- Defines -----------------------------------

DlExceptionDeclare(DlException);

#if defined(DlDebugging)
DlExceptionDeclare(DlAssertException);
#endif

const DlInt32	sExceptionBufLen = 1024;
static	char	sExceptionBuffer[sExceptionBufLen];
const char * const kDefaultString = "Failed to find exception string.";

#pragma mark -
#pragma mark --- global functions---

//----------------------------------------------------------------------------------------
//  SetExceptionDisplay
//
//      set the display for exceptions.
//
//  DlExceptionDisplay displayer   -> the exception displayer
//
//  returns DlExceptionDisplay     <- the old displayer.
//----------------------------------------------------------------------------------------
DlExceptionDisplay	
SetExceptionDisplay(DlExceptionDisplay displayer)
{
	DlExceptionDisplay oldDisp = sExceptionDisplay;
	sExceptionDisplay = displayer;
	return oldDisp;
}


//----------------------------------------------------------------------------------------
//  SetExceptionSourceType
//
//      set the source type for exceptions. This can be either resource or plist.
//
//  DlExceptionSource src      -> the exception source.
//
//  returns DlExceptionSource  <- the old source.
//----------------------------------------------------------------------------------------
DlExceptionSource
SetExceptionSourceType(DlExceptionSource src)
{
	DlExceptionSource oldsrc = sExceptionSource;
	if (src == DlExceptionResource || src == DlExceptionPList) {
		sExceptionSource = src;
	}
	return oldsrc;
}

//----------------------------------------------------------------------------------------
//  SetExceptionSourceName
//
//      set the source name for exceptions.
//
//  const char* name   -> the identifier for strings.
//
//  returns bool       <- return true if the name is not too long.
//----------------------------------------------------------------------------------------
bool
SetExceptionSourceName(const char* name)
{
	DlSizeT nameLen = strlen(name);
	if (nameLen < sizeof(sExceptionSourceName)) {
		strcpy(sExceptionSourceName, name);
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------
//  GetExceptionSourceName
//
//      return the source name for exceptions.
//
//  char* buffer   -> buffer to hold name
//  DlSizeT* len   -> length of buffer in, length used out.
//
//  returns char*  <- the name.
//----------------------------------------------------------------------------------------
char*
GetExceptionSourceName(char* buffer, DlSizeT* len)
{
	DlSizeT srcLen = strlen(sExceptionSourceName);
	if (*len < srcLen + 1) {
		strcpy(buffer, sExceptionSourceName);
	} else {
		buffer = 0;
	}
	
	*len = srcLen + 1;
	return buffer;
}

//------------------------------------------------------------------------------
//	DlException::DlException										constructor
//
//		construct the exception
//
//	err				->	the error code
//	t				->	the alert type
//------------------------------------------------------------------------------
DlException::DlException(DlExceptionCode err, DlExceptionType t) 
	: itsError(err)
	, itsType(t)
	, itsBufUsed(0)
	, itsDisplayed(false) 
{
	//	here on windows we need to acquire a mutex, since the code is not
	//	reentrant due to static string buffer
}

//------------------------------------------------------------------------------
//	DlException::DlException										constructor
//
//		construct the exception
//
//	err				->	the error code
//	t				->	the alert type
//------------------------------------------------------------------------------
DlException::DlException(const char * errString, DlExceptionType t)
	: itsError(0x8000)
	, itsType(t)
	, itsBufUsed(0)
	, itsDisplayed(false) 
{
	DlInt32	len = strlen(errString);
	
	if (len >= sExceptionBufLen)
		len = sExceptionBufLen - 1;
		
	memcpy(sExceptionBuffer, errString, len + 1);
	itsBufUsed = len + 1;
}

//------------------------------------------------------------------------------
//	DlException::DlException										constructor
//
//		construct the exception
//
//	err				->	the error code
//	t				->	the alert type
//------------------------------------------------------------------------------
DlException::DlException(const char * errString, ...)
	: itsError(0x8000)
	, itsType(DlExceptionError)
	, itsBufUsed(0)
 	, itsDisplayed(false) 
{
	va_list la;
	va_start(la, errString);
	
	vsnprintf(sExceptionBuffer, sExceptionBufLen - 1, errString, la);
	va_end(la);
	itsBufUsed = strlen(sExceptionBuffer) + 1;
}

//------------------------------------------------------------------------------
//	DlException::~DlException										destructor
//
//		destroy the exception
//
//------------------------------------------------------------------------------
DlException::~DlException() throw()
{
	//	and free the mutex
}

//------------------------------------------------------------------------------
//	DlException::Display
//
//		Display the message
//
//------------------------------------------------------------------------------
void
DlException::Display(const char* title)  
{
	if (!itsDisplayed) {
		if (sExceptionDisplay) {
			(*sExceptionDisplay)(itsType, "OK", NULL, NULL, title, MakeString());
		} else {
			fprintf(stderr, "%s: %s\n", title, MakeString());
		}
//		DlAlertBox::ShowAlert(itsType, "Ok", NULL, NULL, title, MakeString());
//		displayAlert(kSmallOKAlrt, itsType, MakeString());
		itsDisplayed = true;
	}
}

//------------------------------------------------------------------------------
//	DlException::DisplayChoice
//
//		Display the message
//
//------------------------------------------------------------------------------
bool
DlException::DisplayChoice(const char* title) 
{
	if (!itsDisplayed) {
		itsDisplayed = true;
		if (sExceptionDisplay) {
			return (*sExceptionDisplay)(itsType, "Ok", NULL, NULL, title, MakeString()) == DlExceptionResultYes;
		} else {
			fprintf(stderr, "%s: %s\n", title, MakeString());
			return false;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
//	DlException::DisplayChoice
//
//		Display the message
//
//------------------------------------------------------------------------------
DlExceptionResult
DlException::DisplayYesNoCancel(const char* title)
{
	if (!itsDisplayed) {
		itsDisplayed = true;
		if (sExceptionDisplay) {
			return (*sExceptionDisplay)(itsType, "Ok", NULL, NULL, title, MakeString());
		} else {
			fprintf(stderr, "%s: %s\n", title, MakeString());
		}
	}
	return DlExceptionResultCancel;
}

//------------------------------------------------------------------------------
//	DlException::GetBuffer
//
//		return the exception buffer
//
//	returns					<-	buffer for exception handling
//------------------------------------------------------------------------------
char*
DlException::GetBuffer(DlInt32	sizeNeeded) const {
	DlInt32	bufOffset = GetBufferSize() - sizeNeeded;
	if (bufOffset >= 0) {
		itsBufUsed += sizeNeeded;
		return sExceptionBuffer + bufOffset;
	} else {
		return NULL;
	}
}

//------------------------------------------------------------------------------
//	DlException::GetBufferLength
//
//		return the length of the buffer
//
//	returns					<-	length of buffer for exception handling
//------------------------------------------------------------------------------
DlInt32	
DlException::GetBufferSize() const {
	return sExceptionBufLen - itsBufUsed;
}

//------------------------------------------------------------------------------
//	DlException::PrintToBuffer
//
//		print into the exception buffer.
//
//	stringName				->	resource name for string
//	offset					->	count with this name
//	returns					<-	string
//------------------------------------------------------------------------------
const char*
DlException::PrintToBuffer(const char * stringName, DlInt32 offset, ...) const
{
	const char* fmt = GetString(stringName, offset);
	if (fmt) {
		DlInt32 bufSize = GetBufferSize();
		char* buf = GetBuffer(bufSize);
		if (buf) {
			va_list	ap;
			va_start(ap, offset);
			vsnprintf(buf, bufSize, fmt, ap);
			return buf;
		}
	}
	
	return DefaultStr();
}

#if TARG_OS_MAC

//----------------------------------------------------------------------------------------
//  catValue                                                                       static
//
//      catenate the specified value.
//
//  Str255 str                     -> string to add to
//  DlInt32 val                    -> value to catenate
//
//  returns const unsigned char *  <- string with value catenated
//----------------------------------------------------------------------------------------
static
const unsigned char *
catValue(Str255 str, DlInt32 val)
{
//	Str15	num;
//	::NumToString(val, num);

	char buf[64];
	::snprintf(buf, sizeof(buf), "%d", val);

	size_t bufLen = ::strlen(buf);
	if (str[0] + bufLen <= 255) {
		DlMemCopy(buf, str + str[0] + 1, bufLen);
		str[0] += bufLen;
	}
	return str;
}

//------------------------------------------------------------------------------
//	DlExceptionStrings::GetString
//
//		return the length of the buffer
//
//	stringName				->	the module name DlExceptionName(c)
//	offset					->	the offset withing the class
//	buffer					->	the output buffer
//------------------------------------------------------------------------------
const char *
DlException::GetString(const char * stringName, DlInt32 strOfs) const
{
	Str255	modName;
	char*	buf = NULL;
	modName[0] = strlen(stringName);
	DlMemCopy(stringName, modName+1, modName[0]);
	::catValue(modName, strOfs);
	
	if (sExceptionSource == DlExceptionResource) {
	
		Handle	h = ::GetNamedResource(kErrorResType, modName);
		if (h != NULL) {
			DlInt32	len = ::GetHandleSize(h);
			buf = DlException::GetBuffer(len);

			if (buf) {
				DlMemCopy((*h), buf, len);
			} else {
				return "DlException::No buffer!";
			}
		}
	} else {
		CFStringTracker key { CFStringCreateWithPascalStringNoCopy(kCFAllocatorDefault, modName,
									kCFStringEncodingMacRoman, kCFAllocatorNull) };
		CFStringTracker table { CFStringCreateWithCStringNoCopy(kCFAllocatorDefault,
									sExceptionSourceName, kCFStringEncodingMacRoman, kCFAllocatorNull) };

		CFStringTracker value { CFCopyLocalizedStringFromTable(key, table, "") };
		bool gotValue = value != key;
		if (value) {
			if (gotValue) {
				DlInt32 len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(value), kCFStringEncodingUTF8);
				buf = DlException::GetBuffer(len);
				if (!CFStringGetCString(value, buf, len+1, kCFStringEncodingUTF8)) {
					buf = NULL;
					this->itsBufUsed = 0;
				} else {
					this->itsBufUsed = strlen(buf) + 1;
				}
			} else {
				DlInt32 bufSize = GetBufferSize();
				buf = GetBuffer(bufSize);

				snprintf(buf, bufSize, "Failed to find string for key %s%d", stringName, strOfs);
			}
		}
	}
	
	return buf;
}

#endif

//------------------------------------------------------------------------------
//	DlException::MakeString												virtual
//
//		Make the string to display in the dialog box
//
//	returns					<-	pointer to string
//------------------------------------------------------------------------------
const char *
DlException::MakeString() const throw()
{
	if (itsError == 0x8000)
		return sExceptionBuffer;
	return PrintToBuffer(DlExceptionName(DlException), 0, GetErrorCode());
}

//------------------------------------------------------------------------------
//	DlException::DefaultStr												static
//
//		return the last chance string
//
//	returns					<-	pointer to string
//------------------------------------------------------------------------------
const char *
DlException::DefaultStr()
{
	return kDefaultString;
}

#if defined(DlDebugging)

//------------------------------------------------------------------------------
//	DlAssert
//
//		Assert function
//
//	cond					->	the condition causing the alert
//	file					<->	the file name
//	line					<->	the line
//------------------------------------------------------------------------------
void
DlAssert(const char * cond, const char * file, DlInt32 line) {

	DebugAssert('this', 0, cond, NULL, NULL, file, line, 0);
	Debugger();
}

//------------------------------------------------------------------------------
//	DlAssertException::MakeString										virtual
//
//		Make the string to display in the dialog box
//
//	returns					<-	pointer to string
//------------------------------------------------------------------------------
//const char *	
//DlAssertException::MakeString() const throw()
//{
//	return PrintToBuffer(DlExceptionName(DlAssertException), 0, 
//			itsCond, itsLine, itsFile);
//}

#endif

//	eof
