/*
 *  DebugNew.cp
 *
 *  Copyright © 1993-2001 Metrowerks Inc.  All rights reserved.
 *
 *  $Date: 2003/07/13 14:22:25 $
 *  $Revision: 1.4 $
 * 
 *	A debugging layer over the standard operator new and delete
 *  to detect memory overwrites, incorrect deletes, and memory leaks.
 *  See DebugNew.doc in the DebugNew Demo example (on the reference CD)
 *  for background, usage instructions, and release notes.
 *
 * NOTES: I fixed this up for MACH-O. You must move this in the link order to 
 *	come before the standard libraries...
 */

#pragma only_std_keywords off
#pragma ANSI_strict off

#include "DebugNew.h"
#include <Carbon/Carbon.h>
//#include <MacTypes.h>
//#include <MacMemory.h>
//#include <LowMem.h>
#include <cstdlib>

#define NEWMODE_SIMPLE	1			//	call NewPtr/DisposPtr
#define NEWMODE_MALLOC	2			//	use malloc/free
#define NEWMODE_NORMAL	3			//	use New.cp new/delete               | Set NEWMODE in
#define NEWMODE_FAST	4			//	use New.cp new/delete fast version  | New.cp to same!

#ifndef NEWMODE
	#if TARGET_RT_MAC_CFM && TARGET_CPU_68K
		#define NEWMODE NEWMODE_FAST		//  workaround for CFM68K shared lib runtimes
	#else
		#define NEWMODE NEWMODE_MALLOC		//	mode used to compile this file
	#endif
#endif


#if DEBUG_NEW >= DEBUG_NEW_BASIC

	// For every block allocated, we bump up the size to allow a header
	// and trailer to be put on the block. The header has bookkeeping info
	// to allow us to know if the block was probably a real allocated block,
	// and size info to allow us to find the trailer. The trailer is used to
	// detect overwrites.
	// NOTE: magic, unlikely values are used to tag blocks. This is a heuristic
	// only, since programs could legitimately be writing those same values through
	// wild pointers. When leak testing is on we can do better, since we have a
	// list of allocated blocks. The tag method provides reasonable checking 
	// with less overhead.
	
	struct BlockHeader
	{
	#if DEBUG_NEW == DEBUG_NEW_LEAKS
		BlockHeader* next;
		const char*  file;
		int			 line;
		unsigned long nAlloc;	//	dcs - added alloc number
	#endif
		_CSTD::size_t	size;		// size of user's block in bytes, doesn't include prefix/suffix
		_CSTD::size_t	padSize;	// Hsoi 980821 - size of compiler-inserted padding, if any.
		unsigned long			tag;		// magic value identifying the block
	};
	
	struct BlockTrailer
	{
		unsigned long tag;     	  // magic value at end, used to detect overwrites
	};
	
	enum BlockStatus_t	
	{
		blockValid,		  		// valid allocated block
		blockFree,		  		// free block
		blockPtrNull,	  		// block pointer is NULL
		blockPtrOutOfRange, 	// block pointer outside application heap
		blockBadHeader,			// block has invalid header
		blockBadTrailer,		// block has invalid trailer
		blockNotInList,			// block appears valid, but is not in block list (leak checking only)
		blockFreeOverwritten, 	// dnDontFreeBlocks enabled, zapped free block was overwritten
		blockDeleteNotMatch		// Hsoi 980821 - called new and delete[] or new[] and delete
	};	

	
static void 			DefaultErrorHandler(short);
static void 			ZapBlock(void* ptr, register _CSTD::size_t len, uint32_t value);
static BlockStatus_t 	GetBlockStatus(char* ptr, Boolean fValidateFree);
static void 			ReportBlockStatus(BlockStatus_t status);
static BlockHeader* 	GetHeader(void* ptr);
static void*			FinishAllocate(char* ptr, _CSTD::size_t size, const char* file, int line, bool is_array);


	// global data used by DebugNew
	
unsigned long 						gDebugNewAllocCount; // these three variables are public
unsigned long						gDebugNewAllocCurr;  // total #bytes currently allocated
unsigned long						gDebugNewAllocMax;	 // max #bytes ever allocated

