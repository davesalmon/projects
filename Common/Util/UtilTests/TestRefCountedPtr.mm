//
//  TestRefCountedPtr.m
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
#include "DlRefCountedPtr.h"

#include <memory>

static int derivedCount = 0;
static int baseCount = 0;

class Base {
public:
	Base();
	
	explicit Base(int x);
	
	virtual ~Base();
	
	operator bool();
	
	int get();
private:
	int* p;
};

class Derived : public Base
{
public:
	Derived();
	
	explicit Derived(double d);
	
	virtual ~Derived();
	
	operator bool();
	
private:
	double* q;
};

class Container
{
public:
	Container(Derived* d);
	
	const DlRefCountedPtr<Derived>& get() const;
	
	void set(DlRefCountedPtr<Derived>&p);
	
private:
	DlRefCountedPtr<Derived> ptr;
};

@interface TestRefCountedPtr : XCTestCase

@end

@implementation TestRefCountedPtr

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testSwap {
	{
		DlRefCountedPtr<Derived> ptr(new Derived);
		DlRefCountedPtr<Base> x(new Base);
		DlRefCountedPtr<Derived> other(new Derived(5));
		
		if (*x)
		{
			DlRefCountedPtr<Derived> tmp(ptr);
			ptr = other;
			other = tmp;
			
			XCTAssertTrue(*ptr);
			XCTAssertTrue(*other);
			XCTAssertTrue(*tmp);
			
			x = other;
			XCTAssertTrue(*x);
			XCTAssertTrue(*other);
		}
		else
		{
			XCTAssertTrue(*x);
		}
		
		XCTAssertEqual(*ptr.getCount(), 1);
		XCTAssertEqual(*other.getCount(), 2);
	}
	XCTAssertEqual(derivedCount, 0);
	XCTAssertEqual(baseCount, 0);
}

@end

Container::Container(Derived* d) : ptr(d)
{
}


const DlRefCountedPtr<Derived>&
Container::get() const
{
	return ptr;
}

void Container::set(DlRefCountedPtr<Derived>&p)
{
	ptr = p;
}

Derived::Derived()
: Base(2), q(new double {1})
{
	derivedCount++;
	printf("construct Derived (%lf, %d)\n", *q, get());
}

Derived::Derived(double d)
: Base(10), q(new double {d})
{
	derivedCount++;
	printf("construct Derived (%lf, %d)\n", *q, get());
}

Derived::~Derived()
{
	delete q;
	q = nullptr;
	printf("destruct Derived\n");
	derivedCount--;
}

Derived::operator bool()
{
	printf("derived bool\n");
	return q;
}


Base::Base() : p(new int {5})
{
	baseCount++;
	printf("construct Base (%d)\n", *p);
}

Base::Base(int x) : p(new int  {x})
{
	baseCount++;
	printf("construct Base (%d)\n", *p);
}

Base::~Base()
{
	delete p; p = nullptr;
	printf("destruct Base\n");
	baseCount--;
}

int Base::get()
{
	return p ? *p : -1;
}

Base::operator bool()
{
	printf("base bool\n");
	return p;
}
