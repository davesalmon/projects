include $(BLD_ROOT)/Build/includes/Makefile.conf

DST_ROOT := common/recipes

CCPP_LIB := Recipes

CCPP_INCLUDES += \
	-I. 

HEADER := 		\
	DlArray.h	\
	DlMatrix.h	\
	DlVector.h	\
	GPSRenum.h	\
	cErr.h		\
	colsol.h	\
	eigen.h		\
	matio.h		\
	matrix.h	\
	matutil.h	\
	optimize.h	\
	random.h	\
	recipes.h	\
	stdcerr.h

CCPP_FILES := 		\
	DlVector.c		\
	GPSRenum.cpp	\
	ObjectCErr.c	\
	cholesky.c		\
	colsol.cpp		\
	eigen.c			\
	gauselim.c		\
	ludcmp.c		\
	ludcmpnp.c		\
	matio.c			\
	matutil.c		\
	optimize.c		\
	pivot.c			\
	random.c		\
	recipes.c		\
	svdcmp.c

include $(BLD_ROOT)/Build/includes/Makefile.defs

TESTS_LIBS += $(BUILD_OUTPUT)/common/recipes/lib/Recipes.lib $(BUILD_OUTPUT)/common/util/lib/Util.lib

include $(BLD_ROOT)/Build/includes/Makefile.rules

