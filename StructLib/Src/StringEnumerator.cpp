/*+
 *	File:		StringEnumerator.cpp
 *
 *	Contains:	Interface for string lists
 *	
 *	List of strings.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "StringEnumerator.h"
#include "StringList.h"
//#include "Enumerator.tem"

//---------------------------------- Methods -----------------------------------


StringEnumerator::StringEnumerator()
	: itsList(NEW StringList)
{
	itsList->IncReference();
}

StringEnumerator::StringEnumerator(const StringList * imp)
	: itsList(const_cast<StringList*>(imp))
{
	itsList->IncReference();
}

StringEnumerator::StringEnumerator(const StringEnumerator & e)
	: itsList(e.itsList)
{
	itsList->IncReference();
}

StringEnumerator& 
StringEnumerator::operator=(const StringEnumerator&e)
{
	if (this != &e)
	{
		itsList->DecReference(itsList);
		itsList = e.itsList;
		itsList->IncReference();
	}
	return *this;
}

StringEnumerator::~StringEnumerator()
{
	itsList->DecReference(itsList);
}

bool
StringEnumerator::HasMore() const
{
	return itsList->HasMore();
}

const DlString& 	
StringEnumerator::Next() const
{
	return itsList->Next();
}

void				
StringEnumerator::Reset() const
{
	itsList->Reset();
}

DlInt32 			
StringEnumerator::Length() const
{
	return itsList->Length();
}

//void				
//StringEnumerator::Insert(const DlString & s)
//{
//	itsList->Add(s);
//}

void				
StringEnumerator::Add(const DlString & n)
{
	itsList->Add(n);
}

void				
StringEnumerator::Remove(const DlString & n)
{
	itsList->Remove(n);
}

//#endif

