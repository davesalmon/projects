//+
//	DlDirectoryIterator.h
//
//	©1998 David C. Salmon. All Rights Reserved
//
//	Iterator over the files in a directory.
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

#ifndef _H_DlDirectoryIterator
#define _H_DlDirectoryIterator

#include "dirent.h"
#include <string>

//---------------------------------- Declares ----------------------------------

class DlFileSpec;

//------------------------------------------------------------------------------
//	DlDirectoryIterator	-	Desktop iterator class
//
//		This class defines an iterator for a volume
//------------------------------------------------------------------------------
class	DlDirectoryIterator  {

public:
						DlDirectoryIterator(const DlFileSpec& parent);
				virtual ~DlDirectoryIterator();
	
	virtual std::string	GetPath() const;

	virtual const DlFileSpec* GetFileSpec() const;
	virtual DlFileSpec*		GetNextItem(DlFileSpec& ref);
	
	virtual void		Reset();
	virtual DlInt32		GetCount() const;

private:
    DlDirectoryIterator(const DlDirectoryIterator& d);
    DlDirectoryIterator& operator=(const DlDirectoryIterator& d);
        
	const DlFileSpec&	_parent;
	DIR*	dir;
};

#endif

//	eof
