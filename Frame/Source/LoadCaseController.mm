/*+
 *
 *  LoadCaseController.h
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  Handle operations for the load case panel.
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

#import "LoadCaseController.h"
#import "FrameDocument.h"
#import "FrameAction.h"

#include "FrameStructure.h"

@interface LoadCaseController ()
@property (assign) IBOutlet NSTextField *loadCaseName;
@property (assign) IBOutlet NSButton *okButton;

@property (nonatomic) NSString* origName;
@property (assign) FrameDocument* doc;

- (IBAction)okPressed:(id)sender;
- (IBAction)cancelPressed:(id)sender;

- (void) setDocument: (FrameDocument*) doc andName: (const char*) name;

@end

@implementation LoadCaseController

//----------------------------------------------------------------------------------------
//  runDialogWithDocument:andName:
//
//      run the dialog.
//
//  runDialogWithDocument: FrameDocument* doc  ->
//  andName: const char* name                  ->
//
//  returns bool                               <-
//----------------------------------------------------------------------------------------
+ (bool) runDialogWithDocument: (FrameDocument*) doc andName: (const char*) name
{
	LoadCaseController* controller = [[LoadCaseController alloc]
										   initWithWindowNibName:@"LoadCaseController"];
	
	[controller setDocument: doc andName: name];
	
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
//  windowDidLoad
//
//      update items on load.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)windowDidLoad {
    [super windowDidLoad];

	_loadCaseName.stringValue = _origName;
}

//----------------------------------------------------------------------------------------
//  okPressed:
//
//      handle ok button.
//
//  ok: id sender  ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)okPressed:(id)sender {
	FrameStructure* s = [_doc structure];
	
	if (_origName.length > 0) {
		[_doc performAction: [FrameAction createUpdatingLoadCase:
				s->UpdateLoadCase(s->GetActiveLoadCase(), _loadCaseName.stringValue.UTF8String)]];
	} else {
		[_doc performAction: [FrameAction createUpdatingLoadCase:
							  s->CreateLoadCase(_loadCaseName.stringValue.UTF8String)]];
	}
	[NSApp stopModal];
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
- (IBAction)cancelPressed:(id)sender {
	[NSApp abortModal];
}

- (void) setDocument: (FrameDocument*) doc andName: (const char*) name
{
	if (name) {
		_origName = [NSString stringWithUTF8String: name];
	} else {
		_origName = @"";
	}
	
	_doc = doc;
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
	
	NSString* newName = [_loadCaseName stringValue];
	
	bool doEnable = newName.length > 0 && ![newName isEqualToString: _origName];
	
	if (_okButton.enabled ^ doEnable)
		_okButton.enabled = doEnable;
}

@end
