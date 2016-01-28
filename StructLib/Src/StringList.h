/*+
 *	File:		StringList.h
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

#ifndef _H_StringList
#define _H_StringList

//---------------------------------- Includes ----------------------------------

#include "DlString.h"
#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "StringEnumerator.h"

//---------------------------------- Class -------------------------------------

class DlString;
class frame_data;

class	StringList : public vector<DlString>
{
	friend class StringEnumerator;
	typedef vector<DlString>::const_iterator	StringConstIter;
	typedef vector<DlString>::iterator			StringIter;
public:
	
	explicit StringList();
	StringList(const char * const * s, DlInt32 len);
	
	DlInt32				IndexOf(const DlString & s) const;
	void 				Add(const DlString& s);
	void 				Remove(const DlString& s);

	void				Read(StrInputStream& inp, DlInt32 count, const frame_data & data);
	void 				Write(StrOutputStream& out, const frame_data & data) const;

	bool				HasMore() const;
	const DlString&		Next() const;
	void				Reset() const;
	DlInt32				Length() const;

	static const StringList* GetList(const StringEnumerator& l) { return l.GetList(); }
	static StringList* GetList(StringEnumerator& l) { return l.GetList(); }

private:

	StringConstIter			find(const DlString & s) const;
	StringIter				find(const DlString & s);
	
	mutable std::size_t				_refCount;
	mutable StringConstIter			_current;
	
	void IncReference() const;
	static void DecReference(StringList*& l);
	
	static const DlString*			_empty;
};


//----------------------------------------------------------------------------------------
//  StringList::StringList                                             constructor inline
//
//      construct a string list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline 
StringList::StringList() 
	: _refCount(0), _current(begin()) 
{
}

//----------------------------------------------------------------------------------------
//  StringList::StringList                                             constructor inline
//
//      construct a string list.
//
//  const char * const * s -> array of C strings.
//  DlInt32 len            -> number of strings in the array.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
StringList::StringList(const char * const * s, DlInt32 len) 
	: _refCount(0), _current(begin())
{
	for (DlInt32 i = 0; i < len; i++)
		Add(s[i]);
}

//------------------------------------------------------------------------------
//	StringList::IndexOf
//
//		return the index of s or -1 if not found. This uses binary search to 
//		locate the index.
//
//	s				->	string to find
//	returns			<-	offset of string
//------------------------------------------------------------------------------
inline DlInt32 
StringList::IndexOf(const DlString & s) const
{
	StringConstIter	iter = find(s);
	if (s == *iter)
		return std::distance(begin(), iter);
	return -1;
}

//------------------------------------------------------------------------------
//	StringList::find
//
//		return the iterator at or just before the location of s in the list.
//
//	s				->	string to find
//	returns			<-	iterator postion at or before s
//------------------------------------------------------------------------------
inline StringList::StringConstIter
StringList::find(const DlString& s) const
{
	return std::upper_bound(begin(), end(), s);
}

//------------------------------------------------------------------------------
//	StringList::find
//
//		return the iterator at or just before the location of s in the list.
//
//	s				->	string to find
//	returns			<-	iterator postion at or before s
//------------------------------------------------------------------------------
inline StringList::StringIter
StringList::find(const DlString& s)
{
	return std::upper_bound(begin(), end(), s);
}


//------------------------------------------------------------------------------
//	StringList::Add
//
//		insert the string into the list.
//
//	s				->	string to insert
//------------------------------------------------------------------------------
inline void 
StringList::Add(const DlString& s)
{
	StringIter i = find(s);
	if (i == end() || *i != s)
		insert(i, s);
	Reset();
}

//------------------------------------------------------------------------------
//	StringList::Remove
//
//		Remove the string from the list.
//
//	s				->	string to remove
//------------------------------------------------------------------------------
inline void 
StringList::Remove(const DlString& s)
{
	StringIter i = find(s);
	if (i != end() && *i == s) 
		erase(i);
	Reset();
}

//----------------------------------------------------------------------------------------
//  StringList::Read                                                               inline
//
//      read values from inp to build list.
//
//  StrInputStream& inp    -> inputs stream.
//  DlInt32 count          -> number of values to read.
//  const frame_data&      -> the owning object for this list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
StringList::Read(StrInputStream& inp, DlInt32 count, const frame_data& )
{
	for (DlInt32 i = 0; i < count; i++) {
		Add(inp.GetString());
	}
}

//----------------------------------------------------------------------------------------
//  StringList::Write                                                              inline
//
//      write the values in this list.
//
//  StrOutputStream& out   -> the output stream.
//  const frame_data&      -> the owning object for this list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
StringList::Write(StrOutputStream& out, const frame_data& ) const
{
	StringConstIter i = begin();
	for ( ; i != end(); i++)
		out.PutString(*i);
}

//----------------------------------------------------------------------------------------
//  StringList::HasMore                                                            inline
//
//      return true if there are more values in the list.
//
//  returns bool   <- true if the cursor is not at the end of the list.
//----------------------------------------------------------------------------------------
inline bool
StringList::HasMore() const 
{ 
	return _current != end(); 
}

//----------------------------------------------------------------------------------------
//  StringList::Next                                                               inline
//
//      return the next value in the list based on the cursor (_currect).
//
//  returns const DlString&    <- 
//----------------------------------------------------------------------------------------
inline const DlString&
StringList::Next() const { 
	if (_current != end()) { 
		return *_current++; 
	}
	if (_empty == 0)
		_empty = NEW DlString();
	return *_empty; 
}

//----------------------------------------------------------------------------------------
//  StringList::Reset                                                              inline
//
//      reset the cursor in the list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void	
StringList::Reset() const 
{
	_current = begin();
}

//----------------------------------------------------------------------------------------
//  StringList::Length                                                             inline
//
//      return the number of elements in the list.
//
//  returns DlInt32    <- the size of the list.
//----------------------------------------------------------------------------------------
inline DlInt32
StringList::Length() const 
{ 
	return (DlInt32)size(); 
}


//----------------------------------------------------------------------------------------
//  StringList::IncReference                                                       inline
//
//      increment the reference count for the list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void 
StringList::IncReference() const 
{
	_refCount++;	
}

//----------------------------------------------------------------------------------------
//  StringList::DecReference                                                       inline
//
//      decrease the reference count for the list. Delete the list if 0.
//
//  StringList*& l -> the list to decrement.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void 
StringList::DecReference(StringList*& l) 
{
	_DlAssert(l->_refCount != 0);

	if (--l->_refCount == 0)
	{
		delete l;
		l = 0;
	}
}

#endif

