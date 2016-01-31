/*+
 *
 *  CoordTest.cpp
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  test for the structure library.
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

#include "CoordTest.h"

#include "DlPlatform.h"
#include "WorldPoint.h"
#include "WorldRect.h"

const double kTolerance = 1.0e-15;

#if _DEBUG

int CoordTest::leak_count {1};

static const char* kCoordTestPath { "/tmp/leaks-CoordTest%d.log" };

//----------------------------------------------------------------------------------------
//  CoordTest::SetUp
//
//	set up tests.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
CoordTest::SetUp()
{
	DebugNewForgetLeaks();
}

//----------------------------------------------------------------------------------------
//  CoordTest::TearDown
//
//	tear donw tests.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
CoordTest::TearDown()
{
	char buf[256];
	snprintf(buf, sizeof(buf), kCoordTestPath, leak_count++);

	DebugNewValidateAllBlocks();
	DebugNewReportLeaks(buf);
}

#endif

//----------------------------------------------------------------------------------------
//  TestPoints
//
//	Test point operations.
//
//  returns nothing
//----------------------------------------------------------------------------------------
TEST_F(CoordTest, TestPoints)
{
	WorldPoint p1(1,1);
	WorldPoint p2(2,2);
	
	WorldPoint diff(1,1);
	WorldPoint sum(3,3);
	
	EXPECT_TRUE(p2 - p1 == diff);
	EXPECT_TRUE(p2 + p1 == sum);
	EXPECT_TRUE(p2 + p1 == p1 + p2);
	
	EXPECT_TRUE(WorldPoint::dot(p1, p2) == p1.dot(p2));
	EXPECT_TRUE(p2.dot(p1) == p1.dot(p2));
	EXPECT_TRUE(p2.dot(p1) == 4);
	EXPECT_TRUE(p2.dot(p2) == 8);
	EXPECT_TRUE(sqrt(p2.dot(p2)) == p2.length());
	EXPECT_TRUE(sqrt(diff.dot(diff)) == p2.dist(p1));
	
	EXPECT_TRUE(p1.cross(p2) == -p2.cross(p1));
	EXPECT_TRUE(p1.cross(p1) == 0);
	
	EXPECT_TRUE(p1.transform(WorldPoint(0,0), WorldPoint(1,0)) == p1);
	EXPECT_TRUE((p1.transform(WorldPoint(0,0), p1) - WorldPoint(p1.length(), 0)).abs() <
				WorldPoint(kTolerance, kTolerance));
}

//----------------------------------------------------------------------------------------
//  TestRects
//
//	Test rectangle operations.
//
//  returns nothing
//----------------------------------------------------------------------------------------
TEST_F(CoordTest, TestRects)
{
	EXPECT_TRUE(WorldRect(0,0,1,1) == WorldRect(WorldPoint(0,0),WorldPoint(1,1)));
	
	WorldRect	r;
	r.addBounds(WorldPoint(1,1));
	EXPECT_TRUE(r == WorldRect(0,0,1,1));
	
	r += WorldPoint(2,1);
	EXPECT_TRUE(r == WorldRect(0,0,2,1));
	
	EXPECT_TRUE(WorldRect(0,0,1,1).contains(WorldPoint(0.5,0.5)));
	EXPECT_TRUE(!WorldRect(0,0,1,1).contains(WorldPoint(-0.5,0.5)));
	
	EXPECT_TRUE(WorldRect(WorldPoint(0,0), 1) == WorldRect(-1,-1,1,1));
	
	// rect intersect rect
	EXPECT_TRUE(WorldRect(0,0,1,1) + WorldRect(-1,-1, 0, 0.5) == WorldRect(-1,-1,1,1));
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldRect(0,0,1,1)));
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldRect(1,1,1,1)));
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldRect(0.9,0.9,2,2)));
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldRect(-0.2,-0.2,0.1,0.1)));
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldRect(-0.2,0.2,0.1,0.4)));
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldRect(0.2,-0.2,0.4,0.1)));
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldRect(0.2,0.2,0.4,0.4)));
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldRect(0.5,0.5,0.6,0.6)));		//	enclosed
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldRect(-1,-1,2,2)));		//	enclosing
	EXPECT_TRUE(!WorldRect(0,0,1,1).intersects(WorldRect(1.1,1.1,2,2)));

}

//----------------------------------------------------------------------------------------
//  TestLineIntersect
//
//	Test line intersections.
//
//  returns nothing
//----------------------------------------------------------------------------------------
TEST_F(CoordTest, TestLineIntersect)
{
	// line intersections
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldPoint(0,0),WorldPoint(0.5,0.5)));
	EXPECT_TRUE(!WorldRect(0,0,1,1).intersects(WorldPoint(1.0001,1.0001),WorldPoint(2,1)));
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldPoint(1.0001,1.0001),WorldPoint(0.99,0)));
	//	close on right
	EXPECT_TRUE(WorldRect(0,0,1,1).intersects(WorldPoint(1.0001,1.0001),WorldPoint(0.99,-100.01)));
	EXPECT_TRUE(!WorldRect(0,0,1,1).intersects(WorldPoint(1.0001,1.0001),WorldPoint(0.99,-100.011)));
	EXPECT_TRUE(!WorldRect(0,0,1,1).intersects(WorldPoint(0.99,-100.011), WorldPoint(1.0001,1.0001)));
	
	// close on left
	EXPECT_TRUE(WorldRect(1,1,2,3).intersects(WorldPoint(0.999,0.999), WorldPoint(1.01,23.01)));
	EXPECT_TRUE(!WorldRect(1,1,2,3).intersects(WorldPoint(0.999,0.999), WorldPoint(1.01,23.011)));
}

//----------------------------------------------------------------------------------------
//  TestDistLine
//
//	Test line-line distances.
//
//  returns nothing
//----------------------------------------------------------------------------------------
TEST_F(CoordTest, TestDistLine)
{
	WorldPoint pp;
	
	EXPECT_TRUE(pp.distLine(WorldPoint(-1,-1), WorldPoint(1,1)) == 0);
	//cerr << pp.distLine(WorldPoint(-1,-1), WorldPoint(1,1)) << '\n';
	EXPECT_TRUE(pp.distLine(WorldPoint(1,1), WorldPoint(-1,-1)) == 0);
	EXPECT_TRUE(pp.distLine(WorldPoint(0,1), WorldPoint(1,1)) == 1);
	EXPECT_TRUE(pp.distLine(WorldPoint(1,0), WorldPoint(1,1)) == 1);
	EXPECT_TRUE(pp.distLine(WorldPoint(2,2), WorldPoint(1,1)) == 0);
	EXPECT_TRUE(pp.distLine(WorldPoint(0,2), WorldPoint(3,6)) == 1.2);
	EXPECT_TRUE(WorldPoint(3,4).distLine(WorldPoint(0,2), WorldPoint(3,6)) == 1.2);
	//cerr << WorldPoint(3,4).distLine(WorldPoint(0,2), WorldPoint(3,6)) << '\n';
	EXPECT_TRUE(WorldPoint(-3,-4).distLine(WorldPoint(0,2), WorldPoint(3,6)) == 1.2);
	EXPECT_TRUE(fabs(WorldPoint(-3,-4).distLine(WorldPoint(0,2), WorldPoint(0.3,2.4)) - 1.2) <
				kTolerance);
}

//eof
