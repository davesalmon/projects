/*+
 *	File:		NodeEnumerator.h
 *
 *	Contains:	Interface for node enumerator
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

#ifndef _H_NodeEnumerator
#define _H_NodeEnumerator

//---------------------------------- Includes ----------------------------------

#include "Enumerator.h"

class	Node;
class	NodeList;

//---------------------------------- Class -------------------------------------

typedef class Enumerator<NodeList,Node>	NodeEnumerator;

#if 0

class	NodeEnumerator : public Enumerator<NodeList,Node>
{
public:
	NodeEnumerator(const NodeList * e);
	NodeEnumerator(const NodeEnumerator & e);
	NodeEnumerator & operator= (const NodeEnumerator & e);
	~NodeEnumerator();

	bool			HasMore() const;
	const Node *	Next() const;
	Node *			Next();
	void			Reset() const;
	DlInt32 		Length() const;

	void			Append(Node * n);
	void			Remove(Node * n);

private:
	
	void			Clone();
	
	NodeList * itsImp;

};

#endif

#endif

//eof
