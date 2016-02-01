/*+
 *
 *  FrameClipboard.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  Handle the structure clipboard.
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

#import "FrameClipboard.h"
#include "FrameIO.h"

@interface FrameClipboard ()
{
	std::unique_ptr<FrameStructure> 	_structure;
	
	FrameDocument*		_document;
	WorldRect			_bounds;
	
	NSInteger			_lastCount;
}

@end


@implementation FrameClipboard

static FrameClipboard* sClip = nullptr;

static NSString* kPasteBoardType = @"dcs.Plane-Frame-Data";

//----------------------------------------------------------------------------------------
//  sharedClipboard
//
//      return the one and only clipboard.
//
//  returns FrameClipboard*    <- 
//----------------------------------------------------------------------------------------
+ (FrameClipboard*)sharedClipboard 
{
	if (!sClip) {
		sClip = [[FrameClipboard allocWithZone:[self zone]] init];
	}
	return sClip;
}

//----------------------------------------------------------------------------------------
//  deleteClipboard
//
//      delete the clipboard data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
+ (void) deleteClipboard 
{
	if (sClip)
		[sClip release];
	sClip = nullptr;
}

//----------------------------------------------------------------------------------------
//  setNodes:andElements:forDocument:
//
//      update the clipboard with the selection.
//
//  setNodes: const NodeEnumerator& nodes          -> 
//  andElements: const ElementEnumerator& elems    -> 
//  forDocument: FrameDocument* doc                -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setNodes: (const NodeEnumerator&)nodes
	  andElements: (const ElementEnumerator&)elems
	  forDocument: (FrameDocument*) doc
{
	_document = doc;

	// make a deep copy.
	_structure.reset(NEW FrameStructure(nodes, elems, true));
	_bounds = _structure->GetBounds();

	// and set the global clipboard for data
	NSPasteboard* pb = [NSPasteboard generalPasteboard];
	NSArray* arr = [NSArray arrayWithObject: kPasteBoardType];
//	[pb clearContents];
	[pb declareTypes: arr owner: self];
	
	_lastCount = [pb changeCount];
#if DlDebugging
	NSLog(@"pasteboard has types %@", [pb types]);
#endif
}

//----------------------------------------------------------------------------------------
//  document
//
//      return the document where the nodes and elements came from.
//
//  returns FrameDocument* <- 
//----------------------------------------------------------------------------------------
- (FrameDocument*) document {
	return _document;
}

//----------------------------------------------------------------------------------------
//  empty
//
//      return true if the clipboard is empty.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) empty {
	NSArray* types = [[NSPasteboard generalPasteboard] types];
#if DlDebugging
	NSLog(@"pasteboard has types %@", [[NSPasteboard generalPasteboard] types]);
#endif
	return ![types containsObject: kPasteBoardType];
}

//----------------------------------------------------------------------------------------
//  clear
//
//      clear the clipboard.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) clear {
	// the document is being closed.
	NSPasteboard* pb = [NSPasteboard generalPasteboard];
	if ([pb canReadItemWithDataConformingToTypes:[NSArray arrayWithObject: kPasteBoardType]]) {
		[self pasteboard: pb provideDataForType: kPasteBoardType];
	}
}

//----------------------------------------------------------------------------------------
//  contents
//
//      return the contents of the clipbaord.
//
//  returns const FrameStructure*  <- 
//----------------------------------------------------------------------------------------
- (const FrameStructure*) contents
{
	NSLog(@"pasteboard has types %@", [[NSPasteboard generalPasteboard] types]);
	
	NSPasteboard* pb = [NSPasteboard generalPasteboard];

	// if there is no structure, or if the structure is different, construct
	//	from the pasteboard.
	if (!_structure || [pb changeCount] != _lastCount) {
	
		NSData* data = [pb dataForType: kPasteBoardType];
		if (data != nil) {
			FrameReader reader(data);
			_structure.reset(
				NEW FrameStructure([NSLocalizedString(@"default", nil) UTF8String]));
			_structure->Load(reader);
			
			_bounds = _structure->GetBounds();
			
#if DlDebugging
			_structure->DebugPrint();
#endif
		}
	}

	return _structure.get();
}

//----------------------------------------------------------------------------------------
//  validPasteOffset:forWorld:
//
//      return true if the specified offset results in the copied elements residing inside
//      the current world.
//
//  validPasteOffset: const WorldPoint& offset -> the proposed offset
//  forWorld: const WorldRect& world           -> the world.
//
//  returns bool                               <- true if valid
//----------------------------------------------------------------------------------------
- (bool) validPasteOffset: (const WorldPoint&)offset forWorld: (const WorldRect&) world
{
	// check if the top right is in the world
	if (!world.contains({ offset + _bounds.topRight() }))
		return false;

	// and if bottom left is too.
	if (!world.contains({ offset + _bounds.bottomLeft() }))
		return false;

	return true;
}

//----------------------------------------------------------------------------------------
//  pasteboard:provideDataForType:
//
//      provide data to the pastboard for the specified type.
//
//  pasteboard: NSPasteboard * sender      -> the pasteboard
//  provideDataForType: NSString * type    -> the type desired.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)pasteboard:(NSPasteboard *)sender provideDataForType:(NSString *)type
{
	if ([type isEqualToString: kPasteBoardType]) {
#if DlDebugging
		NSLog(@"Adding data of type %@ to pasteboard", type);
#endif
		_structure->DebugPrint();

		FrameWriter writer(0);
		//	document data
		_structure->Save(writer);
		
		// clear the existing structure.
		_structure.reset();
		
		[sender setData: writer.GetBytes() forType: type];
	} else {
		NSLog(@"pasteboard request for data of unsupported type %@", type);
	}
}



@end