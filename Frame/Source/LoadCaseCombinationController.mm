/*+
 *
 *  LoadCaseCombinationController.mm
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  handle defintion of load case factors.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
-*/

#import "LoadCaseCombinationController.h"
#import "FrameDocument.h"
#import "FrameAction.h"

#include "LoadCaseCombination.h"

@interface LoadCaseCombinationController ()

	@property (assign) IBOutlet NSTextField *nameField;

	@property (assign) IBOutlet NSTableView *tableView;
	@property (assign) IBOutlet NSTableColumn *loadCaseColumn;
	@property (assign) IBOutlet NSTableColumn *factorColumn;

	@property (assign) IBOutlet NSButton *okButton;

	@property FrameDocument* frameDocument;

	@property LoadCaseCombination* combo;

	@property FrameStructure::LoadCaseList loadCases;

	@property std::vector<DlFloat32> origFactors;
	@property std::vector<DlFloat32> factors;
	@property NSString* name;

- (IBAction)ok:(id)sender;
- (IBAction)cancel:(id)sender;

- (void) setFrameDocument: (FrameDocument*) doc andCombo: (LoadCaseCombination*) combo;

- (bool) valuesChanged;

@end

@implementation LoadCaseCombinationController

//----------------------------------------------------------------------------------------
//  runDialogWithDocument:andCombo:
//
//      run the dialog.
//
//  runDialogWithDocument: FrameDocument* doc  -> 
//  andCombo: LoadCaseCombination* combo       -> 
//
//  returns bool                               <- 
//----------------------------------------------------------------------------------------
+ (bool) runDialogWithDocument: (FrameDocument*) doc andCombo: (LoadCaseCombination*) combo
{
	LoadCaseCombinationController* controller = [[LoadCaseCombinationController alloc]
										   initWithWindowNibName:@"LoadCaseCombinationController"];
	
	[controller setFrameDocument: doc andCombo: combo];

	[NSApp 	beginSheet:[controller window]
		modalForWindow:[doc mainWindow]
			modalDelegate:nil
		didEndSelector:nil
		   contextInfo:nil];
	
	int result = [NSApp runModalForWindow:[controller window]];
	
	[NSApp endSheet: [controller window]];
	[controller close];
	[controller release];
	
	return result == NSRunStoppedResponse;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      dealloc
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	[super dealloc];
	[_name release];
}

//----------------------------------------------------------------------------------------
//  numberOfRowsInTableView:
//
//      return the number of rows for the table.
//
//  numberOfRowsInTableView: NSTableView * tableView   -> 
//
//  returns NSInteger                                  <- 
//----------------------------------------------------------------------------------------
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return _loadCases.size();
}

//----------------------------------------------------------------------------------------
//  tableView:objectValueForTableColumn:row:
//
//      return the value for the specified row/column.
//
//  tableView: NSTableView * tableView                     -> 
//  objectValueForTableColumn: NSTableColumn * tableColumn -> 
//  row: NSInteger row                                     -> 
//
//  returns id                                             <- 
//----------------------------------------------------------------------------------------
- (id)             tableView: (NSTableView *)tableView
   objectValueForTableColumn: (NSTableColumn *)tableColumn
						 row: (NSInteger)row
{
	if (tableColumn == _loadCaseColumn) {
		return [NSString stringWithUTF8String: _loadCases[row].first];
	} else if (tableColumn == _factorColumn) {
		LoadCase lc = _loadCases[row].second;
		
		return [NSNumber numberWithFloat: _factors[lc]];
	}
	
	return nil;
}

