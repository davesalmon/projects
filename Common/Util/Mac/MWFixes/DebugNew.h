/*
 *  DebugNew.h
 *
 *  Copyright © 1993-2002 Metrowerks Corporation.  All rights reserved.
 *
 *  $Date: 2003/07/13 14:22:25 $
 *  $Revision: 1.4 $
 * 
 *	A debugging layer over the standard operator new and delete
 *  to detect memory overwrites, incorrect deletes, and memory leaks.
 *  See DebugNew.doc in the "MSL Technical Notes" folder for background,
 *  usage instructions and release notes.
 */
 
#ifndef DebugNew_H
#define DebugNew_H

#include <new>			// Hsoi 990122 added for nothrow_t. Removed cstddef
						// and mcompile.h #includes as new #includes them already
#ifndef _CSTD
#define _CSTD std
#endif

#ifndef _STD
#define _STD std
#endif

#define DEBUG_NEW_OFF		0	// disabled
#define DEBUG_NEW_BASIC		1	// detect overwrites, heuristically detect bad deletes
#define DEBUG_NEW_LEAKS		2	// all basic testing, plus memory leaks

#ifndef DEBUG_NEW							// Default setting if not defined already. You can
	#define DEBUG_NEW	DEBUG_NEW_LEAKS 	// define DEBUG_NEW as 0,1 or 2 in your prefix or elsewhere.
#endif

	// Use NEW like this:			Foo *f = NEW Foo;
	// Use NEW_NOTHROW like this:	Foo *f = NEW_NOTHROW(std::nothrow) Foo;
#if DEBUG_NEW == DEBUG_NEW_OFF
  #define NEW new
  #define NEW_NOTHROW(x) new(x)  // hh 981221
  #define OPERATORNEW(x) operator new(x)
   
#elif DEBUG_NEW == DEBUG_NEW_BASIC
  #define NEW new
  #define NEW_NOTHROW(x) new(x)  // hh 981221
  #define OPERATORNEW(x) operator new(x)
   
#elif DEBUG_NEW == DEBUG_NEW_LEAKS
  #define NEW new(__FILE__, __LINE__)
  #define NEW_NOTHROW(x) new(x, __FILE__, __LINE__)  // hh 981221
  #define OPERATORNEW(x) operator new(x, __FILE__, __LINE__)

  			// Validate all allocated blocks, only available when leak checking is
  			// enabled.
  			
  void  DebugNewValidateAllBlocks();
#else
	#error DEBUG_NEW has an undefined value
#endif // DEBUG_NEW

void* operator new  (_CSTD::size_t size,                         const char*, int) throw(_STD::bad_alloc);  
void* operator new  (_CSTD::size_t size, const _STD::nothrow_t&, const char*, int) throw();  // hh 981221
void* operator new[](_CSTD::size_t size,                         const char*, int) throw(_STD::bad_alloc);  // Hsoi 980821- added
void* operator new[](_CSTD::size_t size, const _STD::nothrow_t&, const char*, int) throw();  // hh 981221

void  operator delete  (void*,                                   const char*, int) throw();
void  operator delete  (void*, const _STD::nothrow_t&,           const char*, int) throw();
void  operator delete[](void*,                                   const char*, int) throw();
void  operator delete[](void*, const _STD::nothrow_t&,           const char*, int) throw();
  
	// Try to validate that a pointer points to a valid, uncorrupted block.
	// Invokes error handler if validation fails. Will not work properly
	// for C++ arrays. Does nothing when DebugNew is disabled
	
void DebugNewValidatePtr(void*);

long DebugNewGetPtrSize(void*);

#if DEBUG_NEW >= DEBUG_NEW_BASIC

	// this variable holds the number of active allocations
	
extern unsigned long gDebugNewAllocCount;

	// this variable holds the total #bytes currently allocated via operator new(),
	// not including DebugNew overhead.
	
extern unsigned long gDebugNewAllocCurr;

	// this variable holds the maximum #bytes allocated at any given point in
	// the program execution, not counting DebugNew overhead. Useful for
	// memory tuning.
	
extern unsigned long gDebugNewAllocMax;

	// this variable holds flags that control run-time behavior
	// of DebugNew. The available flags are defined in the 
	// following enum.
	