//static void* 						gHeapBegin;
//static void* 						gHeapEnd;
static DebugNewErrorHandler_t		gErrorHandler = DefaultErrorHandler;

#if DEBUG_NEW == DEBUG_NEW_LEAKS
unsigned long						gSequentialBlockCount;	// dcs -sequential counter.

	// With leak checking enabled, we must store a list of all
	// allocated blocks so we can tell what has been leaked.
	// The block list is stored as a hash table, with the key
	// being the block address.

#pragma warn_implicitconv off
const int	kHashTableSize		=	499; // a prime number
static BlockHeader* 				gBlockHash[kHashTableSize];
static inline int Hash(const void* p)
{
	// shift to smallest alloc size and truncate to 32 bits
	uint32 tmp = ((unsigned long long)p>>4);
	return tmp % kHashTableSize;
}
#pragma warn_implicitconv reset
#endif


unsigned long						gDebugNewFlags = dnCheckBlocksInApplZone;

#define	  BLOCK_HEADER_SIZE	(sizeof(BlockHeader))
#define	  BLOCK_TRAILER_SIZE (sizeof(BlockTrailer))
#define	  BLOCK_OVERHEAD	(BLOCK_HEADER_SIZE + BLOCK_TRAILER_SIZE)

// Block zapping values. Newly allocated and free blocks are filled with
// these values to detect uses of uninitialize memory and released memory.
// Different values are used so you can look at a piece of memory
// in the debugger and tell if it is allocated but uninitialized or
// if it was released.
#define	  ZAP_UNINITIALIZED	0xF1F1F1F1L
#define	  ZAP_RELEASED		0xF3F3F3F3L


// some random, hopefully not too common, odd values 
#define	  BLOCK_PREFIX_ALLOC		0xD1F3D1F3L
#define	  BLOCK_PREFIX_ALLOC_ARRAY	0xD1F5D1F5L		// Hsoi 980821 added
#define	  BLOCK_PREFIX_FREE			0xF7B9F7B9L
#define	  BLOCK_SUFFIX				0xB7D5B7D5L

#ifndef DEBUG_NEW_NO_GLOBAL_OPERATORS

typedef void* (*_op_new)(_CSTD::size_t);  // hh 981221
typedef void* (*_op_new_nothrow)(_CSTD::size_t, const _STD::nothrow_t&);  // hh 981221

#if NEWMODE==NEWMODE_SIMPLE

	#include <pool_alloc.h>

	namespace
	{
		void* (*alloc_func)(_CSTD::size_t) = __sys_alloc;
		void  (*free_func)(void*)         = __sys_free;
	}

#elif NEWMODE==NEWMODE_MALLOC

	namespace
	{
		void* (*alloc_func)(_CSTD::size_t) = _CSTD::malloc;
		void  (*free_func)(void*)         = _CSTD::free;
	}

#elif NEWMODE==NEWMODE_NORMAL || NEWMODE==NEWMODE_FAST

	#ifndef _MSL_NO_CPP_NAMESPACE
		namespace std {
	#endif
		void* my_alloc(_CSTD::size_t);
		void my_free(void*);
	#ifndef _MSL_NO_CPP_NAMESPACE
		}
	#endif

	namespace
	{
		void* (*alloc_func)(_CSTD::size_t) = _STD::my_alloc;
		void  (*free_func)(void*)         = _STD::my_free;
	}

#endif

void *operator new(_CSTD::size_t size) throw(_STD::bad_alloc)
{
	return DebugNewDoAllocate(size, 0, 0, alloc_func, false);  // hh 981221
}

// hh 981221
void *operator new(_CSTD::size_t size, const _STD::nothrow_t& nt) throw()
{
	return DebugNewDoAllocate(size, 0, 0, alloc_func, nt, false);
}

// Hsoi 980821 added
void *operator new[](_CSTD::size_t size) throw(_STD::bad_alloc)
{
	return DebugNewDoAllocate(size, 0, 0, alloc_func, true);  // hh 981221
}

