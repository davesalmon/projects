/*+
 *
 *  DlStrStream.h
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  Implement stream with string as storage. String-based strings act like files opened
 *	for append. All writes go to the end of the string. If a seek is attempted beyond
 *	the end of the string it is an error.
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

#ifndef _H_DlStrStream
#define _H_DlStrStream

#include "DlStream.h"
#include <string>

class DlStrStream : public DlStream
{
public:
    // constructors
	DlStrStream() : _s(0), _readPos(0) {}
	explicit DlStrStream(const char* s) : _s(NEW std::string(s)), _readPos(0) {}
	explicit DlStrStream(const std::string& s) : _s(NEW std::string(s)), _readPos(0) {}
    
	~DlStrStream();
	
    // copying
	DlStrStream(const DlStrStream& s) : DlStream(s), _s(s._s), _readPos(s._readPos) {}
	DlStrStream& operator=(const DlStrStream& s);

	virtual DlUInt32 ReadBytes(void* buf, DlUInt32 size);
	virtual DlUInt32 WriteBytes(const void* buf, DlUInt32 size);	

	virtual void Seek(DlInt32 offset, DlFilePosition fromWhere);
	virtual DlInt64 Tell() const;
	
	void Reset() {
		_readPos = 0;
	}

	const std::string& Buffer() {
		if (_s == 0) {
			_s = NEW std::string;
		}
		return *_s;
	}

protected:
private:
	std::string *_s;
	std::string::size_type _readPos;
};


#endif

