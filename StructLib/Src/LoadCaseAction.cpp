//
//  LoadCaseAction.cpp
//  StructLib
//
//  Created by David Salmon on 5/7/15.
//
//

#include "LoadCaseAction.h"
#include "frame_data.h"

#pragma mark -
#pragma mark Add Load Case
#pragma mark -

AddLoadCaseAction::AddLoadCaseAction(const char* name, frame_data& data)
	: _data(data), _name(name), _origActLC(data.GetActiveLoadCase())
{
}

AddLoadCaseAction::~AddLoadCaseAction()
{
}

void AddLoadCaseAction::Redo()
{
	_data.CreateLoadCase(_name.c_str());
	_data.SetActiveLoadCase(_data.GetLoadCaseCount()-1);
}

void AddLoadCaseAction::Undo()
{
	_data.RemoveLoadCase();
	_data.SetActiveLoadCase(_origActLC);
}

#pragma mark -
#pragma mark Update Load Case
#pragma mark -

UpdateLoadCaseAction::UpdateLoadCaseAction(LoadCase lc, const char* name, frame_data& data)
:  _data(data), _name(name), _lc(lc)
{
}

UpdateLoadCaseAction::~UpdateLoadCaseAction()
{
}

void UpdateLoadCaseAction::Redo()
{
	std::string name(_data.GetLoadCaseName(_lc));
	_data.UpdateLoadCase(_lc, _name.c_str());
	_name = std::move(name);
}

void UpdateLoadCaseAction::Undo()
{
	Redo();
}

#pragma mark -
#pragma mark Load Case combinations
#pragma mark -

AddLoadCaseComboAction::AddLoadCaseComboAction(const char* name,
											   LoadCaseCombination&& combo,
											   frame_data& data)
: _data(data), _name(name), _combo(combo), _origActLC(data.GetActiveLoadCase())
{
}

AddLoadCaseComboAction::~AddLoadCaseComboAction()
{
}

void AddLoadCaseComboAction::Redo()
{
	_data.AddLoadCaseCombination(_name.c_str(), _combo);
	if (_data.Analyzed()) {
		_data.SetActiveLoadCase(_data.GetLoadCaseCount()-1);
	}
}

void AddLoadCaseComboAction::Undo()
{
	_data.RemoveLoadCase();
	if (_data.Analyzed()) {
		_data.SetActiveLoadCase(_origActLC);
	}
}

#pragma mark -
#pragma mark update load case combinations
#pragma mark -

UpdateLoadCaseComboAction::UpdateLoadCaseComboAction(LoadCase lc, const char* name,
											   LoadCaseCombination&& combo,
											   frame_data& data)
: _data(data), _name(name), _combo(combo), _lc(lc)
{
}

UpdateLoadCaseComboAction::~UpdateLoadCaseComboAction()
{
}

void UpdateLoadCaseComboAction::Redo()
{
	// swap out the combo.
	LoadCaseCombination curr = *_data.GetLoadCaseCombination(_lc);
	std::string name(_data.GetLoadCaseName(_lc));
	_data.UpdateLoadCaseCombination(_lc, _name.c_str(), _combo);
	_combo = std::move(curr);
	_name = std::move(name);
}

void UpdateLoadCaseComboAction::Undo()
{
	Redo();
}
