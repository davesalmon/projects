/*+
 *
 *  TableColumn.m
 *
 *  Copyright © 2005 David C. Salmon. All Rights Reserved.
 *
 *  class to construct table columns for the property editor. The
 *  property type is used to determine the type of NSCell to use
 *  for that column in the table.
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

#import "TableColumn.h"
#include "Property.h"

@implementation TableColumn

+ (TableColumn*) create: (NSString*)title editable: (BOOL)edit dataCell:(NSCell*)cell
{
	TableColumn* col = [[TableColumn alloc] autorelease];
	col->_editable = edit;
	col->_title = title;
	col->_dataCell = cell;
	return col;
}

+ (TableColumn*) createWithPropertyType: (const PropertyType*) prop 
{
	NSString* base = [NSString stringWithCString: prop->name encoding : NSUTF8StringEncoding];
	NSString* columnTitle = NSLocalizedString(base, nil);
	
	NSCell* theCell = nil;
	
	switch (prop->dataType) {
	case PropDataFloat:
	case PropDataInt:
//		theCell = [[NSTextFieldCell alloc] init];
		break;
	case PropDataBool: {
		NSButtonCell* cell = [[NSButtonCell alloc] init];
		
		[cell setTitle: NSLocalizedString(@"Pinned", nil)];
		[cell setButtonType: NSSwitchButton];
		[cell setControlSize: NSSmallControlSize];
		[cell setFont: [NSFont systemFontOfSize: 10]]; 

		theCell = cell;
		} break;
	}
	return [TableColumn create:columnTitle editable:YES dataCell:theCell];
}

- (BOOL) editable {
	return _editable;
}

- (NSString*) title {
	return _title;
}

- (NSCell*) dataCell {
	return _dataCell;
}

+ (void) initializeTable: (NSTableView*)table
				withList: (PropertyTypeEnumerator) props
			   editiable: (bool)edit
				   align: (NSTextAlignment) align
{
	[table setColumnAutoresizingStyle:NSTableViewNoColumnAutoresizing];
	
	int numCols = [table numberOfColumns];
	NSArray* cols = [table tableColumns];
    
    NSFont* theFont;
	NSFont* headerFont;
	
    if (numCols > 0) {
    	theFont = [[[cols objectAtIndex: 0] dataCell] font];
		headerFont = [[[cols objectAtIndex: 0] headerCell] font];
	} else {
        theFont = [NSFont systemFontOfSize: [NSFont smallSystemFontSize]];
		headerFont = [NSFont boldSystemFontOfSize: [NSFont smallSystemFontSize]];
	}

	int reqCols = props.Length() + 1;
    //	NSAssert1(reqCols == , @"#elements does not match #titles for %@", _listName);
	
//	int totalWidth = 0;
	
	for (int i = 0; i < reqCols; ++i) {

		NSTableColumn* theCol;
		
		if (i >= numCols) {
			
			theCol = [[NSTableColumn alloc] initWithIdentifier:
					  [[NSNumber numberWithInt:i] stringValue]];

			[[theCol headerCell] setFont: headerFont];
			
			[table addTableColumn:theCol];
			[theCol release];

			TableColumn* header = [TableColumn createWithPropertyType: props.At(i - numCols)];
			[[theCol headerCell] setStringValue: [header title]];
			
			if ([header dataCell] != nil) {
				[theCol setDataCell: [header dataCell]];
//				[theCol sizeToFit];
			} else {
				[theCol setMinWidth: 76];
				[theCol setWidth: 76];
				
				[[theCol dataCell] setFont: theFont];
				if (align)
					[[theCol dataCell] setAlignment:NSRightTextAlignment];
			}
			
			int headWidth = [[theCol headerCell] cellSize].width;
			
			if (headWidth > [theCol width]) {
				[theCol setWidth: headWidth];
			}

		} else {
			theCol = [cols objectAtIndex: i];
		}
		
		[theCol setEditable: edit];
	}
	
//	printf("The total width of property pane is %d\n", totalWidth);

//	printf("Row width is %.1f\n", [table rectOfRow:0].size.width);
//	printf("SuperView width is %.1f\n", [[table superview] frame].size.width);
	
	// fix the window size
	int diff = [table rectOfRow:0].size.width - [[table superview] frame].size.width;
	if (diff > 0) {
		NSRect newFrame = table.window.frame;
		newFrame.size.width += diff;
		[[table window] setFrame: newFrame display: YES];
	}
}

@end
