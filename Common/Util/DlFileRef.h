/*
 *  DlFileRef.h
 *
 *  Created by David Salmon on 9/22/06.
 *  Copyright 2006 David C. Salmon. All rights reserved.
 *
 *	Define class to track file references. This class is fully inline.
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

#ifndef _H_DlFileRef
#define _H_DlFileRef
//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"
#include <string>

typedef FILE*		DlFileReference;
typedef const char*	DlNativeFilePermission;

/*! \enum DlOpenMode Basic open modes */
enum class DlOpenMode {
	DlNotOpen,		/*!< Designates closed file */
	DlReadOnly,		/*!< open for reading only */
	DlWriteOnly,	/*!< open for writing only */
	DlReadWrite,	/*!< open for read or write */
	DlAppend		/*!< open for append */
};

const DlFileReference	kInvalidFileRef = NULL;

class DlFileRef
{
public:
    DlFileRef();
    
    DlFileRef(const char* name, DlOpenMode theMode);
    
    DlFileRef(DlFileRef&& ref);
	DlFileRef(const DlFileRef& ref) = delete;
	
    ~DlFileRef();
    
    DlFileRef& operator=(DlFileRef&& ref);
    DlFileRef& operator=(const DlFileRef& ref) = delete;
    
    
    bool operator==(const DlFileRef& ref) const;
	
	bool isOpen() const;
	
	std::string getFileName() const;
	
private:

	friend class DlFileSpec;
	
	DlFileRef(DlFileReference f) : fr(f) {}

	static bool isRead(DlOpenMode mode);
	static DlNativeFilePermission DlOpenModeToPerm(DlOpenMode theMode);
	
	// allow data file access to the file reference
    friend class DlDataFile;
	
    void close();
    
	DlFileReference get() const;
//    void set(DlFileReference ref);
	
    DlFileReference fr;
};

//inline
//void DlFileRef::set(DlFileReference ref)
//{
//	close();
//	fr = ref;
//}


//----------------------------------------------------------------------------------------
//  DlFileRef::getFileName                                                         inline
//
//      return the file name associated with this open file.
//
//  returns std::string    <- 
//----------------------------------------------------------------------------------------
inline
std::string
DlFileRef::getFileName() const
{
	if (fr == kInvalidFileRef)
		throw DlException("Cannot determine path unopen file.");
	
	char buffer[MAXPATHLEN];

	int v = fcntl(fileno(fr), F_GETPATH, buffer);

	if (v == -1)
		throw DlException("Failed to determine path for file");
		
	return std::string(buffer);
}


//----------------------------------------------------------------------------------------
//  DlFileSpec::DlOpenModeToPerm
//
//      convert open mode to mac perm.
//
//  DlOpenMode theMode -> the open mode
//
//  returns DlNativeFilePermission     <- the corresponding mac file perm.
//----------------------------------------------------------------------------------------
inline DlNativeFilePermission
DlFileRef::DlOpenModeToPerm(DlOpenMode theMode)
{
	switch (theMode) {
		case DlOpenMode::DlReadOnly:	return "r";
		case DlOpenMode::DlAppend:		return "a";
		case DlOpenMode::DlWriteOnly:	return "w";
		case DlOpenMode::DlReadWrite:	return "w+";
		case DlOpenMode::DlNotOpen:		return nullptr;
	}
}

//----------------------------------------------------------------------------------------
//  DlFileRef::isRead                                                              inline
//
//      return true if the mode signifies reading.
//
//  DlOpenMode mode    -> the mode.
//
//  returns bool       <- true if readonly.
//----------------------------------------------------------------------------------------
inline bool
DlFileRef::isRead(DlOpenMode mode)
{
	return mode == DlOpenMode::DlReadOnly;
}

//----------------------------------------------------------------------------------------
//  DlFileRef::isOpen                                                              inline
//
//      return true if the file is open.
//
//  returns bool   <- true if open.
//----------------------------------------------------------------------------------------
inline
bool
DlFileRef::isOpen() const
{
	return fr != kInvalidFileRef;
}

//----------------------------------------------------------------------------------------
//  DlFileRef::close                                                               inline
//
//      close the file.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
void
DlFileRef::close()
{
    if (fr != kInvalidFileRef)
    {
        fclose(fr);
		fr = kInvalidFileRef;
    }
}

//----------------------------------------------------------------------------------------
//  DlFileRef::DlFileRef                                               constructor inline
//
//      default constructor.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
DlFileRef::DlFileRef() 
	: fr(kInvalidFileRef)
{
}

//----------------------------------------------------------------------------------------
//  DlFileRef::DlFileRef                                               constructor inline
//
//      construct with path and open mode.
//
//  const char* path   -> the path.
//  DlOpenMode theMode -> the open mode.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
DlFileRef::DlFileRef(const char* path, DlOpenMode theMode)
	: fr(kInvalidFileRef)
{
	_DlAssert(theMode != DlOpenMode::DlNotOpen);
	DlNativeFilePermission	perm = DlOpenModeToPerm(theMode);
	
	if (!perm)
		throw DlException("Invalid open mode for %s", path);
	
	fr = ::fopen(path, perm);
	if (!fr)
		throw DlException("Failed to open %s for %s (%s)", path, isRead(theMode) ?
						  "reading" : "writing", strerror(errno));

}

//----------------------------------------------------------------------------------------
//  DlFileRef::DlFileRef                                               constructor inline
//
//      mode constructor. steals the reference from ref.
//
//  DlFileRef&& ref    -> the object to steal from
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
DlFileRef::DlFileRef(DlFileRef&& ref)
	: fr(ref.fr)
{
	ref.fr = kInvalidFileRef;
}

//----------------------------------------------------------------------------------------
//  DlFileRef::~DlFileRef                                               destructor inline
//
//      destructor.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
DlFileRef::~DlFileRef() 
{ 
    close();
}

//----------------------------------------------------------------------------------------
//  DlFileRef::get                                                                 inline
//
//      return the reference.
//
//  returns DlFileReference    <- the open file reference
//----------------------------------------------------------------------------------------
inline
DlFileReference
DlFileRef::get() const 
{
    return fr;
}

//----------------------------------------------------------------------------------------
//  DlFileRef::operator=                                                           inline
//
//      replace this file ref with the one stolen from ref.
//
//  DlFileRef&& ref    -> the object to steal from
//
//  returns DlFileReference    <- the open file reference
//----------------------------------------------------------------------------------------
inline
DlFileRef& 
DlFileRef::operator=(DlFileRef&& ref)
{
    if (this != &ref)
    {
		close();
		fr = ref.fr;
        ref.fr = kInvalidFileRef;
    }
    return *this;
}

//----------------------------------------------------------------------------------------
//  DlFileRef::operator==                                                          inline
//
//      compare open files... Not sure why this is useful.
//
//  DlFileRef& ref    -> the other file.
//
//  returns bool      <- true if same
//----------------------------------------------------------------------------------------
inline
bool 
DlFileRef::operator==(const DlFileRef& ref) const {
    return fr == ref.fr;
}

#endif
