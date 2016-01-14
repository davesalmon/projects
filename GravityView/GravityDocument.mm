/*+
 *
 *  GravityDocument.mm
 *
 *  Copyright © 2008 David C. Salmon. All Rights Reserved.
 *
 *  Document subclass for gravity.
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

//#include "DlPlatform.h"

#import "GravityDocument.h"
#import "ObjMass.h"
#import "GravityView.h"
#import "MassListController.h"
#import "Simulation.h"

#include <string>
#include <sys/stat.h>

//#include "DlFileSpec.h"

const double kComputeUpdateFrequency = 1.0 / 30.0;
const double kReplayUpdateFrequency = 1.0 / 60.0;


@implementation GravityDocument

//----------------------------------------------------------------------------------------
//  init
//
//      create the document.
//
//  returns id <- 
//----------------------------------------------------------------------------------------
- (id)init
{
    self = [super init];
    if (self) {
		_massList = 0;
		_displayStepSize = 10;
		_saveStepSize = 1;
		_viewScale = 20.0;
		_currentPath = 0;
		_drawPolys = NO;
		_simulation = 0;
		_originIndex = -1;
		_fileSaved = NO;
    }
	
    return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      delete the document.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	fprintf(stderr, "rc = %ld\n", (unsigned long)[_massList retainCount]);

	[_massList release];
	[_simulation release];
	
	[super dealloc];
}

//----------------------------------------------------------------------------------------
//  windowNibName
//
//      return the nib name for the document.
//
//  returns NSString * <- 
//----------------------------------------------------------------------------------------
- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document 
	// supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"GravityDocument";
}

//----------------------------------------------------------------------------------------
//  windowControllerDidLoadNib:
//
//      initialization after nib file loaded.
//
//  windowControllerDidLoadNib: NSWindowController * aController   -> the controller.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    [super windowControllerDidLoadNib:aController];
	[aController setShouldCloseDocument: YES];
	
	if (_massList == nil)
	{
		_massList = [[NSMutableArray arrayWithCapacity:10] retain];
		[_massList addObject:[ObjMass create]];
		
		// for testing
		ObjMass* m = [ObjMass create];
		[m setLocation: GPoint(5.0, 0.0)];
		[m setDisplaySize: 0.5];
		[_massList addObject: m];
	}
	else
	{
		[aController setWindowFrameAutosaveName:[self displayName]];
	}
		
	_massController = [MassListController createMassPanel:self];
//	[[_massController window] makeKeyAndOrderFront:self];
	[_massController showWindow:self];
}

//----------------------------------------------------------------------------------------
//  view
//
//      return the gravity view for drawing the simulation.
//
//  returns GravityView*   <- 
//----------------------------------------------------------------------------------------
- (GravityView*) view
{
	return _gview;
}

//----------------------------------------------------------------------------------------
//  simulation
//
//      return the simulation for this document.
//
//  returns Simulation*    <- 
//----------------------------------------------------------------------------------------
- (Simulation*) simulation
{
	return _simulation;
}

//----------------------------------------------------------------------------------------
//  running
//
//      return true if the simulation is being computed.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) running
{
	return _simulation != 0 && [_simulation computing];
}

//----------------------------------------------------------------------------------------
//  hasSimulation
//
//      return true if a simulation has been created.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) hasSimulation
{
	return _simulation != 0 && [_simulation hasSimulation];
}

//----------------------------------------------------------------------------------------
//  startSimulation:
//
//      start the simuation.
//
//  startSimulation: id sender -> ignored
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) startSimulation
{
	if (![self running])
	{
		if (_simulation == 0)
		{
			_simulation = [[Simulation createWithDocument:self] retain];
		}
		[_simulation startComputation];
		[_gview startTimer: kComputeUpdateFrequency];
		[_massController dataChanged];
	}
}

//----------------------------------------------------------------------------------------
//  stopSimulation:
//
//      stop the simulation.
//
//  stopSimulation: id sender  -> ignored
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) stopSimulation
{
	if ([self running])
	{
		[_simulation stopComputation];
	//	[_gview stopSimulation];
	//	_running = NO;
		[_gview setNeedsDisplay:YES];
		[_massController dataChanged];
	}
}

//----------------------------------------------------------------------------------------
//  resetSimulation:
//
//      reset the simulation.
//
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) resetSimulation
{
	if (_simulation != 0)
	{
		[_simulation stopComputation];
		[_simulation release];
		_simulation = 0;
	//	[_gview stopSimulation];
	//	_running = NO;
		[_gview setNeedsDisplay:YES];
		[_massController dataChanged];
	}
}

//----------------------------------------------------------------------------------------
//  replaying
//
//      return true if a siimulation is replaying.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) replaying
{
	return _simulation != 0 && [_simulation replay];
}

//----------------------------------------------------------------------------------------
//  inReplay
//
//      return true if the display is showing the replay.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) inReplay
{
	return _simulation != 0 && [_simulation inReplay];
}

//----------------------------------------------------------------------------------------
//  canReplay
//
//      return true if we have data to replay.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL)canReplay
{
	if (_simulation == 0)
		_simulation = [[Simulation createWithDocument:self] retain];
	return [_simulation canReplay];
}

//----------------------------------------------------------------------------------------
//  startReplay
//
//      start the replay using saved data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)startReplay
{
	if (![self replaying])
	{
		if (_simulation == 0)
			_simulation = [[Simulation createWithDocument:self] retain];
		[_simulation startReplay];
		[_gview startTimer: kReplayUpdateFrequency];
		[_massController dataChanged];
	}
}

//----------------------------------------------------------------------------------------
//  resetReplay
//
//      reset the replay to the beginning.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)resetReplay
{
	if (_simulation != 0)
	{
		[_simulation resetReplay];
		[_gview setNeedsDisplay:YES];
		[_massController dataChanged];
	}
}

//----------------------------------------------------------------------------------------
//  stopReplay
//
//      stop the replay.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)stopReplay
{
	if ([self replaying])
	{
		[_simulation stopReplay];
//		[_gview startTimer];
		[_massController dataChanged];
	}
}


//----------------------------------------------------------------------------------------
//  countMasses
//
//      return the number of masses.
//
//  returns unsigned   <- the count.
//----------------------------------------------------------------------------------------
- (unsigned)countMasses
{
	return [_massList count];
}

//----------------------------------------------------------------------------------------
//  getMass:
//
//      return the specified mass object.
//
//  getMass: unsigned which    -> the specified mass.
//
//  returns ObjMass*           <- the mass object.
//----------------------------------------------------------------------------------------
- (ObjMass*)getMass: (unsigned) which
{
	if (which < [_massList count])
		return [_massList objectAtIndex:which];
	return nil;
}

//----------------------------------------------------------------------------------------
//  addMass:
//
//      add a mass object to the list.
//
//  addMass: ObjMass* mass -> the mass to add.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)addMass: (ObjMass*) mass
{
	[self listChanged: YES];
	
	[[[self undoManager]  prepareWithInvocationTarget:self]
			removeMass: [_massList count]];

	[_massList addObject:mass];
}

//----------------------------------------------------------------------------------------
//  removeMass:
//
//      remove the specified mass object from the list.
//
//  removeMass: unsigned which -> the specified mass.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)removeMass: (unsigned) which
{
	[self listChanged: YES];
	
	[[[self undoManager]  prepareWithInvocationTarget:self]
			addMass: [_massList objectAtIndex: which]];
	
	[_massList removeObjectAtIndex:which];
}

//----------------------------------------------------------------------------------------
//  setMass:forMass:
//
//      update the mass value for the specified row.
//
//  setMass: NSNumber* newM    -> 
//  forMass: unsigned index    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setMass: (NSNumber*) newM forMass: (unsigned) index
{
	if (index < [_massList count])
	{
		[self listChanged: YES];
		ObjMass* m = [_massList objectAtIndex: index];
		
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setMass: [NSNumber numberWithDouble: [m mass]] forMass: index];

		[m setMass: [newM doubleValue]];
	}
}

//----------------------------------------------------------------------------------------
//  setX:forMass:
//
//      update the x coordinate for the specified row.
//
//  setX: NSNumber* x          -> 
//  forMass: unsigned index    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setX: (NSNumber*) x forMass: (unsigned) index
{
	if (index < [_massList count])
	{
		[self listChanged: YES];
		ObjMass* m = [_massList objectAtIndex: index];
		
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setX: [NSNumber numberWithDouble: [m location].getX()] forMass: index];

		[m setXCoord: [x doubleValue]];
	}
}

//----------------------------------------------------------------------------------------
//  setY:forMass:
//
//      update the y coordinate for the specified row.
//
//  setY: NSNumber* y          -> 
//  forMass: unsigned index    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setY: (NSNumber*) y forMass: (unsigned) index
{
	if (index < [_massList count])
	{
		[self listChanged: YES];
		ObjMass* m = [_massList objectAtIndex: index];
		
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setY: [NSNumber numberWithDouble: [m location].getY()] forMass: index];

		[m setYCoord: [y doubleValue]];
	}
}

//----------------------------------------------------------------------------------------
//  setVX:forMass:
//
//      update the x velocity for the specified row.
//
//  setVX: NSNumber* vx        -> 
//  forMass: unsigned index    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setVX: (NSNumber*) vx forMass: (unsigned) index
{
	if (index < [_massList count])
	{
		[self listChanged: YES];
		ObjMass* m = [_massList objectAtIndex: index];
		
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setVX: [NSNumber numberWithDouble: [m velocity].getX()] forMass: index];

		[m setVXCoord: [vx doubleValue]];
	}
}

//----------------------------------------------------------------------------------------
//  setVY:forMass:
//
//      update the y velocity for the specified row.
//
//  setVY: NSNumber* vy        -> 
//  forMass: unsigned index    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setVY: (NSNumber*) vy forMass: (unsigned) index
{
	if (index < [_massList count])
	{
		[self listChanged: YES];
		ObjMass* m = [_massList objectAtIndex: index];
		
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setVY: [NSNumber numberWithDouble: [m velocity].getY()] forMass: index];

		[m setVYCoord: [vy doubleValue]];
	}
}

//----------------------------------------------------------------------------------------
//  setDisplaySize:forMass:
//
//      update the display size for the specified row.
//
//  setDisplaySize: NSNumber* size -> 
//  forMass: unsigned index        -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setDisplaySize: (NSNumber*) size forMass: (unsigned) index
{
	if (index < [_massList count])
	{
		ObjMass* m = [_massList objectAtIndex: index];
		
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setDisplaySize: [NSNumber numberWithInt: [m displaySize]] forMass: index];

		[m setDisplaySize: [size intValue]];
		[self listChanged: NO];
	}
}

//----------------------------------------------------------------------------------------
//  setTailLength:forMass:
//
//      update the number of points in the tail for the specified row.
//
//  setTailLength: NSNumber* size  -> 
//  forMass: unsigned index        -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setTailLength: (NSNumber*) size forMass: (unsigned) index
{
	if (index < [_massList count])
	{
		ObjMass* m = [_massList objectAtIndex: index];
		
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setTailLength: [NSNumber numberWithInt: [m displaySize]] forMass: index];

		[m setTailLength: [size intValue]];
		
		[self listChanged: NO];
	}
}

//----------------------------------------------------------------------------------------
//  setColor:forMass:
//
//      update the color assigned to the specified row.
//
//  setColor: NSColor* color   -> 
//  forMass: unsigned index    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setColor: (NSColor*) color forMass: (unsigned) index
{
	if (index < [_massList count])
	{
		ObjMass* m = [_massList objectAtIndex: index];
		
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setColor: [m color] forMass: index];

		[m setColor: color];
		[self listChanged: NO];
	}
}

//----------------------------------------------------------------------------------------
//  setScale:
//
//      set the scale for the display.
//
//  setScale: double scale -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setScale: (double) scale
{
	if (scale > 1) {
		scale = double(int(scale));
	}
	
	if (scale != _viewScale)
	{
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setScale: _viewScale];

		_viewScale = scale;
		
		if (_simulation != 0)
			[_simulation resetCurves];
		[_gview adjustBounds];
		[self listChanged: NO];
	}
}

//----------------------------------------------------------------------------------------
//  scale
//
//      return the scale for the display.
//
//  returns double <- 
//----------------------------------------------------------------------------------------
- (double)scale
{
	return _viewScale;
}

//----------------------------------------------------------------------------------------
//  setDisplayStepSize:
//
//      set the step size for the display.
//
//  setDisplayStepSize: double stepSize    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setDisplayStepSize: (double) stepSize
{
	if (stepSize > 2) {
		stepSize = double(int(stepSize));
	}
	
	if (stepSize != _displayStepSize)
	{
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setDisplayStepSize: _displayStepSize];

		_displayStepSize = stepSize;
		[self listChanged: NO];
	}
}

//----------------------------------------------------------------------------------------
//  displayStepSize
//
//      return the step size for the display.
//
//  returns double <- 
//----------------------------------------------------------------------------------------
- (double)displayStepSize
{
	return _displayStepSize;
}

//----------------------------------------------------------------------------------------
//  setSaveStepSize:
//
//      set the step size for the output file.
//
//  setSaveStepSize: double stepSize    ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setSaveStepSize: (double) stepSize
{
	if (stepSize != _saveStepSize)
	{
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setSaveStepSize: _saveStepSize];

		_saveStepSize = stepSize;
		[self listChanged: NO];
	}
}

//----------------------------------------------------------------------------------------
//  saveStepSize
//
//      return the time increment to write out to the file.
//
//  returns double <- 
//----------------------------------------------------------------------------------------
- (double)saveStepSize
{
	return _saveStepSize;
}

//----------------------------------------------------------------------------------------
//  setCurrentStep:
//
//      set the current time in the simulation for display in the controller.
//
//  setCurrentStep: double step    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setCurrentStep: (double) step
{
	[_massController setCurrentStep: step];
}

//----------------------------------------------------------------------------------------
//  currentPath
//
//      return the current path to the simulation data file.
//
//  returns NSString*  <-
//----------------------------------------------------------------------------------------
- (NSString*) currentPath
{
	// if we saved the file, we should re-figure the current solution path.
	if (!_fileSaved && _currentPath)
		return _currentPath;
	
	NSString* thePath = 0;
	BOOL isTempPath = false;
	
	NSURL* path = [self fileURL];
	
	if (path && [path isFileURL]) {
		thePath = [[path path] stringByDeletingPathExtension];
	} else {
		thePath = @"/tmp/GravityView/simulation";
		isTempPath = _simulation != nil;
	}
	
	thePath = [thePath stringByAppendingPathExtension:@"out"];
	
	// create the directory for the file, if it does not exist.
	NSString* theDir = [thePath stringByDeletingLastPathComponent];
	
	NSFileManager* mgr = [NSFileManager defaultManager];
	// check if the new directory exists.
	if (![mgr fileExistsAtPath: theDir])
	{
		NSError* err = 0;
		
		// try to create the directories.
		if (![mgr createDirectoryAtPath: theDir
								  withIntermediateDirectories: YES
												   attributes: nil
														error: &err])
		{
			thePath = nil;
			NSLog(@"Failed to create directory %@ because %@", theDir, err);
		}
	}
	
	// if we changed the path, attempt to copy the existing solution to the new path.
	if (![thePath isEqualTo: _currentPath]) {
		// if there is an iter at the old path, attempt to copy it
		// to the new path.
		if ([mgr fileExistsAtPath: _currentPath]) {
			NSError* err;
			[mgr copyItemAtPath:_currentPath toPath: thePath error: &err];
			if (err) {
				NSLog(@"error copying output file: %@", err);
			}
		}
	}
	
	if (_currentPath)
		[_currentPath release];
	_currentPath = [thePath retain];
	_fileSaved = false;
	
	return thePath;
}

//----------------------------------------------------------------------------------------
//  setDrawPolys:
//
//      set true to draw body tails in the simulation.
//
//  setDrawPolys: BOOL draw    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setDrawPolys: (BOOL) draw
{
	_drawPolys = draw;
	if (_simulation)
		[_simulation setDrawPolys:draw];
}

//----------------------------------------------------------------------------------------
//  drawPolys
//
//      return true if tails should be drawn.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) drawPolys
{
	return _drawPolys; //_drawPolys;
}

//----------------------------------------------------------------------------------------
//  originIndex
//
//      return the origin index for the simulation. -1 is center of mass.
//
//  returns int    <- 
//----------------------------------------------------------------------------------------
- (int) originIndex
{
	return _originIndex;
}

//----------------------------------------------------------------------------------------
//  setOriginIndex:
//
//      specify the origin index for the display.
//
//  setOriginIndex: int index  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setOriginIndex: (int) index
{
	if (index >= [self countMasses])
		index = -1;
		
	if (index != _originIndex)
	{
		[[[self undoManager]  prepareWithInvocationTarget:self]
			setOriginIndex: _originIndex];
		
		_originIndex = index;
		[self listChanged: NO];
		[_simulation updateOrigin];
	}
}


//----------------------------------------------------------------------------------------
//  listChanged
//
//      Inform the document that the list has changed.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)listChanged: (BOOL) doStopSim
{
	if (doStopSim) {
		[self stopSimulation];
		[_simulation updateOrigin];
	}
	
	[_gview setNeedsDisplay: YES];
	[_massController dataChanged];
//	[self updateChangeCount:NSChangeDone];
}

//----------------------------------------------------------------------------------------
//  setNoMassController
//
//      set mass controller to nil when the mass controller is destroyed.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setNoMassController
{
	_massController = nil;
}

//----------------------------------------------------------------------------------------
//  dataOfType:
//
//      return the data for the document for save.
//
//  dataOfType: NSString * aType -> the data type
//  error: NSError ** outError   <- returned error
//
//  returns NSData *                           <- the data for this type
//----------------------------------------------------------------------------------------
- (NSData *)dataOfType: (NSString *)aType error: (NSError **)outError
{
	NSData* theData = nil;
	
	[self stopSimulation];
	
	if ([aType isEqualToString:@"DocumentType"])
		theData = [NSKeyedArchiver archivedDataWithRootObject:_massList];
	else
	{
		NSMutableData* data = [NSMutableData data];
		NSKeyedArchiver* a = [[NSKeyedArchiver alloc] initForWritingWithMutableData:data];
		[a setOutputFormat: NSPropertyListXMLFormat_v1_0];
		[a encodeDouble:_displayStepSize forKey:@"_stepSize"];
		[a encodeDouble:_viewScale forKey:@"_viewScale"];
		[a encodeObject:_massList forKey:@"_massList"];
		[a encodeObject:_currentPath forKey:@"_path"];
		[a encodeInt:_originIndex forKey:@"_index"];
		[a encodeDouble:_saveStepSize forKey:@"_saveStepSize"];
		[a finishEncoding];
		[a release];
		theData = data;
	}

	_fileSaved = YES;
	
//	NSURL* path = [self fileURL];
//	NSLog(@"The path at save is %@", [path absoluteString]);
	
    // Insert code here to write your document from the given data.  
	// You can also choose to override -fileWrapperRepresentationOfType: or -writeToFile:ofType: instead.
    
    // For applications targeted for Tiger or later systems, you should use the 
	// new Tiger API -dataOfType:error:.  In this case you can also choose to 
	// override -writeToURL:ofType:error:, -fileWrapperOfType:error:, or 
	// -writeToURL:ofType:forSaveOperation:originalContentsURL:error: instead.

    return theData;
}

//----------------------------------------------------------------------------------------
//  loadDataRepresentation:ofType:
//
//      create the mass list from saved data.
//
//  loadDataRepresentation: NSData * data  -> the saved data
//  ofType: NSString * aType               -> the data type
//  error: NSError ** outError             <- returned error
//
//  returns BOOL                           <- true if we could interpret this data.
//----------------------------------------------------------------------------------------
- (BOOL)readFromData:(NSData *)data ofType:(NSString *)aType error:(NSError**) err
{
	NSString* path = nil;
	
	fprintf(stderr, "the type is %s\n", [aType UTF8String]);
	if ([aType isEqualToString:@"DocumentType"])
	{
		_massList = [[NSKeyedUnarchiver unarchiveObjectWithData:data] retain];
	}
	else
	{
		NSKeyedUnarchiver* u = [[NSKeyedUnarchiver alloc] initForReadingWithData:data];
		_displayStepSize = [u decodeDoubleForKey:@"_stepSize"];
		_viewScale = [u decodeDoubleForKey:@"_viewScale"];
		_massList = [[u decodeObjectForKey:@"_massList"] retain];
		if ([u containsValueForKey:@"_path"])
		{
			path = [u decodeObjectForKey:@"_path"];
			
			if ([u containsValueForKey:@"_index"])
			{
				_originIndex = [u decodeIntForKey:@"_index"];
				
				if ([u containsValueForKey:@"_saveStepSize"])
				{
					_saveStepSize = [u decodeDoubleForKey:@"_saveStepSize"];
				}
			}
		}
		[u finishDecoding];
		[u release];
		//fprintf(stderr, "rc = %ld\n", [_massList retainCount]);
	}
	
	[[_massController window] setViewsNeedDisplay: YES];
//	if (path) {
//		_currentPath = [path retain];
//	}
	
    // For applications targeted for Tiger or later systems, you should use the new Tiger API 
	//	readFromData:ofType:error:.  In this case you can also choose to override 
	// -readFromURL:ofType:error: or -readFromFileWrapper:ofType:error: instead.
    
    return YES;
}

//----------------------------------------------------------------------------------------
//  numberOfRowsInTableView:
//
//      return the number of table rows.
//
//  numberOfRowsInTableView: NSTableView * tableView   -> the view
//
//  returns int                                        <- the rows
//----------------------------------------------------------------------------------------
- (int) numberOfRowsInTableView:(NSTableView *)tableView
{
	return [self countMasses];
}

//----------------------------------------------------------------------------------------
//  tableView:objectValueForTableColumn:row:
//
//      return the value for the specified table cell.
//		NOTE: the column identifiers are set in the nib file.
//
//  tableView: NSTableView * tableView                     -> the view
//  objectValueForTableColumn: NSTableColumn * tableColumn -> the column
//  row: int row                                           -> the row
//
//  returns id                                             <- the object.
//----------------------------------------------------------------------------------------
- (id) tableView: (NSTableView *)tableView objectValueForTableColumn: (NSTableColumn *)tableColumn row:(int)row
{
	int val = [[tableColumn identifier] intValue];
	ObjMass* m = [self getMass: row];
	switch(val)
	{
		case 0:
			return [NSNumber numberWithDouble:[m mass]];
		case 1:
			return [NSNumber numberWithDouble:[m location].getX()];
		case 2:
			return [NSNumber numberWithDouble:[m location].getY()];
		case 3:
			return [NSNumber numberWithDouble:[m velocity].getX()];
		case 4:
			return [NSNumber numberWithDouble:[m velocity].getY()];
		case 5:
			return [NSNumber numberWithInt:[m displaySize]];
		case 6:
			return [m color];
		case 7:
			return [NSNumber numberWithInt:[m tailLength]];
	}
	return nil;
}

//----------------------------------------------------------------------------------------
//  tableView:setObjectValue:forTableColumn:row:
//
//      set the underlying value for the specified table cell.
//
//  tableView: NSTableView * tableView             -> the table
//  setObjectValue: id object                      -> the changed value object
//  forTableColumn: NSTableColumn * tableColumn    -> the table column.
//  row: int row                                   -> the row
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)tableView:(NSTableView *)tableView 
		setObjectValue:(id)object 
		forTableColumn:(NSTableColumn *)tableColumn 
		row:(int)row
{
	int val = [[tableColumn identifier] intValue];
		
//	[self stopSimulation];
	
	switch(val)
	{
		case 0:
			[self setMass: object forMass: row];
			break;
		case 1:
			[self setX: object forMass: row];
			break;
		case 2:
			[self setY: object forMass: row];
			break;
		case 3:
			[self setVX: object forMass: row];
			break;
		case 4:
			[self setVY: object forMass: row];
			break;
		case 5:
			[self setDisplaySize: object forMass: row];
			break;
		case 6:
			[self setColor: object forMass: row];
			break;
		case 7:
			[self setTailLength: object forMass: row];
		default:
			return;
	}	
}

//----------------------------------------------------------------------------------------
//  keyDown:
//
//      handle key events.
//
//  keyDown: NSEvent* theEvent -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) keyDown:(NSEvent*) theEvent
{
	NSString* str = [theEvent characters];
	if ([str length] > 0)
	{
		if ([str characterAtIndex:0] == '}')
		{
			[self setScale: _viewScale / 2];
		}
		else if ([str characterAtIndex:0] == '{')
		{
			[self setScale: _viewScale * 2];
		}
		else if ([str characterAtIndex:0] == '+')
		{
			[self setDisplayStepSize: _displayStepSize * 2];
		}
		else if ([str characterAtIndex:0] == '-')
		{
			[self setDisplayStepSize: _displayStepSize / 2];
		}
	}
}

//----------------------------------------------------------------------------------------
//  windowDidResignMain:
//
//      order the controller window off screen when the main window goes.
//
//  windowDidResignMain: NSNotification * aNotification    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)windowDidResignMain:(NSNotification *)aNotification
{
	if (_massController)
		[[_massController window] orderOut: self];
}

//----------------------------------------------------------------------------------------
//  windowDidBecomeMain:
//
//      order the controller window on when the associated window becomes front.
//
//  windowDidBecomeMain: NSNotification * aNotification    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)windowDidBecomeMain:(NSNotification *)aNotification
{
	if (_massController)
		[[_massController window] orderFront: self];
}


@end
