/*!
 *	\file		DlTypes.h
 *
 *	\brief contains type definitions.
 *
 *	\author David C. Salmon
 *
 *	\note	COPYRIGHT (C) 1990-96 by David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *	To Do:
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

#ifndef _H_DlTypes
#define	_H_DlTypes

#ifndef TARG_OS_MAC
#error "DlPlatform.h must be included before this file."
#endif

#define	DlFalse	0
#define	DlTrue	!DlFalse
#define	DlNil	NULL

#ifndef NEW
#define NEW new
#define NEW_NOTHROW(x) new(x)  // hh 981221
#define OPERATORNEW(x) operator new(x)
#endif

/// \typedef DlSizeT
/// generic size type.
#if _MSL_USING_MW_C_HEADERS
	typedef std::size_t			DlSizeT;
#else
	typedef size_t				DlSizeT;
#endif

typedef char					DlChar;		///< ascii character type.
typedef	wchar_t					DlWChar;	///< unicode character type.
#if TARG_OS_MAC
typedef UniChar					DlUChar;	///< short unicode character type.
#else
typedef uint16_t				DlUChar;
#endif

typedef	int32_t					DlInt32;	///< integer type.
typedef	uint32_t				DlUInt32;	///< unsigned type.
typedef	int16_t					DlInt16;	///< short interger type.
typedef	uint16_t				DlUInt16;	///< short unsigned integer type.
typedef	uint8_t					DlUInt8;	///< signed byte type.
typedef	int8_t					DlInt8;		///< unsigned byte type
typedef int64_t					DlInt64;	///< signed 64 bit integer
typedef uint64_t				DlUInt64;	///< unsigned 64 bit integer
/// boolean type
#ifndef __cplusplus
typedef	int						DlBoolean;	
#else
typedef	bool					DlBoolean;
#endif
typedef DlUInt32				DlStatus;	///< status code type

/// \defgroup FPOptions Floating point options
/// @{
///	Definition of floating point type. The default floating point
/// type can be changed by defining DlFloatOption to be one of
/// - #DlUseBestDouble. This causes DlFloat to be defined as the best
///	double type for the machine.
/// - #DlUseSingle. Use smallest floating point type.
/// - #DlUseANSIDouble. Use ansi floating point type.

///	Use the best floating point type.
#define	DlUseBestDouble	1
/// Use small floating point type.
#define	DlUseSingle		2
/// Use ANSI floating point.
#define	DlUseANSIDouble	3	

/// scanner string for 32 bit float.
#define	kScanDlFloat32String	"%f"
/// scanner string for 64 bit float.
#define	kScanDlFloat64String	"%lf"
/// scanner string for extended (80 bit) float.
#define	kScanDlFloatExString	"%Lf"

/*! \enum DlFilePosition (for seek) */
enum DlFilePosition {
	DlFromStart = SEEK_SET,
	DlFromCurrent = SEEK_CUR,
	DlFromEnd = SEEK_END
};

/// \typedef DlFloat32
///	standard short floating point type.

/// \typedef DlFloat64
/// standard double type.

///	\typedef DlFloatEx
///	extended floating point type.

/// \typedef DlFloat
///	default floating point type
#if TARG_OS_MAC 

	typedef	float				DlFloat32;
#	if TARG_COMPILER_MWERKS
#		if !TARG_API_OSX
#			include <math.h>
#		endif
		typedef double			DlFloat64;
		typedef	double_t		DlFloatEx;
#	else
		typedef double	DlFloat64;
		typedef double	DlFloatEx;
#	endif
#	ifndef DlFloatOption
#		define	DlFloatOption	DlUseBestDouble
#	endif

#else	//	!macintosh
#	define	DlForiegn
	typedef	float				DlFloat32;
	typedef	double				DlFloat64;
	typedef	double				DlFloatEx;

#	ifndef DlFloatOption
#		define	DlFloatOption	DlUseANSIDouble
#	endif

#endif	//	macintosh

#if DlFloatOption == DlUseSingle
	typedef	DlFloat32			DlFloat;
#elif DlFloatOption == DlUseANSIDouble
	typedef	DlFloat64			DlFloat;
#else
	typedef	DlFloatEx			DlFloat;
#endif

#if TARG_OS_MAC 

	#define	kScanDlFloatString	(sizeof( DlFloat ) == sizeof( DlFloat32 ) ? 	\
									kScanDlFloat32String : 						\
									sizeof( DlFloat ) == sizeof( DlFloat64 ) ?	\
										kScanDlFloat64String : 					\
										kScanDlFloatExString)

#else

	#define	kScanDlFloatString	(sizeof( DlFloat ) == sizeof( DlFloat32 ) ? 	\
									kScanDlFloat32String : 						\
									kScanDlFloat64String)

#endif

/// @}

/// Determine count of elements in an array.
#define DlArrayElements(a)	(sizeof(a) / sizeof(a[0]))

#if !TARG_OS_MAC

	typedef unsigned char                   Str255[256];
	typedef unsigned char                   Str63[64];
	typedef unsigned char                   Str32[33];
	typedef unsigned char                   Str31[32];
	typedef unsigned char                   Str27[28];
	typedef unsigned char                   Str15[16];
	/*
	    The type Str32 is used in many AppleTalk based data structures.
	    It holds up to 32 one byte chars.  The problem is that with the
	    length byte it is 33 bytes long.  This can cause weird alignment
	    problems in structures.  To fix this the type "Str32Field" has
	    been created.  It should only be used to hold 32 chars, but
	    it is 34 bytes long so that there are no alignment problems.
	*/
	typedef unsigned char                   Str32Field[34];
	/*
	    QuickTime 3.0:
	    The type StrFileName is used to make MacOS structs work 
	    cross-platform.  For example FSSpec or SFReply previously
	    contained a Str63 field.  They now contain a StrFileName
	    field which is the same when targeting the MacOS but is
	    a 256 char buffer for Win32 and unix, allowing them to
	    contain long file names.
	*/
	typedef Str255                          StrFileName;

	typedef unsigned char *                 StringPtr;
	typedef StringPtr *                     StringHandle;
	typedef const unsigned char *           ConstStringPtr;
	typedef const unsigned char *           ConstStr255Param;
	typedef const unsigned char *           ConstStr63Param;
	typedef const unsigned char *           ConstStr32Param;
	typedef const unsigned char *           ConstStr31Param;
	typedef const unsigned char *           ConstStr27Param;
	typedef const unsigned char *           ConstStr15Param;
	typedef ConstStr255Param                ConstStrFileNameParam;

#	if TARG_CPP
		inline unsigned char StrLength(ConstStr255Param string) { return (*string); }
#	else
#		define StrLength(string) (*(unsigned char *)(string))
#	endif

#endif	//	macintosh


#endif

//	eof
