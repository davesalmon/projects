/*!
 * 	\file DlUnicode.h
 * 	\author David C. Salmon
 *	\date 7/2/2003
 *
 *	Copyright (C) 2003 David C. Salmon. All rights reserved.
 *
 *	Implement utilities to support 16-bit unicode characters. This is primarily
 *	to support OSX.
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

#ifndef _H_DlUnicode
#define _H_DlUnicode

#include <string>
#include "DlException.h"

const DlUChar UTF_16BOM = u'\ufeff';

const uint8 UTF16BEChars[] { 0xfe, 0xff };
const uint8 UTF16LEChars[] { 0xff, 0xfe };
const uint8 UTF8Chars[] { 0xef, 0xbb, 0xbf };

/// \typedef unistring
/// STL string class for 16 bit characters. On windows, this is the same
///	as wstring.

typedef std::char_traits<DlUChar>	UniCharTraits;
typedef std::allocator<DlUChar>		UniCharAlloc;
typedef std::basic_string<DlUChar, UniCharTraits, UniCharAlloc > unistring;

/// Determine the number of characters required to decode the UTF8 character.
/// @param b the first byte in the sequence.
/// @return the number of bytes required to decode character.
DlInt32 		DlUTF8Len(DlUInt8 b);

/// Return the number of bytes required to UTF8 encode the specified character.
/// @param ch the character to encode.
/// @return the number of bytes required.
DlSizeT 		DlUCharLen(DlUChar ch);

/// Decode the UTF8 characters contained in buf.
/// @param buf the encoded bytes.
/// @param len the number bytes (DlUTF8Len()).
/// @return the decoded character.
DlUChar 		DlDecodeUTF8(const DlUInt8* buf, DlInt32 len);

/// Encode the UTF8 character into buf.
/// @param c the character to encode.
/// @param buf (returned) the encoded bytes.
/// @return the number of bytes returned in buf.
DlSizeT 		DlEncodeUTF8(DlWChar c, DlUInt8 buf[3]);

/// Return the number of characters in the UTF8 encoded string.
/// @param str the string to determine the length of.
/// @return the number of unicode characters in the string.
DlSizeT 		DlUTF8StrLen(const DlChar* str);

DlSizeT			DlUniStrLen(const DlUChar* str);

/// Convert the specified number of characters from UTF8 to unicode.
///	@param buffer the characters to convert.
///	@param len the number of characters to convert.
///	@return string.
unistring 		DlUTF8ToUnicode(const DlChar* buffer, DlSizeT len=0);

std::u16string	DlUTF8ToU16String(const DlChar* buffer, DlSizeT len=0);

/// Convert the specified number of characters from UTF8 to unicode.
///	@param buffer the characters to convert.
///	@param len the number of characters to convert.
///	@return string.
std::wstring 	DlUTF8ToWide(const DlChar* buffer, DlSizeT len=0);

/// Convert the specified number of characters from buffer from unicode to UTF8.
///	@param buffer the characters to convert.
/// @param len the number of characters to convert.
///	@return string.
std::string		DlUnicodeToUTF8(const DlUChar* buffer, DlSizeT len=0);

/// Convert a buffer of characters from unicode to UTF8. Returns false if the destination 
///	buffer is too small.
///	@param buffer the characters to convert.
/// @param len the number of characters to convert.
///	@param buf the output buffer.
///	@param bufLen the size of the output buffer.
///	@return true if converted, otherwise false.
bool  			DlUnicodeToUTF8(const DlUChar* buffer, DlSizeT len, DlChar* buf, DlSizeT bufLen);

/// Convert the specified number of character from unicode to wide.
///	@param buffer the characters to convert.
/// @param len the number of characters to convert.
///	@return wstring.
std::wstring	DlUnicodeToWide(const DlUChar* buffer, DlSizeT len = 0);

std::u16string	DlUnicodeToU16(const DlUChar* buffer, DlSizeT len=0);

/// Convert the specified number of characters to unistring.
///	@param buffer the characters to convert.
/// @param len the number of characters to convert.
///	@return unistring.
unistring 		DlWideToUnicode(const wchar_t* buffer, DlSizeT len=0);

unistring		DlU16ToUnicode(const char16_t* buffer, DlSizeT len=0);

// ----------------------------------------------------------------------------

inline DlInt32
DlUTF8Len(DlUInt8 b)
{
    if ((b & 0xf0) == 0xf0)         // extended unicode - unsupported
        return -1;
    if ((b & 0xe0) == 0xe0)         // 3 bytes
        return 3;
    else if ((b & 0xc0) == 0xc0)    // 2 bytes
        return 2;
    else if ((b & 0x80) == 0)       // 1 byte
        return 1;
    return -1;                      // invalid
}

// ----------------------------------------------------------------------------
inline DlSizeT
DlUCharLen(DlUChar ch)
{
    if ((ch & 0xff80) == 0)
        return 1;
    else if ((ch & 0xf800) == 0)
        return 2;
    else
        return 3;
}

#endif
