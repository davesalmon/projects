/*+
 *
 *  MassListController.mm
 *
 *  Copyright © 2008 David C. Salmon. All Rights Reserved.
 *
 *  Controller subclass for the mass table view.
 *
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
-*/

#import "MassListController.h"
#import "GravityDocument.h"
#import "ObjMass.h"
#import "ColorCell.h"
#import "ResultFile.h"

@implementation MassListController
//----------------------------------------------------------------------------------------
//  createMassPanel:
//
//      Create the Mass editor panel.
//
//  createNodeLoadPanel: GravityDocument* doc    -> the document
//
//  returns MassListController*                <- the window controller
//----------------------------------------------------------------------------------------
+ (MassListController*)createMassPanel: (GravityDocument*) doc
{
    MassListController* theController = nil;

    theController = [[MassListController alloc] initWithDocument: doc];
//    [theController showWindow:theController];
    [theController setShouldCloseDocument:NO];
    [doc addWindowController:theController];
    [theController release];
    return theController;
}

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      Initialize with the current document.
//
//  initWithDocument: GravityDocument* doc   -> the document
//
//  returns id                             <- self
//----------------------------------------------------------------------------------------
- (id) initWithDocument: (GravityDocument*) doc
{
	self = [super initWithWindowNibName:@"MassListController" owner:self];
	_gravDocument = doc;

	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      destroy the object.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	fprintf(stderr, "destroy mass list controller\n");

	[_gravDocument setNoMassController];
	
	[super dealloc];
}

//------------------------------------------------------------------------------
//  windowTitleForDocumentDisplayName:
//
//      Return the title for the window.
//
//  windowTitleForDocumentDisplayName: NSString* displayName   -> The default display name.
//
//  returns NSString*                                          <- The new display name.
//------------------------------------------------------------------------------
- (NSString*)windowTitleForDocumentDisplayName:(NSString*)displayName
{
	return (NSString*)[NSString stringWithFormat:@"%@ - %s",
			displayName, "masses"];
}

//----------------------------------------------------------------------------------------
//  awakeFromNib
//
//      handle initialization of table columns.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) awakeFromNib
{
	ColorCell* cell = [[[ColorCell alloc]initImageCell:nil] autorelease];
	[cell setEditable: YES];
	[cell setTarget: self];
	[cell setAction: @selector(colorClick:)];
	[_colorColumn setDataCell: cell];
	[_table setDataSource: _gravDocument];
	[self setResultsPath: [_gravDocument currentPath]];
	[self setCurrentStep: 0];
	[self dataChanged];
	[self setWindowFrameAutosaveName:[self windowTitleForDocumentDisplayName:
		[_gravDocument displayName]]];
	[self menuNeedsUpdate: nil];
	[_centerMenu selectItemAtIndex:[_gravDocument originIndex]+1];
	
	NSNumberFormatter *formatter = [[[NSNumberFormatter alloc] init] autorelease];
	[formatter setNumberStyle:NSNumberFormatterDecimalStyle];
	[formatter setMaximumFractionDigits:1];
	[_currentStepField setFormatter: formatter];
}

//----------------------------------------------------------------------------------------
//  remove:
//
//      remove the selected mass.
//
//  remove: id sender  -> ignored
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)remove:(id)sender
{
	int row = [_table selectedRow];
	if (row >= 1)
	{
		[_gravDocument removeMass: row];
		[_table noteNumberOfRowsChanged];
	}
	else
	{
		// display error
		NSBeginAlertSheet(@"Error", @"Ok", nil, nil, [self window], nil, nil, nil, 
						nil, @"You must have at least one mass."); 
	}
}

//----------------------------------------------------------------------------------------
//  colorClick:
//
//      handle click in the color selector.
//
//  colorClick: id sender  -> ignored
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) colorClick: (id)sender
{
//	fprintf(stderr, "click\n");
	int sel = [_table selectedRow];
	if (sel >= 0)
	{
//		ObjMass *m = [_gravDocument getMass:sel];
		NSColorPanel* panel = [NSColorPanel sharedColorPanel];
		[panel setTarget: self];
		[panel setAction: @selector(changeColor:)];
		[panel makeKeyAndOrderFront:self];
	}
}

//----------------------------------------------------------------------------------------
//  changeColor:
//
//      callback for color picker to change the color.
//
//  changeColor: id sender -> ignored
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) changeColor:(id) sender
{
	int sel = [_table selectedRow];
	if (sel >= 0)
	{
		NSColorPanel* panel = [NSColorPanel sharedColorPanel];
		[_gravDocument setColor:[panel color] forMass:sel];
	}
}

