/*+
 *	File:		Node.h
 *
 *	Contains:	Node point interface
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


#ifndef _H_Node
#define _H_Node

#include "StrDefines.h"
#include "NodeLoad.h"

class NodeImp;
class NodeLoad;
class Displacement;
class DlString;
class WorldPoint;

class PropertyType;

class Node
{
public:
	enum {
		  Free = 0
		, FixX = (1<<0)
		, FixY = (1<<1)
		, FixTheta = (1<<2)
	};


	Node() : imp(0) {}
	Node(NodeImp* node) : imp(node) {}
//	~Node();

	operator NodeImp* () const { return imp; }

	bool 				Empty() const { return imp == 0; }

	//	coordinate access
	//	Note: this is different from property and load access in that
	//	the world must be mapped to consistent units always.
	const WorldPoint&	GetCoords() const;
	void				SetCoords(const WorldPoint & newLoc);
	DlString			GetCoord(DlInt32 which, bool wantunits);
	void				SetCoord(DlInt32 which, const char* str);

	//	restraint access
						// if coord bit is set, dof is restrained.
	DlUInt32			GetRestraint() const;
	bool				GetRestrained(DlInt32 whichDof) const;

//	DlUInt32			SetRestraint(DlUInt32 restCode);
//	void				SetRestrained(DlInt32 whichDof, bool fixed);

	NodeLoad			GetLoad(LoadCase lc) const;
	DlInt32				GetEquationNumber(DlInt32 i) const;
	
	// Return the property type for node force/settle at the specified DOF
	const PropertyType* GetLoadTypeForDOF(DlInt32 dof) const;
	
	// return the node load type for this dof.
	NodeLoadType		GetNodeLoadType(DlInt32 dof) const;
	
	// return the property types for node displacements
	const PropertyType* GetReactionTypeForDOF(DlInt32 dof) const;

#if DlDebugging
	int id() const; // { return imp->_id }
#endif
private:
	NodeImp* imp;

};

#endif