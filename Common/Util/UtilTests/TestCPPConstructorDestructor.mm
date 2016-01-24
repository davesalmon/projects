//
//  TestCPPConstructorDestructor.m
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

#include <string>

static void setFunc(const char* f);
static bool AssertLastFunction(const char* s);

class A;
static A getA();

class A {
public:
	A();
	
	A(const std::initializer_list<int>& arr);
	A(const A& copy);
	A(A&& move);
	
	A& operator=(const A& copy);
	A& operator=(A&& move);
	
	~A();
	
	bool operator==(const A& a) const;
	
	size_t size() const;
	const int* ptr() const;
	
private:
	
	size_t count;
	int* memList;
};


@interface TestCPPConstructorDestructor : XCTestCase

@end

@implementation TestCPPConstructorDestructor

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testConstructors {
	A a { 1, 2, 3, 4, 5 };
	A b { 1, 2, 3, 4, 5 };
	
	XCTAssertTrue(AssertLastFunction("initializer"));
	XCTAssertTrue(a == b);
	
	A c(a);
	XCTAssertTrue(AssertLastFunction("copy"));
	XCTAssertTrue(a == c);
	
	A d(std::move(a));
	XCTAssertTrue(AssertLastFunction("move"));
	XCTAssertTrue(b == d);
	XCTAssertTrue(c == d);
	XCTAssertEqual(a.size(), 0);
	XCTAssertEqual(a.ptr(), nullptr);
	
	a = b;
	XCTAssertTrue(AssertLastFunction("copy="));
	XCTAssertTrue(a == b);
	
	a = std::move(b);
	XCTAssertTrue(AssertLastFunction("move="));
	XCTAssertTrue(a == d);
	XCTAssertEqual(b.size(), 0);
	XCTAssertEqual(b.ptr(), nullptr);
	
	b = getA();
	XCTAssertTrue(AssertLastFunction("move="));
	XCTAssertTrue(a == b);
	
	// in this case, object is constructed in-place, rvo
	A e(getA());
	XCTAssertTrue(AssertLastFunction("initializer"));
	XCTAssertTrue(a == e);
	
	// this is the same case
	A f = getA();
	XCTAssertTrue(AssertLastFunction("initializer"));
	XCTAssertTrue(f == e);
	
	// as should this one.
	A g { getA() };
	XCTAssertTrue(AssertLastFunction("initializer"));
	XCTAssertTrue(f == e);
}

@end

static std::string LastFunction;

static void setFunc(const char* f)
{
	LastFunction = f;
}

static bool AssertLastFunction(const char* s)
{
	return LastFunction == s;
}

static A getA()
{
	A test { 1, 2, 3, 4, 5 };
	
	return test;
}

A::A() : count(0), memList(nullptr)
{
	setFunc("default");
	printf("default constructor\n");
}

A::A(const std::initializer_list<int>& arr) : count(arr.size()), memList(new int[arr.size()])
{
	printf("initializer constructor\n");
	setFunc("initializer");
	
	int* p = memList;
	for (auto val : arr)
		*p++ = val;
}

A::A(const A& copy) : count(copy.count), memList(nullptr)
{
	setFunc("copy");
	printf("copy constructor\n");
	if (count > 0) {
		memList = new int[count];
		int *p = memList;
		int *q = copy.memList;
		for (auto i = 0; i < count; i++)
			*p++ = *q++;
	}
}

A::A(A&& move) : count(move.count), memList(move.memList)
{
	setFunc("move");
	printf("move constructor\n");
	move.count = 0;
	move.memList = nullptr;
}

A& A::operator=(const A& copy)
{
	setFunc("copy=");
	printf("copy assignment\n");
	
	if (&copy != this)
	{
		if (memList)
			delete [] memList;
		
		count = copy.count;
		memList = count > 0 ? new int[count] : nullptr;
		
		if (count > 0) {
			memList = new int[count];
			int *p = memList;
			int *q = copy.memList;
			for (auto i = 0; i < count; i++)
				*p++ = *q++;
		}
	}
	
	return *this;
}

A& A::operator=(A&& move)
{
	setFunc("move=");
	printf("move assignment\n");
	
	if (&move != this)
	{
		if (memList)
			delete [] memList;
		
		memList = move.memList;
		count = move.count;
		
		move.count = 0;
		move.memList = nullptr;
	}
	
	return *this;
}

A::~A()
{
	printf("deconstructor\n");
	delete[] memList;
}

bool A::operator==(const A& a) const
{
	if (a.count != count)
		return false;
	
	int *p = memList;
	int *q = a.memList;
	
	for (auto i = 0; i < count; i++)
		if (*p++ != *q++)
			return false;
	return true;
}

size_t A::size() const { return count; }
const int* A::ptr() const { return memList; }

