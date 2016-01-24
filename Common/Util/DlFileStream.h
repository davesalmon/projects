//+
//	DlFileStream.h
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains the specialization of DlStream protocol for files.
//-
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

#ifndef _H_DlFileStream
#define _H_DlFileStream

//---------------------------------- Includes ----------------------------------

#include "DlStream.h"
#include "DlFileSpec.h"

//---------------------------------- Defines -----------------------------------

//---------------------------------- Declares ----------------------------------

class DlDataFile;

class DlFileStream : public DlStream {

public:

	DlFileStream(const DlFileSpec& spec, DlOpenMode mode);
	DlFileStream(const DlDataFile& file, DlOpenMode mode);
    
    DlFileStream(const DlFileStream& d) : DlStream(d), _file(d._file) {}
    DlFileStream& operator=(const DlFileStream& d);

	virtual ~DlFileStream();
    
	virtual DlUInt32 ReadBytes(void * buf, DlUInt32 size);
	virtual DlUInt32 WriteBytes(const void * buf, DlUInt32 size);
	
	virtual void Seek(DlInt32 offset, DlFilePosition fromWhere);
	virtual DlInt64 Tell() const;

	DlDataFile* GetFile() const {
		return _file;
	}
	
private:
    
	DlDataFile* _file;
};


#endif