// hh 981221
void *operator new[](_CSTD::size_t size, const _STD::nothrow_t& nt) throw()
{
	return DebugNewDoAllocate(size, 0, 0, alloc_func, nt, true);
}

#if DEBUG_NEW == DEBUG_NEW_LEAKS

void* operator new(_CSTD::size_t size, const char* file, int line) throw(_STD::bad_alloc)
{
	return DebugNewDoAllocate(size, file, line, alloc_func, false);  // hh 981221
}

// hh 981221
void* operator new(_CSTD::size_t size, const _STD::nothrow_t& nt, const char* file, int line) throw()
{
	return DebugNewDoAllocate(size, file, line, alloc_func, nt, false);
}

// Hsoi 980821 added
void* operator new[](_CSTD::size_t size, const char* file, int line) throw(_STD::bad_alloc)
{
	return DebugNewDoAllocate(size, file, line, alloc_func, true);  // hh 981221
}

// hh 981221
void* operator new[](_CSTD::size_t size, const _STD::nothrow_t& nt, const char* file, int line) throw()
{
	return DebugNewDoAllocate(size, file, line, alloc_func, nt, true);
}

#endif // DEBUG_NEW == DEBUG_NEW_LEAKS

void operator delete(void* ptr) throw()
{
	DebugNewDoFree(ptr, free_func, false);
}

void operator delete(void* ptr, const _STD::nothrow_t&) throw()
{
	operator delete(ptr);
}

void operator delete(void* ptr, const char*, int) throw()
{
	operator delete(ptr);
}

void operator delete(void* ptr, const _STD::nothrow_t&, const char*, int) throw()
{
	operator delete(ptr);
}

// Hsoi 980821 added
void operator delete[](void* ptr) throw()
{
	DebugNewDoFree(ptr, free_func, true);
}

void operator delete[](void* ptr, const _STD::nothrow_t&) throw()
{
	operator delete[](ptr);
}

void operator delete[](void* ptr, const char*, int) throw()
{
	operator delete[](ptr);
}

void operator delete[](void* ptr, const _STD::nothrow_t&, const char*, int) throw()
{
	operator delete[](ptr);
}

#endif	// !DEBUG_NEW_NO_GLOBAL_OPERATORS


void DebugNewDoFree(void* ptr, void (*free_func)(void* ptr), bool is_array)
{
	// Hsoi 980821 added is_array argument and logic
	
		// Allocation counter should be > zero. If not, then delete was
		// called too many times.
		
		// JWW - ...except if the thing to delete is NULL
	if (!gDebugNewAllocCount && ptr)
	{
		gErrorHandler(dbgnewTooManyFrees);
		return;
	}
	
	BlockStatus_t status = GetBlockStatus((char*)ptr, false);
	switch (status)
	{
		case blockFree:
		case blockPtrOutOfRange:
		case blockBadHeader:
		case blockBadTrailer:
			ReportBlockStatus(status);
			return;
			
		case blockPtrNull:	 // it's legal to pass NULL to operator delete
			return;

        case blockValid:
        case blockNotInList:
        case blockFreeOverwritten: 	// dnDontFreeBlocks enabled, zapped free block was overwritten
        case blockDeleteNotMatch:		// Hsoi 980821 - called new and delete[] or new[] and delete
            break;
	}

	BlockHeader* h = GetHeader(ptr);
	if (!h)
	{
		ReportBlockStatus(blockBadHeader);
		return;
	}
	
	unsigned long goodTag = is_array ? (long)BLOCK_PREFIX_ALLOC_ARRAY : (long)BLOCK_PREFIX_ALLOC;
	if (h->tag != goodTag)
	{
		ReportBlockStatus(blockDeleteNotMatch);
		return;
	}

	const Boolean fDoFree = !(gDebugNewFlags & dnDontFreeBlocks);

#if DEBUG_NEW == DEBUG_NEW_LEAKS
//		printf("free block %lu is %p\n", h->nAlloc, ptr);

		// find the block in the block list
		int hash = Hash(h);
        if (hash < 0 || hash >= kHashTableSize)
            NSLog(@"hash is %d\n", hash);
		BlockHeader* curr = gBlockHash[hash];
		BlockHeader* prev = 0;
		while (curr)
		{
			if (curr == h)
				break;
			prev = curr;
			curr = curr->next;	
		}
		if (!curr)
		{
			gErrorHandler(dbgnewBlockNotInList);
			return;
		}
		else if (fDoFree)
		{
			if (prev)
				prev->next = curr->next;
			else
				gBlockHash[hash] = curr->next;
			curr->next = 0;
		}
#endif
 		ZapBlock(ptr, h->size + BLOCK_TRAILER_SIZE, ZAP_RELEASED);
   				
   		// mark block freed, and free it
   		h->tag = BLOCK_PREFIX_FREE;
   		if (fDoFree)
   		{
   			--gDebugNewAllocCount;
   			gDebugNewAllocCurr -= h->size;
			free_func((char*)ptr - BLOCK_HEADER_SIZE);  
		}
}

