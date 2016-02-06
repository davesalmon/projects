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
#import "FrameAppController.h"

#include "FrameStructure.h"
#include "graphics.h"

//typedef struct PreferenceColors {
//	DlUInt32			_axialColor;
//	DlUInt32			_shearColor;
//	DlUInt32			_momentColor;
//	DlUInt32			_displacementColor;
//	DlUInt32			_elementColor;
//	DlUInt32			_elementSelectionColor;
//	DlUInt32			_nodeColor;
//	DlUInt32			_nodeSelectionColor;
//	DlUInt32			_lateralLoadColor;
//	DlUInt32			_axialLoadColor;
//	DlUInt32			_activeColor;
//}	PreferenceColors;

//static PreferenceColors kDefaultColors = {
//	  graphics::makeColor(0x88, 0x00, 0x00)	//0xff880000	//	axial
//	, graphics::makeColor(0x00, 0x88, 0x00)	//0xff008800	// shear
//	, graphics::makeColor(0x00, 0x00, 0xaa)	//0xff0000aa	// moment
//	, graphics::makeColor(0x88, 0x88, 0x88, 0x88)	//0x88888888	// displacement
//	, graphics::makeColor(0x00, 0x00, 0x00)	// element
//	, graphics::makeColor(0x77, 0xaa, 0x77)	// 0xff77aa77	// selected elem
//	, graphics::makeColor(0xff, 0xff, 0xff)	// 0xffffffff	// node
//	, graphics::makeColor(0xff, 0x88, 0x88)	// 0xffff8888	// selected node
//	, graphics::makeColor(0x88, 0x88, 0x88) // 0xff888888	// lateral load
//	, graphics::makeColor(0xff, 0x88, 0x88)	//0xffff8888	// axial load
//};

//constexpr DlUInt32 kYellowColor { graphics::makeColor(0xff, 0xff, 0x00) };
//constexpr DlUInt32 kTealColor { graphics::makeColor(0x22, 0x77, 0x55) }; //0x6644


const DlUInt32 kAxialColor				=  0;
const DlUInt32 kShearColor				=  1;
const DlUInt32 kMomentColor				=  2;
const DlUInt32 kDisplacementColor		=  3;
const DlUInt32 kElementColor			=  4;
const DlUInt32 kElementSelectedColor	=  5;
const DlUInt32 kNodeColor				=  6;
const DlUInt32 kNodeSelectedColor		=  7;
const DlUInt32 kLateralLoadColor		=  8;
const DlUInt32 kAxialLoadColor			=  9;
const DlUInt32 kActiveLoadPropColor		= 10;
const DlUInt32 kNumColors				= 11;


static DlUInt32 kPreferenceColors[kNumColors];

NSString* const kColorIdentifiers[] = {
	@"AxialColor",
	@"ShearColor",
	@"MomentColor",
	@"DisplacementColor",
	@"ElementColor",
	@"ElementSelectedColor",
	@"NodeColor",
	@"NodeSelectedColor",
	@"LateralLoadColor",
	@"AxialLoadColor",
	@"ActiveLoadPropColor"
};

static
DlInt32 findColorIndex(NSString* name)
{
	for (auto i = 0; i < DlArrayElements(kColorIdentifiers); i++) {
		const NSString* colorName = kColorIdentifiers[i];
		if ([colorName isEqualToString: name]) {
			return i;
		}
	}
	
	return -1;
}

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
	[FrameAppController ensureUserDefaults];
	[self update];
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
		return kPreferenceColors[kActiveLoadPropColor];
	}
	
	FrameStructure* s = [doc structure];
	
	if (s->ActiveLoadCaseIsEditable()) {
		ElementLoad l = [doc hilitedElementLoad];
		if (!l.Empty() && l == e.GetLoad(s->GetActiveLoadCase())) {
			return kPreferenceColors[kActiveLoadPropColor];
		}
	}
	
	if ([doc elementSelection]->Contains(e)) {
		return kPreferenceColors[kElementSelectedColor];
	}
	
	return kPreferenceColors[kElementColor];
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
			return kPreferenceColors[kActiveLoadPropColor];
		}
	}
	
	if ([doc nodeSelection]->Contains(n))
		return kPreferenceColors[kNodeSelectedColor];
	
	return kPreferenceColors[kNodeColor];;
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
	return kPreferenceColors[kAxialColor];
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
	return kPreferenceColors[kMomentColor];
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
	return kPreferenceColors[kShearColor];
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
	return kPreferenceColors[kDisplacementColor];
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
	return kPreferenceColors[kLateralLoadColor];
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
	return kPreferenceColors[kAxialLoadColor];
}

+ (void) setColor: (NSColor*) color forKey: (NSString*) key
{
	DlInt32 whichColor = findColorIndex(key);
	if (whichColor >= 0 && whichColor < kNumColors) {
		kPreferenceColors[whichColor] = colorFromNSColor(color);
		
		for (FrameDocument* doc in [[NSDocumentController sharedDocumentController] documents]) {
			[doc refresh];
		}
		
	} else {
		NSLog(@"[FrameColor setColor ...] called with invalid key %@", key);
	}
}

+ (void) update
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	
	for (auto i = 0; i < DlArrayElements(kColorIdentifiers); i++) {
		NSString* colorName = kColorIdentifiers[i];
		kPreferenceColors[i] = [defs integerForKey: colorName];
	}
}

+ (void) pushToDefaults
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];

	for (auto i = 0; i < DlArrayElements(kColorIdentifiers); i++) {
		NSString* colorName = kColorIdentifiers[i];
		
		[defs setInteger: kPreferenceColors[i] forKey: colorName];
	}

}


@end
