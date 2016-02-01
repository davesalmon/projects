/*+
 *
 *  FrameIO.h
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

#include "StrInputStream.h"
#include "StrOutputStream.h"

@class NSData;
@class NSMutableData;

class FrameReader : public StrInputStream
{
public:
	FrameReader(NSData* data);
	
    virtual ~FrameReader();
    
	virtual bool 		GetBool();
	virtual DlUInt8 	GetByte();
	virtual DlInt32 	GetInt();

	virtual DlFloat32	GetFloat();	
	virtual DlFloat64 	GetDouble();
	virtual DlString	GetString();

private:
    FrameReader(const FrameReader& f);
    FrameReader& operator=(const FrameReader& f);

	DlUInt32	GetOne();
	void		GetBuffer(void* buf, DlUInt32 len);

	const DlUInt8* 	_bytes;
	DlUInt32		_len;
	DlUInt32		_pos;
};

class FrameWriter : public StrOutputStream
{
public:
	FrameWriter(int initialLength);

	virtual ~FrameWriter();

	virtual void PutBool(bool val);
	virtual void PutByte(DlUInt8 val);
	virtual void PutInt(DlInt32 val);
	virtual void PutFloat(DlFloat32 val);
	virtual void PutDouble(DlFloat64 val);
	virtual void PutString(const char* str);

	NSData* GetBytes() const { return _data; }

private:
    FrameWriter(const FrameWriter& f);
    FrameWriter& operator=(const FrameWriter& f);
        
	NSMutableData* 	_data;
	int				_pos;
};
