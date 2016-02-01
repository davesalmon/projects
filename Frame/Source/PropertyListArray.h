/*+
 *
 *  PropertyListArray.mm
 *
 *  Copyright © 2005 David C. Salmon. All Rights Reserved.
 *
 *  Implement data access class for property list.
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
#include "PropertyEnumerator.h"
#include "PropertyTypeEnumerator.h"

@class FrameDocument;

@interface PropertyListArray : NSObject
{
	FrameDocument* _frameDocument;
	PropertyEnumerator* _props;
}

+ (PropertyListArray*)createWithDocument: (FrameDocument*)doc;
- (id) initWithDocument: (FrameDocument*)doc;

- (PropertyTypeEnumerator) properties;

- (int) numberOfRows;

- (id) getItemAtRow: (int)row column: (int)col;
- (bool) setItem: (id)item forRow: (int)row column:(int)col;

- (bool) canEditItemAtRow: (int)row column: (int)col;

- (void) attachRow: (int)row;
- (void) detach;

- (void) selectRow: (int)row;
- (bool) hasSelection;

- (void) createElementWithValue: (id)item forColumn: (int) col;

- (bool) canAttach;
- (Property) canDelete; // return the active property if it can be deleted

- (FrameDocument*) frameDocument;

@end

// eof
