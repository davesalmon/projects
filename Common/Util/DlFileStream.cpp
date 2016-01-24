//+
//	DlFileStream.cpp
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "DlFileStream.h"
#include "DlDataFile.h"

//---------------------------------- Routines ----------------------------------

DlFileStream::DlFileStream(const DlFileSpec& spec, DlOpenMode mode)
	: _file(NEW DlDataFile(spec))
{
	_file->OpenFile(mode);
}

DlFileStream::DlFileStream(const DlDataFile& file, DlOpenMode mode)
	: _file(NEW DlDataFile(file.GetSpec()))
{
	_file->OpenFile(mode);
}

DlFileStream::~DlFileStream()
{
	if (LastReference()) {
		delete _file;
	}
}

DlFileStream& 
DlFileStream::operator=(const DlFileStream& d)
{
    if (&d != this)
    {
        if (LastReference())
            delete _file;
        
        DlStream::operator=(d);
        _file = d._file;
    }
    
    return *this;
}

DlUInt32
DlFileStream::ReadBytes(void * buf, DlUInt32 size)
{
	return _file->Read(buf, size);
}

DlUInt32
DlFileStream::WriteBytes(const void * buf, DlUInt32 size)
{
	return _file->Write(buf, size);
}

void 
DlFileStream::Seek(DlInt32 offset, DlFilePosition fromWhere)
{
	_file->Seek(offset, fromWhere);
}

DlInt64 
DlFileStream::Tell() const
{
	return _file->Tell();
}


//	eof
