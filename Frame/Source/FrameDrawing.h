/*+
 *	File:		FrameDrawing.h
 *
 *	Contains:	Define a drawing delegate.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *	Discussion:
 *
 *	Delegates are defined that implement this protocols. The classes that do so are
 *	supplied to FrameView such that the view can pass the appropriate messages to 
 *	classes designed to handle the specific tasks.
 *
 *	To Do:
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

#import <Cocoa/Cocoa.h>

class graphics;

//-------------------------------------------------------
// FrameDrawing
//
//	protocol implementing the following methods:
//
//	1) drawRect -- draw the portion of the structure
//		occupying the specified rectangle.
//
//	2) graphics -- return the current graphics object.
//
@protocol FrameDrawing  <NSObject>
- (void)		drawRect:(const NSRect&)rect;
- (graphics*) 	graphics;
@end
