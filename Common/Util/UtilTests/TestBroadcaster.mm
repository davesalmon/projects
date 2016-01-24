/*+
 *
 *  TestBroadcaster.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  test the broadcast implementation.
 *
-*/
/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#include "DlPlatform.h"
#include "DlBroadcaster.h"
#include "DlListener.h"
#include "DlStringUtil.h"

#include <string>

@interface TestBroadcaster : XCTestCase

@end

// define some messages.
const DlUInt32 kMessage1 = 'msg1';
const DlUInt32 kMessage2 = 'msg2';
const DlUInt32 kMessage3 = 'msg3';

//const char* const kMessageText1 = NULL;
const char* const kMessageText2 = "This is the message.";
const SInt32 kMessageValueBad = 11;

#define ListenerAssertTrue(expression, ...) \
	_XCTPrimitiveAssertTrue(testObj, expression, @#expression, __VA_ARGS__)

#define ListenerFail(...) \
	_XCTPrimitiveFail(testObj, __VA_ARGS__)

// --------------------------------------------------
// define a message class.
//
class MyClass
{
public:
	MyClass() : value(10), s("AnotherTest") {}
	
	Boolean equals(const MyClass& t) const
	{
		return value == t.value && s == t.s;
	}
	
private:
	int value;
	std::string s;
};

// --------------------------------------------------
// define the primary listener
//
class Listener1 : public DlListener {
public:
	Listener1(id t) : count(0), testObj(t) {}
	virtual void ListenToMessage(DlUInt32 msg, void * data) const;
	
	const MyClass& getMyClass() const { return c1; }
private:
	mutable int count;
	MyClass c1;
	
	id testObj;
};

//----------------------------------------------------------------------------------------
//  Listener1::ListenToMessage
//
//      define the listen message for listener1.
//
//  DlUInt32 msg   -> 
//  void * data    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
Listener1::ListenToMessage(DlUInt32 msg, void * data) const
{
	switch(msg) {
		case kMessage1:
			ListenerAssertTrue(data == NULL && count == 0);
			break;
		case kMessage2:
			ListenerAssertTrue(DlStrCmp(kMessageText2, (const char*)data) == 0 && count == 1);
			break;
		case kMessage3:
		{
			const MyClass* c = (const MyClass*) data;
			ListenerAssertTrue(c1.equals(*c) && count == 2);
		}
			break;
		default:
		{
			//            printf("got value %d with count %d\n", *(const SInt32*)data, count);
			ListenerAssertTrue(((*(const SInt32*)data) == kMessageValueBad) && count == 3);
		}
			break;
	}
	
	count += 1;
}

// --------------------------------------------------
// define a secondary listener.
//
class Listener2 : public DlListener {
public:
	Listener2(id testObj) : testObj(testObj) {}
	virtual void ListenToMessage(DlUInt32 msg, void * data) const;

private:
	id testObj;
};

// --------------------------------------------------
// implement listen for secondary.
//
//----------------------------------------------------------------------------------------
//  Listener2::ListenToMessage
//
//      define the listen message for listener2.
//
//  DlUInt32 msg   -> 
//  void * data    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
Listener2::ListenToMessage(DlUInt32 msg, void * data) const
{
	ListenerFail("### Error: Removed broadcaster got message");
}

@implementation TestBroadcaster

//----------------------------------------------------------------------------------------
//  setUp
//
//      
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setUp {
    [super setUp];
}

//----------------------------------------------------------------------------------------
//  tearDown
//
//      
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)tearDown {
    [super tearDown];
}

//----------------------------------------------------------------------------------------
//  testBroadcast
//
//      test the broadcaster.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testBroadcast {
	
	DlBroadcaster br;
	{
		Listener1 l(self);
		Listener2 l2(self);
		
		// test add and remove listener
		
		br.AddListener(&l);
		br.AddListener(&l2);
		br.RemoveListener(&l);
		br.RemoveListener(&l2);
		br.AddListener(&l);
		
		// here there should only be listener 1 remaining.
		XCTAssertEqual(l.BroadcasterCount(), 1);
		XCTAssertEqual(l2.BroadcasterCount(), 0);
		
		br.BroadcastMessage(kMessage1, NULL);
		br.BroadcastMessage(kMessage2, (void*)kMessageText2);
		br.BroadcastMessage(kMessage3, (void*)&l.getMyClass());
		br.BroadcastMessage(1, (void*)&kMessageValueBad);
	}
	
	// now the listeners have been destroyed.
	XCTAssertEqual(br.ListenerCount(), 0);
	
	br.BroadcastMessage(kMessage1, NULL);
	br.BroadcastMessage(kMessage2, (void*)"Text message");
	br.BroadcastMessage(1, (void*)"Text message");
}


@end
