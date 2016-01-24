
//#if __MWERKS__
//#pragma check_header_flags on

// when building with the command line tools
//	we need to use precompiled headers build with the command line tools
//	as well. The makefile defines CMD_LINE_TOOL

//#define _MSL_USING_MW_C_HEADERS 1

//#ifdef CMD_LINE_TOOL
//#include <CmdLineHeaders.h>
//#else
//#include <MSL MacHeadersMach-O.h>
//#endif
//#endif

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

#define NDEBUG	1
#define DEBUG_NEW 0
