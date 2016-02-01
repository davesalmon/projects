/*+
 *
 *  FrameClipboard.h
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  Define the structure clipboard.
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

#import <Cocoa/Cocoa.h>
#include "FrameStructure.h"
#include "WorldRect.h"

@class FrameDocument;

@interface FrameClipboard : NSObject

// create and delete the clipboard.
+ (FrameClipboard*)sharedClipboard;
+ (void) deleteClipboard;

// copy the specified nodes and elements to the clipboard.
- (void) setNodes: (const NodeEnumerator&)nodes
	  andElements: (const ElementEnumerator&)elems
	  forDocument: (FrameDocument*) doc;

// return the document the clip data was take from
- (FrameDocument*) document;

// return true if clipboard is empty.
- (bool) empty;

// clear the clipboard.
- (void) clear;

//	get the contents
- (const FrameStructure*) contents;

//- (bool) isDeepCopy;

// reture true if the paste offset results in a location inside the world.
- (bool) validPasteOffset: (const WorldPoint&)offset forWorld: (const WorldRect&) world;

//	respond to external request for data
- (void)pasteboard:(NSPasteboard *)sender provideDataForType:(NSString *)type;

@end
