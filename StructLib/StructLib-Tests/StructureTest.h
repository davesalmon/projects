/*+
 *
 *  StructureTests.h
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

#ifndef StructLib_StructureTest_h
#define StructLib_StructureTest_h

class StructLibTest : public testing::Test {

protected:
	
	using ActPtr = std::unique_ptr<Action>;

	virtual void SetUp() {
		if (!frame) {
#if _DEBUG
			DebugNewForgetLeaks();
#endif
			frame = NEW FrameStructure("default");
		}
	}
	
	virtual void TearDown() override {
	if (finalize) {
		delete frame;
		frame = nullptr;
		finalize = false;
#if _DEBUG
		DebugNewValidateAllBlocks();
		DebugNewReportLeaks();
#endif
	}
}

protected:

	const int ActLoadCase = 0;

	void addNode(DlFloat64 x, DlFloat64 y);
	void removeNode(DlInt32 index);
	void addRestraint(DlInt32 nodeIndex, DlUInt32 restCode);

	void addElement(DlInt32 node1, DlInt32 node2);
	void removeElement(DlInt32 index);

	void addLateralLoad(DlInt32 elemIndex, DlFloat load);
	void addLateralLoad(DlInt32 elemIndex, DlFloat load, LoadCase lc);
	void assignLateralLoad(DlInt32 loadIndex, LoadCase lc);

	void addJointLoad(DlInt32 nodeIndex, DlFloat xLoad, DlFloat yLoad, DlFloat zLoad);
	void addJointLoad(DlInt32 nodeIndex, DlFloat xLoad, DlFloat yLoad, DlFloat zLoad, LoadCase lc);

	static FrameStructure* frame;
	static bool finalize;
};


#endif