void* DebugNewDoAllocate(_CSTD::size_t size, const char* file, int line,
		void* (*alloc_func)(_CSTD::size_t size), bool is_array)	
{		
	// hh 000118 Rewrote to use malloc/free		
	char* p;
	while (true)
	{
		p = (char*)alloc_func(size+BLOCK_OVERHEAD);
		if (p != 0)
			break;
		_STD::new_handler f = _STD::set_new_handler(0);
		_STD::set_new_handler(f);
		if (!f)
			throw _STD::bad_alloc();
		f();
	}

	return FinishAllocate(p, size, file, line, is_array);		
}

// hh 981221
void* DebugNewDoAllocate(_CSTD::size_t size, const char* file, int line,
		void* (*alloc_func)(_CSTD::size_t size), const _STD::nothrow_t&, bool is_array)	
{		
	// hh 000118 Rewrote to use malloc/free		
	char* p;
	while (true)
	{
		p = (char*)alloc_func(size+BLOCK_OVERHEAD);
		if (p != 0)
			break;
		_STD::new_handler f = _STD::set_new_handler(0);
		_STD::set_new_handler(f);
		if (!f)
			return 0;
		try
		{
			f();
		}
		catch (...)
		{
			return 0;
		}
	}

	return FinishAllocate(p, size, file, line, is_array);
}

// Hsoi 990122 added - created to consolidate common code in both
// versions of DebugNewDoAllocate
static void* FinishAllocate(char* p, _CSTD::size_t size, const char* file, int line, bool is_array)
{
#if DEBUG_NEW < DEBUG_NEW_LEAKS
#pragma unused(file, line)
#endif

	++gDebugNewAllocCount;
	gDebugNewAllocCurr += size;
	if (gDebugNewAllocCurr > gDebugNewAllocMax)
		gDebugNewAllocMax = gDebugNewAllocCurr;

		// zap data area + trailer, so blocks of sizes that are
		// not multiples of four get fully zapped.
		
	ZapBlock(p+BLOCK_HEADER_SIZE, size+BLOCK_TRAILER_SIZE, ZAP_UNINITIALIZED);
	
	BlockHeader* h = (BlockHeader*)p;
	h->size = size;
	h->tag = is_array ? (long)BLOCK_PREFIX_ALLOC_ARRAY : (long)BLOCK_PREFIX_ALLOC;
	h->padSize = 0; // assume no padding. No way to tell this if there
					// is compiler-inserted padding at this stage anyways.
	
	BlockTrailer* t = (BlockTrailer*)(p + size + BLOCK_HEADER_SIZE);
	t->tag = BLOCK_SUFFIX;
	
#if DEBUG_NEW == DEBUG_NEW_LEAKS
	++gSequentialBlockCount;
		// new blocks go to front of the block list
	int hash = Hash(h);
	h->next = gBlockHash[hash];
	gBlockHash[hash] = h;
	h->file = file;
	h->line = line;
	h->nAlloc = gSequentialBlockCount;
#endif
	p += BLOCK_HEADER_SIZE;
//	printf("block %lu is %p\n", h->nAlloc, p);
	return p;
}


