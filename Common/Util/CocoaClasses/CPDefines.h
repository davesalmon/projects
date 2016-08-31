//
//  CPDefines.h
//
//  Created by David Salmon on 6/29/15.
//  Copyright (c) 2015 Dave Salmon. All rights reserved.
//

#ifndef _H_CPDefines
#define _H_CPDefines

#import <Foundation/Foundation.h>

#if TARGET_OS_IPHONE

#define CPView			UIView
#define CPBezierPath	UIBezierPath
#define CPColor			UIColor
#define CPFont			UIFont

#define CPRect			CGRect
#define CPPoint			CGPoint
#define CPSize			CGSize

#define NSMakeSize		CGSizeMake
#define NSMakeRect		CGRectMake

#else

#define CPView			NSView
#define CPBezierPath	NSBezierPath
#define CPColor			NSColor
#define CPFont			NSFont

#define CPRect			NSRect
#define CPPoint			NSPoint
#define CPSize			NSSize

#define addLineToPoint	lineToPoint

#endif

#endif	// _H_CPDefines
