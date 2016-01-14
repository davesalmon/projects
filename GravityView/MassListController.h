//
//  MassListController.h
//  GravityView
//
//  Created by David Salmon on 12/24/08.
//  Copyright 2008 David Salmon. All rights reserved.
//
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

#import <Cocoa/Cocoa.h>

@class GravityDocument;

@interface MassListController : NSWindowController {

    IBOutlet NSButton*		_newButton;
    IBOutlet NSButton*		_removeButton;
	
	IBOutlet NSButton*		_toggleSimulation;
	IBOutlet NSButton*		_resetSimulation;
	
	IBOutlet NSButton*		_toggleReplay;
	IBOutlet NSButton*		_resetReplay;
	
	IBOutlet NSTextField*	_currentStepLabel;
	IBOutlet NSTextField*	_currentStepField;
	
	IBOutlet NSTextField*	_currentResultsPath;
	
    IBOutlet NSTableView*	_table;
	IBOutlet NSTableColumn* _colorColumn;

	IBOutlet NSPopUpButton*	_centerMenu;
	
	IBOutlet NSTextField*	_displayStepSizeField;	// contains the display step size
	IBOutlet NSStepper*		_displayStepper;
	
	IBOutlet NSTextField*	_viewScaleField;
	IBOutlet NSStepper*		_scaleStepper;
	
	IBOutlet NSTextField*	_saveStepSizeField;		// contains the save step size
	IBOutlet NSStepper*		_saveStepStepper;
	
	GravityDocument*		_gravDocument;
}

+ (MassListController*)createMassPanel: (GravityDocument*) doc;

- (id) initWithDocument: (GravityDocument*) doc;

// remove the selected mass
- (IBAction) remove:(id)sender;
// create a new row (mass)
- (IBAction) create:(id)sender;

- (IBAction) toggleSimulation:(id)sender;
- (IBAction) resetSimulation:(id)sender;

- (IBAction) toggleReplay:(id)sender;
- (IBAction) resetReplay:(id)sender;

- (IBAction) centerMenuSelect:(id)sender;
- (IBAction) updateDisplayStep:(id)sender;
- (IBAction) updateSaveStep:(id)sender;
- (IBAction) updateViewScale:(id)sender;

- (void)updateUI;
- (void)dataChanged;

- (void)setCurrentStep : (double) step;

//-(BOOL) checkSimFile;

- (IBAction) toggleDrawPolys:(id)sender;
//- (IBAction) chooseFile:(id)sender;

- (void)setResultsPath: (NSString*) path;
- (NSString*) resultsPath;

- (void)menuNeedsUpdate:(NSMenu *)menu;


@end