static BlockStatus_t GetBlockStatus(char* ptr, Boolean fValidateFree)
{
	if (!ptr)
		return blockPtrNull;

// JWW - This check isn't good under OS X, so it's completely taken out for Carbon
// It'd probably be better to test the system version Gestalt value around this
// instead of completely ignoring the test under Carbon, but that would make
// execution time slower.
#if !TARGET_API_MAC_CARBON
	if (gDebugNewFlags & dnCheckBlocksInApplZone)
	{
		// Validate that pointer is in the application heap. This
		// will also trap attempts to free pointers whose values
		// are inside uninitialized or free blocks, i.e.
		// 0xF1F1F1F1 or 0xF3F3F3F3.
		THz applZone = LMGetApplZone();
		if ((void*)ptr < &applZone->heapData || ptr > applZone->bkLim)
			return blockPtrOutOfRange;
	}
#endif
	
	const BlockHeader& h = *GetHeader(ptr);
	if (!(&h))
	{
		return blockBadHeader;
	}
	
	// check block header
			
	if (h.tag == BLOCK_PREFIX_FREE)
	{
		if (fValidateFree)
		{
				// Check that free block contains the zap values, if not
				// then it was probably overwritten through a dangling
				// pointer
				
			const unsigned long* p = (const unsigned long*) ((char*)&h + BLOCK_HEADER_SIZE);
			long count = (long)((h.size+BLOCK_TRAILER_SIZE) / 4);
			while (--count >= 0)
			{
				if (*p++ != ZAP_RELEASED)
					return blockFreeOverwritten;
			}
		}
		return blockFree;
	}
		
	if ((h.tag != BLOCK_PREFIX_ALLOC) && (h.tag != BLOCK_PREFIX_ALLOC_ARRAY))
		return blockBadHeader;
		
	const BlockTrailer& t = *(BlockTrailer*) (ptr + (h.size - h.padSize));

	// check block trailer
	
	if (t.tag != BLOCK_SUFFIX)
		return blockBadTrailer;
		
	return blockValid;
}

static void ReportBlockStatus(BlockStatus_t status)
{
	switch (status)
	{
		case blockFree:
			gErrorHandler(dbgnewFreeBlock);
			break;
			
		case blockPtrNull:
			gErrorHandler(dbgnewNullPtr);
			break;
		
		case blockPtrOutOfRange:
			gErrorHandler(dbgnewPointerOutsideHeap);
			break;
			
		case blockBadHeader:
			gErrorHandler(dbgnewBadHeader);
			break;
		
		case blockBadTrailer:
			gErrorHandler(dbgnewBadTrailer);
			break;
			
		case blockNotInList:
			gErrorHandler(dbgnewBlockNotInList);
			break;
			
		case blockFreeOverwritten:
			gErrorHandler(dbgnewFreeBlockOverwritten);
			break;
		
		case blockDeleteNotMatch:
			gErrorHandler(dbgnewMismatchedNewDelete);
			break;
		
		default:
			gErrorHandler(-1);
			break;
	}
}

DebugNewErrorHandler_t DebugNewSetErrorHandler(DebugNewErrorHandler_t newHandler)
{
	DebugNewErrorHandler_t oldHandler = gErrorHandler;
	if (newHandler)
		gErrorHandler = newHandler;
	return oldHandler;
}

static void DefaultErrorHandler(short err)
{
	const unsigned char* s;
	switch (err)
	{
		case dbgnewNullPtr:
			s = "\pDebugNew: null pointer";
			break;
			
		case dbgnewTooManyFrees:
			s = "\pDebugNew: more deletes than news";
			break;
			
		case dbgnewPointerOutsideHeap:
			s = "\pDebugNew: delete or validate called for pointer outside application heap";
			break;
			
		case dbgnewFreeBlock:
			s = "\pDebugNew: delete or validate called for free block";
			break;
			
		case dbgnewBadHeader:
			s = "\pDebugNew: unknown block, block header was overwritten, or mismatched new and delete[]";
			break;
			
		case dbgnewBadTrailer:
			s = "\pDebugNew: block trailer was overwritten";
			break;
			
		case dbgnewBlockNotInList:
			s = "\pDebugNew: block valid but not in block list (internal error)";
			break;
			
		case dbgnewFreeBlockOverwritten:
			s = "\pDebugNew: free block overwritten, could be dangling pointer";
			break;
									
		case dbgnewMismatchedNewDelete:
			s = "\pDebugNew: mismatched regular/array new and delete (probably delete on new[] block)";
			break;
								
		default:
			s = "\pDebugNew: undefined error";
			break;
	}
	DebugStr(s);
}


