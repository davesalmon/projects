/*+
 *
 *  UnitsFormatter.m
 *
 *  Copyright © 2007 David C. Salmon. All Rights Reserved.
 *
 *  format units.
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

#import "UnitsFormatter.h"
#import "FrameDocument.h"

@implementation UnitsFormatter

//----------------------------------------------------------------------------------------
//  formatterWithDocument:units:precision:showingTag:
//
//      create a formatter.
//
//  formatterWithDocument: FrameDocument* doc  -> the frame doc.
//  units: UnitType u                          -> the type of units for this formatter.
//  precision: int p                           -> the number of digits after decimal.
//  showingTag: bool show                      -> true to show the units in string.
//
//  returns UnitsFormatter*                    <- this
//----------------------------------------------------------------------------------------
+ (UnitsFormatter*) formatterWithDocument: (FrameDocument*) doc 
								 units: (UnitType) u 
                             precision: (int) p
                            showingTag: (bool) show
{
    UnitsFormatter* theFormatter = [[[UnitsFormatter alloc] 
        	initWithDocument:doc units:u precision: p showingTag: show] autorelease];
    
    return theFormatter;
}

//----------------------------------------------------------------------------------------
//  initWithDocument:units:precision:showingTag:
//
//      initialize a formatter.
//
//  initWithDocument: FrameDocument* doc   -> the frame doc.
//  units: UnitType u                      -> the type of units for this formatter.
//  precision: int p                       -> the number of digits after decimal.
//  showingTag: bool show                  -> true to show the units in string.
//
//  returns id                             <- self
//----------------------------------------------------------------------------------------
- (id) initWithDocument: (FrameDocument*) doc 
				  units: (UnitType) u 
              precision: (int) p
             showingTag: (bool) show
{
	_document = doc;
    _units = u;
    _precision = p;
    _showTag = show;
    
    return self;
}

//----------------------------------------------------------------------------------------
//  stringForObjectValue:
//
//      return the string for the specified object.
//
//  stringForObjectValue: id obj   -> the object (with double value)
//
//  returns NSString *             <- the formatted string.
//----------------------------------------------------------------------------------------
- (NSString *)stringForObjectValue:(id)obj
{
	double val = [obj doubleValue];
    DlString s = UnitTable::FormatValue(val, _units, DlFloatFormat(_precision), _showTag);
    return [NSString stringWithCString: s.get()
							 encoding : NSUTF8StringEncoding];
}

//----------------------------------------------------------------------------------------
//  getObjectValue:forString:errorDescription:
//
//      return an object for the specified string. The string can have any units string
//		known to the application applied. The result will be a value in "_units" if
//		possible. If the units cannot be converted, and error is raised.
//
//  getObjectValue: id * obj               -> the output value.
//  forString: NSString * string           -> the string (with units)
//  errorDescription: NSString ** error    -> any errors.
//
//  returns BOOL                           <- true if converted.
//----------------------------------------------------------------------------------------
- (BOOL)getObjectValue:(id *)obj forString:(NSString *)string 
			errorDescription:(NSString **)error
{
	try
    {
		double value = UnitTable::ParseValue([string UTF8String], _units);
    	*obj = [NSNumber numberWithDouble:value];
    	return YES;
    }
    catch(...)
    {
    	if (error != nil)
    		*error = @"Conversion failed";
        return NO;
    }
}

@end

// eof

