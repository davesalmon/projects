/*+
 *
 *  FrameColor.mm
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  handle color assignments for frame app.
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

#import "FrameColor.h"
#import "FrameDocument.h"

#include "FrameStructure.h"
#include "graphics.h"

typedef struct PreferenceColors {
	DlUInt32			_axialColor;
	DlUInt32			_shearColor;
	DlUInt32			_momentColor;
	DlUInt32			_displacementColor;
	DlUInt32			_elementColor;
	DlUInt32			_elementSelectionColor;
	DlUInt32			_nodeColor;
	DlUInt32			_nodeSelectionColor;
	DlUInt32			_lateralLoadColor;
	DlUInt32			_axialLoadColor;
	DlUInt32			_activeColor;
}	PreferenceColors;

static PreferenceColors kDefaultColors = {
	graphics::makeColor(0x88, 0x00, 0x00)	//0xff880000	//	axial
	, graphics::makeColor(0x00, 0x88, 0x00)	//0xff008800	// shear
	, graphics::makeColor(0x00, 0x00, 0xaa)	//0xff0000aa	// moment
	, graphics::makeColor(0x88, 0x88, 0x88, 0x88)	//0x88888888	// displacement
	, graphics::makeColor(0x00, 0x00, 0x00)	// element
	, graphics::makeColor(0x77, 0xaa, 0x77)	// 0xff77aa77	// selected elem
	, graphics::makeColor(0xff, 0xff, 0xff)	// 0xffffffff	// node
	, graphics::makeColor(0xff, 0x88, 0x88)	// 0xffff8888	// selected node
	, graphics::makeColor(0x88, 0x88, 0x88) // 0xff888888	// lateral load
	, graphics::makeColor(0xff, 0x88, 0x88)	//0xffff8888	// axial load
};

constexpr DlUInt32 kYellowColor { graphics::makeColor(0xff, 0xff, 0x00) };
constexpr DlUInt32 kTealColor { graphics::makeColor(0x22, 0x77, 0x55) }; //0x6644

@implementation FrameColor

//----------------------------------------------------------------------------------------
//  initialize
//
//      static initializer.
//
//  returns nothing
//----------------------------------------------------------------------------------------
+ (void) initialize
{
	DlUInt32 theColor = colorFromNSColor([NSColor selectedControlColor]);
	kDefaultColors._nodeSelectionColor = theColor;
	kDefaultColors._elementSelectionColor = theColor;
	
	kDefaultColors._activeColor = colorFromNSColor([NSColor alternateSelectedControlColor]);
}

//----------------------------------------------------------------------------------------
//  colorForElement:inDocument:
//
//      return the color for the specified element.
//
//  colorForElement: Element e     -> 
//  inDocument: FrameDocument* doc -> 
//
//  returns DlUInt32               <- 
//----------------------------------------------------------------------------------------
+ (DlUInt32) colorForElement: (Element)e inDocument: (FrameDocument*) doc
{
	Property p = [doc hilitedProperty];
	if (!p.Empty() && e.GetProperty() == p) {
		return kDefaultColors._activeColor;
	}
	
	FrameStructure* s = [doc structure];
	
	if (s->ActiveLoadCaseIsEditable()) {
		ElementLoad l = [doc hilitedElementLoad];
		if (!l.Empty() && l == e.GetLoad(s->GetActiveLoadCase())) {
			return kDefaultColors._activeColor;
		}
	}
	
	if ([doc elementSelection]->Contains(e)) {
		return kDefaultColors._elementSelectionColor;
	}
	
	return kDefaultColors._elementColor;
}

//----------------------------------------------------------------------------------------
//  colorForNode:inDocument:
//
//      return the color for the specified node.
//
//  colorForNode: Node n           -> 
//  inDocument: FrameDocument* doc -> 
//
//  returns DlUInt32               <- 
//----------------------------------------------------------------------------------------
+ (DlUInt32) colorForNode: (Node)n inDocument: (FrameDocument*) doc
{
	FrameStructure* s = [doc structure];
	if (s->ActiveLoadCaseIsEditable()) {
		NodeLoad l = [doc hilitedNodeLoad];
		if (!l.Empty() && n.GetLoad(s->GetActiveLoadCase()) == l) {
			return kDefaultColors._activeColor;
		}
	}
	
	if ([doc nodeSelection]->Contains(n))
		return kDefaultColors._nodeSelectionColor;
	
	return kDefaultColors._nodeColor;
}

//----------------------------------------------------------------------------------------
//  axialColor
//
//      return the axial force diagram color.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
+ (DlUInt32) axialColor
{
	return kDefaultColors._axialColor;
}

//----------------------------------------------------------------------------------------
//  momentColor
//
//      return the moment diagram color.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
+ (DlUInt32) momentColor
{
	return kDefaultColors._momentColor;
}

//----------------------------------------------------------------------------------------
//  shearColor
//
//      return the shear diagram color.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
+ (DlUInt32) shearColor
{
	return kDefaultColors._shearColor;
}

//----------------------------------------------------------------------------------------
//  displacementColor
//
//      return the displacement diagram color.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
+ (DlUInt32) displacementColor
{
	return kDefaultColors._displacementColor;
}

//----------------------------------------------------------------------------------------
//  lateralLoadColor
//
//      return the lateral load color.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
+ (DlUInt32) lateralLoadColor
{
	return kDefaultColors._lateralLoadColor;
}

//----------------------------------------------------------------------------------------
//  axialLoadColor
//
//      return the axial load color.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
+ (DlUInt32) axialLoadColor
{
	return kDefaultColors._axialLoadColor;
}

@end
