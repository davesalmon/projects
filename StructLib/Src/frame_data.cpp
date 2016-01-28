/*+
 *	File:		frame_data.cpp
 *
 *	Contains:	Interface for 2D frame structure
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
#include "StrDefines.h"
#include "frame_data.h"
#include "FrameStructure.h"
#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "DlString.h"
#include "colsol.h"
#include "ElementFactory.h"
#include "PropertyFactory.h"

//---------------------------------- Class -------------------------------------

class SolverProgress : public EqSolveProgress, public DlBroadcaster
{
public:
	SolverProgress(DlListener* listener);
	
	bool	Broadcast();
	//	called for transitions
	virtual bool SetStage(EqSolveProgress::Stage stage);
	//	return false to cancel analysis
	virtual bool Processing(DlInt32 i, DlInt32 n);
private:

	AnalysisBroadcast	_data;
};

//----------------------------------------------------------------------------------------
//  frame_data::frame_data                                                    constructor
//
//      construct frame_data.
//
//  const char* defaultPropName    -> the default property name.
//
//  returns nothing
//----------------------------------------------------------------------------------------
frame_data::frame_data(const char* defaultPropName)
	: itsMajorVersion(kCurrentMajorVersion)
	, itsMinorVersion(kCurrentMinorVersion)
	, itsActiveLoadCase(0)
	, itsActiveElementType(ElementFactory::sElementFactory.GetDefaultElementType())
	, itsActiveProperty(0)
	, defaultPropertyTitle(defaultPropName)
{
	setupProperties();
	CreateLoadCase("default");
}

//----------------------------------------------------------------------------------------
//  frame_data::~frame_data                                                    destructor
//
//      destroy frame_data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
frame_data::~frame_data()
{
	clear();
}

//----------------------------------------------------------------------------------------
//  frame_data::frame_data                                                    constructor
//
//      construct frame_data for copy/paste.
//
//  const NodeList* nodes          -> the nodes copied.
//  const ElementList* elements    -> the attached elements.
//  bool deepCopy                  -> true to make a deep copy.
//
//  returns nothing
//----------------------------------------------------------------------------------------
frame_data::frame_data(const NodeList* nodes, const ElementList* elements, bool deepCopy)
	: itsMajorVersion(kCurrentMajorVersion)
	, itsMinorVersion(kCurrentMinorVersion)
	, itsActiveLoadCase(0)
{
	//	now we need to fix up the loads and elements
	NodeCloneMap theNodeMap;

	if (deepCopy) {
		nodes->Clone(&itsNodes, &itsNodeLoads, theNodeMap);
		elements->Clone(&itsElements, theNodeMap, &itsProperties, &itsElemLoads, &itsNodes);
	} else {
		nodes->ShallowClone(&itsNodes, theNodeMap);
		elements->ShallowClone(&itsElements, theNodeMap, &itsNodes);
	}
	
	setupProperties();
}


//----------------------------------------------------------------------------------------
//  frame_data::read
//
//      read the frame_data from the stream.
//
//  StrInputStream & s -> the input stream
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
frame_data::read(StrInputStream & s)
{
	itsMajorVersion = s.GetInt();
	itsMinorVersion = s.GetInt();
	
	DlInt32	nodeCount = s.GetInt();
	DlInt32 elemCount = s.GetInt();
	DlInt32 nodeLoadCount = s.GetInt();
	DlInt32 elemLoadCount = s.GetInt();
	DlInt32 propertyCount = s.GetInt();
	
	clear();

	fprintf(stderr, "minor v = %d", itsMinorVersion);
	
	// FIXME: take this out
//	itsMinorVersion = 4;
	
//	fprintf(stderr, "node loads\n");
	itsNodeLoads.Read(s, nodeLoadCount, *this);
//	fprintf(stderr, "elem loads\n");
	itsElemLoads.Read(s, elemLoadCount, *this);
//	fprintf(stderr, "properties\n");
	itsProperties.Read(s, propertyCount, *this);
//	fprintf(stderr, "nodes\n");
	itsNodes.Read(s, nodeCount, *this);
//	fprintf(stderr, "elements\n");
	itsElements.Read(s,elemCount, *this);
	
	itsActiveElementType = s.GetString();
	itsActiveProperty = IndexToProp(s.GetInt());
	
	if (GetVersion() >= kFrameVersionCombos) {
		DlInt32 nCombos = s.GetInt();
		
		combos.reserve(nCombos);
		
		for (auto i = 0; i < nCombos; i++) {
			combos.push_back(LoadCaseCombination(s));
		}
	} else {
		CreateLoadCase("default");
	}
	
	// build the load case list.
	if (GetVersion() >= kFrameVersionDynamicLoadCounts) {
		DlInt32 nLc = s.GetInt(); // number of load cases
		
		itsLoadCases.reserve(nLc);
		
		for (int i = 0; i < nLc; i++) {
			int comboIndex = s.GetInt();
			// read in the name and combo index.
			itsLoadCases.push_back(LoadCaseEntry(comboIndex, s.GetString().get()));
			// if there is a combo, construct the loads for it.
			if (comboIndex >= 0) {
				updateLoadCombos(i, combos[comboIndex].GetFactors());
			}
		}
	}
	
//	UpdateLoadCaseCount();
}

//----------------------------------------------------------------------------------------
//  frame_data::write
//
//      write the frame_data to the stream.
//
//  StrOutputStream& s -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
frame_data::write(StrOutputStream& s) const
{
	s.PutInt(kCurrentMajorVersion);
	s.PutInt(kCurrentMinorVersion);
	s.PutInt(itsNodes.size());
	s.PutInt(itsElements.size());
	s.PutInt(itsNodeLoads.size());
	s.PutInt(itsElemLoads.size());
	s.PutInt(itsProperties.size());
	
	itsNodeLoads.Write(s, *this);
	itsElemLoads.Write(s, *this);
	itsProperties.Write(s, *this);
	itsNodes.Write(s, *this);
	itsElements.Write(s, *this);
	
	s.PutString(itsActiveElementType);
	s.PutInt(PropToIndex(itsActiveProperty));
	
	s.PutInt(combos.size());
	for (auto i = 0; i < combos.size(); i++) {
		combos[i].Write(s);
	}
	
	s.PutInt(itsLoadCases.size());
	for (const auto& lce : itsLoadCases) {
		s.PutInt(lce.first);
		s.PutString(lce.second.c_str());
	}
}

void
frame_data::ListenToMessage(DlUInt32 msg, void* data) const
{
	frame_data* fd = const_cast<frame_data*>(this);
	
	if (msg == MessageNodeLoadAssignmentChanged) {
		for (int i = 0; i < itsLoadCases.size(); i++) {
			if (itsLoadCases[i].first >= 0)
				fd->updateNodeLoadCombos(i, combos[itsLoadCases[i].first].GetFactors());
		}
	} else if (msg == MessageElemLoadAssignmentChanged) {
		for (int i = 0; i < itsLoadCases.size(); i++) {
			if (itsLoadCases[i].first >= 0)
				fd->updateElemLoadCombos(i, combos[itsLoadCases[i].first].GetFactors());
		}
	}
}

//----------------------------------------------------------------------------------------
//  frame_data::CanAnalyze
//
//      return true if the structure can be analyzed.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
bool
frame_data::CanAnalyze() const 
{
	return itsNodes.Length() > 0 &&
		itsElements.Length() > 0 && 
		(itsNodeLoads.Length() > 0 || itsElemLoads.Length() > 0);
}

//----------------------------------------------------------------------------------------
//  frame_data::Analyzed
//
//      return true if the structure has results.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
bool
frame_data::Analyzed() const {
	return results.size() > 0;
}

//----------------------------------------------------------------------------------------
//  frame_data::InitAnalysis
//
//      initial data for analysis.
//
//  AnalysisData* data -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::InitAnalysis(AnalysisData* data) 
{
	itsNodes.PrepareToAnalyze(&itsElements);

	data->nodeCount = itsNodes.Length();
	data->elemCount = itsElements.Length();
	data->eqCount = itsNodes.GetEquationCount();
	data->lcCount = GetLoadCaseCount();
}

//----------------------------------------------------------------------------------------
//  frame_data::Analyze
//
//      analyze the structure.
//
//  DlListener* l  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
frame_data::Analyze(DlListener* l) 
{
	try {
		
//		DlUInt32 lcCount = GetLoadCaseCount();
		
		SolverProgress	progress(l);
		//	first create the matrix	
		valarray<DlFloat64>	matrix(0.0, itsNodes.GetMatrixSize());
		itsElements.AssembleStiffness(matrix, itsNodes.GetStarts());
		
		//	and solve it
		colsol(true, itsNodes.GetEquationCount(), matrix, itsNodes.GetStarts(), 0, true, &progress);
		
		ClearAnalysis(true);
		
		for (LoadCase i = 0; i < itsLoadCases.size(); i++) {
			
			if (IsDefinedLoadCase(i)) {
				
				LoadCaseResults* res = NEW LoadCaseResults(i, itsNodes, itsElements);
				results[i] = res;
			
				itsElements.AssembleLoads(res->GetDisplacements(), i);
				itsNodes.AssembleLoads(res->GetDisplacements(), i);
				
				colsol(false, itsNodes.GetEquationCount(), matrix, itsNodes.GetStarts(), 
					&res->GetDisplacements(), true, &progress);
				
				itsElements.RecoverResults(*res);
				
	#if DlDebugging
				for (int i = 0; i < res->GetReactions().size(); i++) {
					printf("reaction in dof %d is %.2lf\n", i+1, res->GetReactions()[i]);
				}
	#endif
			} else {
				results[i] = nullptr;
			}
		}
		
		// and the combinations.
		for (LoadCase i = 0; i < itsLoadCases.size(); i++) {
			const auto& p = itsLoadCases[i];
			if (p.first >= 0) {
				results[i] = NEW LoadCaseResults(i, combos[p.first], *this);
			}
		}

	} catch(EqSolveFailure& solFailed) {
		ClearAnalysis(false);
		
		switch(solFailed.getReason()) {
		case EqSolveFailure::UserCancelled:
			return;
		case EqSolveFailure::Singular:
		case EqSolveFailure::NonPositiveDefinite:
		case EqSolveFailure::NeedPivot:
			throw DlException("Analysis failed! Matrix singularity detected at equation %d.", 
				solFailed.getEquation());
			break;
		}
		
		throw DlException("Analysis failed.");
	
	} catch(std::exception& ex) {
		ClearAnalysis(0);
		throw DlException(ex.what());
	}
	
}

//----------------------------------------------------------------------------------------
//  frame_data::NodeLoadToIndex
//
//      convert a node load to an index into the list for writing.
//
//  const NodeLoadImp * ld -> the load.
//
//  returns DlInt32        <- the index.
//----------------------------------------------------------------------------------------
DlInt32
frame_data::NodeLoadToIndex(LoadCase lc, const NodeLoadImp * ld) const
{
	if (ld)
		return itsNodeLoads.IndexOf(ld);
	return -1;
}

//----------------------------------------------------------------------------------------
//  frame_data::NodeToIndex
//
//      convert a node to an index into the list for writing.
//
//  const NodeImp * nd -> the node.
//
//  returns DlInt32    <- the index.
//----------------------------------------------------------------------------------------
DlInt32
frame_data::NodeToIndex(const NodeImp * nd) const
{
	if (nd)
		return itsNodes.IndexOf(nd);
	return -1;
}

//----------------------------------------------------------------------------------------
//  frame_data::ElemToIndex
//
//      convert an element to an index into the list for writing.
//
//  const ElementImp * elem    -> the element.
//
//  returns DlInt32            <- the index.
//----------------------------------------------------------------------------------------
DlInt32
frame_data::ElemToIndex(const ElementImp * elem) const
{
	if (elem)
		return itsElements.IndexOf(elem);
	return -1;
}

//----------------------------------------------------------------------------------------
//  frame_data::ElemLoadToIndex
//
//      convert an element load to an index into the list for writing.
//
//  const ElemLoadImp * eld    -> the element load.
//
//  returns DlInt32            <- the index.
//----------------------------------------------------------------------------------------
DlInt32
frame_data::ElemLoadToIndex(LoadCase lc, const ElemLoadImp * eld) const
{
	if (eld)
		return itsElemLoads.IndexOf(eld);
	return -1;
}

//----------------------------------------------------------------------------------------
//  frame_data::PropToIndex
//
//      convert a property to an index into the list for writing.
//
//  const PropertyImp * prop   -> the property.
//
//  returns DlInt32            <- the index.
//----------------------------------------------------------------------------------------
DlInt32
frame_data::PropToIndex(const PropertyImp * prop) const
{
	if (prop)
		return itsProperties.IndexOf(prop);
	return -1;
}


//----------------------------------------------------------------------------------------
//  frame_data::IndexToNodeLoad
//
//      convert an index into a node load for reading.
//
//  DlInt32 index          -> the index.
//
//  returns NodeLoadImp *  <- the node load.
//----------------------------------------------------------------------------------------
NodeLoadImp *
frame_data::IndexToNodeLoad(LoadCase lc, DlInt32 index) const
{
	if (index < 0 || index >= itsNodeLoads.Length())
		return NULL;
	return itsNodeLoads.ElementAt(index);
}

//----------------------------------------------------------------------------------------
//  frame_data::IndexToNode
//
//      convert an index into a node for reading.
//
//  DlInt32 index      -> the index.
//
//  returns NodeImp *  <- the node.
//----------------------------------------------------------------------------------------
NodeImp *
frame_data::IndexToNode(DlInt32 index) const
{
	if (index < 0)
		return NULL;
	return itsNodes.ElementAt(index);
}

//----------------------------------------------------------------------------------------
//  frame_data::IndexToElem
//
//      convert an index into an element for reading.
//
//  DlInt32 index          -> the index.
//
//  returns ElementImp *   <- the element load.
//----------------------------------------------------------------------------------------
ElementImp *
frame_data::IndexToElem(DlInt32 index) const
{
	if (index < 0)
		return NULL;
	return itsElements.ElementAt(index);
}

//----------------------------------------------------------------------------------------
//  frame_data::IndexToElemLoad
//
//      convert an index into an element load for reading.
//
//  DlInt32 index          -> the index.
//
//  returns ElemLoadImp *  <- the element load.
//----------------------------------------------------------------------------------------
ElemLoadImp *
frame_data::IndexToElemLoad(LoadCase lc, DlInt32 index) const
{
	if (index < 0 || index >= itsElemLoads.Length())
		return NULL;
	return itsElemLoads.ElementAt(index);
}

//----------------------------------------------------------------------------------------
//  frame_data::IndexToProp
//
//      convert an index into a property for reading.
//
//  DlInt32 index          -> the index.
//
//  returns PropertyImp *  <- the property.
//----------------------------------------------------------------------------------------
PropertyImp *
frame_data::IndexToProp(DlInt32 index) const
{
	if (index < 0)
		return NULL;
	return itsProperties.ElementAt(index);
}

//----------------------------------------------------------------------------------------
//  frame_data::GetResults
//
//      return the results for the specified load case.
//
//  LoadCase lc                    -> the load case.
//
//  returns const LoadCaseResults* <- the property.
//----------------------------------------------------------------------------------------
const LoadCaseResults* 
frame_data::GetResults(LoadCase lc) const 
{
	if (results.size() > lc)
		return results[lc];

	return NULL;
}

//----------------------------------------------------------------------------------------
//  frame_data::SetDisplacementMagnifier
//
//      set the displacement magnifier for the specified load case.
//
//  LoadCase lc        -> the load case.
//  DlFloat64 mag      -> the factor
//
//  returns nothing.
//----------------------------------------------------------------------------------------
void
frame_data::SetDisplacementMagnifier(LoadCase lc, DlFloat64 mag)
{
	if (results.size() > lc)
		results[lc]->SetDisplacementMagnifier(mag);
}

//----------------------------------------------------------------------------------------
//  frame_data::CreateLoadCase
//
//      create a new load case with the specified name.
//
//  const char* name   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::CreateLoadCase(const char* name)
{
	itsLoadCases.push_back(LoadCaseEntry(-1, name));
}

//----------------------------------------------------------------------------------------
//  frame_data::UpdateLoadCase
//
//      update the name for the specified load case.
//
//  LoadCase lc        -> 
//  const char* name   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::UpdateLoadCase(LoadCase lc, const char* name)
{
	_DlAssert(lc < itsLoadCases.size());
	itsLoadCases[lc].second = name;
}

//----------------------------------------------------------------------------------------
//  frame_data::RemoveLoadCase
//
//      remove the last load case created. For undo.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::RemoveLoadCase()
{
	LoadCaseEntry& lcs = itsLoadCases.back();
	
	if (lcs.first != -1) {
		updateLoadCombos(itsLoadCases.size()-1, std::vector<DlFloat32>());
		combos.pop_back(); // and remove the combo
	}
	
	itsLoadCases.pop_back();
}

//----------------------------------------------------------------------------------------
//  frame_data::GetLoadCaseCount
//
//      return the number of defined load cases.
//
//  returns DlUInt32   <- the number of defined load cases.
//----------------------------------------------------------------------------------------
DlUInt32
frame_data::GetLoadCaseCount() const
{
	return itsLoadCases.size();
}

//----------------------------------------------------------------------------------------
//  frame_data::GetLoadCaseName
//
//      return the name for the specified load case.
//
//  LoadCase lc            -> 
//
//  returns const char*    <- 
//----------------------------------------------------------------------------------------
const char*
frame_data::GetLoadCaseName(LoadCase lc) const
{
	_DlAssert(lc < itsLoadCases.size());
	return itsLoadCases[lc].second.c_str();
}

//----------------------------------------------------------------------------------------
//  frame_data::IsDefinedLoadCase
//
//      return true if the load case is not a combination.
//
//  LoadCase lc    -> 
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
bool
frame_data::IsDefinedLoadCase(LoadCase lc) const
{
	return itsLoadCases[lc].first == -1;
}

void
frame_data::updateNodeLoadCombos(LoadCase lc, const std::vector<DlFloat32>& factors)
{
	// loop over all the nodes.
	itsNodes.Reset();
	while (itsNodes.HasMore()) {
		auto p = itsNodes.Next()->UpdateLoadCombination(lc, factors);
		if (p.first && p.second)
			itsComboNodeLoads.Add(p.second);
		else if (!p.first && p.second) {
			itsComboNodeLoads.Remove(p.second);
		}
	}
}

void
frame_data::updateElemLoadCombos(LoadCase lc, const std::vector<DlFloat32>& factors)
{
	// need to loop over all the elements creating new loads for this load case.
	itsElements.Reset();
	while (itsElements.HasMore()) {
		auto p = itsElements.Next()->UpdateLoadCombination(lc, factors);
		if (p.first && p.second) {
			itsComboElemLoads.Add(p.second);
		} else if (!p.first && p.second) {
			itsComboElemLoads.Remove(p.second);
		}
	}
}

//----------------------------------------------------------------------------------------
//  frame_data::updateLoadCombos
//
//      update the loading for each node and element for the combined loading.
//
//  LoadCase lc    -> 
//  const s        -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::updateLoadCombos(LoadCase lc, const std::vector<DlFloat32>& factors)
{
	updateNodeLoadCombos(lc, factors);
	updateElemLoadCombos(lc, factors);
}

//----------------------------------------------------------------------------------------
//  frame_data::AddLoadCaseCombination                                             inline
//
//      add the specified load case combination.
//
//  const LoadCaseCombination& combo   -> the new combination.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::AddLoadCaseCombination(const char* name, const LoadCaseCombination& combo)
{
	// add a new load case, with the last element in the list as its index
	itsLoadCases.push_back(LoadCaseEntry(combos.size(), name));
	combos.push_back(combo);
	
	updateLoadCombos(itsLoadCases.size() - 1, combo.GetFactors());
	
	if (Analyzed()) {
		_DlAssert(results.size() + 1 == itsLoadCases.size());
		LoadCase curr = results.size();
		results.push_back(NEW LoadCaseResults(curr, combo, *this));
	}
}

//----------------------------------------------------------------------------------------
//  frame_data::GetLoadCaseCombination
//
//      return the combination.
//
//  DlInt32 which                  -> the index of the combination.
//
//  returns LoadCaseCombination&   <- the combination at that index.
//----------------------------------------------------------------------------------------
void
frame_data::UpdateLoadCaseCombination(LoadCase lc, const char* name,
									  const LoadCaseCombination& val)
{
	_DlAssert(lc < itsLoadCases.size());
	
	DlInt16 index = itsLoadCases[lc].first;
	_DlAssert(index >= 0 && index < combos.size());
	
	itsLoadCases[lc].second = name;
	combos[index] = val;
	
	updateLoadCombos(lc, val.GetFactors());

	if (Analyzed()) {
		delete results[lc];
		results[lc] = NEW LoadCaseResults(lc, val, *this);
	}
}

//----------------------------------------------------------------------------------------
//  frame_data::ClearAnalysis
//
//      Clear the analysis data and update the number of element in the result
//		vector. Pass 0 to indicate no analysis results.
//
//  DlInt32 newLoadCases   -> the new number of load cases.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::ClearAnalysis(bool createLoadCases)
{
	for (std::size_t i = 0; i < results.size(); i++) {
		delete results[i];
        results[i] = 0;
	}

	if (createLoadCases) {
		results.resize(itsLoadCases.size());
	} else {
		results.resize(0);
	}
}

//----------------------------------------------------------------------------------------
//  frame_data::SetActiveElementType
//
//      set the active element type.
//
//  const char* theType    -> the new active element type.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::SetActiveElementType(const char* theType) 
{
	itsActiveElementType = DlString(theType);
	itsActiveProperty = itsProperties.FindPropertyForElement(itsActiveElementType);
	if (!itsActiveProperty) {
		PropertyImp* newProp = PropertyFactory::sPropertyFactory
								.CreateProperty(
												defaultPropertyTitle.c_str(),
												itsActiveElementType
												);
		itsProperties.Add(newProp);
		
		itsActiveProperty = Property(newProp);
	}
}

//----------------------------------------------------------------------------------------
//  frame_data::SetActiveProperty
//
//      set the active property.
//
//  Property theProp   -> the new active property type.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::SetActiveProperty(Property theProp)
{
	if (theProp) {
		itsActiveProperty = theProp;
	} else {
		itsActiveProperty = itsProperties.FindPropertyForElement(itsActiveElementType);
		if (!itsActiveProperty) {
			PropertyImp* newProp = PropertyFactory::sPropertyFactory
									.CreateProperty(
													defaultPropertyTitle.c_str(),
													itsActiveElementType
													);
			itsProperties.Add(newProp);

			itsActiveProperty = Property(newProp);
		}
	}
}

//----------------------------------------------------------------------------------------
//  frame_data::setupProperties
//
//      set up the property system.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::setupProperties()
{
	itsNodes.IncReference();
	itsElements.IncReference();
	itsNodeLoads.IncReference();
	itsElemLoads.IncReference();
	itsProperties.IncReference();

	// and listen for changed to the loads
	itsNodes.AddListener(this);
	itsElements.AddListener(this);
	
	//	add node load
	if (itsNodeLoads.Length() == 0) {
		NodeLoadImp* theLoad = NEW NodeLoadImp();
		itsNodeLoads.Add(theLoad);
	}
	
	if (itsElemLoads.Length() == 0) {
		//	add element load
		ElemLoadImp* theELoad = NEW ElemLoadImp();
		itsElemLoads.Add(theELoad);
	}

	//	create one property type for each element.
	StringEnumerator elemTypes = ElementFactory::sElementFactory.GetElementTypes();
	while (elemTypes.HasMore()) {
		// if there is no property type that matches the element type
		// create one.
		const char* elementType = elemTypes.Next();
		
		PropertyEnumerator theProps(itsProperties.FindCompatibleForElement(elementType));
		if (!itsProperties.HasPropertyForElement(elementType)) {
			PropertyImp* theProp = PropertyFactory::sPropertyFactory
									.CreateProperty(
													defaultPropertyTitle.c_str(),
													elementType
													);
			itsProperties.Add(theProp);
		}
	}

	itsActiveProperty = itsProperties.FindPropertyForElement(itsActiveElementType);
}

//----------------------------------------------------------------------------------------
//  frame_data::clear
//
//      clear all structure data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
frame_data::clear() 
{
	itsProperties.Erase();
	itsElemLoads.Erase();
	itsNodeLoads.Erase();
	itsElements.Erase();
	itsNodes.Erase();
	
	itsLoadCases.clear();
	itsComboElemLoads.Erase();
	itsComboNodeLoads.Erase();
	
	combos.clear();
	
	ClearAnalysis(0);
}

//----------------------------------------------------------------------------------------
//  SolverProgress::SolverProgress                                            constructor
//
//      construct a solver progress object.
//
//  DlListener* listener   -> the listener.
//
//  returns nothing
//----------------------------------------------------------------------------------------
SolverProgress::SolverProgress(DlListener* listener) 
{
	_data.stage = AnalysisStageInit;
	_data.reason = FailureReasonNoFailure;
	_data.eqNum = 0;
	_data.total = 0;
	_data.cancel = false;
	if (listener)
		AddListener(listener);
}
	
//----------------------------------------------------------------------------------------
//  SolverProgress::SetStage
//
//      set the stage of the solution.
//		called for transitions.
//
//  EqSolveProgre  -> 
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
bool 
SolverProgress::SetStage(EqSolveProgress::Stage stage) 
{
	switch(stage) {
	case	EqSolveProgress::Initializing:		_data.stage = AnalysisStageInit;	break;
	case	EqSolveProgress::Decomposing:		_data.stage = AnalysisStageDecomp;	break;
	case	EqSolveProgress::BackSubstituting:	_data.stage = AnalysisStageBackSub;	break;
	case	EqSolveProgress::Done:				_data.stage = AnalysisStageRecover;	break;
	}
	
	_data.eqNum = 0;
	_data.cancel = false;
	return Broadcast();
}

//----------------------------------------------------------------------------------------
//  SolverProgress::Processing
//
//      call to mark analysis progress.
//		return false to cancel analysis
//
//  DlInt32 i      -> the current mark.
//  DlInt32 n      -> the total marks.
//
//  returns bool   <- false to cancel analysis.
//----------------------------------------------------------------------------------------
bool
SolverProgress::Processing(DlInt32 i, DlInt32 n) 
{
	_data.eqNum = i;
	_data.total = n;
	return Broadcast();
}

//----------------------------------------------------------------------------------------
//  SolverProgress::Broadcast
//
//      broadcast progress.
//
//  returns bool   <- false to cancel analysis.
//----------------------------------------------------------------------------------------
bool	
SolverProgress::Broadcast() 
{
	BroadcastMessage(kAnalysisMessage, &_data);
	return !_data.cancel;
}


//	eof
