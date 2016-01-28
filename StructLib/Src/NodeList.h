/*+
 *	File:		NodeList.h
 *
 *	Contains:	Frame Node List interface
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

#ifndef _H_NodeList
#define _H_NodeList

//---------------------------------- Includes ----------------------------------

#include "EnumeratorImp.h"
#include "NodeEnumerator.h"
#include "WorldRect.h"
#include "NodeImp.h"

class Node;
class StrInputStream;
class StrOutputStream;
class ElementList;
class NodeLoadList;

typedef std::map<const NodeImp*, NodeImp*>					NodeCloneMap;
typedef std::map<const NodeImp*, NodeImp*>::iterator		NodeCloneMapIter;
typedef std::map<const NodeImp*, NodeImp*>::const_iterator	NodeCloneMapCIter;

//---------------------------------- Class -------------------------------------

class	NodeList : public EnumeratorImp<NodeImp, Node> 
{
public:

	NodeList();
	
	NodeEnumerator 		Select(const WorldRect & r) const;
	NodeImp * 			Nearest(const WorldPoint & p) const;
	NodeImp *			SelectOne(const WorldRect & r) const;
	NodeImp *			Find(const WorldPoint & p) const;
	NodeEnumerator		FindAttachedLoad(LoadCase lc, NodeLoad theLoad) const;
	bool				Duplicates(const NodeList* movedNodes, DlFloat64 tol) const;
	void 				StitchMap(const NodeList* movedNodes, DlFloat64 tol, NodeCloneMap& nodeMap) const;

	//	clone this list into newNodes, with loads going to newLoads.
	//	create and return a map of the original node pointer to the new ones.
	void				Clone(NodeList* newNodes, NodeLoadList* newLoads,
							NodeCloneMap& nodeMap) const;

	void				ShallowClone(NodeList* newNodes, NodeCloneMap& nodeMap) const;

	void				PrepareToAnalyze(const ElementList *elems);

//	void				UpdateLoadCase(bool* isAssigned) const;

	//	build the list by reading it. Implemented in EnumeratorImp.
//	void				Read(StrInputStream& inp, DlInt32 count, const frame_data& data);
//	void 				Write(StrOutputStream& out, const frame_data& data) const;

	WorldRect			ComputeBounds() const;
	
	DlInt32				GetEquationCount() const;
	DlInt32				GetReactionCount() const;
	DlInt32				GetMatrixSize() const;
	
	const std::valarray<DlInt32>&	
						GetStarts() const;
						
	void				AssembleLoads(std::valarray<DlFloat64>& rhs, LoadCase lc);
	
	// return a list of nodes where the loads have no effect.
	NodeEnumerator		CheckLoadEquations(NodeLoadType theType) const;
	
//	void				Add(Node *n);
//	void				Remove(Node *n);
//	void				Draw(const graphics& g, NodeEnumerator selected) const;
	
	virtual DlUInt32	GetListID() const;

	static const NodeList* GetList(const NodeEnumerator& l) { return l.GetList(); }
	static NodeList* GetList(NodeEnumerator& l) { return l.GetList(); }

private:

	void SetStarts(const ElementList *elems); 
	
	DlInt32	itsNumEquations;
	DlInt32 itsNumReactions;
	DlInt32 itsNumMatrixElems;
	
	std::valarray<DlInt32>	itsStarts;	//	row starts
//	std::valarray<double>	itsMatrix;

};

//---------------------------------- Inlines -----------------------------------

inline DlInt32 
NodeList::GetEquationCount() const 
{
	return itsNumEquations;
}

inline DlInt32 
NodeList::GetReactionCount() const 
{
	return itsNumReactions;
}

inline const std::valarray<DlInt32>&	
NodeList::GetStarts() const 
{
	return itsStarts;
}

inline DlInt32
NodeList::GetMatrixSize() const
{
	return itsNumMatrixElems;
}


#if 0
inline void NodeList::Add(const Node& n) 
{
	EnumeratorImp<NodeImp>::Add((n));
}

inline void NodeList::Remove(const Node& n)
{
	EnumeratorImp<NodeImp>::Remove(static_cast<NodeImp*>(n));
}
#endif

#endif

//	eof
