/*+
 *
 *  ResultTableController.m
 *
 *  Copyright © 2005 David C. Salmon. All Rights Reserved.
 *
 *  handle the results table window.
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

#import "ResultTableController.h"
#import "FrameDocument.h"
#import "TableColumn.h"

#include "FrameStructure.h"
#include "PropertyTypeEnumerator.h"
#include "LoadCaseResults.h"
#include "Element.h"

#include <vector>

@interface ResultTableController ()
{
	FrameDocument* 					_frameDocument;
//	const PropertyTypeEnumerator*	_resultTypes;
	bool							_updatingSelection;	// true if selection update in progress
	std::vector<NSInteger>			_orderLookup;		// lookup the order of the elements
}

// return a string for the specified load case, row and column.
- (NSString*) stringForRow: (NSInteger)row andCol: (NSInteger)col;
- (PropertyTypeEnumerator) resultTypes;

- (PropertyTypeEnumerator) elemResultTypes : (const char*) elementType;

@end

@implementation ResultTableController

//----------------------------------------------------------------------------------------
//  createResultTable:
//
//      create the results table window.
//
//  createResultTable: FrameDocument* doc  -> the document.
//
//  returns ResultTableController*         <- the controller.
//----------------------------------------------------------------------------------------
+ (ResultTableController*) createResultTable: (FrameDocument*) doc
{
    ResultTableController* theController = nil;
    
    theController = [[ResultTableController alloc] initWithDocument: doc];
    //    [theController showWindow:theController];
    [theController setShouldCloseDocument:NO];
    [doc addWindowController:theController];
    [theController release];
    return theController;
}

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      initialize the results table for the specified document.
//
//  initWithDocument: FrameDocument* doc   -> the document
//
//  returns id                             <- self
//----------------------------------------------------------------------------------------
- (instancetype) initWithDocument: (FrameDocument*)doc
{
    self = [super initWithWindowNibName:@"ResultsWindow" owner:self];

    _frameDocument = doc;

    return self;
}

//----------------------------------------------------------------------------------------
//  refresh
//
//      refresh the view.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) refresh
{
//	[_table reloadData];
	[_table noteNumberOfRowsChanged];
//	[_table setNeedsDisplay: YES];
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      dealloc the view.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
    [super dealloc];
}

//----------------------------------------------------------------------------------------
//  awakeFromNib
//
//      set up the table.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) awakeFromNib
{
	[TableColumn initializeTable: _table
						withList: [self resultTypes]
					   editiable: NO
						   align: NSRightTextAlignment];
	
	[self updateSelection:true];
	
	for (NSTableColumn* tc in [_table tableColumns]) {
		NSSortDescriptor* colDescriptor =
				[NSSortDescriptor sortDescriptorWithKey: [tc identifier]
											  ascending: YES
											   selector: @selector(compare:)];
		[tc setSortDescriptorPrototype: colDescriptor];
	}
	
	int diff = [_table rectOfRow:0].size.width - [[_table superview] frame].size.width;
	if (diff > 0) {
		NSRect newFrame = _table.window.frame;
		newFrame.size.width += diff;
		[[_table window] setFrame: newFrame display: YES];
	}
}

//----------------------------------------------------------------------------------------
//  resultTypes
//
//      return the master result type list.
//
//  returns PropertyTypeEnumerator <- the result type enumerator.
//----------------------------------------------------------------------------------------
- (PropertyTypeEnumerator) resultTypes
{
	return [_frameDocument structure]->GetMergedResultTypes();
}

//----------------------------------------------------------------------------------------
//  elemResultTypes:
//
//      return the result types for the specified element.
//
//  elemResultTypes: const char* elementType   -> the element type.
//
//  returns PropertyTypeEnumerator             <- the result types.
//----------------------------------------------------------------------------------------
- (PropertyTypeEnumerator) elemResultTypes : (const char*) elementType
{
	return [_frameDocument structure]->GetResultTypes(elementType);
}

//----------------------------------------------------------------------------------------
//  updateSelection
//
//      Update the selection shown in the inspector to correspond to the structure view.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateSelection: (bool)fromFrameDocument
{
	FrameStructure* s = [_frameDocument structure];

	ElementEnumerator elems = s->GetElements();
	if (elems.Length() > 0) {
		_updatingSelection = fromFrameDocument;
		NSMutableIndexSet* selected = [NSMutableIndexSet indexSet];
		for (DlUInt32 i = 0; i < elems.Length(); i++) {
//			Element item = elems.At(i);
			if ([_frameDocument elementSelected: i]) {
				[selected addIndex: i];
			}
		}
		[_table selectRowIndexes:selected byExtendingSelection:FALSE];
	}
}

//----------------------------------------------------------------------------------------
//  stringForLoadCase:row:andCol:
//
//      generate the string value for the specified loadcase, row and column in the table.
//
//  stringForLoadCase: LoadCase lc -> 
//  row: int row                   -> 
//  andCol: int col                -> 
//
//  returns NSString*              <- 
//----------------------------------------------------------------------------------------
- (NSString*) stringForRow: (NSInteger)baseRow andCol: (NSInteger)col
{
	DlString val;
	
	NSInteger row = _orderLookup[baseRow];
	
//	printf("value for row %ld(%ld)\n", row, baseRow);
	
	if (col == 0) {
		val = DlString((long)row + 1, DlIntFormat(0, DlIntFormatType::Decimal));
	} else {
		const FrameStructure* s = [_frameDocument structure];
		const LoadCaseResults* res = s->GetResults();
		
		if (res != 0) {
			// FIXME: review for efficiency. Perhaps this should be processed in the Element.

			// this is the property in the column
			const PropertyType* dataType = [self resultTypes].At(col - 1);
			Element e = s->GetElements().At(row);
			
			// lookup the property in the element
			PropertyTypeEnumerator eProps =  [self elemResultTypes: e.GetType()];
			
			DlInt32 theCol = eProps.IndexOf(dataType);
			if (theCol < eProps.Length()) {
				const ElementForce& frc = res->GetElementForce(row);
				DlFloat64 value = DlChop(e.GetResultValue(theCol, frc, *res), 1e-10);
				val = UnitTable::FormatValue(value, dataType->units,
										 DlFloatFormat(4, 0, DlFloatFormatType::Fit), true);
			} else {
				val = DlString("--");
			}
		} else {
			val = DlString("--");
		}
	}
	
	return [NSString stringWithCString: val
							 encoding : NSUTF8StringEncoding];
}

- (BOOL) validateMenuItem: (NSMenuItem*) anItem
{
	return anItem.action == @selector(copy:);
}

- (IBAction) cut: (id) sender
{
}

- (IBAction) paste: (id) sender
{
}

- (IBAction) clear: (id) sender
{
	// maybe something here.
}


//----------------------------------------------------------------------------------------
//  copy:
//
//      handle the copy menu command.
//
//  copy: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) copy: (id) sender
{
#if DlDebugging
	NSLog(@"copy");
#endif
	
//	LoadCase lc = [_frameDocument loadCase];
	const FrameStructure* s = [_frameDocument structure];
	const LoadCaseResults* res = s->GetResults();

	if (res != 0) {
		NSPasteboard* pb = [NSPasteboard generalPasteboard];
		
		NSMutableString* theData = [NSMutableString string];
		
		NSIndexSet* rows = [_table selectedRowIndexes];
		
		int nCols = [_table numberOfColumns];

		// write out the header row.
		for (int i = 0; i < nCols; i++) {
			if (i != 0)
				[theData appendString:@"\t"];
			NSTableColumn* c = [[_table tableColumns] objectAtIndex: i];

			[theData appendString: [c.headerCell stringValue]];
		}
		
		[theData appendString:@"\n"];

		// and then either the selected row or all the rows.
		if (rows != nil) {
			
			[rows enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL* stop) {
				
				for (int i = 0; i < nCols; i++) {
					if (i != 0)
						[theData appendString:@"\t"];
					[theData appendString:
					 	[self stringForRow: _orderLookup[idx] andCol: i]
					 ];
				}
				[theData appendString:@"\n"];
				
			}];
		} else {
			for (int row = 0; row < [_table numberOfRows]; row++) {
				for (int i = 0; i < nCols; i++) {
					if (i != 0)
						[theData appendString:@"\t"];
					[theData appendString:
					 	[self stringForRow: _orderLookup[row] andCol: i]
					 ];
				}
				[theData appendString:@"\n"];
			}
		}
		
		NSArray* pbTypes = [NSArray arrayWithObjects:
							NSTabularTextPboardType, NSStringPboardType, nil];
		
		[pb declareTypes: pbTypes owner: nil];
		[pb setString: theData forType: NSTabularTextPboardType];
		[pb setString: theData forType: NSStringPboardType];
		
#if DlDebugging
		NSLog(@"pasteboard has types %@", [pb types]);
#endif

	}
}

#pragma mark -
#pragma mark =========== Table Data Source ============
#pragma mark -

//----------------------------------------------------------------------------------------
//  numberOfRowsInTableView:
//
//      return the number of table rows.
//
//  numberOfRowsInTableView: NSTableView * tableView   -> 
//
//  returns int                                        <- 
//----------------------------------------------------------------------------------------
- (NSInteger) numberOfRowsInTableView: (NSTableView *)tableView
{
	FrameStructure* s = [_frameDocument structure];
	NSInteger size = s->GetElements().Length();
	
	// create the order lookup if needed.
	if (_orderLookup.size() != size) {
		_orderLookup.resize(size);
		for (auto i = 0; i < size; i++) {
			_orderLookup[i] = i;
		}
	}
	
	return size;
}

//----------------------------------------------------------------------------------------
//  tableView:objectValueForTableColumn:row:
//
//      return the value for the table cell.
//
//  tableView: NSTableView * tableView                     -> 
//  objectValueForTableColumn: NSTableColumn * tableColumn -> 
//  row: int row                                           -> 
//
//  returns id                                             <- 
//----------------------------------------------------------------------------------------
- (id)             tableView: (NSTableView*)tableView
   objectValueForTableColumn: (NSTableColumn*)tableColumn
						 row: (NSInteger)row
{
	int colNum = [[tableColumn identifier] intValue];

	if (colNum == 0) {
		return [NSNumber numberWithInt:_orderLookup[row] + 1];
	}

	return [self stringForRow: row andCol: colNum];
}

//----------------------------------------------------------------------------------------
//  tableViewSelectionDidChange:
//
//      the selected cell in the table changed.
//
//  tableViewSelectionDidChange: NSNotification * notification -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) tableViewSelectionDidChange: (NSNotification *)notification
{
	if (!_updatingSelection) {
		_updatingSelection = true;
		
		NSIndexSet* rows = [_table selectedRowIndexes];

		[_frameDocument clearSelections];

		if (rows) {
			
			[rows enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL* stop) {
				Element e = [_frameDocument structure]->GetElements().At(_orderLookup[idx]);
				[_frameDocument selectElement: e withFlags: NSShiftKeyMask];
			}];
		}
	}
	
	_updatingSelection = false;

}

// =========================================
//	Define class to sort elements.
//
//	The order is tracked using _orderLookup a one to one mapping that keeps track of the
//	order of the rows in the table.
//
class CompareElementValues
{
public:
	CompareElementValues(int field_, FrameStructure* s_,
						 std::vector<NSInteger>&ol, bool asc)
	: field(field_), s(s_), orderLookup(ol), ascending(asc)
	{
		
	}

	bool operator () (NSUInteger row1, NSUInteger row2);
	
private:
	std::vector<NSInteger>& orderLookup;
	int field;
	FrameStructure* s;
	bool ascending;
};

//----------------------------------------------------------------------------------------
//  tableView:sortDescriptorsDidChange:
//
//      resort the table when the descriptors change.
//
//  tableView: NSTableView * tableView                 -> the view
//  sortDescriptorsDidChange: NSArray * oldDescriptors -> the old descriptors
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) tableView: (NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors
{
	NSArray* d = [tableView sortDescriptors];

	bool asc = true;
	
	if (d && [d count] > 0) {
		
		NSSortDescriptor* desc = [d objectAtIndex: 0];
		NSInteger col = [[desc key] intValue];
		asc = [desc ascending];
		
		if (col > 0) {
			
			int field = col - 1;
			
			CompareElementValues cmp(field, [_frameDocument structure],
									 _orderLookup, asc);
			
			std::sort(_orderLookup.begin(), _orderLookup.end(), cmp);
			
#if DlDebugging
//			for (auto i = 0; i < _orderLookup.size(); i++) {
//				printf("_orderLookup[%d]=%ld\n", i, _orderLookup[i]);
//			}
#endif
			[_table reloadData];

			return;
		}
	}

	// default order
	for (auto i = 0; i < _orderLookup.size(); i++) {
		_orderLookup[i] = asc ? i : _orderLookup.size() - i - 1;
	}

	[_table reloadData];
}

//----------------------------------------------------------------------------------------
//  CompareElementValues::operator ()
//
//      return true if row1 less than row2. For std::sort.
//
//  NSUInteger row1    -> the first row
//  NSUInteger row2    -> the second row
//
//  returns bool       <- true if data for row1 should come before data for row2
//----------------------------------------------------------------------------------------
bool
CompareElementValues::operator () (NSUInteger row1, NSUInteger row2)
{
	const LoadCaseResults* res = s->GetResults();
	DlFloat64 value1, value2;
	
	{
		NSUInteger row = row1; //orderLookup[row1];
		Element e = s->GetElements().At(row);
		const ElementForce& frc = res->GetElementForce(row);
		value1 = DlChop(e.GetResultValue(field, frc, *res), 1e-10);
	}
	
	{
		NSUInteger row = row2; //orderLookup[row2];
		Element e = s->GetElements().At(row);
		const ElementForce& frc = res->GetElementForce(row);
		value2 = DlChop(e.GetResultValue(field, frc, *res), 1e-10);
	}

	return ascending ? value1 < value2 : value2 < value1;
}

#pragma mark -
#pragma mark =============== window ===============
#pragma mark -


//----------------------------------------------------------------------------------------
//  windowTitleForDocumentDisplayName:
//
//      return the window title.
//
//  windowTitleForDocumentDisplayName: NSString * displayName  -> 
//
//  returns NSString *                                         <- 
//----------------------------------------------------------------------------------------
- (NSString *)windowTitleForDocumentDisplayName: (NSString *)displayName
{
    return [displayName stringByAppendingString:@"-Results"];
}

//----------------------------------------------------------------------------------------
//  windowWillClose:
//
//      handle window closing.
//
//  windowWillClose: NSNotification * aNotification    ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowWillClose: (NSNotification *) aNotification
{
	[_frameDocument resultsPanelIsClosing];
}

@end
