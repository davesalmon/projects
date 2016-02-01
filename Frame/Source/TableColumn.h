/*+
 *
 *  TableColumn.h
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

#import <Cocoa/Cocoa.h>

class PropertyType;
class PropertyTypeEnumerator;

@interface TableColumn : NSObject 
{
@private
	BOOL _editable;
	NSString* _title;
	NSCell* _dataCell;
}

+ (TableColumn*) create: (NSString*) title editable: (BOOL) edit dataCell: (NSCell*) cell;
+ (TableColumn*) createWithPropertyType: (const PropertyType*) prop;

+ (void) initializeTable: (NSTableView*)table
				withList: (PropertyTypeEnumerator) props
			   editiable: (bool)edit
				   align: (NSTextAlignment) align;

- (BOOL) editable;
- (NSString*) title;
- (NSCell*) dataCell;

//- (BOOL) isEqual: (id) anObject;
//- (unsigned) hash;

@end

