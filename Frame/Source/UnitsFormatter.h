/*+
 *
 *  UnitsFormatter.h
 *
 *  Copyright © 2007 David C. Salmon. All Rights Reserved.
 *
 *  NSFormatter subclass to format values containing units.
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

//
//  UnitsFormatter.h
//  Frame
//
//  Created by David Salmon on 8/12/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "UnitTable.h"

@class FrameDocument;

@interface UnitsFormatter : NSFormatter {
	FrameDocument* _document;
    UnitType _units;
    int _precision;
    bool _showTag;
}

+ (UnitsFormatter*) formatterWithDocument: (FrameDocument*) doc 
								 units: (UnitType) u 
                             precision: (int) p
                            showingTag: (bool) show;

- (id) initWithDocument: (FrameDocument*) doc 
                  units: (UnitType) u 
              precision: (int) p
             showingTag: (bool) show;

- (NSString *)stringForObjectValue:(id)obj;
- (BOOL)getObjectValue:(id *)obj forString:(NSString *)string errorDescription:(NSString **)error;


@end

// eof
