/*+
 *	File:		ElementList.h
 *
 *	Contains:	Frame Element List interface
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#ifndef _H_ElementList
#define _H_ElementList

//--------------------------------------- Includes ---------------------------------------

#include "EnumeratorImp.h"
#include "ElementEnumerator.h"
#include "ElementImp.h"
#include "NodeList.h"

#include <valarray>
#include <set>

class	frame_data;
class	StrInputStream;
class	StrOutputStream;
class	Element;
class	LoadCaseResults;
class	ElemLoadList;
class	PropertyList;

//--------------------------------------- Class ------------------------------------------

class	ElementList : public EnumeratorImp<ElementImp, Element>
{
public:
	typedef std::set<const ElementImp*> ElementSet;
	typedef std::vector<const NodeImp*> NodeVector;

	ElementList();

	ElementEnumerator 	Attached(const NodeList* nodes) const;
	ElementEnumerator	Select(const NodeList* nodes) const;
	ElementEnumerator	Select(const WorldRect& rect) const;
	ElementImp*			SelectOne(const WorldRect & r, DlFloat64& loc) const;
	DlInt32 			CountAttached(const NodeImp* node) const; 
	bool				IsDuplicate(const NodeImp* n1, const NodeImp* n2) const;

	ElementEnumerator	FindAttachedProperty(const Property& prop) const;
	ElementEnumerator	FindAttachedLoad(LoadCase lc, const ElementLoad& l) const;
	
	//	clone the elements in this list into newList, preserving properties 
	//	and loads
	void			Clone(ElementList* newList, const NodeCloneMap& nodeMap, 
						PropertyList* newProps, ElemLoadList* newLoads,
						NodeList* nodes) const;

	void			ShallowClone(ElementList* newList, const NodeCloneMap& nodeMap,
						  NodeList* nodes) const;

	void			MapNodes(const NodeCloneMap& nodeMap, ElementEnumerator& changedElems, 
							NodeVector& startNodes, NodeVector& endNodes);

	ElementSet		FindDuplicates(const ElementList* incomingElements) const;

//	void			UpdateLoadCase(bool* isAssigned) const;

	void			GetConnectivity(DlInt32 numEqs, std::valarray<DlInt32>& connect, 
						std::valarray<DlInt32>& starts) const;
				
	// build the starts vector by determining the minimum dof attached to each node.
	void			SetStarts(std::valarray<DlInt32>& starts, DlInt32 maxDof) const;
	
	void			AssembleStiffness(std::valarray<DlFloat64>& mat, 
						const std::valarray<DlInt32>& maxa) const;

	void			AssembleLoads(std::valarray<DlFloat64>& rhs, LoadCase lc) const;

	void			RecoverResults(LoadCaseResults& results) const;

	//	build the list by reading it
	void			Read(StrInputStream& inp, DlInt32 count, const frame_data& data);
	void 			Write(StrOutputStream& out, const frame_data& data) const;


	virtual DlUInt32	GetListID() const;
	static const ElementList* GetList(const ElementEnumerator& l) { return l.GetList(); }
	static ElementList* GetList(ElementEnumerator& l) { return l.GetList(); }
};

//--------------------------------------- Inlines ----------------------------------------

#endif

//	eof