//----------------------------------------------------------------------------------------
//  create:
//
//      create a new mass.
//
//  create: id sender  -> ignored.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)create:(id)sender
{
	[_gravDocument addMass: [ObjMass create]];
	[_table noteNumberOfRowsChanged];
}

//----------------------------------------------------------------------------------------
//  startSimulation:
//
//      start or stop the simulation.
//
//  startSimulation: id sender -> ignored
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)toggleSimulation:(id)sender
{
	if ([_gravDocument running])
		[_gravDocument stopSimulation];
	else
		[_gravDocument startSimulation];
	[self updateUI];
}

//----------------------------------------------------------------------------------------
//  stopSimulation:
//
//      reset the simulation.
//
//  stopSimulation: id sender  -> ignored
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)resetSimulation:(id)sender
{
	[_gravDocument resetSimulation];
	[self updateUI];
}

//----------------------------------------------------------------------------------------
//  dataChanged
//
//      called when the data is changed to update the UI
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)dataChanged
{
	[_table setNeedsDisplay: YES];
	

	[_displayStepSizeField setDoubleValue:[_gravDocument displayStepSize]];
	[_displayStepper setDoubleValue:[_gravDocument displayStepSize]];

	[_viewScaleField setDoubleValue:[_gravDocument scale]];
	[_scaleStepper setDoubleValue:[_gravDocument scale]];
	
	int saveStep = int(log2([_gravDocument saveStepSize]));
	[_saveStepStepper setIntValue:saveStep];
	[_saveStepSizeField setDoubleValue:[_gravDocument saveStepSize]];

	[self updateUI];
}

//----------------------------------------------------------------------------------------
//  updateUI
//
//      update the name of the buttons for the current app state.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)updateUI
{
	BOOL isRunning = [_gravDocument running];
	BOOL isReplay = [_gravDocument replaying];
	BOOL isActive = isRunning || isReplay;
	
	
	[_currentStepLabel setEnabled: isActive];
	[_currentStepField setEnabled: isActive];
	[_currentResultsPath setEnabled: !isActive];

	[_resetSimulation setEnabled: !isActive && [_gravDocument hasSimulation]];
	[_resetReplay setEnabled: !isActive && [_gravDocument inReplay]];

	if (isRunning)
	{
		[_toggleSimulation setTitle:@"Stop Simulation"];
		[_toggleSimulation setEnabled:YES];
		[_toggleReplay setEnabled: NO];
		[_saveStepStepper setEnabled: NO];
	}
	else if (isReplay)
	{
		[_toggleSimulation setEnabled:NO];
		[_toggleReplay setTitle:@"Stop"];
		[_toggleReplay setEnabled: YES];
		[_saveStepStepper setEnabled: NO];
	}
	else
	{
		if ([_gravDocument hasSimulation])
			[_toggleSimulation setTitle:@"Resume Simulation"];
		else
			[_toggleSimulation setTitle:@"Start Simulation"];
		[_toggleSimulation setEnabled: YES];
		
		[_toggleReplay setTitle:@"Replay"];
		[_toggleReplay setEnabled: [_gravDocument canReplay]];
//		[_stopSim setEnabled:YES];
		[_saveStepStepper setEnabled: YES];
	}
}

//----------------------------------------------------------------------------------------
//  toggleReplay:
//
//      toggle the replay button.
//
//  toggleReplay: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) toggleReplay: (id) sender
{
	if ([_gravDocument replaying])
		[_gravDocument stopReplay];
	else
		[_gravDocument startReplay];
	[self updateUI];
}

//----------------------------------------------------------------------------------------
//  resetReplay:
//
//      reset the replay back to the beginning.
//
//  resetReplay: id sender -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) resetReplay: (id) sender
{
	[_gravDocument resetReplay];
	[self updateUI];
}

//----------------------------------------------------------------------------------------
//  acceptsFirstResponder
//
//      set accepts first responder.
//
//  returns BOOL   <- returns YES
//----------------------------------------------------------------------------------------
- (BOOL) acceptsFirstResponder
{
	return YES;
}

//----------------------------------------------------------------------------------------
//  keyDown:
//
//      handle key press for controller.
//
//  keyDown: NSEvent* event    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) keyDown:(NSEvent*) event
{
	[_gravDocument keyDown: event];
}

//----------------------------------------------------------------------------------------
//  setCurrentStep:
//
//      set the step size.
//
//  setCurrentStep: double step   ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setCurrentStep : (double) step
{
	[_currentStepField setDoubleValue: step];
}