//----------------------------------------------------------------------------------------
//  tableView:setObjectValue:forTableColumn:row:
//
//      set the value for the specified row.
//
//  tableView: NSTableView * tableView             -> 
//  setObjectValue: id object                      -> 
//  forTableColumn: NSTableColumn * tableColumn    -> 
//  row: NSInteger row                             -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)       tableView: (NSTableView *)tableView
		  setObjectValue: (id)object
		  forTableColumn: (NSTableColumn *)tableColumn
					 row: (NSInteger)row
{
	if (tableColumn == _factorColumn) {
		NSLog(@"Got value %f for row %ld",  [object floatValue], row);
		LoadCase lc = _loadCases[row].second;
		_factors[lc] = [object floatValue];
	}
}

//----------------------------------------------------------------------------------------
//  windowDidLoad
//
//      update items on load.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)windowDidLoad {
    [super windowDidLoad];
	
	if (_combo->GetFactors().size() > 0) {
		FrameStructure* s = _frameDocument.structure;
		
		_nameField.stringValue = [NSString stringWithUTF8String:
								  s->GetLoadCaseName(s->GetActiveLoadCase())];
	}
}

//----------------------------------------------------------------------------------------
//  ok:
//
//      handle ok button.
//
//  ok: id sender  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)ok:(id)sender {

	[[self window] makeFirstResponder: [self window]];

	// add the load factor set to the structure.
	[NSApp stopModal];

	FrameStructure* s = _frameDocument.structure;
	bool isUpdate = _combo->GetFactors().size() > 0;
	
	for (int i = 0; i < _loadCases.size(); i++) {
		_combo->SetFactor(i, _factors[i]);
	}

	if (isUpdate) {
		
		[_frameDocument performAction: [FrameAction createUpdatingLoadCase:
		 	s->UpdateLoadCaseCombination(s->GetActiveLoadCase(), [_nameField.stringValue UTF8String],
								   std::move(*_combo))]];
	} else {
		
		[_frameDocument performAction: [FrameAction createUpdatingLoadCase:
										s->AddLoadCaseCombination([_nameField.stringValue UTF8String],
																	 std::move(*_combo))]];
	}
}

//----------------------------------------------------------------------------------------
//  cancel:
//
//      handle cancel button.
//
//  cancel: id sender  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)cancel:(id)sender {
	[NSApp abortModal];
}

//----------------------------------------------------------------------------------------
//  setFrameDocument:andCombo:
//
//      set the document and combo values.
//
//  setFrameDocument: FrameDocument* doc   -> 
//  andCombo: LoadCaseCombination* combo   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setFrameDocument: (FrameDocument*) doc andCombo: (LoadCaseCombination*) combo
{
	_frameDocument = doc;
	_combo = combo;

	FrameStructure* s = _frameDocument.structure;

	_loadCases = s->GetEditableLoadCases();
	
	_factors.resize(s->GetLoadCaseCount());
	
	for (auto i = 0; i < _loadCases.size(); i++) {
		if (i < _combo->GetFactors().size())
			_factors[i] = _combo->GetFactor(i);
	}
	
	_origFactors = _factors;
	if (_combo->GetFactors().size() > 0) {
		_name = [[NSString stringWithUTF8String: s->GetLoadCaseName(s->GetActiveLoadCase())] retain];
	} else {
		_name = @"";
	}
}

//----------------------------------------------------------------------------------------
//  valuesChanged
//
//      return true if the values have changed.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) valuesChanged
{
	bool changed = [_nameField.stringValue compare: _name] != NSOrderedSame;
	if (!changed)
		changed = _factors.size() != _origFactors.size();
	
	if (!changed) {
		
		for (auto i = 0; i < _factors.size(); i++)
			if (_factors[i] != _origFactors[i])
				return true;
	}
	
	return changed;
}

//----------------------------------------------------------------------------------------
//  controlTextDidChange:
//
//      handle typing.
//
//  controlTextDidChange: NSNotification * obj -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)controlTextDidChange:(NSNotification *)obj
{
//	NSLog(@"Got text change message");
	
	bool doEnable = [self valuesChanged];
	
	if (_okButton.enabled ^ doEnable)
		_okButton.enabled = doEnable;
}

@end
