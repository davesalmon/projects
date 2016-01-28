/*+
 *	File:		frame_data.h
 *
 *	Contains:	Internal interface for 2D frame structure
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

#ifndef _H_frame_data
#define _H_frame_data

//---------------------------------- Includes ----------------------------------

#include "NodeList.h"
#include "ElementList.h"
#include "NodeLoadList.h"
#include "ElemLoadList.h"
#include "PropertyList.h"
#include "DlListener.h"
#include "LoadCaseResults.h"
#include "LoadCaseCombination.h"

struct AnalysisData;

//---------------------------------- Class -------------------------------------

class	frame_data : public DlListener
{
	// load case entry has the combo index and the name
	typedef std::pair<DlInt16, std::string> LoadCaseEntry;
	typedef std::vector<LoadCaseEntry> LoadCaseList;
	
public:
	frame_data(const char* defaultPropName);
	frame_data(const NodeList* nodes, const ElementList* elements, bool deepCopy);
	~frame_data();

	void read(StrInputStream& s);
	void write(StrOutputStream& s) const;

	void AddListener(DlListener* l);
	void RemoveListener(DlListener* l);

	virtual void ListenToMessage(DlUInt32 msg, void* data) const;

	NodeList& Nodes() 							{ return itsNodes; }
	const NodeList& Nodes() const				{ return itsNodes; }

	ElementList& Elements() 					{ return itsElements; }
	const ElementList& Elements() const 		{ return itsElements; }

	NodeLoadList& NodeLoads() 					{ return itsNodeLoads; }
	const NodeLoadList& NodeLoads() const 		{ return itsNodeLoads; }
 
	ElemLoadList& ElementLoads() 				{ return itsElemLoads; }
	const ElemLoadList& ElementLoads() const 	{ return itsElemLoads; }

	PropertyList& Properties() 					{ return itsProperties; }
	const PropertyList& Properties() const 		{ return itsProperties; }

	bool CanAnalyze() const;
	bool Analyzed() const;
	void InitAnalysis(AnalysisData* data);
	void Analyze(DlListener* listener);

	DlInt32	NodeLoadToIndex(LoadCase lc, const NodeLoadImp * ld) const;
	DlInt32 NodeToIndex(const NodeImp * nd) const;
	DlInt32 ElemToIndex(const ElementImp * elem) const;
	DlInt32 ElemLoadToIndex(LoadCase lc, const ElemLoadImp * eld) const;
	DlInt32 PropToIndex(const PropertyImp * prop) const;
	
	NodeLoadImp*	IndexToNodeLoad(LoadCase lc, DlInt32 index) const;
	NodeImp*		IndexToNode(DlInt32 index) const;
	ElementImp*		IndexToElem(DlInt32 index) const;
	ElemLoadImp*	IndexToElemLoad(LoadCase lc, DlInt32 index) const;
	PropertyImp*	IndexToProp(DlInt32 index) const;

	const LoadCaseResults*	GetResults(LoadCase lc) const;
	DlInt32					CountResults() const;
	
//
// load cases
//
	void	CreateLoadCase(const char* name);
	void 	UpdateLoadCase(LoadCase lc, const char* name);
	void	RemoveLoadCase();
	
	// return the total load cases including combinations
	DlUInt32	GetLoadCaseCount() const;

	// return the name for the specified load case
	const char*	GetLoadCaseName(LoadCase lc) const;
	bool		IsDefinedLoadCase(LoadCase lc) const;
	
	void		SetActiveLoadCase(LoadCase lc);
	LoadCase	GetActiveLoadCase() const;
	
	//
	// load case combinations
	//
	
	void						AddLoadCaseCombination(const char* name,
													   const LoadCaseCombination& combo);
	void						UpdateLoadCaseCombination(LoadCase lc, const char* name,
														  const LoadCaseCombination& val);
	const LoadCaseCombination*	GetLoadCaseCombination(LoadCase lc) const;
//	bool						HasLoadCaseCombination(DlInt32 which) const;
	DlUInt32					GetLoadCaseCombinationCount() const;

	void		SetDisplacementMagnifier(LoadCase lc, DlFloat64 mag);
	
	void 		ClearAnalysis(bool createLoadCases);

	const char*	GetActiveElementType() const { return itsActiveElementType; }
	void		SetActiveElementType(const char* theType);
	
	Property	GetActiveProperty() const { return Property(itsActiveProperty); }
	void 		SetActiveProperty(Property prop);
    	
	DlInt64 	GetVersion() const { return MakeVersion(itsMajorVersion, itsMinorVersion); }
	
	
	
private:

	frame_data(const frame_data& f);
    frame_data& operator=(const frame_data& f);

	void setupProperties();
	void clear();

	void updateNodeLoadCombos(LoadCase lc, const std::vector<DlFloat32>& factors);
	void updateElemLoadCombos(LoadCase lc, const std::vector<DlFloat32>& factors);
	void updateLoadCombos(LoadCase lc, const std::vector<DlFloat32>& factors);
	
//	LoadCaseResults* combineLoadCases(const LoadCaseCombination& which) const;

	NodeList		itsNodes;
	ElementList		itsElements;
	NodeLoadList	itsNodeLoads;
	ElemLoadList	itsElemLoads;
	PropertyList	itsProperties;
	
	LoadCaseList	itsLoadCases;
	
	ElemLoadList	itsComboElemLoads;
	NodeLoadList	itsComboNodeLoads;
	
	DlInt32			itsActiveLoadCase;
	
	DlString		itsActiveElementType;
	PropertyImp*	itsActiveProperty;
	
	std::vector<LoadCaseResults*>	results;
	std::vector<LoadCaseCombination> combos;
	
	DlInt32 itsMajorVersion;
	DlInt32 itsMinorVersion;
	
	std::string defaultPropertyTitle;
};

//----------------------------------------------------------------------------------------
//  frame_data::AddListener                                                        inline
//
//      add the specified listener to all objects.
//
//  DlListener* l  -> the listener.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void 
frame_data::AddListener(DlListener* l) {
	if (l) {
		itsNodes.AddListener(l);
		itsElements.AddListener(l);
		itsNodeLoads.AddListener(l);
		itsElemLoads.AddListener(l);
		itsProperties.AddListener(l);
	}
}

//----------------------------------------------------------------------------------------
//  frame_data::RemoveListener                                                     inline
//
//      remove the specified listener from all objects.
//
//  DlListener* l  -> the listener.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void 
frame_data::RemoveListener(DlListener* l) {
	if (l) {
		itsNodes.RemoveListener(l);
		itsElements.RemoveListener(l);
		itsNodeLoads.RemoveListener(l);
		itsElemLoads.RemoveListener(l);
		itsProperties.RemoveListener(l);
	}
}

//----------------------------------------------------------------------------------------
//  frame_data::GetLoadCaseCombination                                             inline
//
//      return the combination.
//
//  DlInt32 which                      -> the index of the combination.
//
//  returns const LoadCaseCombination& <- the combination.
//----------------------------------------------------------------------------------------
inline
const LoadCaseCombination*
frame_data::GetLoadCaseCombination(LoadCase lc) const
{
	_DlAssert(lc < itsLoadCases.size());
	DlInt16 index = itsLoadCases[lc].first;
	return index >= 0 ? &combos[index] : nullptr;
}

//----------------------------------------------------------------------------------------
//  frame_data::GetLoadCaseCombinationCount                                        inline
//
//      return the number of defined lc combos.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
inline
DlUInt32
frame_data::GetLoadCaseCombinationCount() const
{
	return combos.size();
}

//----------------------------------------------------------------------------------------
//  frame_data::SetActiveLoadCase                                                  inline
//
//      set the active load case.
//
//  LoadCase lc    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
frame_data::SetActiveLoadCase(LoadCase lc)
{
	_DlAssert(lc < itsLoadCases.size());
	itsActiveLoadCase = lc;
}

//----------------------------------------------------------------------------------------
//  frame_data::GetActiveLoadCase                                                  inline
//
//      return the active load case.
//
//  returns LoadCase   <- 
//----------------------------------------------------------------------------------------
inline LoadCase
frame_data::GetActiveLoadCase() const
{
	return itsActiveLoadCase;
}

inline DlInt32
frame_data::CountResults() const
{
	return results.size();
}

#endif


//	eof
