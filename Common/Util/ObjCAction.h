//
//  ObjCAction.h
//
//  Created by David Salmon on Fri Nov 29 2002.
//  Copyright (c) 2002 David C. Salmon. All rights reserved.
//
//	Interface for DlAction to Cocoa classes.
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

#import <Foundation/Foundation.h>

class DlAction;

@interface ObjCAction : NSObject {
	DlAction* _action;
}

+ (ObjCAction*)create:(DlAction*) action;

// return a string from the actions.strings file.
+ (NSString*) actionString: (const char*) key;

- init:(DlAction*)action;

- (void)perform;

- (bool)canUndo;

// Title requires an action.strings resource where localizable
//	strings are stored. The keys for these strings are the _name
//	field of the DlAction classes.
// A "NoUndo" key should be present for operations that cannot be
//	undone.
- (NSString*) title;

@end
