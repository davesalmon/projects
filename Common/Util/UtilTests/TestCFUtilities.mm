/*+
 *
 *  TestCFUtilities.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  test the basic core foundation utilities.
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
#include "CFStringTracker.h"
#include "DlCFUtilities.h"
#include "DlHttpRequest.h"
#include "DlAppSupportFiles.h"

@interface TestCFUtilities : XCTestCase

@end

@implementation TestCFUtilities

//----------------------------------------------------------------------------------------
//  setUp
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setUp {
    [super setUp];
//	DebugNewForgetLeaks();
//	CFAllocatorSetDefault(kCFAllocatorMalloc);
}

//----------------------------------------------------------------------------------------
//  tearDown
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)tearDown {
//	DebugNewReportLeaks("/tmp/TestCFUtilities-leaks.log");
	[super tearDown];
}

//----------------------------------------------------------------------------------------
//  testStrings
//
//      test string conversions.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testStrings {
//	DebugNewForgetLeaks();

	CFStringTracker t("This is a utf8 string");
	XCTAssertEqual(GetCFStringUTF8Value(t), "This is a utf8 string");
	
	const wchar_t* const kUniString { L"\u2155\u2154\u0498A" };
	const char* const kUTF8String { "\xe2\x85\x95\xe2\x85\x94\xd2\x98" "A" };
	const char16_t* const kCU16String { u"\u2155\u2154\u0498A" };
	
	std::string utf8 { GetCFStringUTF8Value(CFStringTracker(kUniString)) };
	XCTAssertEqual(utf8, kUTF8String);
	
	std::u16string c16 { GetCFStringU16StringValue(CFStringTracker(kUTF8String)) };
	XCTAssertEqual(c16, kCU16String);
	
	unistring uni { GetCFStringUnicodeValue(CFStringTracker(kUTF8String)) };
	XCTAssertEqual(uni.length(), c16.length());
	for (auto i=0; i < uni.length(); i++)
		XCTAssertEqual(uni[i], static_cast<DlUChar>(c16[i]));
	
	CFStringTracker t2 { CFStringTracker(kUniString) };
	XCTAssertEqual(GetCFStringUTF8Value(t2), kUTF8String);
	
	unistring theString(GetCFStringUnicodeValue(t2));
	
	int ofs = 0;
	for (wchar_t c : GetCFStringWideValue(t2))
		XCTAssertEqual(c, theString[ofs++]);
	
	printf("tesgt\n");
	
//	DebugNewReportLeaks("/tmp/TestCFUtilities-strings-leaks.log");
}

//----------------------------------------------------------------------------------------
//  myDictionaryFunction                                                           static
//
//      list dictionary contents.
//
//  const void *key    -> 
//  const void *value  -> 
//  void *context      -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
static
void
myDictionaryFunction(const void *key, const void *value, void *context)
{
	std::string keyString;
	std::string valueString;
	
	if (CFGetTypeID(key) == CFStringGetTypeID())
		keyString = GetCFStringUTF8Value(static_cast<CFStringRef>(key));
	
	if (CFGetTypeID(value) == CFStringGetTypeID())
		valueString = GetCFStringUTF8Value(static_cast<CFStringRef>(value));
	
	printf("Got pair (%s, %s)\n", keyString.c_str(), valueString.c_str());
}

//----------------------------------------------------------------------------------------
//  testHTTPGet
//
//      test the http GET methods.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testHTTPGet {
	
//	DebugNewForgetLeaks();
//	for (int i = 0; i < 10; i++)
	{
	CFRefTracker<CFDictionaryRef> headers;
	std::string utf8Response;
	
	const std::string url { "http://en.cppreference.com/w/" };
	
	headers = GetHttpResponse(utf8Response, url, false, "dcs"); //, void (*callback)(void*) = 0, void* data=0);
	
	CFDictionaryApplyFunction(headers, myDictionaryFunction, 0);
	
	printf("length is %ud\n", (DlUInt32)utf8Response.length());

	//	CFIndex count = CFDictionaryGetCount(headers);
	
	//	printf("%s\n", utf8Response.c_str());
	
	// and try with Http
	DlHttpRequest req;
	
	req.AddHeader("User-Agent", "dcs");
	
	XCTAssertTrue(req.Transact("/w/", "en.cppreference.com"));
	
	DlInt32 len;
	const void* newResp = req.GetResponse(len);
	
	std::string otherResp((const char*)newResp, len);
	printf("my length is %ud\n", (DlUInt32)otherResp.length());

	DlUInt32 compLen = (DlUInt32)utf8Response.length();
	if (compLen > otherResp.length())
		compLen = (DlUInt32)otherResp.length();
	
	for (auto i = 0; i < compLen; i++)
	{
		if (utf8Response[i] != otherResp[i])
		{
			printf("differs at offset %u (%c, %c)\n", i, utf8Response[i], otherResp[i]);
		}
	}
	
	XCTAssertEqual(utf8Response.length(), otherResp.length());
	
	XCTAssertEqual(utf8Response, otherResp);
	}
//	DebugNewReportLeaks("/tmp/TestCFUtilities-http-leaks.log");
}

//- (void)testPerformanceHTTPGet {
//
//	// This is an example of a performance test case.
//	[self measureBlock:^{
//		// Put the code you want to measure the time of here.
//		[self testHTTPGet];
//	}];
//
//}

//----------------------------------------------------------------------------------------
//  testHTTPGet
//
//      test the http GET methods.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testStringLookup {
	
	std::string s(DlGetBundleString("astring", nullptr, "UtilTest", DlGetBundleId().c_str()));
	
	XCTAssertTrue(s == "this is a string");
}

@end