extern unsigned long gDebugNewFlags;

enum	  	// gDebugNewFlags bits, refer to DebugNew.doc for information
{
	dnDontFreeBlocks = 1,	// don't release free blocks back to allocator,
							// provides extended checking if leak checking enabled
	dnCheckBlocksInApplZone = 2 // valid blocks must be in application zone
};


	// For use with class-specific operator new and operator delete.
	// Allows use of DebugNew's block checking with your own allocator.
	//
	// Call DebugNewDoAllocate from your operator new, passing 
	// in the file, line, a pointer to a function that
	// allocates the necessary amount of memory, and whether this
	// is an array new or not. To actually 
	// track the file and line you must also provide:
	//   void* operator new(size_t size, const char*, int);  
	// for the class. Otherwise pass NULL for the file and
	// zero for the line number.
	//
	// Call DebugNewDoFree from your operator delete, passing in
	// a pointer to a function to deallocate the memory.

void* DebugNewDoAllocate(_CSTD::size_t size, const char* file, int line,
						void* (*alloc_func)(_CSTD::size_t size), bool is_array = false);
void* DebugNewDoAllocate(_CSTD::size_t size, const char* file, int line,
						void* (*alloc_func)(_CSTD::size_t size), const _STD::nothrow_t&, bool is_array = false);  // hh 981221
void  DebugNewDoFree(void* ptr, void (*free_func)(void* ptr), bool is_array = false);

	// Call to write memory leak tracking status to a file called
	// "leaks.log" in the application directory. Returns the
	// number of leaks.
	
long DebugNewReportLeaks(const char* name=0);

	// Call to tell DebugNew to ignore any currently allocated
	// blocks in the leak report. Useful at app startup to ignore
	// memory allocated at startup that doesn't get explicitly
	// freed.
	
void DebugNewForgetLeaks();

	// You can optionally provide an error handler to be called when
	// errors are detected. The default routine just calls DebugStr.
	// Call DebugNewSetErrorHandler to set handler, it returns the
	// old handler.
	
typedef void (*DebugNewErrorHandler_t)(short);

DebugNewErrorHandler_t	DebugNewSetErrorHandler(DebugNewErrorHandler_t newHandler);

enum DebugNewError_t			// error codes passed to error handler
{
	dbgnewNullPtr,				// validate called with NULL pointer
	dbgnewTooManyFrees,			// #deletes doesn't match #news, too many deletes
	dbgnewPointerOutsideHeap,	// validate or free call for pointer outside application heap
	dbgnewFreeBlock,			// validate or delete called for free block
	dbgnewBadHeader,			// validate or delete called for block with bad header. 
								// Bad ptr or overwritten header, or delete[] called on
								// block allocated with new.
	dbgnewBadTrailer,			// validate or delete called with block whose trailer was overwritten
	dbgnewBlockNotInList,		// validate or delete called with block with valid header/trailer, 
								// but block is not in block list (internal error?)
	dbgnewFreeBlockOverwritten, // dnDontFreeBlocks was enabled, and a free block was
								// modified. Usually indicates a write through a dangling pointer.
	dbgnewMismatchedNewDelete	// Hsoi 980821 - delete called for memory allocated with new[],
								// (or perhaps delete[] on new).
};	

#endif  // DEBUG_NEW >= DEBUG_NEW_BASIC

#endif	// DebugNew_H

// hh   980126 changed from <stddef.h>
// hh   980126 added <mcompile.h>
// hh   980126 added MSIPLSTD:: to size_t in 3 places
// Hsoi 980821 Changed MSIPLSTD to _STD
// Hsoi 980821 Added is_array argument to DoAllocate and DoFree
// Hsoi 980821 Added dbgnewMismatchedNewDelete
// hh   981221 Added new(nothrow)
// Hsoi 990122 Added #include <new>. Removed #include <mcompile.h>, <cstddef> as <new> #includes them
// hh   000118 Rewrote to use malloc/free
// JWW  010116 Changed _STD::size_t to _CSTD::size_t
// hh   010124 Added missing delete operators
// JWW  011108 Moved new and delete prototypes outside the check for DEBUG_NEW == DEBUG_NEW_LEAKS
