/*+
 *
 *  DlCFUtilities.cpp
 *
 *  Copyright © 2006 David C. Salmon. All Rights Reserved.
 *
 *  C++ Wrappers for Core Foundation objects.
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

#include "DlPlatform.h"
#include "DlCFUtilities.h"
#include "DlAssert.h"
#include "DlException.h"
#include <memory>

#include "CFStringTracker.h"

static std::unique_ptr<UniChar[]> GetUniChars(CFStringRef ref, std::size_t& len);

//----------------------------------------------------------------------------------------
//  GetCFStringWideValue
//
//      return wstring from CFString.
//
//  CFStringRef ref        -> the CF string ref.
//
//  returns std::wstring   <- the wstring.
//----------------------------------------------------------------------------------------
std::wstring 
GetCFStringWideValue(CFStringRef ref) 
{
	std::size_t len;
	std::unique_ptr<UniChar[]> chars { GetUniChars(ref, len) };
	if (len) {
		std::wstring dest;
		dest.resize(len);
		const UniChar* ptr = chars.get();
		for (std::size_t i = 0; i < len; ++i, ++ptr)
			dest[i] = static_cast<wchar_t>(*ptr);
		return dest;
	}
	return std::wstring();
}

//----------------------------------------------------------------------------------------
//  GetCFStringAsciiValue
//
//      return string from CFString assuming mac-roman encoding.
//
//  CFStringRef ref        -> the string ref.
//
//  returns std::string    <- the string.
//----------------------------------------------------------------------------------------
std::string 
GetCFStringAsciiValue(CFStringRef ref) 
{
	std::size_t len = CFStringGetLength(ref);
	if (len) {
		len *= 3;
		std::unique_ptr<char[]> chars(NEW char[len+1]);
		if (CFStringGetCString(ref, chars.get(), len+1, kCFStringEncodingMacRoman)) {
			return std::string(chars.get());
		}
	}
	return std::string();
}

//----------------------------------------------------------------------------------------
//  GetCFStringUTF8Value
//
//      return string from CFString assuming utf-8 encoding.
//
//  CFStringRef ref        -> the string ref.
//
//  returns std::string    <- the utf-8 string.
//----------------------------------------------------------------------------------------
std::string 
GetCFStringUTF8Value(CFStringRef ref)
{
	std::size_t len = CFStringGetLength(ref);
	if (len) {
		len *= 3;
		std::unique_ptr<char[]> chars(NEW char[len+1]);
		if (CFStringGetCString(ref, chars.get(), len+1, kCFStringEncodingUTF8)) {
			return std::string(chars.get());
		}
	}
	return std::string();
}

//----------------------------------------------------------------------------------------
//  GetCFStringUnicodeValue
//
//      return unistring from CFString.
//
//  CFStringRef ref    -> the string ref
//
//  returns unistring  <- the unicode string (with UniChar characters)
//----------------------------------------------------------------------------------------
unistring
GetCFStringUnicodeValue(CFStringRef ref) 
{
	std::size_t len;
	std::unique_ptr<UniChar[]> chars { GetUniChars(ref, len) };
	if (len) {
		return unistring(chars.get(), len);
	}
	return unistring();
}

//----------------------------------------------------------------------------------------
//  GetCFStringUnicodeValue
//
//      return unistring from CFString.
//
//  CFStringRef ref    -> the string ref
//
//  returns unistring  <- the unicode string (with UniChar characters)
//----------------------------------------------------------------------------------------
std::u16string
GetCFStringU16StringValue(CFStringRef ref)
{
	static_assert(sizeof(char16_t)==sizeof(UniChar), "unicode char sizes do not match!");

	std::size_t len;
	std::unique_ptr<UniChar[]> chars { GetUniChars(ref, len) };
	if (len) {
		return std::u16string(reinterpret_cast<const char16_t*>(chars.get()), len);
	}
	return std::u16string();
}

//----------------------------------------------------------------------------------------
//  GetPropertyList
//
//      return a property list dictionary.
//
//  CFStringRef resourceName   -> the list name
//  CFStringRef resourceType   -> the resource type
//  CFBundleRef bundle         -> the bundle to search. can be NULL
//  CFStringRef subdir         -> the sub dir
//
//  returns CFDictionaryRef    <- the plist dictionary.
//----------------------------------------------------------------------------------------
CFRefTracker<CFDictionaryRef>
GetPropertyList(CFStringRef resourceName, 
					CFStringRef resourceType, CFBundleRef bundle, CFStringRef subdir) 
{
	CFDictionaryRef result = NULL;
	CFURLRef accessRef = CFBundleCopyResourceURL(bundle ? bundle : CFBundleGetMainBundle(), 
							resourceName, resourceType, subdir);
	if (accessRef) {
		CFDataRef	theData = NULL;
		if (CFURLCreateDataAndPropertiesFromResource(
                    kCFAllocatorDefault, accessRef, 
                    &theData, NULL, NULL, NULL)) {
			CFStringTracker errString;
			CFPropertyListRef props = CFPropertyListCreateFromXMLData(
                    kCFAllocatorDefault, theData, 
					kCFPropertyListImmutable, &errString.ref());
			if (props == NULL) {
				printf("Failed to access resource %s.%s. Error is %s\n", 
					GetCFStringAsciiValue(resourceName).c_str(), 
					GetCFStringAsciiValue(resourceType).c_str(),
					GetCFStringAsciiValue(errString.get()).c_str());
			} else {
				if (CFGetTypeID(props) == CFDictionaryGetTypeID()) {
					result = static_cast<CFDictionaryRef>(props);
				} else {
					printf("%s.%s does not contain a dictionary\n", 
							GetCFStringAsciiValue(resourceName).c_str(), 
							GetCFStringAsciiValue(resourceType).c_str());

					CFRelease(props);
				}
			}
			CFRelease(theData);
		}
		CFRelease(accessRef);
	}
	return result;
}

//----------------------------------------------------------------------------------------
//  GetDictionaryValue
//
//      return a dictionary value froma dictionary
//
//  CFStringRef key            -> the key
//  CFDictionaryRef dict       -> the dictionary
//
//  returns CFDictionaryRef    <- the sub dictionary
//----------------------------------------------------------------------------------------
CFDictionaryRef
GetDictionaryValue(CFStringRef key, CFDictionaryRef dict)
{
	const void* val = CFDictionaryGetValue(dict, key);
	if (CFGetTypeID(val) == CFDictionaryGetTypeID())
		return static_cast<CFDictionaryRef>(val);
	return NULL;
}

//----------------------------------------------------------------------------------------
//  GetDictionaryValue
//
//      return a dictionary value.
//
//  const char* key            -> the key
//  CFDictionaryRef dict       -> the dictionary
//
//  returns CFDictionaryRef    <- the sub dictionary
//----------------------------------------------------------------------------------------
CFDictionaryRef	
GetDictionaryValue(const char* key, CFDictionaryRef dict) 
{
	return GetDictionaryValue(CFStringTracker(key).get(), dict);
}

void
PrintStringDictionary(CFDictionaryRef dict)
{
	size_t size = CFDictionaryGetCount(dict);
	CFTypeRef *keysTypeRef = (CFTypeRef *) malloc( size * sizeof(CFTypeRef) );
	CFDictionaryGetKeysAndValues(dict, (const void **) keysTypeRef, NULL);
	
	fprintf(stderr, "dict has %ld keys", size);
	for (size_t i = 0; i < size; i++) {
		CFStringRef key = (CFStringRef)keysTypeRef[i];
		
		std::string s(GetCFStringUTF8Value(key));
		std::string v(GetStringAsciiValue(key, dict));
		
		fprintf(stderr, "%s = %s\n", s.c_str(), v.c_str());
	}
}

//----------------------------------------------------------------------------------------
//  GetStringAsciiValue
//
//      return an ascii string value from a dictionary
//
//  const char* key        -> the key
//  CFDictionaryRef dict   -> the dictionary
//
//  returns std::string    <- the value.
//----------------------------------------------------------------------------------------
std::string 
GetStringAsciiValue(const char* key, CFDictionaryRef dict) 
{
	return GetStringAsciiValue(CFStringTracker(key).get(), dict);
}

//----------------------------------------------------------------------------------------
//  GetStringAsciiValue
//
//      return an ascii string value.
//
//  CFStringRef key        -> the key
//  CFDictionaryRef dict   -> the dictionary
//
//  returns std::string    <- the value.
//----------------------------------------------------------------------------------------
std::string 
GetStringAsciiValue(CFStringRef key, CFDictionaryRef dict) 
{
	const void* val = CFDictionaryGetValue(dict, key);
	if (CFGetTypeID(val) == CFStringGetTypeID())
		return GetCFStringAsciiValue(static_cast<CFStringRef>(val));
	return std::string();
}

//----------------------------------------------------------------------------------------
//  GetStringUTF8Value
//
//      return a utf-8 string value.
//
//  const char* key        -> the key
//  CFDictionaryRef dict   -> the dictionary
//
//  returns std::string    <- the value.
//----------------------------------------------------------------------------------------
std::string 
GetStringUTF8Value(const char* key, CFDictionaryRef dict) 
{
	return GetStringUTF8Value(CFStringTracker(key).get(), dict);
}

//----------------------------------------------------------------------------------------
//  GetStringUTF8Value
//
//      return a utf-8 string value.
//
//  CFStringRef key        -> the key
//  CFDictionaryRef dict   -> the dictionary
//
//  returns std::string    <- the value.
//----------------------------------------------------------------------------------------
std::string 
GetStringUTF8Value(CFStringRef key, CFDictionaryRef dict) 
{
	const void* val = CFDictionaryGetValue(dict, key);
	if (CFGetTypeID(val) == CFStringGetTypeID())
		return GetCFStringUTF8Value(static_cast<CFStringRef>(val));
	return std::string();
}

//----------------------------------------------------------------------------------------
//  GetUniChars
//
//      get unicode chars.
//
//  CFStringRef ref                -> the string
//  size_t& len                    <-> the length
//
//  returns std::auto_ptr<UniChar> <- array of UniChar
//----------------------------------------------------------------------------------------
std::unique_ptr<UniChar[]>
GetUniChars(CFStringRef ref, size_t& len)
{
	len = CFStringGetLength(ref);
	if (len) {
		CFRange chars = { 0, static_cast<CFIndex>(len) };
		std::unique_ptr<UniChar[]> buffer(NEW UniChar[len]);
		CFStringGetCharacters(ref, chars, buffer.get());
		return buffer;
	}
	
	return std::unique_ptr<UniChar[]>(0);
}

//----------------------------------------------------------------------------------------
//  GetHttpResponse
//
//      Get a reponse from an http server using CFNetwork stuff. If code is specified,
//		non-200 responses are returned. otherwise an exception is thrown.
//
//  std::string& resp		<- the response
//  const std::string& url	-> the url
//  bool autoRedirect		-> true to automatically redirect.
//  const char* useragent	-> the user-agent string
//	DlUInt32* code			-> the response code.
//	void (*callback)(void*)	-> callback function.
//	void* data				-> passed to callback.
//
//  returns nothing
//----------------------------------------------------------------------------------------
CFRefTracker<CFDictionaryRef> 
GetHttpResponse(std::string& resp, const std::string& url, 
	bool autoRedirect, const char* useragent,
	DlUInt32* code, void (*callback)(void*), void* data)
{
	// create the request
	CFRefTracker<CFHTTPMessageRef> myRequest(CFHTTPMessageCreateRequest(
		kCFAllocatorDefault, CFSTR("GET"), 
		CFRefTracker<CFURLRef>(CFURLCreateWithString(kCFAllocatorDefault, 
			CFStringTracker(url.c_str()).get(), NULL)), 
		kCFHTTPVersion1_1));

	// set a header value
	CFHTTPMessageSetHeaderFieldValue(myRequest, CFSTR("User-Agent"), 
		CFStringTracker(useragent != 0 ? useragent : "test").get());

	// read the header
	CFRefTracker<CFHTTPMessageRef> httpResp(CFHTTPMessageCreateEmpty(
			kCFAllocatorDefault, FALSE));

	// create the stream
	CFRefTracker<CFReadStreamRef> readRef(CFReadStreamCreateForHTTPRequest(
		kCFAllocatorDefault, myRequest));
	
	if (!httpResp)
	{
		throw DlException("Failed to create response for %s", url.c_str());
	}

	if (!readRef)
	{
		throw DlException("Failed to create stream for %s", url.c_str());
	}

	// tell it to redirect
	if (autoRedirect) {
		// if the URL is malformed, it will throw here...
		try {
		CFReadStreamSetProperty(readRef,
			kCFStreamPropertyHTTPShouldAutoredirect, kCFBooleanTrue);
		} catch(...) {
			throw DlException("Malformed URL %s", url.c_str());
		}
	}
	
	if (!CFReadStreamOpen(readRef))
	{
		throw DlException("Failed to open stream for %s", url.c_str());
	}
	
	CFRefTracker<CFDictionaryRef> headers;
	
	// now read the data
	bool headerComplete = false;
	while(true)
	{
		if (CFReadStreamHasBytesAvailable(readRef))
		{
			char buffer[16384];
			
			CFIndex bytesRead = CFReadStreamRead(readRef,
				(UInt8*)buffer, sizeof(buffer));
				
			if (bytesRead == 0)
			{
				if (CFReadStreamGetStatus(readRef) == kCFStreamStatusAtEnd)
					break;
			}
			else if (bytesRead < 0)
			{
				CFStreamError error = CFReadStreamGetError(readRef);
				throw DlException("Failed to read response for %s (%ld)",
					url.c_str(), error.error);
			}
			else
			{
//				std::string s(buffer, bytesRead);
//				fprintf(stderr, "Read %s\n", s.c_str());
			
				if (!CFHTTPMessageAppendBytes(httpResp, (UInt8*)buffer, bytesRead))
				{
					throw DlException("Invalid response for %s", url.c_str());
				}

				if (!headerComplete)
				{
					if (CFHTTPMessageIsHeaderComplete(httpResp))
					{
						headerComplete = true;
						UInt32 theCode = CFHTTPMessageGetResponseStatusCode(httpResp);
						
						if (code != 0)
							*code = theCode;
						else if (theCode != 200)
						{
							throw DlException("received status code %lu", theCode);
						}
						
						// we need to create a separate message
						//	to extract the headers from the stream.
						CFRefTracker<CFHTTPMessageRef> m(
							(CFHTTPMessageRef)CFReadStreamCopyProperty(readRef, 
							kCFStreamPropertyHTTPResponseHeader)); 
						
						headers = CFRefTracker<CFDictionaryRef>(
							CFHTTPMessageCopyAllHeaderFields(m));
					}
				}
			}
		}
		else
		{
			if (callback)
				(*callback)(data);
			if (CFReadStreamGetStatus(readRef) == kCFStreamStatusAtEnd)
				break;
		}
	}
	
	if (!headerComplete)
		throw DlException("Incomplete response for %s", url.c_str());
	
//#if DlDebugging
//	PrintStringDictionary(headers);
//#endif

	CFRefTracker<CFDataRef> body(CFHTTPMessageCopyBody(httpResp));
	CFIndex len = CFDataGetLength(body);

	resp.assign((char*)CFDataGetBytePtr(body), len);
	size_t cstrl = strlen(resp.c_str());
	
#if DlDebugging
	fprintf(stderr, "len = %ld, cstrlen= %ld\n", len, cstrl);
	fprintf(stderr, "body is %s\n", resp.c_str());
#endif
	
	// check if there is some other encoding.
	if (cstrl != len && cstrl < 2) {
		// if 0x or x0 we are BE or LE.
		CFStringEncoding enc = resp.length() == 0 ? kCFStringEncodingUTF16BE : kCFStringEncodingUTF16LE;
		CFRefTracker<CFStringRef> str(
				CFStringCreateFromExternalRepresentation(
														 kCFAllocatorDefault,
														 body,
														 enc));
		resp.assign(GetCFStringUTF8Value(str));
	}

	return headers;
}

//----------------------------------------------------------------------------------------
//  EscapeURLString
//
//      escape the illegal characters in the specified string for url path.
//
//  const std::string& urlString -> the string
//
//  returns std::string    <- the escaped string.
//----------------------------------------------------------------------------------------
std::string
EscapeURLString(const std::string& urlString)
{
	CFStringTracker s(urlString.c_str());
	
	CFStringTracker newString(CFURLCreateStringByAddingPercentEscapes(
						kCFAllocatorDefault, s, NULL, NULL, kCFStringEncodingUTF8));

	return GetCFStringUTF8Value(newString);
}

//----------------------------------------------------------------------------------------
//  DlGetBundleIDString
//
//      return the bundle identifier.
//
//  const char* key        -> 
//  const char* def        -> 
//  const char* table      -> 
//
//  returns std::string    <- 
//----------------------------------------------------------------------------------------
std::string
DlGetBundleString(const char* key, const char* def,
					const char* table, const char* bndlId)
{
	// note that we may not release the bundle.
	CFBundleRef bndlRef(nullptr);
	
	CFStringTracker lookupKeyRef(key);
	if (bndlId)
		bndlRef = CFBundleGetBundleWithIdentifier(CFStringTracker(bndlId));
	
	CFStringTracker tableRef;
	if (table)
		tableRef = CFStringTracker(table);
	
	// get the value
	CFStringTracker valueRef(lookupKeyRef, tableRef, bndlRef);
	
	
	if (def == nullptr || valueRef.get() != lookupKeyRef.get()) {
		return GetCFStringUTF8Value(valueRef.get());
	}
	
	return std::string(def);
}
