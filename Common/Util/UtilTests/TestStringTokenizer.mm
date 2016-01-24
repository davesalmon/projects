//
//  TestStringTokenizer.m
//
//  Copyright © 2014 David C. Salmon. All Rights Reserved.
//
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
//#include "DlStringUtil.h"

#include "DlStringTokenizer.h"
#include <vector>

@interface TestStringTokenizer : XCTestCase

@end

@implementation TestStringTokenizer

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testTokenizer {
	//
	//	check tokenizer
	//
	// build answer
	std::vector<std::string>	res;
	
	res.push_back("This");
	res.push_back("is");
	res.push_back("a");
	res.push_back("tokenized");
	res.push_back("string");
	
	// check tokens
	const char* const kTokString = "This is a \t\ttokenized\nstring\n";
	DlStringTokenizer toks(kTokString, " \t\n");
	//    std::vector<std::string>::const_iterator i;
	for(auto i = res.begin(); i != res.end(); i++) {
		std::string tok(toks.nextToken());
		XCTAssertTrue(tok == *i);
	}
}


@end
