/*+
 *
 *  AnalysisPanelController.mm
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Handle display and events during an analysis.
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

#import "AnalysisPanelController.h"
#import "FrameDocument.h"

#include "DlListener.h"

class AnalysisListener : public DlListener {
public:
	AnalysisListener(AnalysisPanelController* cont) : itsController(cont) {
	}
	
	void ListenToMessage(DlUInt32 msg, void* data) const {
		if (msg == kAnalysisMessage)
			[itsController handleMessage: (AnalysisBroadcast*)data];
	}
private:
    AnalysisListener(const AnalysisListener& a);
    AnalysisListener& operator=(const AnalysisListener& a); 
    
	AnalysisPanelController* itsController;
};

@interface AnalysisPanelController ()
{
	IBOutlet NSTextField*	_elementCount;
	IBOutlet NSTextField*	_equationCount;
	IBOutlet NSTextField*	_nodeCount;
	IBOutlet NSTextField*	_loadCaseCount;
	IBOutlet NSTextField*	_messageField;
	
	IBOutlet NSButtonCell*			_okButton;
	
	IBOutlet NSTextField*			_stageLabel;
	IBOutlet NSTextField*			_stageField;
	IBOutlet NSProgressIndicator*	_progress;
	
	FrameDocument* _frameDocument;
}

- (IBAction) okButton: (id)sender;
- (IBAction) cancelButton: (id)sender;

- (void) setErrorMessage: (NSString*)message;

@end

@implementation AnalysisPanelController


//----------------------------------------------------------------------------------------
//  runDialogWithDocument:
//
//      Run the analysis dialog. This should actually run in a separate thread.
//
//  runDialogWithDocument: FrameDocument* doc  -> the document
//
//  returns bool                               <- true if we ran it.
//----------------------------------------------------------------------------------------
+ (bool) runDialogWithDocument: (FrameDocument*)doc 
{
	AnalysisPanelController* controller = [[AnalysisPanelController alloc] 
		initWithWindowNibName:@"AnalysisPanel"];

	[NSApp 	beginSheet:[controller window] 
			modalForWindow:[doc mainWindow]
			modalDelegate:nil 
			didEndSelector:nil 
			contextInfo:nil];
	[controller setFrameDocument: doc];

	int result = [NSApp runModalForWindow:[controller window]];
	
	[NSApp endSheet: [controller window]];
	[controller close];
	[controller release];

	return result == NSRunStoppedResponse;
}

//----------------------------------------------------------------------------------------
//  setDocument:
//
//      Set the document for analysis.
//
//  setDocument: FrameDocument* doc    -> the document
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setFrameDocument: (FrameDocument*)doc
{
//    [super setDocument: doc];
	_frameDocument = doc;
	
	FrameStructure* s = [_frameDocument structure];
	AnalysisData theData;
	s->InitAnalysis(&theData);
	
	[_elementCount setIntValue: theData.elemCount];
	[_equationCount setIntValue: theData.eqCount];
	[_nodeCount setIntValue: theData.nodeCount];
	[_loadCaseCount setIntValue: theData.lcCount];
	
	NodeEnumerator nodes = s->CheckNodeLoadEquations(NodeLoadIsDisp);
	
	if (nodes.Length() > 0) {
		
		NSString* string = NSLocalizedString(
					@"Settlement applied to unrestrained DOF on selected nodes.\n"
					"Either restrain the DOF or remove the settlement.", nil);
		
		[self setErrorMessage: string];
		[doc setSelectedNodes: nodes];
	}
}

- (void) setErrorMessage: (NSString*)message
{
	[_stageLabel setHidden: YES];
	[_stageField setHidden: YES];
	[_progress setHidden: YES];
	
	[_okButton setEnabled: NO];

	[_messageField setStringValue: message];
	[_messageField setHidden: NO];
}


//----------------------------------------------------------------------------------------
//  okButton:
//
//      Handle the ok button.
//
//  okButton: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) okButton: (id)sender 
{
	try {
		
		AnalysisListener l(self);
		[_frameDocument structure]->Analyze(&l);
		[NSApp stopModal];
	
	} catch(DlException& ex) {
		[self setErrorMessage: [NSString stringWithUTF8String: ex.what()]];
	}
}

//----------------------------------------------------------------------------------------
//  cancelButton:
//
//      Handle the cancel button.
//
//  cancelButton: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) cancelButton: (id)sender 
{
	[NSApp abortModal];
}

//----------------------------------------------------------------------------------------
//  handleMessage:
//
//      Handle messages during the analysis. This allows us to update the UI.
//
//  handleMessage: AnalysisBroadcast* data -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) handleMessage: (AnalysisBroadcast*)data
{
}

@end
