/*+
 *
 *  DlCFUtilities.h
 *
 *  Copyright © 2006 David C. Salmon. All Rights Reserved.
 *
 *  Define utility methods to convert between Core Foundation stuff and C++.
 *
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

#ifndef _H_DlCFUtilities
#define _H_DlCFUtilities

#include "CFRefTracker.h"

#include <string>
#include "DlUnicode.h"

std::wstring	GetCFStringWideValue(CFStringRef ref);
std::string		GetCFStringAsciiValue(CFStringRef ref);
std::string		GetCFStringUTF8Value(CFStringRef ref);

std::u16string	GetCFStringU16StringValue(CFStringRef ref);
unistring		GetCFStringUnicodeValue(CFStringRef ref);

CFRefTracker<CFDictionaryRef> GetPropertyList(CFStringRef resourceName,
					CFStringRef resourceType, CFBundleRef bundle, CFStringRef subdir);

CFDictionaryRef	GetDictionaryValue(CFStringRef key, CFDictionaryRef dict);
CFDictionaryRef	GetDictionaryValue(const char* key, CFDictionaryRef dict);

std::string GetStringAsciiValue(const char* key, CFDictionaryRef dict);
std::string GetStringAsciiValue(CFStringRef key, CFDictionaryRef dict);
std::string GetStringUTF8Value(const char* key, CFDictionaryRef dict);
std::string GetStringUTF8Value(CFStringRef key, CFDictionaryRef dict);

CFRefTracker<CFDictionaryRef> GetHttpResponse(std::string& resp, const std::string& url, bool autoRedirect,
		const char* useragent=0, DlUInt32* code=0, void (*callback)(void*) = 0, void* data=0);

std::string EscapeURLString(const std::string& urlString);

/// Return a string from the specified string bundle.
/// @param key the key for the string.
/// @param defaultValue the response if there is no value for key.
/// @param table the table to use for lookup.
/// @param bundle id the bundle to use for lookup.
/// @return the string value in the table.
std::string DlGetBundleString(const char* key, const char* defaultValue=0,
							  const char*table=0, const char* bundleId=0);


#endif
