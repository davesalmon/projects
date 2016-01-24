//
//  TestSockets.m
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
#include "DlListenerSocket.h"
#include "DlConnectorSocket.h"
#include "DlBufferedSocket.h"
#include "DlSimpException.h"

#include <thread>

#define MYAssertTrue(obj, expression, ...) \
	_XCTPrimitiveAssertTrue(obj, expression, @#expression, __VA_ARGS__)

const char* const kSendMessage = "this is the message";
const char* const kResponseMessage = "this is the response";

static void listenForMessage(const std::string& message, id obj)
{
	try {
		printf("create listener\n");
		DlListenerSocket listener("127.0.0.1", 8089); // keep from being pestered
		//		DlListenerSocket listener("0.0.0.0", 8089);
		DlBufferedSocket sock(listener.Accept());
		printf("accepted connection\n");
		
		// read the message
		std::string line;
		sock.ReadLine(line);
		
		MYAssertTrue(obj, line == message);
		
		// and respond.
		sock.WriteLine(kResponseMessage);
		sock.Flush();
		
		printf("exit listener\n");
		
	} catch(const std::exception& ex) {
		printf("failed to create listener %s\n", ex.what());
	} catch(...) {
		printf("unknown exception\n");
	}
}


@interface TestSockets : XCTestCase

@end

@implementation TestSockets

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testConnect {
	try {
		printf("start thread\n");
		
		std::thread t(listenForMessage, kSendMessage, self);
		
		// give the listener time...
		sleep(1);
		
		//		printf("create socket\n");
		try {
			printf("connect socket\n");
			DlConnectorSocket connect;
			DlBufferedSocket sock(connect.Connect("127.0.0.1", 8089));
			printf("connected socket\n");
			
			// write the message to the listener.
			sock.WriteLine(kSendMessage);
			sock.Flush();
			
			// read back the response
			std::string line;
			sock.ReadLine(line);
			
			XCTAssertTrue(line == kResponseMessage);
			
		} catch (const std::exception& ex) {
			printf("got socket exception: %s\n", ex.what());
			t.detach(); // could not write to socket.
			XCTFail("Failed with socket exception");
		}
		
		if (t.joinable()) {
			t.join();
			
			printf("joined!\n");
		}
		
	} catch(const std::exception& ex) {
		printf("got thread exception: %s", ex.what());
		XCTFail("Failed with thread exception");
	}
}

@end
