/*+
 *
 *  DlDirectoryIterator.cpp
 *
 *  Copyright © 2006 David C. Salmon. All Rights Reserved.
 *
 *  Implement a directory iterator.
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


#include "DlPlatform.h"
#include "DlDirectoryIterator.h"
#include "DlFileSpec.h"

using namespace std;

//----------------------------------------------------------------------------------------
//  DlDirectoryIterator::DlDirectoryIterator                                  constructor
//
//      Construct a directory iterator for the specified directory.
//
//  const FSRef& parent    -> the directory reference.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlDirectoryIterator::DlDirectoryIterator(const DlFileSpec& parent)
	: _parent(parent)
	, dir(opendir(parent.UTF8Path()))
{
	if (!dir)
		throw DlException("Cant iterator over \"%s\"", _parent.UTF8Path(), strerror(errno));
}

DlDirectoryIterator::~DlDirectoryIterator()
{
	if (dir)
		closedir(dir);
}

//----------------------------------------------------------------------------------------
//  DlDirectoryIterator::GetPath
//
//      return the path for the parent directory.
//
//  returns std::string    <- the parent directory path.
//----------------------------------------------------------------------------------------
std::string
DlDirectoryIterator::GetPath() const
{
	return _parent.GetPath();
}

//----------------------------------------------------------------------------------------
//  DlDirectoryIterator::GetFSRef
//
//      return the FSRef for the parent directory.
//
//  returns const FSRef*   <- parent reference.
//----------------------------------------------------------------------------------------
const DlFileSpec*
DlDirectoryIterator::GetFileSpec() const
{
	return &_parent;
}

//----------------------------------------------------------------------------------------
//  DlDirectoryIterator::GetNextItem
//
//      move to the next item, returning a pointer to ref or NULL.
//
//  FSRef& ref     -> place holder for FSRef
//
//  returns FSRef* <- pointer to ref.
//----------------------------------------------------------------------------------------
DlFileSpec*
DlDirectoryIterator::GetNextItem(DlFileSpec& ref)
{
	while (struct dirent* entry = readdir(dir)) {
		// if not . or .. then use it.
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
			ref = DlFileSpec(_parent, entry->d_name);
			return &ref;
		}
	}
	
	return nullptr;
}

//----------------------------------------------------------------------------------------
//  DlDirectoryIterator::Reset
//
//      reset the iterator.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlDirectoryIterator::Reset()
{
	rewinddir(dir);
}

//----------------------------------------------------------------------------------------
//  DlDirectoryIterator::GetCount
//
//      return the number of items.
//
//  returns DlInt32    <- the number of items in the directory.
//----------------------------------------------------------------------------------------
DlInt32
DlDirectoryIterator::GetCount() const
{
	long pos = telldir(dir);
	
	rewinddir(dir);
	DlInt32 count = 0;
	
	while(struct dirent* entry = readdir(dir))
	{
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			count++;
	}
	
	seekdir(dir, pos);
	
	return count;
}

