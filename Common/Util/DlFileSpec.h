/*!
 *	\file DlFileSpec.h
 *	\author David C. Salmon
 *	\date 2006
 *
 *	Copyright © 2001 David C. Salmon. All Rights Reserved
 *
 *	\brief 
 *	The DlFileSpec class abstracts file system paths to a cross-platform definition
 *	and can encapsulate files and directories. 
 *	
 *	It supports translation between
 *	string-based path defintions and OS specific definitions. This is especially
 *	important for macintosh where there are many things that cannot be done with the
 *	string-base file path alone.
 *
 *	This class supports:
 *	- specifying file system paths as strings, and converting to native.
 *	- resolving native objects into paths.
 *	- determining the parent directory of for a given DlFileSpec.
 *	- deleting file system objects.
 *	- renaming file system objects.
 *	- determining existance of files.
 *	- directory creation.
 *
 *	For the macintosh is also supports:
 *	- resolving aliases.
 *	- setting file type and creator codes.
 *
 *	The notion here is that DlFileSpec always corresponds to a file system
 *	object. This is necessary for the macfs version of this code, but not
 *	actually for the unix version.
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

#ifndef _H_DlFileSpec
#define _H_DlFileSpec
//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"

#include "DlException.h"
#include "DlAssert.h"
#include "DlUnicode.h"
#include "DlFileRef.h"

#include <string>

//---------------------------------- Declares ----------------------------------


/*!	
	\class DlFileSpec

	Cross platform file specifier. Also supports the basic functions for
	manipulating file names, temporary files, creating, deleting, exchange.
	This class also supports opening files and testing for existance.

	\note A DlFileSpec may ONLY be constructed for a file that exists.
 */
class DlFileSpec {

public:

	enum {
		BothForks = -1,	/*!< Use both file forks (create only). */
		DataFork,		/*!< Use the data fork. */
		ResourceFork	/*!< Use the resource fork. */
	};

	// default constructor. Current working directory.
	explicit DlFileSpec();
	
	/*!
    	\brief Construct file from the name. 
    
    	The file is created if doCreate is true the file is created. If doCreate is false and
    	the file does not exist an exception is thrown.
	
    	Use CreateDirectoryPath to create a directory.
    	
    	\param name the file name to create a spec for.
    	\param doCreate specify true to create the file.
    */
	explicit DlFileSpec(const char* name, bool doCreate=false);

	/*!
	 \brief construct from parent and name.
	 
	 Construct a DlFileSpec by specifying the parent spec and the object name in the parant.
	 The name may be a path (\see DlFileSpecFromUnixPath). You may optionally request that
	 the named object be created if it does not exist, and specify that the target is a directory.
	 
	 If the specified object does not exist, and exception is thrown.
	 
	 \param parent the parent of the object.
	 \param name the name of the object.
	 \param doCreate specify true to create the object if it does not exist.
	 \param isDirectory specify true if the target object is a directory.
	 */
	DlFileSpec(const DlFileSpec& parent, const char* name, bool doCreate=false, bool isDirectory=false);

	/*!
	 \brief Construct from open file (mac only).
	 
	 Use the file reference number to create a DlFileSpec for an open file. The reference
	 number must be for the data fork of a file.
	 
	 @param refNum the reference number for the file.
	 */
	explicit DlFileSpec(const DlFileRef& refNum);
	
	// copy constructor and operator =
	DlFileSpec(const DlFileSpec& other);
	
	// move constructor
	DlFileSpec(DlFileSpec&& o);
	
	//	change this spec to the one defined in other
	DlFileSpec& operator=(const DlFileSpec& other);
	DlFileSpec& operator= (DlFileSpec&& other);
	
	DlFileSpec	GetParent() const;
	bool		HasParent() const;
	
	//	create a specifier for a temporary file. This file
	//	will be located on the same volume as the current object if 
	//	possible.
	DlFileSpec	MakeTemporary() const;
	DlFileRef	OpenTemporary() const;
	
