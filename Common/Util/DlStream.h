/*
 *  DlStream.h
 *
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
 *	Abstract base class for byte-stream behavior.
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

#ifndef _H_DlStream
#define _H_DlStream

#include "DlTypes.h"

#include <assert.h>

class DlStream {
public:
	DlStream() : _refCount(NEW DlInt32(1)) {}
	
	virtual ~DlStream();
	
	DlStream(const DlStream& s);

	DlStream& operator=(const DlStream& s);

	virtual DlUInt32 ReadBytes(void * buf, DlUInt32 size) =  0;
	virtual DlUInt32 WriteBytes(const void * buf, DlUInt32 size) = 0;

	virtual void Seek(DlInt32 offset, DlFilePosition fromWhere);
	virtual DlInt64 Tell() const;

protected:
	bool LastReference() const {
		assert(_refCount != 0);
		return *_refCount == 1;
	}
private:
	void _DecReference();
	void _IncReference();

	DlInt32* _refCount;
};

inline
DlStream::DlStream(const DlStream& s) 
	: _refCount(s._refCount)
{
	_IncReference();
}

inline
void DlStream::_DecReference() {
	if (--(*_refCount) == 0) {
		delete _refCount;
		_refCount = 0;
	}
}

inline
void DlStream::_IncReference() {
	(*_refCount)++;
}

inline
DlStream& DlStream::operator=(const DlStream& s) {
	if (this != &s) {
		_DecReference();
		_refCount = s._refCount;
		_IncReference();
	}

	return *this;
}

#endif