static void ZapBlock(void* ptr, register _CSTD::size_t len, uint32_t value)
{
	uint32_t* p = (uint32_t*) ptr;
	len /= sizeof(uint32_t);
	while (len-- > 0)
		*p++ = value;
}

#if DEBUG_NEW == DEBUG_NEW_LEAKS

void  DebugNewValidateAllBlocks()
{
	// traverse the block list and validate every block
	for (int i = 0; i < kHashTableSize; i++)
	{
		BlockHeader* curr = gBlockHash[i];
		while (curr)
		{
			BlockStatus_t status = GetBlockStatus((char*)curr + BLOCK_HEADER_SIZE, true);
			if (status != blockValid && status != blockFree)
			{
				ReportBlockStatus(status);
				// abort on any error. Not strictly necessary for some errors,
				// but we could crash if next field is trashed.
				break;
			}		
			curr = curr->next;
		}
	}
}

#include <cstdio>
//#include <Files.h>
//#include <TextUtils.h>
//#include <Processes.h>

//static _CSTD::FILE *FSSpecOpen(FSSpec& fsspec, const char *mode)
//{	
//	_CSTD::FILE* file = 0;
//	short savedVol;
//	long dirID;
//	OSErr err;
//	
//	err = HGetVol(0, &savedVol, &dirID);
//	if (err == noErr)
//	{
//		err = HSetVol(0, fsspec.vRefNum, fsspec.parID);
//		if (err == noErr)
//		{
//			char name[256];
//			unsigned char theSize = fsspec.name[0];
//			BlockMoveData(&fsspec.name[1], name, theSize);
//			name[theSize] = 0;
//			
//			file = _CSTD::fopen(name, mode);
//		}
//		HSetVol(0, savedVol, dirID);
//	}
//	return file;
//}


long DebugNewReportLeaks(const char* name)
{
//	ProcessSerialNumber psn;
//	OSErr err = GetCurrentProcess(&psn);
//	if (err != noErr) return 0;
	
//	ProcessInfoRec info;
//	int i;
	
//	info.processInfoLength = sizeof(info);
//	info.processName = 0;
#if !__LP64__
//	FSSpec spec;
//	info.processAppSpec = &spec;
#else
//	FSRef ref;
//	info.processAppRef = &ref;
#endif
	
//	err = GetProcessInformation(&psn, &info);
//	if (err != noErr) return 0;
	
	if (name == 0)
		name = "/tmp/leaks.log";
	
	//ConstStringPtr s = "\pleaks.log";
	//BlockMoveData(s, spec.name, s[0]+1);
	_CSTD::FILE* f = fopen(name, name != 0 ? "a" : "w");
	if (!f) return 0;

	unsigned long count = 0;
	unsigned long leakCount = 0;
	unsigned long bytesLeaked = 0;
	BlockHeader* curr;
	for (int i = 0; i < kHashTableSize; i++)
	{
		curr = gBlockHash[i];
		while (curr)
		{
			++count;
			if ((curr->tag == BLOCK_PREFIX_ALLOC || curr->tag == BLOCK_PREFIX_ALLOC_ARRAY) && curr->line >= 0)
			{
				bytesLeaked += curr->size;
				++leakCount;
			}
			curr = curr->next;
		}
	}
	
	if(count != gDebugNewAllocCount)
		fprintf(f, "Warning: length of block list different from count of allocated blocks (internal error).\n");
		
	fprintf(f, "Maximum #bytes allocated at any point via operator new: %ld\n", gDebugNewAllocMax);
	
	if (!leakCount)
	{
		fprintf(f, "No memory leaks.\n");
		fclose(f);
		return 0;
	}
	if (leakCount == 1)
		fprintf(f,"There is 1 memory leak of %lu bytes:\n", bytesLeaked);
	else
		fprintf(f,"There are %ld memory leaks, totaling %lu bytes:\n", leakCount, bytesLeaked);
	
	unsigned long totalAlloc = 0;
	
	for (int i = 0; i < kHashTableSize; i++)
	{
		curr = gBlockHash[i];
		while (curr)
		{
			if ((curr->tag == BLOCK_PREFIX_ALLOC || curr->tag == BLOCK_PREFIX_ALLOC_ARRAY) && curr->line >= 0)
			{
				if (curr->file)
					fprintf(f,"  %s line: %d, size: %lu", curr->file, curr->line, curr->size);
				else
					fprintf(f,"  <unknown>, size: %lu, index: %lu", curr->size, curr->nAlloc);
				
				// Hsoi 980821 - compiler-inserted padding is part of BlockHeader.size and
				// will show up in the leaks.log (above). To help people compensate when
				// reviewing the log, we'll notify them of any compiler-inserted padding
				// (see comments in GetHeader for more information). If you see the ptr
				// has padding, subtract that value from the reported size to get the true
				// pointer/data size.
				
				if (curr->padSize > 0)
					fprintf(f," (compiler-inserted padding: %lu)", curr->padSize);
				
				fprintf(f, "\n");
			}
			totalAlloc += curr->size; // count freed blocks, since gDebugNewAllocCurr does
			curr = curr->next;
		}
	}
	if (totalAlloc != gDebugNewAllocCurr)
		fprintf(f, "Warning: total allocations in block list different from gDebugNewAllocCurr.\n");

	fclose(f);
	return leakCount;
}

