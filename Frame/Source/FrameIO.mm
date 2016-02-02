/*+
 *
 *  FrameIO.mm
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Define frame reader and writer implementations of structure input/output
 *	streams.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
-*/

#include "DlPlatform.h"
#include "FrameIO.h"
#include "DlException.h"

FrameReader::FrameReader(NSData* data)
	: _bytes((const DlUInt8*)[data bytes]), _len([data length]), _pos(0)
{
}

FrameReader::~FrameReader()
{
}

inline DlInt32 float32ToInt(DlFloat32 f)
{
	return *(DlInt32 *)(&f);
}

inline uint64_t float64ToInt(DlFloat64 f)
{
	return *(uint64_t *)(&f);
}

inline DlFloat32 intToFloat32(DlInt32 f)
{
	return *(DlFloat32*)(&f);
}

inline DlFloat64 intToFloat64(uint64_t f)
{
	return *(DlFloat64*)(&f);
}

inline DlUInt32 
FrameReader::GetOne() 
{
	if (_len == _pos)
		throw DlException("File too short");
//	_len -= 1;
	return _bytes[_pos++];
}

inline void
FrameReader::GetBuffer(void* buf, DlUInt32 len) {
	if ((_len - _pos) < len)
		throw DlException("File too short");
//	_len -= len;
	memcpy(buf, _bytes + _pos, len);
	_pos += len;
}

bool 
FrameReader::GetBool()
{
	return GetOne() != 0;
}

DlUInt8 
FrameReader::GetByte() 
{
	return GetOne();
}

DlInt32
FrameReader::GetInt()
{
	DlInt32 val;
	GetBuffer(&val, sizeof(val));
	return CFSwapInt32BigToHost(val);
}

DlFloat32
FrameReader::GetFloat()
{
	DlInt32 val;
	GetBuffer(&val, sizeof(val));
	return intToFloat32(CFSwapInt32BigToHost(val));
}

DlFloat64
FrameReader::GetDouble()
{
	uint64_t val;
	GetBuffer(&val, sizeof(val));
	return intToFloat64(CFSwapInt64BigToHost(val));
}

DlString
FrameReader::GetString()
{
	DlString str;
	while (1) {
		char c = (char)GetOne();
		if (c == 0)
			break;
		str += c;
	}
	return str;
}


FrameWriter::FrameWriter(int initialLength)
	: _data([[NSMutableData dataWithCapacity: initialLength]retain])
	, _pos(0)
{
}

FrameWriter::~FrameWriter()
{
	[_data release];
}

void 
FrameWriter::PutBool(bool val)
{
	const DlUInt8 _trueValue = 0xff;
	const DlUInt8 _falseValue = 0;
	[_data appendBytes: (val ? &_trueValue : &_falseValue) length: sizeof(DlUInt8)];
}

void 
FrameWriter::PutByte(DlUInt8 val)
{
	[_data appendBytes: &val length: sizeof(val)];
}

void 
FrameWriter::PutInt(DlInt32 val)
{
	val = CFSwapInt32HostToBig(val);
	[_data appendBytes: &val length: sizeof(val)];
}

void 
FrameWriter::PutFloat(DlFloat32 val)
{
	DlInt32 wv = CFSwapInt32HostToBig(float32ToInt(val));
	[_data appendBytes: &wv length: sizeof(wv)];
}

void 
FrameWriter::PutDouble(DlFloat64 val)
{
	uint64_t wv = CFSwapInt64HostToBig(float64ToInt(val));
	[_data appendBytes: &wv length: sizeof(wv)];
}

void 
FrameWriter::PutString(const char* str)
{
	DlUInt32 len = strlen(str);
	[_data appendBytes: str length: len + 1];
}

