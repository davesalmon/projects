/*+
 *
 *  ElementInspectorTree.h
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  handle the element inspector tree data.
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

#import <Foundation/Foundation.h>
#import "InspectorTree.h"

@class FrameDocument;
@protocol InspectorOutlineItem;

@interface ElementInspectorTree : InspectorTree

+ (ElementInspectorTree*) createWithDocument: (FrameDocument*) doc;

- (instancetype) initWithDocument: (FrameDocument*) doc;

- (id<InspectorOutlineItem>) createItemForIndex: (DlInt32)index andDocument: (FrameDocument*) doc;

- (DlInt32) itemCountForDocument: (FrameDocument*) doc;

@end
