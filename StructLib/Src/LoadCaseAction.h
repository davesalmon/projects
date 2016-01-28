//
//  LoadCaseAction.h
//  StructLib
//
//  Created by David Salmon on 5/7/15.
//
//

#ifndef __StructLib__LoadCaseAction__
#define __StructLib__LoadCaseAction__

#include "Action.h"
#include "LoadCaseCombination.h"
#include "frame_data.h"

class frame_data;
class LoadCaseCombination;

/* ------------------------------------------------------
 * AddLoadCaseAction
 *
 *	add load cases to the structure.
 ------------------------------------------------------*/
class	AddLoadCaseAction : public Action
{
public:
	AddLoadCaseAction(const char* name, frame_data& data);
	
	virtual ~AddLoadCaseAction();
	
	virtual void Redo();
	virtual void Undo();
	
private:
	AddLoadCaseAction(const AddLoadCaseAction& a);
	AddLoadCaseAction& operator=(const AddLoadCaseAction& a);
	
	frame_data& _data;
	std::string _name;
	LoadCase 	_origActLC;
};

/* ------------------------------------------------------
 * UpdateLoadCaseAction
 *
 *	add load cases to the structure.
 ------------------------------------------------------*/
class	UpdateLoadCaseAction : public Action
{
public:
	UpdateLoadCaseAction(LoadCase lc, const char* name, frame_data& data);
	
	virtual ~UpdateLoadCaseAction();
	
	virtual void Redo();
	virtual void Undo();
	
private:
	UpdateLoadCaseAction(const UpdateLoadCaseAction& a);
	UpdateLoadCaseAction& operator=(const UpdateLoadCaseAction& a);
	
	frame_data& _data;
	std::string _name;
	LoadCase	_lc;
};

/* ------------------------------------------------------
 * AddLoadCaseComboAction
 *
 *	add load case combination to the structure.
 ------------------------------------------------------*/
class	AddLoadCaseComboAction : public Action
{
public:
	AddLoadCaseComboAction(const char* name, LoadCaseCombination&& combo, frame_data& data);
	
	virtual ~AddLoadCaseComboAction();
	
	virtual void Redo();
	virtual void Undo();
	
private:
	AddLoadCaseComboAction(const AddLoadCaseComboAction& a);
	AddLoadCaseComboAction& operator=(const AddLoadCaseComboAction& a);
	
	frame_data& _data;
	std::string _name;
	LoadCaseCombination _combo;
	LoadCase _origActLC;
};

/* ------------------------------------------------------
 * UpdateLoadCaseComboAction
 *
 *	update load cases for the structure.
   ------------------------------------------------------*/
class	UpdateLoadCaseComboAction : public Action
{
public:
	UpdateLoadCaseComboAction(LoadCase lc, const char* name, LoadCaseCombination&& combo,
							  frame_data& data);
	
	virtual ~UpdateLoadCaseComboAction();
	
	virtual void Redo();
	virtual void Undo();
	
private:
	UpdateLoadCaseComboAction(const UpdateLoadCaseComboAction& a);
	UpdateLoadCaseComboAction& operator=(const UpdateLoadCaseComboAction& a);
	
	frame_data& 		_data;
	std::string 		_name;
	LoadCaseCombination _combo;
	LoadCase 			_lc;
};


#endif /* defined(__StructLib__LoadCaseAction__) */