	//	compare specs
	bool	operator == (const DlFileSpec& otherSpec) const;

	//	return true if this is a file
	bool	IsFile() const;

	//	return true if this is a directory
	bool	IsDirectory() const;

	//	return file type
	DlUInt32 GetType() const;
	DlUInt32 GetCreator() const;

	//	open the file
	DlFileRef Open(DlOpenMode theMode) const;

	//	return the size of the file	
	DlInt64	GetFileSize() const;
	
	//	return true if name exists as a child of this
	bool 	Exists(const char* name) const;
	
	//	return true if the specified fork exists
	bool 	Exists() const;
	
	//	delete the file/directory
	void 	Delete() const;
	void	Delete(const char* name) const;
	
	//	rename the file/directory
	void	Rename(const char* newName);

	//	rename the file/directory
	void	Move(const DlFileSpec& newParent);

	//	exchange contents for files if backupDelim is specified, a
	// backup file with that delimeter is left at the scene.
	void 	Exchange(const DlFileSpec& oldFile, char backupDelim=0) const;
	
	//	return the name for this file/directory
	std::string GetName() const;
	
	// return the extension portion of the name, after the .
	std::string GetExtension() const;
	
	//	add/replace an extension to the spec
	DlFileSpec	AddExtension(const char* ext, bool doCreate = true) const;
	
	//	return the file/direcotory path
	std::string GetPath() const;
	const char* UTF8Path() const;
	std::string GetResolvedPath() const;
	
	// return a backup name for XXX.YYY as XXXd.YYY where d is delim
	std::string GetBackupName(char delim) const;

	//	copy the name into a buffer
	char*	CopyName(char* buf, int bufSize) const;

	//	create all directories in path
	static DlFileSpec CreateDirectoryPath(const char* path);
    static bool PathExists(const char* path);
    static void PathDelete(const char* path, bool recurse);

	static DlNativeFilePermission DlOpenModeToPerm(DlOpenMode theMode);
	
	// find the prefs file folder and create fileName in there.
//	static DlFileSpec	MakePrefsFile(const char* fileName, bool doCreate = true);
	
	// find the app support folder and create the file in there.
	static DlFileSpec	MakeAppSupportFolder(const char* subName, bool doCreate = true);
	
	/*!
	 \brief Create a file spec from a partial unix style path.
	 
	 Specify true/false for doCreate and isDirectory. If doCreate is false
	 and the object does not exist an exception is thrown. Also, if
	 is directory is true and the named object is not a directory, an
	 exception is thrown.
	 
	 @param parent the parent of this file or directory.
	 @param name the relative path name from the parent.
	 @param doCreate create the file/directory if it does not exist.
	 @param isDirectory set to true if the path describes a directory.
	 */
//	static DlFileSpec DlFileSpecFromUnixPath(const DlFileSpec& parent, const char* name,
//											 bool doCreate=true, bool isDirectory=false);
	
private:
	std::string	_name;
};

inline DlFileSpec::DlFileSpec(const DlFileSpec& other)
	: _name(other._name)
{
}

inline DlFileSpec::DlFileSpec(DlFileSpec&& o)
	: _name(std::move(o._name))
{
}

//------------------------------------------------------------------------------
//	DlFileSpec::operator=
//
//		assign spec to this
//
//	returns				<-	reference to this
//------------------------------------------------------------------------------
inline DlFileSpec&
DlFileSpec::operator=(const DlFileSpec& o)
{
	if (this != &o) {
		_name = o._name;
	}
	
	return *this;
}

//------------------------------------------------------------------------------
//	DlFileSpec::operator=
//
//		assign spec to this
//
//	returns				<-	reference to this
//------------------------------------------------------------------------------
inline DlFileSpec&
DlFileSpec::operator=(DlFileSpec&& o)
{
	if (this != &o) {
		_name = std::move(o._name);
	}
	
	return *this;
}

inline
const char*
DlFileSpec::UTF8Path() const
{
	return _name.c_str();
}

#endif	//	_H_DlFileSpec


