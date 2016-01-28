
include $(BLD_ROOT)/Build/includes/Makefile.conf

CCPP_FILES_SRC_DIR := ./Src
#HEADER_SRC_DIR := ./Common
DST_ROOT := structlib

CCPP_LIB := StructLib

CCPP_INCLUDES += \
	-I. -I./Interface -I./Src -I$(BLD_ROOT)/Common/Recipes

CCPP_FILES :=					\
	AddElementAction.cpp		\
	AddElemLoadAction.cpp		\
	AddNodeAction.cpp			\
	AddNodeLoadAction.cpp		\
	MoveNodeAction.cpp			\
	BaseEnumerator.cpp			\
	Displacement.cpp			\
	Element.cpp					\
	ElementEnumerator.cpp		\
	ElementImp.cpp				\
	ElementList.cpp				\
	ElementLoad.cpp				\
	ElementLoadEnumerator.cpp	\
	ElemLoadImp.cpp				\
	ElemLoadList.cpp			\
	frame_data.cpp				\
	FrameStructure.cpp			\
	LoadCaseResults.cpp			\
	Node.cpp					\
	NodeEnumerator.cpp			\
	NodeImp.cpp					\
	NodeList.cpp				\
	NodeLoad.cpp				\
	NodeLoadEnumerator.cpp		\
	NodeLoadImp.cpp				\
	NodeLoadList.cpp			\
	PasteStructureAction.cpp	\
	PointEnumerator.cpp			\
	PointEnumeratorImp.cpp		\
	PrismElement.cpp			\
	Property.cpp				\
	PropertyAction.cpp			\
	PropertyEnumerator.cpp		\
	PropertyImp.cpp				\
	PropertyList.cpp			\
	PropertyTypeEnumerator.cpp	\
	PropertyTypeList.cpp		\
	RemoveNodeAction.cpp		\
	StitchAction.cpp			\
	StringEnumerator.cpp		\
	StringList.cpp				\
	UnitTable.cpp				\
	WorldPoint.cpp				\
	WorldRect.cpp

HEADER := 							\
	Interface/Action.h				\
	Interface/Displacement.h		\
	Interface/Element.h				\
	Interface/ElementEnumerator.h	\
	Interface/ElementLoad.h			\
	Interface/ElementLoadEnumerator.h	\
	Interface/Enumerator.h			\
	Interface/FrameStructure.h		\
	Interface/Node.h				\
	Interface/NodeEnumerator.h		\
	Interface/NodeLoad.h			\
	Interface/NodeLoadEnumerator.h	\
	Interface/PointEnumerator.h		\
	Interface/Property.h			\
	Interface/PropertyEnumerator.h	\
	Interface/Settlement.h			\
	Interface/StrDefines.h			\
	Interface/StrErrCode.h			\
	Interface/StrMessage.h			\
	Interface/StringEnumerator.h	\
	Interface/StrInputStream.h		\
	Interface/StrOutputStream.h		\
	Interface/TextInputStream.h		\
	Interface/UnitTable.h			\
	Interface/WorldPoint.h			\
	Interface/WorldRect.h			\
	Src/AddElementAction.h			\
	Src/AddElemLoadAction.h			\
	Src/AddNodeAction.h				\
	Src/AddNodeLoadAction.h			\
	Src/MoveNodeAction.h			\
	Src/BaseEnumerator.h			\
	Src/ElementImp.h				\
	Src/ElementList.h				\
	Src/ElemLoadImp.h				\
	Src/ElemLoadList.h				\
	Src/EnumeratorImp.h				\
	Src/frame_data.h				\
	Src/LoadCaseResults.h			\
	Src/NodeImp.h					\
	Src/NodeList.h					\
	Src/NodeLoadImp.h				\
	Src/NodeLoadList.h				\
	Src/PasteStructureAction.h		\
	Src/PointEnumeratorImp.h		\
	Src/PrismElement.h				\
	Src/PropertyAction.h			\
	Src/PropertyImp.h				\
	Src/PropertyList.h				\
	Src/PropertyTypeEnumerator.h	\
	Src/PropertyTypeList.h			\
	Src/RemoveNodeAction.h			\
	Src/StitchAction.h				\
	Src/StringList.h				\
	Src/ValueIter.h					\
	Src/wingraphics.h

CCPP_PREFIX := structure.h

include $(BLD_ROOT)/Build/includes/Makefile.defs

TESTS_LIBS += $(BUILD_OUTPUT)/structlib/lib/StructLib.lib  \
			  $(BUILD_OUTPUT)/common/recipes/lib/Recipes.lib \
              $(BUILD_OUTPUT)/common/util/lib/Util.lib

#CCPP_COMPILER_ARGS += -DTARGET_COMMAND_LINE

include $(BLD_ROOT)/Build/includes/Makefile.rules
