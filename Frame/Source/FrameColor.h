/*+
 *
 *  FrameColor.h
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  Define the colors used by the application. This might eventually
 *  be used by preferences to allow users to select colors.
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

#include "Element.h"
#include "Node.h"

@class FrameDocument;

@interface FrameColor : NSObject

// colors
+ (DlUInt32) colorForElement: (Element)e inDocument: (FrameDocument*) s;
+ (DlUInt32) colorForNode: (Node)n inDocument: (FrameDocument*) s;

+ (DlUInt32) axialColor;
+ (DlUInt32) momentColor;
+ (DlUInt32) shearColor;
+ (DlUInt32) displacementColor;
+ (DlUInt32) lateralLoadColor;
+ (DlUInt32) axialLoadColor;

@end
