//
//  CFStringTracker.h
//
//  Created by David Salmon on 2/23/14.
//  Copyright © 2014-2016 David C. Salmon. All Rights Reserved.
//
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

#ifndef Common_Util_CFStringTracker_h
#define Common_Util_CFStringTracker_h

#include "CFRefTracker.h"
#include "DlUnicode.h"

class CFStringTracker : public CFRefTracker<CFStringRef> {
public:
	CFStringTracker();
	
	CFStringTracker(CFStringRef ref);
	
	// construct from c-string
	CFStringTracker(const char* s, CFStringEncoding enc = kCFStringEncodingUTF8);
	
	// construct from wide-string
	CFStringTracker(const wchar_t* s);
	
	// construct from mac unicode string
	CFStringTracker(const UniChar* s, std::size_t len);
	
	// construct from pascal string
	CFStringTracker(const unsigned char* s, CFStringEncoding enc = kCFStringEncodingUTF8);
	
	// construct from string table
	CFStringTracker(CFStringRef key, CFStringRef table, CFBundleRef ref);

	// construct from string table
	CFStringTracker(const char* key, CFStringRef table);
	
	// construct from string table.
	CFStringTracker(CFStringRef key, CFStringRef table);
	
	CFStringRef get() const;
		
private:
};

//----------------------------------------------------------------------------------------
//  CFStringTracker::CFStringTracker                                   constructor inline
//
//      Default. Construct empty CFString
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
CFStringTracker::CFStringTracker()
{
}

//----------------------------------------------------------------------------------------
//  CFStringTracker::CFStringTracker                                   constructor inline
//
//      Wrap an existing CFStringRef
//
//  CFStringRef ref    -> the ref
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
CFStringTracker::CFStringTracker(CFStringRef ref)
: CFRefTracker<CFStringRef>(ref)
{
}

//----------------------------------------------------------------------------------------
//  CFStringTracker::CFStringTracker                                   constructor inline
//
//      Construct with C-string. Use the specified encoding (defaults to UTF8).
//
//  const char* s          -> the string
//  CFStringEncoding enc   -> the encodign
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
CFStringTracker::CFStringTracker(const char* s, CFStringEncoding enc)
: CFRefTracker<CFStringRef>(CFStringCreateWithCString(kCFAllocatorDefault, s, enc))
{
}

//----------------------------------------------------------------------------------------
//  CFStringTracker::CFStringTracker                                   constructor inline
//
//      Construct with string of wchar_t
//
//  const wchar_t* s   -> the string
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
CFStringTracker::CFStringTracker(const wchar_t* s)
{
	unistring us(DlWideToUnicode(s));
	*this = CFStringCreateWithCharacters(kCFAllocatorDefault, us.c_str(), us.length());
}

//----------------------------------------------------------------------------------------
//  CFStringTracker::CFStringTracker                                   constructor inline
//
//      Construct with string of UniChar
//
//  const UniChar* s   -> the string
//  std::size_t        -> the length
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
CFStringTracker::CFStringTracker(const UniChar* s, std::size_t len)
: CFRefTracker<CFStringRef>(CFStringCreateWithCharacters(kCFAllocatorDefault, s, len))
{
}

//----------------------------------------------------------------------------------------
//  CFStringTracker::CFStringTracker                                   constructor inline
//
//      construct from pascal style string with the specified encoding (default UTF8).
//
//  const unsigned char* s -> pascal style string
//  CFStringEncoding enc   -> the encoding
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
CFStringTracker::CFStringTracker(const unsigned char* s, CFStringEncoding enc)
: CFRefTracker<CFStringRef>(CFStringCreateWithPascalString(kCFAllocatorDefault, s, enc))
{
}

//----------------------------------------------------------------------------------------
//  CFStringTracker::CFStringTracker                                   constructor inline
//
//      Construct for string table lookup.
//
//  const char* key    -> the key
//  CFStringRef table  -> the table
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
CFStringTracker::CFStringTracker(const char* key, CFStringRef table)
: CFRefTracker<CFStringRef>(CFCopyLocalizedStringFromTable(CFStringTracker(key).get(), table, 0))
{
}

//----------------------------------------------------------------------------------------
//  CFStringTracker::CFStringTracker                                   constructor inline
//
//      Construct for string table lookup.
//
//  CFStringRef key    -> the key
//  CFStringRef table  -> the table
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
CFStringTracker::CFStringTracker(CFStringRef key, CFStringRef table)
: CFRefTracker<CFStringRef>(CFCopyLocalizedStringFromTable(key, table, 0))
{
}

// construct from string table
inline
CFStringTracker::CFStringTracker(CFStringRef key, CFStringRef table, CFBundleRef bndl)
: CFRefTracker<CFStringRef>(CFCopyLocalizedStringFromTableInBundle(key, table, bndl, 0))
{
}

//----------------------------------------------------------------------------------------
//  CFStringTracker::get                                                           inline
//
//      return CFStringRef
//
//  returns CFStringRef    <- the reference.
//----------------------------------------------------------------------------------------
inline
CFStringRef
CFStringTracker::get() const
{
	return (CFStringRef)*this;
}

#endif // Common_Util_CFStringTracker_h

