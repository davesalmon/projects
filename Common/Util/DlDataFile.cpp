//+
//	DlDataFile.cpp
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains the data file class.
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
#include "DlDataFile.h"
#include "DlException.h"

#include <errno.h>
#include <unistd.h>
#include <cstdio>
using namespace std;

//---------------------------------- Routines ----------------------------------

DlDataFile::DlDataFile(DlFileRef&& ref)
	: _fileRef(std::move(ref))
	, _spec(DlFileSpec(_fileRef))
{
	
}

//------------------------------------------------------------------------------
//	DlDataFile::DlDataFile
//
//		construct the file object.
//
//	spec	-> the file defintion
//------------------------------------------------------------------------------
DlDataFile::DlDataFile(const DlFileSpec& spec) 
	: _fileRef()
	, _spec(spec)
{
}

//------------------------------------------------------------------------------
//	DlDataFile::Open
//
//		open the file.
//
//	theMode	-> the mode to use
//------------------------------------------------------------------------------
void
DlDataFile::OpenFile(DlOpenMode theMode)
{
	_DlAssert(theMode != DlOpenMode::DlNotOpen);

	if (!_fileRef.isOpen()) {
		_fileRef = _spec.Open(theMode);
	}
}

//------------------------------------------------------------------------------
//	DlDataFile::Close
//
//		close the file.
//
//------------------------------------------------------------------------------
void
DlDataFile::CloseFile()
{
	_fileRef.close();
}

//------------------------------------------------------------------------------
//	DlDataFile::Read
//
//		write the buffer to the file.
//
//	buf		-> the buffer
//	len		-> bytes to read
//	returns	<- bytes actually read
//------------------------------------------------------------------------------
DlUInt32
DlDataFile::Read(void* buf, DlUInt32 len)
{
	DlExceptionCode err = 0;

	clearerr(_fileRef.get());
	len = fread(buf, 1, len, _fileRef.get());
	if (feof(_fileRef.get()))
		return len;
	
	err = ferror(_fileRef.get());

	if (err != 0 || len == 0) {
		throw DlException("Failed to read from %s (%s)", _spec.GetPath().c_str(), strerror(err));
	}

	return len;
}

//------------------------------------------------------------------------------
//	DlDataFile::Write
//
//		write the buffer to the file.
//
//	buf		-> the buffer
//	len		-> bytes to write
//	returns	<- bytes written (always == len)
//------------------------------------------------------------------------------
DlUInt32
DlDataFile::Write(const void* buf, DlUInt32 len)
{
	DlUInt32 writeLen = len;
	DlExceptionCode err = 0;

	clearerr(_fileRef.get());
	len = fwrite(buf, 1, len, _fileRef.get());
	err = ferror(_fileRef.get());

	if (len != writeLen)
		throw DlException("Failed to write to %s (%s)", _spec.GetPath().c_str(), strerror(err));
	return len;
}

//------------------------------------------------------------------------------
//	DlDataFile::Seek
//
//		set the mark.
//
//	offset		-> mark offset
//	fromWhere	-> start of mark
//------------------------------------------------------------------------------
void
DlDataFile::Seek(DlInt64 offset, DlFilePosition fromWhere)
{
	_DlAssert(_fileRef.isOpen());

	int err = 0;

	_DlAssert(_fileRef.get() != 0);
	int	where = SEEK_SET;
	switch(fromWhere) {
	case DlFromStart:	where = SEEK_SET; break;
	case DlFromCurrent:	where = SEEK_CUR; break;
	case DlFromEnd:		where = SEEK_END; break;
	}

	if (fseek(_fileRef.get(), offset, where) != 0)
		err = errno; //ferror(_fileRef.get());
	
	if (err)
		throw DlException("Seek failed for file %s (%s)", _spec.GetPath().c_str(), strerror(err));

}

//------------------------------------------------------------------------------
//	DlDataFile::Tell
//
//		Get the location of the mark.
//
//	return		<- current mark
//------------------------------------------------------------------------------
DlInt64
DlDataFile::Tell() const 
{
	_DlAssert(_fileRef.isOpen());
	DlInt64 pos = 0;
	int err = 0;

	_DlAssert(_fileRef.get() != 0);
	pos = ftell(_fileRef.get());
	if (pos == -1)
		err = errno;

	if (err)
		throw DlException("Tell failed for file %s (%s)", _spec.GetPath().c_str(), strerror(err));

	return pos;
}

//------------------------------------------------------------------------------
//	DlDataFile::FileSize
//
//		return the size of the (open) file.
//
//	return		<- file size
//------------------------------------------------------------------------------
DlInt64
DlDataFile::FileSize() const 
{
	_DlAssert(_fileRef.isOpen());
	DlInt64 fileLen = 0;
	int err = 0;

	long pos = ftell(_fileRef.get());
	if (pos != -1) {
		fseek(_fileRef.get(), 0, SEEK_END);
		fileLen = ftell(_fileRef.get());
		if (fileLen != -1)
			fseek(_fileRef.get(), pos, SEEK_SET);
		else
			err = errno;
	} else {
		err = errno;
	}

	if (err)
		throw DlException("Failed to get file length for %s (%s)",
						  _spec.GetPath().c_str(), strerror(err));
	return fileLen;
}

//	eof