void DebugNewForgetLeaks()
{
	for (int i = 0; i < kHashTableSize; i++)
	{
		BlockHeader* curr = gBlockHash[i];
		while (curr)
		{
			if (curr->tag == BLOCK_PREFIX_ALLOC || curr->tag == BLOCK_PREFIX_ALLOC_ARRAY)
			{
				curr->file = NULL;
				curr->line = -1;
			}
			curr = curr->next;
		}
	}
}

#else
	// leak checking disabled, does nothing
long DebugNewReportLeaks()
{
	return 0;
}
void DebugNewForgetLeaks()
{
}
#endif	// DEBUG_NEW_LEAKS

#endif  //  DEBUG_NEW >= DEBUG_NEW_BASIC

void DebugNewValidatePtr(void* ptr)
{
#if DEBUG_NEW == DEBUG_NEW_OFF
#pragma unused(ptr)
#endif

#if DEBUG_NEW >= DEBUG_NEW_BASIC
	BlockStatus_t status = GetBlockStatus((char*)ptr, true);
#if DEBUG_NEW == DEBUG_NEW_LEAKS
	if (status == blockValid)
	{
		const BlockHeader* h = GetHeader(ptr);
		if (!h)
		{
			ReportBlockStatus(blockBadHeader);
			return;
		}

		// find the block in the block list
		int hash = Hash(h);
		BlockHeader* curr = gBlockHash[hash];
		while (curr)
		{
			if (curr == h)
				break;
			curr = curr->next;	
		}
		if (!curr)
			status = blockNotInList;
	}
#endif
	if (status != blockValid)
		ReportBlockStatus(status);
#endif
}

long DebugNewGetPtrSize(void* ptr)
{
#if DEBUG_NEW == DEBUG_NEW_OFF
#pragma unused(ptr)
#endif

	long size = -1;
	
#if DEBUG_NEW >= DEBUG_NEW_BASIC
	BlockStatus_t status = GetBlockStatus((char*)ptr, true);
	if (status == blockValid)
	{
		const BlockHeader* h = GetHeader(ptr);
		if (!h)
		{
			ReportBlockStatus(blockBadHeader);
			return size;
		}

		size = (long)(h->size - h->padSize);
	}
	else
		ReportBlockStatus(status);
#endif
	return size;
}

// Hsoi 980821 - added

#if DEBUG_NEW >= DEBUG_NEW_BASIC

