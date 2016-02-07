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

// define the color indexes
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

//----------------------------------------------------------------------------------------
//  findColorIndex                                                                 static
//
//      return the index for the specified name.
//
//  NSString* name     -> 
//
//  returns DlInt32    <- 
//----------------------------------------------------------------------------------------
static
DlInt32
findColorIndex(NSString* name)
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

//----------------------------------------------------------------------------------------
//  setColor:forKey:
//
//      set the color for the specified key.
//
//  setColor: NSColor* color   -> 
//  forKey: NSString* key      -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------
//  update
//
//      update the colors from the preferences.
//
//  returns nothing
//----------------------------------------------------------------------------------------
+ (void) update
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	
	for (auto i = 0; i < DlArrayElements(kColorIdentifiers); i++) {
		NSString* colorName = kColorIdentifiers[i];
		kPreferenceColors[i] = [defs integerForKey: colorName];
	}
}

//----------------------------------------------------------------------------------------
//  pushToDefaults
//
//      push the colors to the defaults.
//
//  returns nothing
//----------------------------------------------------------------------------------------
+ (void) pushToDefaults
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];

	for (auto i = 0; i < DlArrayElements(kColorIdentifiers); i++) {
		NSString* colorName = kColorIdentifiers[i];
		
		[defs setInteger: kPreferenceColors[i] forKey: colorName];
	}
}


@end
