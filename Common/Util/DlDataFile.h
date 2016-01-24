//+
//	DlDataFile.h
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains the data file class.
//
//	See DlFileException for exception class
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

#ifndef _H_DlDataFile
#define _H_DlDataFile

//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"

#include "DlFileSpec.h"
#include "DlAssert.h"

//#include "UDebugging.h"

//---------------------------------- Defines -----------------------------------

//---------------------------------- Declares ----------------------------------

class DlDataFile {

public:

	DlDataFile(DlFileRef&& ref);
	
	DlDataFile(const DlFileSpec& spec);
	~DlDataFile() {CloseFile();}

	void	OpenFile(DlOpenMode theMode);
	void	CloseFile();

	DlUInt32 Read(void* buf, DlUInt32 len);
	DlUInt32 Write(const void* buf, DlUInt32 len);

	void Seek(DlInt64 offset, DlFilePosition fromWhere);
	DlInt64 Tell() const ;

	DlInt64 FileSize() const;
//	void SetFileSize(DlInt64 fileSize);
	
	bool AtEOF() const;

	const DlFileSpec& GetSpec() const {
		return _spec;
	}
	
private:
	DlDataFile(const DlDataFile& file);
	DlDataFile& operator=(const DlDataFile& file);

	DlFileRef	_fileRef;
	DlFileSpec	_spec;
};

inline
bool
DlDataFile::AtEOF() const
{
	return Tell() == FileSize();
}

#endif