//----------------------------------------------------------------------------------------
//  chooseFile:
//
//      select the file. does nothing.
//
//  chooseFile: id sender  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
//- (IBAction) chooseFile:(id)sender
//{
//	NSSavePanel* saver = [NSSavePanel savePanel];
//	[saver setCanCreateDirectories:TRUE];
//	[saver setTitle:@"Select or Create output file."];
//	[saver setPrompt:@"Select"];
//	[saver setExtensionHidden:TRUE];
//	
//	[saver setAllowedFileTypes:[NSArray arrayWithObject:@"out"]];
//	
//	NSString* currentPath = [_gravDocument currentPath];
//
//	if (currentPath)
//		[saver setNameFieldStringValue: [currentPath lastPathComponent]];
//	
//	// run the dialog.
//	if ([saver runModal] == NSFileHandlingPanelOKButton) {
//		[self setResultsPath: [[saver URL] path]];
//		[self updateResults:nil];
//	}
//}

//----------------------------------------------------------------------------------------
//  toggleDrawPolys:
//
//      handle draw tails checkbox.
//
//  toggleDrawPolys: id sender -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) toggleDrawPolys:(id)sender
{
	NSButton* b = sender;
	[_gravDocument setDrawPolys: [b state] == NSOnState];
}

//----------------------------------------------------------------------------------------
//  setResultsPath:
//
//      set the path for the file.
//
//  setResultsPath: NSString* path -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setResultsPath: (NSString*) path
{
	NSLog(@"set results path to %@", path);
	if (path)
		[_currentResultsPath setStringValue:path];
}

//----------------------------------------------------------------------------------------
//  resultsPath
//
//      return the path for the file.
//
//  returns NSString*  <-
//----------------------------------------------------------------------------------------
- (NSString*) resultsPath
{
	return [_gravDocument currentPath];
}

//----------------------------------------------------------------------------------------
//  centerMenuSelect:
//
//      select the location of the display center.
//
//  centerMenuSelect: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) centerMenuSelect:(id)sender
{
	int sel = [_centerMenu indexOfSelectedItem];
	[_gravDocument setOriginIndex: sel - 1];
}

//----------------------------------------------------------------------------------------
//  updateDisplayStep:
//
//      update the display step size from the stepper.
//
//  updateDisplayStep: id sender   -> _displayStepper
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)updateDisplayStep:(id)sender {
	double displayStep = [_displayStepper doubleValue];
	[_gravDocument setDisplayStepSize: displayStep];
}

//----------------------------------------------------------------------------------------
//  updateSaveStep:
//
//      update the save step size from its stepper.
//
//  updateSaveStep: id sender  -> _saveStepStepper
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)updateSaveStep:(id)sender {
	double value = [_saveStepStepper doubleValue];
	
	value = pow(2, value);
	
	[_gravDocument setSaveStepSize:value];
}

//----------------------------------------------------------------------------------------
//  updateViewScale:
//
//      update the view scale size from its stepper.
//
//  updateViewScale: id sender -> _scaleStepper
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)updateViewScale:(id)sender {
	double scale = [_scaleStepper doubleValue];
	[_gravDocument setScale: scale];
}

//----------------------------------------------------------------------------------------
//  menuNeedsUpdate:
//
//      Update the display center menu.
//
//  menuNeedsUpdate: NSMenu * menu -> the menu.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)menuNeedsUpdate:(NSMenu *)menu
{
//	int item = [_gravDocument originIndex] + 1;
	unsigned count = [_gravDocument countMasses];
	unsigned items = [_centerMenu numberOfItems];
	if (items < count + 1)
	{
		for (unsigned i = items; i < count + 1; i++)
		{
			[_centerMenu addItemWithTitle: [NSString stringWithFormat: @"Mass #%d", i]];
		}
	}
	else if (items > count + 1)
	{
		for (int i = items - 1; i > count + 1; i--)
		{
			[_centerMenu removeItemAtIndex:i];
		}
	}
}

//----------------------------------------------------------------------------------------
//  windowDidUpdate:
//
//      update the location for the simulation output.
//
//  windowDidUpdate: NSNotification * notification -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)windowDidUpdate:(NSNotification *)notification
{
	NSString* currPath = [_currentResultsPath stringValue];
	NSString* newPath = [_gravDocument currentPath];
	if ([currPath compare: newPath] != NSOrderedSame) {
		[self setResultsPath: newPath];
	}
}

@end
