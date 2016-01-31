/*+
 *
 *  StructureTest.cpp
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

#include "gtest/gtest.h"

#include "FrameStructure.h"
#include "Node.h"
#include "Element.h"
#include "WorldPoint.h"
#include "NodeEnumerator.h"
#include "ElementEnumerator.h"
#include "Action.h"
#include "ElementLoad.h"
#include "LoadCaseResults.h"
#include "NodeLoad.h"

#include "StructureTest.h"

FrameStructure* StructLibTest::frame {nullptr};
bool StructLibTest::finalize {false};

#if _DEBUG
	int StructLibTest::frame_count {1};
static const char* kStructTestPath { "/tmp/leaks-StructTest%d.log" };
#endif

void
StructLibTest::SetUp()
{
	if (!frame) {
#if _DEBUG
		DebugNewForgetLeaks();
#endif
		frame = NEW FrameStructure("default");
	}
}

void
StructLibTest::TearDown()
{
	if (finalize) {
		delete frame;
		frame = nullptr;
		finalize = false;
		
#if _DEBUG
		DebugNewValidateAllBlocks();
		
		char buf[256];
		snprintf(buf, sizeof(buf), kStructTestPath, frame_count++);
		
		DebugNewReportLeaks(buf);
#endif
	}
}

//----------------------------------------------------------------------------------------
//  StructLibTest::addNode
//
//      add a node to the frame.
//
//  DlFloat64 x    -> 
//  DlFloat64 y    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::addNode(DlFloat64 x, DlFloat64 y)
{
	Node n1;
	WorldPoint pt1 {x, y};
	
	ActPtr(frame->AddNode(pt1, n1))->Perform();
}

//----------------------------------------------------------------------------------------
//  StructLibTest::removeNode
//
//      remove a node from the frame.
//
//  DlInt32 index  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::removeNode(DlInt32 index)
{
	ActPtr(frame->RemoveNodes({frame->GetNode(index)}, {}))->Perform();
}

//----------------------------------------------------------------------------------------
//  StructLibTest::addRestraint
//
//      add a restraint to a node.
//
//  DlInt32 nodeIndex  -> the node
//  DlUInt32 restCode  -> the restraint.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::addRestraint(DlInt32 nodeIndex, DlUInt32 restCode)
{
	ActPtr(frame->SetRestraints({frame->GetNode(nodeIndex)}, restCode))->Perform();
}


//----------------------------------------------------------------------------------------
//  StructLibTest::addElement
//
//      add an element to the frame.
//
//  DlInt32 node1  -> node 1
//  DlInt32 node2  -> node 2
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::addElement(DlInt32 node1, DlInt32 node2)
{
//	NodeEnumerator nodes = frame->GetNodes();
	Element elem1(0);
	ActPtr(frame->AddElement(frame->GetNode(node1), frame->GetNode(node2),
								frame->GetActiveElementType(), elem1))->Perform();
}

//----------------------------------------------------------------------------------------
//  StructLibTest::removeElement
//
//      remove an element from the frame.
//
//  DlInt32 index  -> the element.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::removeElement(DlInt32 index)
{
	ActPtr(frame->RemoveNodes({}, {frame->GetElement(index)}))->Perform();
}

//----------------------------------------------------------------------------------------
//  StructLibTest::addLateralLoad
//
//      add a lateral load to an element using the active load case.
//
//  DlInt32 elemIndex  -> the element
//  DlFloat loadValue  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::addLateralLoad(DlInt32 elemIndex, DlFloat loadValue)
{
	addLateralLoad(elemIndex, loadValue, ActLoadCase);
}

//----------------------------------------------------------------------------------------
//  StructLibTest::addLateralLoad
//
//      add a lateral load to an element.
//
//  DlInt32 elemIndex  -> the element.
//  DlFloat loadValue  -> the load value.
//  LoadCase lc        -> the load case.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::addLateralLoad(DlInt32 elemIndex, DlFloat loadValue, LoadCase lc)
{
	_DlAssert(elemIndex < frame->GetElements().Length());
	// add a load to the element
	ElementLoad load(0);
	
	ActPtr(frame->CreateElementLoad(load))->Perform();
	
	load.SetValue(ElementLoadLateral, loadValue);
	
	frame->SetActiveLoadCase(lc);
	ActPtr(frame->AssignElementLoads(
			{frame->GetElement(elemIndex)}, load))->Perform();
}

//----------------------------------------------------------------------------------------
//  StructLibTest::assignLateralLoad
//
//      assign a lateral load to every element.
//
//  DlInt32 loadIndex  -> the index of the load.
//  LoadCase lc        -> the load case to use.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::assignLateralLoad(DlInt32 loadIndex, LoadCase lc)
{
	frame->SetActiveLoadCase(lc);
	ActPtr(frame->AssignElementLoads(
			{frame->GetElements()},
			{frame->GetElementLoads().At(loadIndex)}))->Perform();
}

//----------------------------------------------------------------------------------------
//  StructLibTest::addJointLoad
//
//      create a new joint load and assign it to the node.
//
//  DlInt32 nodeIndex  -> the node.
//  DlFloat xLoad      -> the x load.
//  DlFloat yLoad      -> the y load.
//  DlFloat zLoad      -> the z load.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::addJointLoad(DlInt32 nodeIndex, DlFloat xLoad, DlFloat yLoad, DlFloat zLoad)
{
	addJointLoad(nodeIndex, xLoad, yLoad, zLoad, ActLoadCase);
}

//----------------------------------------------------------------------------------------
//  StructLibTest::addJointLoad
//
//      create a new joint load.
//
//  DlInt32 nodeIndex  -> the node
//  DlFloat xLoad      -> the x comp
//  DlFloat yLoad      -> the y comp
//  DlFloat zLoad      -> the m comp
//  LoadCase lc        -> the load case.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
StructLibTest::addJointLoad(DlInt32 nodeIndex, DlFloat xLoad, DlFloat yLoad, DlFloat zLoad, LoadCase lc)
{
	_DlAssert(nodeIndex < frame->GetNodes().Length());
	NodeLoad load(0);

	ActPtr(frame->CreateNodeLoad(load))->Perform();
	
	load.SetValue(0, xLoad);
	load.SetValue(1, yLoad);
	load.SetValue(2, zLoad);
	
	frame->SetActiveLoadCase(lc);
	ActPtr action(frame->AssignNodeLoads({frame->GetNode(nodeIndex)}, load));
	action->Perform();
}

//----------------------------------------------------------------------------------------
//  printDisplacement                                                              static
//
//      print the displacement.
//
//  Node n     -> the node
//  DlInt32    -> ...
//  void* ctx  -> the context.
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void
printDisplacement(Node n, DlInt32, void* ctx)
{
	const LoadCaseResults*res = static_cast<LoadCaseResults*>(ctx);
	DlFloat64 disp[DOF_PER_NODE];
	res->GetDisplacement(n, disp);

	printf("\n(%lf, %lf, %lf)", disp[0], disp[1], disp[2]);
}

#pragma mark -
#pragma mark Define tests
#pragma mark -

//----------------------------------------------------------------------------------------
//  AddRemoveLoads
//
//	Test adding and removing loads
//
//  returns nothing
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest,AddRemoveLoads)
{
	// create two nodes
	addNode(0.0, 0.0);
	addNode(1.0, 0.0);
	
	// add an element
	addElement(0, 1);
	
	// add a lateral load (in addition to the 0 one) and check the count.
	addLateralLoad(0, 2.0);
	EXPECT_EQ(frame->GetElementLoads().Length(), 2);
	
	// check that the load at index 1 is the correct one.
	ElementLoad l = frame->GetElementLoads().At(1);
	EXPECT_TRUE(l);
	
	// check that the load is actually assinged.
	
	EXPECT_EQ(frame->GetElement(0).GetLoad(0), l);
	
	// unassign the load
	ActPtr(frame->AssignElementLoads({frame->GetElement(0)}, ElementLoad()))->Perform();
	EXPECT_FALSE(frame->GetElement(0).GetLoad(0));
	
	// remove the load from the list and check
	ActPtr(frame->RemoveElementLoad(l))->Perform();
	EXPECT_EQ(frame->GetElementLoads().Length(), 1);
	
	// add a lateral load (in addition to the 0 one) and check the count.
	ActPtr(frame->CreateLoadCase("test"))->Perform();
	addLateralLoad(0, 2.0);
	addLateralLoad(0, 3.0, 1);

	DlFloat64 actLoad = frame->GetElement(0).GetLoad(0).GetValue(ElementLoadLateral, false).getFloat();
	EXPECT_EQ(actLoad, 2.0);
	
	actLoad = frame->GetElement(0).GetLoad(1).GetValue(ElementLoadLateral, false).getFloat();
	EXPECT_EQ(actLoad, 3.0);

	// unassign the first load
	frame->SetActiveLoadCase(0);
	ActPtr(frame->AssignElementLoads({frame->GetElement(0)}, ElementLoad()))->Perform();
	EXPECT_FALSE(frame->GetElement(0).GetLoad(0));
	
	actLoad = frame->GetElement(0).GetLoad(1).GetValue(ElementLoadLateral, false).getFloat();
	EXPECT_EQ(actLoad, 3.0);

	addJointLoad(0, 1.0, 2.0, 3.0);
	addJointLoad(1, 4.0, 5.0, 6.0);
	
	actLoad = frame->GetNode(0).GetLoad(0).GetValue(1, false).getFloat();
	EXPECT_EQ(actLoad, 2.0);

	actLoad = frame->GetNode(1).GetLoad(0).GetValue(1, false).getFloat();
	EXPECT_EQ(actLoad, 5.0);

	frame->SetActiveLoadCase(0);
	ActPtr(frame->AssignNodeLoads({frame->GetNode(0)}, NodeLoad()))->Perform();

	NodeLoad nl = frame->GetNode(0).GetLoad(0);
	EXPECT_FALSE(nl);
	
	finalize = true;
}

//----------------------------------------------------------------------------------------
//  AddRemoveLoadCombos
//
//	Test adding and removing loads
//
//  returns nothing
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest,AddRemoveLoadCombos)
{
	// to load case 1
	ActPtr(frame->CreateLoadCase("test"))->Perform();
	EXPECT_EQ(frame->GetLoadCaseCount(), 2);

	// to load case 2
	ActPtr(frame->CreateLoadCase("test2"))->Perform();
	EXPECT_EQ(frame->GetLoadCaseCount(), 3);
	
	// create two nodes
	addNode(0.0, 0.0);
	addNode(0.5, 0.0);
	addNode(1.0, 0.0);
	
	// add an element
	addElement(0, 1);
	addElement(1, 2);

	// add a lateral load (in addition to the 0 one) and check the count.
	addLateralLoad(0, 1.0, 0);
	addLateralLoad(0, 2.0, 1);
	addLateralLoad(0, 3.0, 2);
	
	addLateralLoad(1, 4.0, 0);
	addLateralLoad(1, 5.0, 1);
//	addLateralLoad(1, 6.0, 2);

	addJointLoad(1, 1.0, 2.0, 3.0, 0);
	addJointLoad(1, 4.0, 5.0, 6.0, 1);
	addJointLoad(1, 7.0, 8.0, 9.0, 2);

// ----------- lc 3 --------------
	ActPtr(frame->AddLoadCaseCombination("test3",
					LoadCaseCombination(std::vector<DlFloat32>{1.0, 0.0, 1.0})))->Perform();
	EXPECT_EQ(frame->GetLoadCaseCount(), 4);

	Element e(frame->GetElement(0));
	
	// check that the load at index 1 is the correct one.
	ElementLoad l = e.GetLoad(3);
	EXPECT_TRUE(l);
	
	if (l) {
		DlFloat64 actLoad = l.GetValue(ElementLoadLateral, false).getFloat();
		EXPECT_EQ(actLoad, 4.0);
	}

	Node n(frame->GetNode(1));
	NodeLoad nl(n.GetLoad(3));
	EXPECT_TRUE(nl);
	if (nl) {
		DlFloat64 actLoad = nl.GetValue(0, false).getFloat();
		EXPECT_EQ(actLoad, 8.0);
		EXPECT_EQ(nl.GetValue(2, false).getFloat(), 12.0);
	}

	EXPECT_FALSE(frame->GetNode(0).GetLoad(3));
	
	EXPECT_TRUE(frame->GetElement(1).GetLoad(3));

// ----------- lc 4 --------------
	ActPtr(frame->AddLoadCaseCombination("test4",
					LoadCaseCombination(std::vector<DlFloat32>{0.0, 0.0, 2.0})))->Perform();
	EXPECT_EQ(frame->GetLoadCaseCount(), 5);

	// check that the load at index 1 is the correct one.
	l = e.GetLoad(4);
	EXPECT_TRUE(l);
	
	if (l) {
		DlFloat64 actLoad = l.GetValue(ElementLoadLateral, false).getFloat();
		EXPECT_EQ(actLoad, 6.0);
	}
	
	nl = n.GetLoad(4);
	EXPECT_TRUE(nl);
	if (nl) {
		DlFloat64 actLoad = nl.GetValue(0, false).getFloat();
		EXPECT_EQ(actLoad, 14.0);
		EXPECT_EQ(nl.GetValue(1, false).getFloat(), 16);
		EXPECT_EQ(nl.GetValue(2, false).getFloat(), 18.0);
	}
	
	EXPECT_FALSE(frame->GetNode(0).GetLoad(3));
	EXPECT_FALSE(frame->GetElement(1).GetLoad(4));

// --------- remove load case
	
//	ActPtr(frame->Remove
	
	{
		ActPtr ptr = ActPtr(frame->AddLoadCaseCombination("test5",
										 LoadCaseCombination(std::vector<DlFloat32>{0.1, 0.6, 2.0})));
		ptr->Perform();
		EXPECT_EQ(frame->GetLoadCaseCount(), 6);
		EXPECT_TRUE(e.GetLoad(5));
		EXPECT_TRUE(n.GetLoad(5));
		
		ptr->Perform(); // undo
		EXPECT_EQ(frame->GetLoadCaseCount(), 5);
		EXPECT_FALSE(e.GetLoad(5));
		EXPECT_FALSE(n.GetLoad(5));
	}
	
	finalize = true;
}

//----------------------------------------------------------------------------------------
//  BuildComplex
//
//		Build a frame.
//
//		1    2
//		0    3
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest, BuildComplex)
{
	// add four nodes
	addNode(0.0, 0.0);
	addNode(0.0, 1.0);
	addNode(1.0, 1.0);
	addNode(1.0, 0.0);
	
	//	NodeEnumerator nodes = frame->GetNodes();
	
	// add four elements two walls, roof and brace.
	addElement(0, 1);	// column
	addElement(1, 2);	// beam
	addElement(2, 3);	// column
	addElement(0, 2);	// brace
	
	// create and assign lateral load to column 0
	addLateralLoad(0, 1.0);
	// assign it to all elements.
	assignLateralLoad(1, 0);

	// restrain node 0 in the x and y directions
	addRestraint(0, Node::FixX | Node::FixY);
	// and node 2 in the y direction.
	addRestraint(2, Node::FixY);

	// add a joint load to node 1 in the x direction in load case 1
	//addJointLoad(1, 1, 0, 0, 1);
	
	ASSERT_TRUE(frame->CanAnalyze());
	
	AnalysisData theData;
	frame->InitAnalysis(&theData);

	ASSERT_NO_THROW(frame->Analyze(0));
	
	frame->SetActiveLoadCase(0);
	const LoadCaseResults* res = frame->GetResults();

	frame->DoForEachNode(printDisplacement, (void*)res);

//	operator new (256);
	
	finalize = true;

}

//----------------------------------------------------------------------------------------
//  SBAddRemoveNodes
//
//      Testing adding and removing nodes.
//      Step one of simple beam structure.
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest,SBAddRemoveNodes)
{
	addNode(0.0, 0.0);
	addNode(1.0, 0.0);
	
	addNode(2.0, 0.0);
	
	removeNode(2);
		
	//	NodeEnumerator nodes = frame->GetNodes();

	EXPECT_EQ(frame->CountNodes(), 2);

	EXPECT_TRUE(frame->GetNode(0).GetCoords() == WorldPoint(0.0, 0.0));
}

//----------------------------------------------------------------------------------------
//  SBAddRemoveElements
//
//      Test adding and removing elements.
//		Step 2 of simple beam
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest,SBAddRemoveElements)
{
	//	NodeEnumerator nodes = frame->GetNodes();
	// add an element connecting the two nodes.
	addElement(0, 1);

	EXPECT_EQ(frame->CountElements(), 1);

	addElement(1, 0);

	ASSERT_EQ(frame->CountElements(), 2);

	EXPECT_TRUE(frame->GetElement(0).StartNode() == frame->GetNode(0));
	EXPECT_TRUE(frame->GetElement(0).EndNode() == frame->GetNode(1));
	
	removeElement(1);
	
	ASSERT_EQ(frame->CountElements(), 1);
	
	EXPECT_TRUE(frame->GetElement(0).StartNode() == frame->GetNode(0));
	EXPECT_TRUE(frame->GetElement(0).EndNode() == frame->GetNode(1));
}

//----------------------------------------------------------------------------------------
//  SBAddLoad
//
//      Test adding loads. Adds a lateral load of unit force/length.
//		Step 3 of simple beam.
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest,SBAddLoad)
{
	addLateralLoad(0, 1.0);

	DlFloat64 actLoad = frame->GetElement(0).GetLoad(ActLoadCase)
			.GetValue(ElementLoadLateral, false).getFloat();
	
	ASSERT_EQ(1.0, actLoad);
}

//----------------------------------------------------------------------------------------
//  SBAddRestraints
//
//      Test adding restraints to the structure
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest,SBAddRestraints)
{
	addRestraint(0, Node::FixX | Node::FixY);
	addRestraint(1, Node::FixY);
	
	ASSERT_EQ(frame->GetNode(0).GetRestraint(), Node::FixX | Node::FixY);
	ASSERT_EQ(frame->GetNode(1).GetRestraint(), Node::FixY);
}

// analyze a simple beam
//----------------------------------------------------------------------------------------
//  SBAnalyze
//
//      Analyse the simple beam. Check the results against known values.
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest,SBAnalyze)
{
	ASSERT_TRUE(frame->CanAnalyze());
	// analyse the structure. In this case we have a simple beam with
	// a uniform load of one, length of one and stiffness (EI) of 1. The moment
	// at the center is wL^2/8 which is 0.125. Deflection at center is
	// 5 w L^4 / (384 EI)
	
	AnalysisData theData;
	frame->InitAnalysis(&theData);
	
	ASSERT_NO_THROW(frame->Analyze(0));
	
	const Element& elem = frame->GetElement(0);
	frame->SetActiveLoadCase(0);
	const LoadCaseResults* res = frame->GetResults();
	
	const ElementForce& frc = res->GetElementForce(0);
	
	DlFloat64 value = elem.GetResultValue(8, frc, *res);
	
	EXPECT_EQ(value, 0.125);
	
	value = elem.DOFDispAt(1, 0, 0.5, *res);
	
//	value = elem.LateralDisplacementAt(frc, 0.5, *res);
	
	EXPECT_TRUE(value + 5.0/384.0 < 1.0e-9);
	
//	printf("value is %lf\n", value);
	
	finalize = true;
}


// Anaylze a larger structure (simple beam with 1000 elements).
const int MAX_NODES = 10001;
const DlFloat64 NODE_OFFSET = 1.0 / (MAX_NODES - 1);

//----------------------------------------------------------------------------------------
//  BuildLarger
//
//      build a simple beam with 1000 elements.
//		assign unit lateral load in lc 0, and a central point load in lc 1.
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest, BuildLarger)
{
	DlFloat64 loc = 0;
	for (auto i = 0; i < MAX_NODES; i++)
	{
		addNode(loc, 0.0);
		loc += NODE_OFFSET;
	}
	
	//	NodeEnumerator nodes = frame->GetNodes();
	
	for (auto i = 0; i < MAX_NODES - 1; i++)
	{
		addElement(i, i+1);
	}
	
	addLateralLoad(0, 1.0);
	assignLateralLoad(1, 0);

	addRestraint(0, Node::FixX | Node::FixY);
	addRestraint(MAX_NODES - 1, Node::FixY);

	// to load case 1
	ActPtr(frame->CreateLoadCase("test"))->Perform();
	
	ASSERT_EQ(frame->GetLoadCaseCount(), 2);
	
	addJointLoad((MAX_NODES - 1) / 2, 0, -1, 0, 1);
	
	ASSERT_TRUE(frame->CanAnalyze());
}

//----------------------------------------------------------------------------------------
//  PrepareLarger
//
//      init the analysis
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest,PrepareLarger)
{
	AnalysisData theData;
	frame->InitAnalysis(&theData);
}

//----------------------------------------------------------------------------------------
//  SolveLarger
//
//      Solve the equations.
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest, SolveLarger)
{
	ASSERT_NO_THROW(frame->Analyze(0));
}

//----------------------------------------------------------------------------------------
//  ResultsLarger
//
//      Check the results of the larger structure.
//
//----------------------------------------------------------------------------------------
TEST_F(StructLibTest,ResultsLarger)
{
	DlInt32 elemIndex = (MAX_NODES - 1) / 2;
	const Element& elem = frame->GetElement(elemIndex);
	
	{
		frame->SetActiveLoadCase(0);
		const LoadCaseResults* resLateral = frame->GetResults();
		
		const ElementForce& frc = resLateral->GetElementForce(elemIndex);
		
		DlFloat64 value = elem.GetResultValue(2, frc, *resLateral);

		EXPECT_LE(value - 0.125, 1.0e-7);

		value = elem.DOFDispAt(1, elemIndex, 0.0, *resLateral);

		//value = elem.LateralDisplacementAt(frc, 0.0, *resLateral);

		EXPECT_LE(value + 5.0/384.0, 1.0e-5);
	}
	
	{
		frame->SetActiveLoadCase(1);
		// solution is PL/4 for moment, and P L^3 / 48EI
		const LoadCaseResults* resPoint = frame->GetResults();
		
		const ElementForce& frcPoint = resPoint->GetElementForce(elemIndex);
		
		DlFloat64 value = elem.GetResultValue(2, frcPoint, *resPoint);

		EXPECT_LE(value - 0.25, 1.0e-7);

		value = elem.DOFDispAt(1, elemIndex, 0.0, *resPoint);
		//value = elem.LateralDisplacementAt(frcPoint, 0.0, *resPoint);

		//printf("value is %lf\n", value);

		EXPECT_LE(value + 1.0/48, 1.0e-5);
	}

	finalize = true;
}


//eof

