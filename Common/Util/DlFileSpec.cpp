//+
//	DlFileSpec.cpp
//
//	Copyright ÔøΩ 2001 David C. Salmon. All Rights Reserved
//
//	Contains the file operations class.
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

// ---------------------------- Includes --------------------------------------

#include "DlPlatform.h"

#include "DlFileSpec.h"
#include "DlStringUtil.h"
#include "DlUnicode.h"
#include "DlDirectoryIterator.h"
#include "DlStringTokenizer.h"
#include "DlExchangeFiles.h"
#include "DlAppSupportFiles.h"

//#include <string>
#include <cctype>
#include <ctime>

#include <cstdio>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/errno.h>

using namespace std;

// ---------------------------- Declares --------------------------------------

//----------------------------------------------------------------------------------------
//  absolutePath                                                                   static
//
//      return the absolute path for the file, resolving all ../.. but not symlinks.
//
//  const char* path   -> the original path.
//
//  returns string     <- the resolved path.
//----------------------------------------------------------------------------------------
static string
absolutePath(const char* path)
{
	if (path == nullptr)
		throw DlException("No name specified in file specification");
		
	// check for starts with ./
	while (!DlStrNICmp(path, "./", 2))
		path += 2;
	
	string theName;

	if (path[0] != '/') {
		char buffer[MAXPATHLEN];
		theName.assign(getcwd(buffer, MAXPATHLEN));
		if (path[0]) {
			theName.append(1, '/');
			theName.append(path);
		}
	} else {
		theName.assign(path);
	}
	
	// replace ../
	//
	// a/x/../y -> a/y
	while (true) {
		auto loc = theName.rfind("/..");
		if (loc == string::npos)
			break;
		
		if (loc == 0)
			throw DlException("Invalid path \"%s\"", theName.c_str());
		
		auto loc2 = theName.rfind("/", loc-1);
		if (loc2 == string::npos)
			throw DlException("Invalid path \"%s\"", theName.c_str());
		
		loc += 3;
		
		theName.erase(loc2, loc - loc2);
	}

	// and ./
	while (true) {
		auto loc = theName.rfind("/.");
		if (loc == string::npos)
			break;
		
		theName.erase(loc, 2);
	}
		
	return theName;
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::DlFileSpec                                                    constructor
//
//      default constructor. Initialize to cwd.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlFileSpec::DlFileSpec()
: _name(absolutePath(""))
{
}

/*------------------------------------------------------------------------------
 * DlFileSpec::DlFileSpec                                            constructor
 *
 * construct the spec from a path. If doCreate is true, create the file.
 *
 * const char* name	-> the path name
 * bool doCreate	-> true to create.
 *----------------------------------------------------------------------------*/
DlFileSpec::DlFileSpec(const char* name, bool doCreate)
	: _name(absolutePath(name))
{
	if (doCreate && !Exists()) {
		FILE* f = fopen(_name.c_str(), "a");
		if (f != nullptr)
			fclose(f);
		else
			throw DlException("file \"%s\" could not be created (%s).",
							  UTF8Path(), strerror(errno));
	}
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::DlFileSpec                                                    constructor
//
//      construct a file path.
//
//  const DlFileSpec& parent   -> the parent directory
//  const char* name           -> the name.
//  bool doCreate              -> true to create the object.
//  bool isDirectory           -> true if target is directory.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlFileSpec::DlFileSpec(const DlFileSpec& parent, const char* addName, bool doCreate,
		bool isDirectory)
{
	if (!parent.IsDirectory())
		throw DlException("Invalid parent directory \"%s\"", parent.GetPath().c_str());
	
	if (!addName || !*addName)
		throw DlException("Invalid empty file or directory name.");

	if (*addName == '/')
		throw DlException("Cannot create absolute path in folder.");
	
	_name = parent._name;

	if (_name[_name.length() - 1] != '/')
		_name.append(1, '/');
	_name.append(addName);
	
	if (!Exists()) {
		if (doCreate) {
			if (isDirectory) {
				if (mkdir(_name.c_str(), S_IRWXU) == -1)
					throw DlException("Failed to create directory \"%s\" (%s)",
									  _name.c_str(), strerror(errno));
			} else {
				FILE* f = fopen(_name.c_str(), "a");
				if (f != nullptr)
					fclose(f);
				else
					throw DlException("file \"%s\" could not be created (%s).",
									  _name.c_str(), strerror(errno));
			}
		} else if (isDirectory) {
			throw DlException("file \"%s\" could not be found.", UTF8Path());
		}
	}
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::GetParent
//
//      return the parent for this spec.
//
//  returns DlFileSpec <- file spec for parent of this.
//----------------------------------------------------------------------------------------
DlFileSpec
DlFileSpec::GetParent() const
{
	auto loc = _name.rfind('/');
	
	if (loc != string::npos)
	{
		string newName(_name);
		newName.erase(loc);
		
		return DlFileSpec(newName.c_str());
	}
	
	throw DlException("%s has no parent directory", GetPath().c_str());
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::HasParent
//
//      return true if a parent exists.
//
//  returns bool   <- true if parent exists.
//----------------------------------------------------------------------------------------
bool
DlFileSpec::HasParent() const
{
	//	return _name.find('/') != string::npos;
	return _name != "/";
}

/*------------------------------------------------------------------------------
 * DlFileSpec::DlFileSpec                                            constructor
 *
 * Describe function
 *
 * short refNum	-> reference number of object.
 *----------------------------------------------------------------------------*/
DlFileSpec::DlFileSpec(const DlFileRef& refNum)
{
	_name.assign(refNum.getFileName());
}

/*------------------------------------------------------------------------------
 * DlFileSpec::Open                                                             
 *
 * Describe function
 *
 * DlOpenMode theMode	-> argument description here
 * int whichFork	-> argument description here
 *----------------------------------------------------------------------------*/
DlFileRef
DlFileSpec::Open(DlOpenMode theMode) const
{
	return DlFileRef(UTF8Path(), theMode);
}

//------------------------------------------------------------------------------
//	DlFileSpec::MakeTemporary
//
//		create a temporary file
//
//	returns				<-	the spec
//------------------------------------------------------------------------------
DlFileSpec
DlFileSpec::MakeTemporary() const
{
	DlFileSpec par = GetParent();
	string path = par._name + "/DlFile-TmpFile-XXXXXX";
	
	char buf[MAXPATHLEN];
	
	strcpy(buf, path.c_str());
	
	return DlFileSpec(::mktemp(buf));
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::OpenTemporary
//
//      return the reference to an open temporary file in tthe directory specified by this
//      object.
//
//  returns DlFileRef  <- 
//----------------------------------------------------------------------------------------
DlFileRef
DlFileSpec::OpenTemporary() const
{
	DlFileSpec par = GetParent();
	string path = par._name + "/DlFile-TmpFile-XXXXXX";
	
	char buf[MAXPATHLEN];
	
	strcpy(buf, path.c_str());
	
	int fd = ::mkstemp(buf);
	
	if (fd == -1)
		throw DlException("Failed to create temporary file (%s)", strerror(errno));
	
	FILE* of = fdopen(fd, "r+");
	
	if (!of)
		throw DlException("Failed to create temporary file (%s)", strerror(errno));
	
	return DlFileRef(of);
}


//----------------------------------------------------------------------------------------
//  refToFileSpec                                                                  static
//
//      convert a reference to a file spec.
//
//  const FSRef& ref   -> 
//
//  returns DlFileSpec <- 
//----------------------------------------------------------------------------------------
static DlFileSpec
refToFileSpec(const FSRef& ref)
{
	char buf[2048];
	OSStatus err = FSRefMakePath(&ref, (UInt8*)buf, sizeof(buf) - 1);
	if (err == noErr)
		return DlFileSpec(buf);

	throw DlException("Failed to create file path");
}

/*------------------------------------------------------------------------------
* DlFileSpec::MakeAppSupportFolder                                                     
*
* Describe function
*
* const char* subName	-> folder name in app support directory
* bool doCreate			-> true to create folder if not found
* DlFileSpec			<- return description here
*----------------------------------------------------------------------------*/
DlFileSpec
DlFileSpec::MakeAppSupportFolder(const char* subName, bool doCreate)
{
	string path(DlGetApplicationSupportFolder(subName, doCreate));
	
	return DlFileSpec(path.c_str());
}

//------------------------------------------------------------------------------
//	DlFileSpec::operator==
//
//		return true if specs are the same
//
//	DlFileSpec& otherSpec	->	spec to compare
//	returns					<-	true if same
//------------------------------------------------------------------------------
bool
DlFileSpec::operator==(const DlFileSpec& otherSpec) const
{
//	return _name == otherSpec._name;
	return GetResolvedPath() == otherSpec.GetResolvedPath();
}

/*------------------------------------------------------------------------------
* DlFileSpec::IsFile                                                      
*
* Describe function
*
* bool	<- return description here
*----------------------------------------------------------------------------*/
bool
DlFileSpec::IsFile() const
{
	struct stat statbuf;
	
	if (stat(_name.c_str(), &statbuf) != -1)
	{
		return (statbuf.st_mode & S_IFREG) != 0;
	}
	else
	{
		return false;
	}

	return !IsDirectory();
}

/*------------------------------------------------------------------------------
 * DlFileSpec::IsDirectory                                                      
 *
 * Describe function
 *
 * bool	<- return description here
 *----------------------------------------------------------------------------*/
bool
DlFileSpec::IsDirectory() const
{
	struct stat statbuf;
	
	if (stat(_name.c_str(), &statbuf) != -1)
	{
		return (statbuf.st_mode & S_IFDIR) != 0;
	}
	else
	{
		return false;
	}
}

/*------------------------------------------------------------------------------
 * DlFileSpec::GetFileSize                                                      
 *
 * Describe function
 *
 * int whichFork	-> argument description here
 * DlInt32	<- return description here
 *----------------------------------------------------------------------------*/
DlInt64
DlFileSpec::GetFileSize() const
{
	struct stat buffer;
	if (stat(_name.c_str(), &buffer) != -1)
		return buffer.st_size;
	else
		throw DlException("failed to determine file size (%s).",
						  strerror(errno));
}

/*------------------------------------------------------------------------------
 * DlFileSpec::Exists                                                           
 *
 * return true if the file/fork exists
 *
 * int whichFork	-> which fork to check
 * bool				<- true if object exists
 *----------------------------------------------------------------------------*/
bool
DlFileSpec::Exists() const
{
	struct stat statbuf;
	return stat(_name.c_str(), &statbuf) != -1;
}

/*------------------------------------------------------------------------------
 * DlFileSpec::Exists                                                           
 *
 * return true if the file/fork exists in this directory
 *
 * int whichFork	-> which fork to check
 * bool				<- true if object exists
 *----------------------------------------------------------------------------*/
bool
DlFileSpec::Exists(const char* name) const
{
	_DlAssert(IsDirectory());
	
	return DlFileSpec(*this, name, false).Exists();
}

/*------------------------------------------------------------------------------
 * DlFileSpec::Delete                                                           
 *
 * Delete the specified fork of the object
 *
 * int whichFork	-> which fork to delete
 *----------------------------------------------------------------------------*/
void
DlFileSpec::Delete() const
{
	int res = 0;
	
	if (!IsDirectory()) {
		res = unlink(_name.c_str());
	}
	else {
		res = rmdir(_name.c_str());
	}
	
	if (res == -1)
		throw DlException("Cant delete \"%s\" (%s)",
						  GetPath().c_str(), strerror(errno));
}

/*------------------------------------------------------------------------------
 * DlFileSpec::Delete                                                           
 *
 * Delete the specified fork of the object
 *
 * int whichFork	-> which fork to delete
 *----------------------------------------------------------------------------*/
void
DlFileSpec::Delete(const char* name) const
{
	DlFileSpec(*this, name).Delete();
}


/*------------------------------------------------------------------------------
 * DlFileSpec::Rename                                                           
 *
 * Renname the file to the name given by spec
 *
 * const char* name		-> name of the file
 *----------------------------------------------------------------------------*/
void
DlFileSpec::Rename(const char* name)
{
	DlFileSpec file(GetParent(), name, false);
	
	if (!(file == *this))
	{
		if (file.Exists())
			throw DlException("Cant rename \"%s\" to \"%s\" (%s)",
							  GetName().c_str(), name, "destination exists");

		if (rename(UTF8Path(), file.UTF8Path()) == -1)
		{
			throw DlException("Cant move \"%s\" to \"%s\" (%s)",
						  _name.c_str(), file.UTF8Path(), strerror(errno));
		}
		
		*this = file;
	}
}

/*------------------------------------------------------------------------------
 * DlFileSpec::Move                                                           
 *
 * Move this file to the parent given by spec
 *
 * const DlFileSpec& spec	-> new parent
 *----------------------------------------------------------------------------*/
void
DlFileSpec::Move(const DlFileSpec& spec)
{
	if (!spec.IsDirectory())
		throw DlException("Cant move \"%s\" to \"%s\" (%s)",
						  UTF8Path(), spec.UTF8Path(), "destination is not a folder");

	// move this file into the directory spec
	DlFileSpec newFile(spec, GetName().c_str());
	
	if (rename(_name.c_str(), newFile._name.c_str()) == -1)
	{
		throw DlException("Cant move \"%s\" to \"%s\" (%s)",
						  _name.c_str(), spec.GetPath().c_str(), strerror(errno));
	}
	
	*this = newFile;
}

/*------------------------------------------------------------------------------
 * DlFileSpec::Exchange                                                         
 *
 * Exchange the contents of this file with the specified temporary. If the
 *	backup delimiter is specified, then a backup is left with that delimeter
 *	at the end of the base name part of the file.
 *
 * const DlFileSpec& tempFile	-> file to swap with this
 *----------------------------------------------------------------------------*/
void
DlFileSpec::Exchange(const DlFileSpec& tempFile, char backupDelim) const
{
	DlExchangeFiles(*this, tempFile, backupDelim);
}

/*------------------------------------------------------------------------------
 * DlFileSpec::GetName                                                          
 *
 * return file/direcotory name of this object
 *
 * string	<- return description here
 *----------------------------------------------------------------------------*/
string
DlFileSpec::GetName() const
{
	if (_name.length()==1)
		return _name;

	auto loc = _name.rfind('/');
	if (loc == string::npos)
		return _name;
		
	return _name.substr(loc+1);
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::GetBackupName
//
//      return a file name that differs from this files name by appending a single
//		character onto the base portion.
//
//  char delim     -> 
//
//  returns string <- 
//----------------------------------------------------------------------------------------
string
DlFileSpec::GetBackupName(char delim) const
{
	if (delim == 0)
		return GetName();
	
	string s(GetName());
	
	auto loc = s.rfind('.');
	if (loc != string::npos) {
		s.insert(loc, 1, delim);
	} else {
		s.append(1, delim);
	}

	return s;
}

/*------------------------------------------------------------------------------
 * DlFileSpec::GetPath                                                          
 *
 * return the path to this file/directory
 *
 * string	<- return path to file
 *----------------------------------------------------------------------------*/
string
DlFileSpec::GetPath() const
{
	return _name;
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::GetResolvedPath
//
//      return the fully resolved path (including sym links).
//
//  returns string <- the path
//----------------------------------------------------------------------------------------
string
DlFileSpec::GetResolvedPath() const
{
	char buffer[MAXPATHLEN];
	realpath(_name.c_str(), buffer);
	return string(buffer);
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::GetExtension
//
//      return the extension for the file.
//
//  returns string <- the extension
//----------------------------------------------------------------------------------------
string
DlFileSpec::GetExtension() const
{
	string::size_type pos = _name.rfind('.');
	return  _name.substr(pos);
}

//------------------------------------------------------------------------------
//	DlFileSpec::AddExtension
//
//		add the specifed extension to the name
//
//	ext					->	extension to add
//	returns				<-	new file spec with extension
//------------------------------------------------------------------------------
DlFileSpec
DlFileSpec::AddExtension(const char* ext, bool doCreate) const
{
	if (ext[0] == '.')
		ext++;
	
	string	baseName = GetName();
	string::size_type pos = baseName.rfind('.');
	if (pos != string::npos)
		baseName.erase(pos);
	
	DlInt32 baseLen = baseName.length();
	DlInt32 extLen = strlen(ext);
	
	if (baseLen + extLen > 255) {
		baseLen = 255 - extLen;
		baseName.erase(baseLen);
	}
	
	baseName.append(1, '.');
	baseName.append(ext);
	
	return DlFileSpec(baseName.c_str(), doCreate);
}


/*------------------------------------------------------------------------------
 * DlFileSpec::CopyName                                                         
 *
 * copy the name into the buffer
 *
 * char* buf	-> buffer to copy to
 * int bufSize	-> total size of buffer
 * char*		<- return buffer
 *----------------------------------------------------------------------------*/
char*
DlFileSpec::CopyName(char* buf, int bufSize) const
{
	int len = _name.length();
	if (len > bufSize - 1)
		len = bufSize - 1;
	strncpy(buf, _name.c_str(), len);
	buf[len] = 0;
	return buf;
}

/*------------------------------------------------------------------------------
 * DlFileSpec::CreateDirectoryPath                                         
 *
 * Create a new directory specified by path
 *
 * const char* path	-> the directory path
 * DlFileSpec		<- spec for directory
 *----------------------------------------------------------------------------*/
DlFileSpec
DlFileSpec::CreateDirectoryPath(const char* path)
{
	DlFileSpec theSpec(path);
	
	DlStringTokenizer tok(theSpec._name.c_str(), "/");
	
	string part;
	
	DlFileSpec trans("/");
	
	while (tok.nextToken(part)) {
		trans = DlFileSpec(trans, part.c_str(), true, true);
	}
	
	return theSpec;
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::PathExists
//
//      return true if the object specified by path exists.
//
//  const char* path   -> the path
//
//  returns bool       <- return true if there is a file system entity at path.
//----------------------------------------------------------------------------------------
bool 
DlFileSpec::PathExists(const char* path)
{
	return DlFileSpec(path).Exists();
}

//----------------------------------------------------------------------------------------
//  RecursiveDelete                                                                static
//
//      delete specified directory and all sub directories.
//
//  auto_ptr<DlDirectoryIterator> iter -> the directory iterator.
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void 
RecursiveDelete(auto_ptr<DlDirectoryIterator> iter)
{
    DlFileSpec ref;
    while(iter->GetNextItem(ref))
    {
        if (DlFileSpec(ref).IsDirectory())
            RecursiveDelete(auto_ptr<DlDirectoryIterator>(new DlDirectoryIterator(ref)));
        DlFileSpec(ref).Delete();
    }
}

//----------------------------------------------------------------------------------------
//  DlFileSpec::PathDelete
//
//      delete the object at path.
//
//  const char* path   -> 
//  bool recurse       -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
DlFileSpec::PathDelete(const char* path, bool recurse)
{
    DlFileSpec spec(path);
    if (recurse)
        RecursiveDelete(auto_ptr<DlDirectoryIterator>(new DlDirectoryIterator(spec)));
   	spec.Delete();
}

