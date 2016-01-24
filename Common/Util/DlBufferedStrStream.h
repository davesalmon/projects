/*
 *  DlBufferedStrStream.h
 *  Common
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright (c) 2002 David C. Salmon. All rights reserved.
 *
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

#ifndef _H_DlBufferedStrStream
#define _H_DlBufferedStrStream

#include "DlBufferedStream.h"
#include "DlStrStream.h"

class DlBufferedStrStream : public DlBufferedStream {
public:
	explicit DlBufferedStrStream()
		: DlBufferedStream(_stream)
		, _stream()
	{
	}

	explicit DlBufferedStrStream(const char* string)
		: DlBufferedStream(_stream)
		, _stream(string) {
	}

	explicit DlBufferedStrStream(DlStrStream& stream)
		: DlBufferedStream(_stream)
		, _stream(stream) {
	}
	
	virtual ~DlBufferedStrStream() {
		DlBufferedStream::Flush();
	}

	const char* Buffer() {
		DlBufferedStream::Flush();
		return _stream.Buffer().c_str();
	}

	long Length() {
		DlBufferedStream::Flush();
		return _stream.Buffer().length();
	}
	
private:
	DlStrStream _stream;
};

#endif	// DlBufferedStrStream
