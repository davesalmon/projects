/*+
 *
 *  FrameAction.h
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Encapsulate Action C++ objects in objective C class.
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

class Action;
class FrameStructure;

@interface FrameAction : NSObject {
	Action* _action;
	NSView* _view;
	bool	_clearSelection;
	bool	_checkDuplicates;
//	bool	_savePasteOffset;
	bool	_updateLoadCase;
}

+ (FrameAction*) create: (Action*)action;
+ (FrameAction*) create: (Action*)action withView: (NSView*)vw;
+ (FrameAction*) createClearingSelection: (Action*)action;
+ (FrameAction*) createClearingSelection: (Action*)action withView: (NSView*)vw;
+ (FrameAction*) createUpdatingLoadCase: (Action*)action;

- (id) init: (Action*)action withView: (NSView*)vw clearSelection: (bool)clr;

- (void) setCheckDuplicates: (bool) doCheck;
//- (void) setSavePasteOffset: (bool) doSave;

- (bool) perform : (FrameStructure*) structure;

- (bool) clearSelection;
- (bool) canUndo;
- (bool) checkDuplicates;
- (bool) updateLoadCaseMenu;
//- (bool) savePasteOffset;

- (NSString*) title;

- (NSString*) description;

@end
