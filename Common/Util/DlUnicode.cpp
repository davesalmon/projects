/*!
 * 	\file DlUnicode.cpp
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

#include "DlPlatform.h"
#include "DlTypes.h"
#include "DlUnicode.h"
#include "DlStringUtil.h"

#if TARG_OS_MAC && !TARG_OS_DARWIN
#include <Files.h>
#endif

#include <memory>

using namespace std;

// ----------------------------------------------------------------------------
DlUChar
DlDecodeUTF8(const DlUInt8 *buf, DlInt32 len)
{
	#define UTF8_VALID(c) ((c & 0xc0) == 0x80)
    switch(len) {
    case 1:	if ((buf[0] & 0x80) == 0)
    	return *buf;
    case 2: if (UTF8_VALID(buf[1]))
       	return (buf[1] & 0x3f) | ((buf[0] & 0x1f) << 6);
    case 3: if (UTF8_VALID(buf[1]) && UTF8_VALID(buf[2]))
    	return (buf[2] & 0x3f) | ((buf[1] & 0x3f) << 6) | ((buf[0] & 0xf) << 12);
    }
    throw DlException("Invalid UTF8 sequence");
}

inline DlUChar
DlDecodeUTF8(const DlChar *str, DlInt32 len)
{
	return DlDecodeUTF8(reinterpret_cast<const DlUInt8*>(str), len);
}


// ----------------------------------------------------------------------------
DlSizeT
DlEncodeUTF8(DlWChar c, DlUInt8 buf[3])
{
    if ((c & 0xff80) == 0) {
        buf[0] = static_cast<DlUInt8>(c);
        return 1;
    } else if ((c & 0xf800) == 0) {
        buf[0] = static_cast<DlUInt8>(((c >> 6) & 0x1ff) | 0xc0);
        buf[1] = static_cast<DlUInt8>((c & 0x3f) | 0x80);
        return 2;
    } else {
        buf[0] = static_cast<DlUInt8>(((c >> 12) & 0xff) | 0xe0);
        buf[1] = static_cast<DlUInt8>(((c >> 6) & 0x3f) | 0x80);
        buf[2] = static_cast<DlUInt8>((c & 0x3f) | 0x80);
        return 3;
    }
}

// ----------------------------------------------------------------------------
DlSizeT 
DlUTF8StrLen(const DlChar* str)
{
	DlSizeT len =  0;

	while(*str) {
		DlInt32 cLen = DlUTF8Len(static_cast<DlUInt8>(*str));
		if (cLen < 0)
			throw DlException("Bad utf8 character encountered!");
		len ++;
		str += cLen;
	}
	return len;
}

// ----------------------------------------------------------------------------
//
//	CROSS PLATFORM DEFINITIONS.
//
// ----------------------------------------------------------------------------

inline DlUChar
DlNextWide(const DlChar*& str)
{
	//	const DlUInt8* buf =
	DlSizeT count = DlUTF8Len(static_cast<DlUInt8>(*str));
	DlUChar ch = DlDecodeUTF8(str, count);
	str += count;
	return ch;
}

template <typename T>
std::basic_string<T> ConvertUTF8ToString(const DlChar* buffer, DlSizeT len)
{
	std::basic_string<T> s;
	
	if (len == 0) {
		while(*buffer) {
			s += DlNextWide(buffer);
		}
	} else {
		while(len--) {
			s += DlNextWide(buffer);
		}
	}
	return s;
}

// ----------------------------------------------------------------------------
unistring
DlUTF8ToUnicode(const DlChar* buffer, DlSizeT len) 
{
	return ConvertUTF8ToString<DlUChar>(buffer, len);
}

// ----------------------------------------------------------------------------
std::u16string
DlUTF8ToU16String(const DlChar* buffer, DlSizeT len)
{
	return ConvertUTF8ToString<char16_t>(buffer, len);
}

// ----------------------------------------------------------------------------
std::wstring
DlUTF8ToWide(const DlChar* buffer, DlSizeT len)
{
	return ConvertUTF8ToString<wchar_t>(buffer, len);
}

// ----------------------------------------------------------------------------
std::string
DlUnicodeToUTF8(const DlUChar* buffer, DlSizeT len) 
{
	string result;
	if (len == 0) {
		while(*buffer) {
			DlUInt8 buf[3];
			DlSizeT count = DlEncodeUTF8(*buffer++, buf);
			result.append((DlChar*)buf, count);
		}
	} else {
		while(len--) {
			DlUInt8 buf[3];
			DlSizeT count = DlEncodeUTF8(*buffer++, buf);
			result.append((DlChar*)buf, count);
		}
	}
	return result;
}

// ----------------------------------------------------------------------------
bool
DlUnicodeToUTF8(const DlUChar* buffer, DlSizeT len, DlChar* outBuf, DlSizeT bufLen) 
{
	DlSizeT count = 0;
	while(len--) {
		DlUInt8 buf[3];
		DlSizeT nChars = DlEncodeUTF8(*buffer++, buf);
		count += nChars;
		if (count > bufLen)
			return false;
		memcpy(outBuf, buf, count);
	}
	return true;
}

// ----------------------------------------------------------------------------
std::wstring
DlUnicodeToWide(const DlUChar* buffer, size_t len)
{
	wstring result;
	if (len == 0) {
		while(*buffer)
			result += *buffer++;
	} else {
		result.resize(len);
		for (size_t i = 0; i < len; ++i, ++buffer)
			result[i] = static_cast<wchar_t>(*buffer);
	}
	return result;
}

std::u16string
DlUnicodeToU16(const DlUChar* buffer, DlSizeT len)
{
	static_assert(sizeof(char16_t)==sizeof(UniChar), "unicode char sizes do not match!");
	return std::u16string(reinterpret_cast<const char16_t*>(buffer), len);
}

// ----------------------------------------------------------------------------
unistring
DlWideToUnicode(const wchar_t* buffer, DlSizeT len)
{
	unistring result;
	if (len == 0) {
		while(*buffer)
			result += static_cast<DlUChar>(*buffer++);
	} else {
		result.resize(len);
		for (size_t i = 0; i < len; ++i, ++buffer)
			result[i] = static_cast<DlUChar>(*buffer);
		
	}
	return result;
}

unistring
DlU16ToUnicode(const char16_t* buffer, DlSizeT len)
{
	if (len != 0)
		return unistring(reinterpret_cast<const DlUChar*>(buffer), len);
	else
		return unistring(reinterpret_cast<const DlUChar*>(buffer));
}

// eof