static BlockHeader* GetHeader(void* ptr)
{
	// The header should be located just before the pointer
	BlockHeader* h = (BlockHeader*)((char*)ptr - BLOCK_HEADER_SIZE);
	
	if ((h->tag == BLOCK_PREFIX_ALLOC) ||
		(h->tag == BLOCK_PREFIX_ALLOC_ARRAY) ||
		(h->tag == BLOCK_PREFIX_FREE))
	{
		// We should be ok.
		h->padSize = 0;
		return h;
	}

	// If we get here, the header block might be corrupt, or there might
	// be something extra in the header (e.g. compiler inserted padding).
	// As of this writing (21 Aug 98), the Metrowerks C/C++ compilers
	// and runtimes (at least on MacOS) function this way. Any array
	// allocation where the array element type is a class with a non-trivial
	// constructor (e.g. classes with virtual members/bases or user
	// defined constructors) will allocate an additional 8-byte
	// (sizeof(_CSTD::size_t)*2) header to store the size of a single
	// class object (in the first half of the header block) and the
	// total number of array elements (in the second half of the block).
	// See the Runtime Common source NMWException.cp, and functions like
	// __construct_new_array() and __destroy_new_array().
	//
	// NB: This compiler behavior may change future versions of the
	// compiler. For example, this header may only be allocated if
	// the class has a destructor. Furthermore, Win32/x86 compilers
	// may use a completely different strategy to be compatible with
	// the WinABI.
	//
	// To attempt to contend with this padding, we'll assume our header is
	// back there somewhere, so we'll walk backwards looking for
	// a tag. If we find it, correct h for this offset and return.
	// If do not find a known tag, either the header is corrupt
	// or the inserted padding is greater than BLOCK_HEADER_SIZE (unlikely
	// but always possible).
	//
	// This assumes ptr is a valid C++ new/new[] pointer, and that
	// there is a BlockHeader somewhere close before ptr.
	
	h = 0;
	
	char* bufferHead = (char*)ptr - BLOCK_HEADER_SIZE;
	_CSTD::size_t pad = 0;
	for ( char* ref = (char*)ptr; ref >= bufferHead; --ref )
	{
		unsigned long refAsLong = *((long*)ref);
		
		if ((refAsLong == BLOCK_PREFIX_ALLOC) ||
			(refAsLong == BLOCK_PREFIX_ALLOC_ARRAY) ||
			(refAsLong == BLOCK_PREFIX_FREE))
		{
			// Found it. Adjust for the BlockHeader
			
			h = (BlockHeader*)(ref - (sizeof(BlockHeader) - sizeof(long)));
			h->padSize = pad - sizeof(long);
			return h;
		}
		
		pad++;
	}

	// problem			
	
	if ( h == 0 )
		gErrorHandler(dbgnewBadHeader);
	
	return h;
}

#endif // DEBUG_NEW >= DEBUG_NEW_BASIC

#pragma only_std_keywords reset
#pragma ANSI_strict reset

// hh   980126 added "using namespace std"
// Hsoi 980819 Fixed a bug in DebugNewReportLeaks that cause an incorrect number of leaks to be reported
// Hsoi 980819 Added support for array new and array delete
// Hsoi 980821 Created GetHeader, implemented throughout code
// hh   981221 Added new(nothrow)
// Hsoi 990122 Fixed FSSpecOpen to use HSetVol/HGetVol instead of SetVol/GetVol
// Hsoi 990122 Wrapped GetHeader with a check #if DEBUG_NEW >= DEBUG_NEW_BASIC
// Hsoi 990122 Added FinishAllocate() method to consolidate code from DebugNewDoAllocate overloads
// Hsoi 990316 Cleaned up implicit arithmetic conversion warnings.
// hh   000118 Rewrote to use malloc/free
// JWW	000202 Updated for Carbon compliance
// JWW	000217 Ignore the delete count on NULL deletions
// hh	000612 Defines NEWMODE to decide what allocator to use
// JWW  010116 Changed _STD::size_t to _CSTD::size_t
// JWW  010120 Took out general "using namespace std;" and added specific namespace scoping
// JWW  010327 Added a few missing _STD:: qualifiers to some nothrow_t types
