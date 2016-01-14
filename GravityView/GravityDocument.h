//
//  MyDocument.h
//  GravityView
//
//  Created by David Salmon on 12/23/08.
//  Copyright David Salmon 2008 . All rights reserved.
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

@class ObjMass;
@class GravityView;
@class MassListController;
@class ResultFile;
@class Simulation;

@interface GravityDocument : NSDocument  <NSTableViewDelegate, NSTableViewDataSource>
{
	IBOutlet GravityView* _gview;
//	System* _system;
	NSMutableArray* _massList;
	
	double _viewScale;
	double _displayStepSize;
	double _saveStepSize;
	
	NSString* _currentPath;
	int _originIndex;
	
	MassListController* _massController;
	
//	BOOL _running;
	BOOL _drawPolys;
	BOOL _fileSaved;	// keep track of when the file was saved.
	
	Simulation* _simulation;
}

- (GravityView*) view;

- (Simulation*) simulation;

- (BOOL) running;			// YES if simulation is active
- (BOOL) hasSimulation;		// YES is simulation is paused
- (void) startSimulation;
- (void) stopSimulation;
- (void) resetSimulation;

- (BOOL) replaying;			// YES if replay is playing
- (BOOL) inReplay;			// YES if replay is paused.
- (BOOL) canReplay;
- (void) startReplay;
- (void) stopReplay;
- (void) resetReplay;

- (unsigned)countMasses;
- (ObjMass*)getMass: (unsigned) which;
- (void)addMass: (ObjMass*) mass;
- (void)removeMass: (unsigned) which;

- (void)setX: (NSNumber*) x forMass: (unsigned) index;
- (void)setY: (NSNumber*) y forMass: (unsigned) index;

- (void)setVX: (NSNumber*) vx forMass: (unsigned) index;
- (void)setVY: (NSNumber*) vy forMass: (unsigned) index;

- (void)setDisplaySize: (NSNumber*) size forMass: (unsigned) index;
- (void)setTailLength: (NSNumber*) size forMass: (unsigned) index;
- (void)setColor: (NSColor*) color forMass: (unsigned) index;

- (void) setCurrentStep: (double) step;

- (void)setScale: (double) scale;
- (double)scale;

- (void)setDisplayStepSize: (double) stepSize;
- (double)displayStepSize;


- (void)setSaveStepSize: (double) stepSize;
- (double)saveStepSize;

- (NSString*)currentPath;

- (void) setDrawPolys: (BOOL) size;
- (BOOL) drawPolys;

- (int) originIndex;
- (void) setOriginIndex: (int) index;

- (void)listChanged: (BOOL) doStopSim;

- (void) setNoMassController;

// -----------------------
// dataSource protocol
- (int) numberOfRowsInTableView:(NSTableView *)tableView;
- (id) tableView: (NSTableView *)tableView objectValueForTableColumn: (NSTableColumn *)tableColumn row:(int)row;
- (void)tableView:(NSTableView *)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn row:(int)row;

- (void) keyDown:(NSEvent*) event;

// window delegate methods
- (void)windowDidResignMain:(NSNotification *)aNotification;
- (void)windowDidBecomeMain:(NSNotification *)aNotification;

@end
