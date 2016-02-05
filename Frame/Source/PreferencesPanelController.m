//
//  PreferencesPanelController.m
//  Frame
//
//  Created by David Salmon on 2/4/16.
//
//

#import "PreferencesPanelController.h"
#import "FrameColor.h"
#import "FrameAppController.h"
#import "UnitsFormatter.h"

#include "graphics.h"

@interface PreferencesPanelController ()

- (IBAction)ok:(id)sender;
- (IBAction)cancel:(id)sender;


- (IBAction)ColorWellClicked:(id)sender;


@property (assign) IBOutlet NSBox *appColorsBox;
@property (assign) IBOutlet NSMatrix *worldForm;

@property (assign) IBOutlet NSButton *visibleCheckBox;
@property (assign) IBOutlet NSButton *snapCheckBox;
@property (assign) IBOutlet NSTextField *gridSpacing;

@property (assign) IBOutlet NSTextField *scale;

@end

@implementation PreferencesPanelController

//----------------------------------------------------------------------------------------
//  runDialogWithDocument:
//
//      Run the analysis dialog. This should actually run in a separate thread.
//
//  runDialogWithDocument: FrameDocument* doc  -> the document
//
//  returns bool                               <- true if we ran it.
//----------------------------------------------------------------------------------------
+ (bool) runDialog
{
	PreferencesPanelController* controller = [[PreferencesPanelController alloc]
										   initWithWindowNibName:@"PreferencesPanel"];
	
	
	int result = [NSApp runModalForWindow:[controller window]];
	
//	[NSApp endSheet: [controller window]];
	[controller close];
	[controller release];
	
//	[[controller window] orderOut: nil];
	
	return result == NSModalResponseStop;
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];

	for(NSView *view in [[_appColorsBox contentView] subviews])
	{
		if ([view isKindOfClass:[NSColorWell class]]) {
			NSColorWell* well = (NSColorWell*)view;
			NSString* tag = [view identifier];
			
			DlUInt32 val = [defs integerForKey: tag];

			well.color = makeNSColor(val);
		}
	}

	[self updateData];
}

- (IBAction)ok:(id)sender {
	[NSApp stopModalWithCode: NSModalResponseStop];
	
	// here we need to grab the data and update preferences.
}

- (IBAction)cancel:(id)sender {
	[NSApp stopModalWithCode: NSModalResponseAbort];
}

- (IBAction)ColorWellClicked: (NSColorWell* )sender {
	// invoke the color picker.
	NSLog(@"got message from color well %@ with color %@", sender.identifier, sender.color);
	[FrameColor setColor: sender.color forKey: sender.identifier updateDefaults: NO];
}

//----------------------------------------------------------------------------------------
//  updateData
//
//      update the date from the document.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateData
{
	FrameAppController* app = [FrameAppController instance];
	
	const WorldRect& wr = [app getDefaultWorld];
	double scale = [app getDefaultScale];
	FrameGrid* grid = [app getDefaultGrid];
	
	// fill in the dimensions form.
	UnitsFormatter* fmt = [UnitsFormatter
						   formatterWithDocument: nil
								 units: UnitsLength
						   precision: 2
						   showingTag: true];
	
	[_scale setDoubleValue: 100.0 / scale];
	[_scale setFormatter: fmt];
	
	[[_worldForm cellWithTag:0] setFormatter: fmt];
	[[_worldForm cellWithTag:0] setDoubleValue: wr.left()];
	
	[[_worldForm cellWithTag:1] setFormatter: fmt];
	[[_worldForm cellWithTag:1] setDoubleValue: wr.bottom()];
	
	[[_worldForm cellWithTag:2] setFormatter: fmt];
	[[_worldForm cellWithTag:2] setDoubleValue: wr.right()];
	
	[[_worldForm cellWithTag:3] setFormatter: fmt];
	[[_worldForm cellWithTag:3] setDoubleValue: wr.top()];
	
	WorldPoint spacing = [grid spacing];
	
	[_gridSpacing setDoubleValue: spacing.x()];
	[_gridSpacing setFormatter: fmt];
	
	// fill in the check boxes.
	[_snapCheckBox setState: [grid snapOn] ? NSOnState : NSOffState];
	[_visibleCheckBox setState: [grid gridOn] ? NSOnState : NSOffState];	
}

@end
