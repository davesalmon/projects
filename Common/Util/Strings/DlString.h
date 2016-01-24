//+
//	DlString.cp
//
//	©1999 David C. Salmon. All Rights Reserved
//
//	Implement a string class
//
//		Operations supported:
//			Expand	- increase the memory for a string
//			Fill	- fill a string with a char
//			Length	- return the string length
//		Operators defined:
//			[]		- return a reference to a char (const and non-const)
//			const char *	- cast to const char *
//			=		- assignment from LString or char *
//			>		- test greater
//			<		- test less
//			==		- test equal
//			!=		- test not equal
//
//-
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

#ifndef	_H_DlString
#define	_H_DlString

//---------------------------------- Includes ----------------------------------

#include	"DlException.h"
#include	"DlAssert.h"
#include	"DlStorage.h"

#include	<cstring>
#if USE_STREAMS
#include	<iostream>
#endif

#include "DlStringUtil.h"

using namespace std;


//---------------------------------- Defines -----------------------------------

const char kFloatingOverflowChar = '*';

//---------------------------------- Declares -----------------------------------

enum class DlFloatFormatType : uint16 {
	  Fixed = 0
	, Exponential
	, Fit
};

const char* const kFloatFormatList[static_cast<int>(DlFloatFormatType::Fit)+1] = {
	  "%*.*f"
	, "%*.*e"
	, "%*.*g"
};

	enum class DlIntFormatType : uint16 {
	  Decimal = 0
	, Octal
	, Hex
	, Unsigned
};

const char* const kIntFormatList[static_cast<int>(DlIntFormatType::Unsigned)+1] = {
	  "%*ld"
	, "%*lo"
	, "%*lx"
	, "%*lu"
};

const char* const kLongLongFormatList[static_cast<int>(DlIntFormatType::Unsigned)+1] = {
	  "%*lld"
	, "%*llo"
	, "%*llx"
	, "%*llu"
};

const char* const kEmptyString = "";

//-------------------------------------------------------------------------------
//	class DlFloatFormat
//
//		Define string to float conversions exceptions
//
//-------------------------------------------------------------------------------
class	DlFloatFormat {
public:
	DlFloatFormat(size_t prec = 4, size_t width = 0,
				  DlFloatFormatType form = DlFloatFormatType::Fixed) :
			itsPrec(prec), itsWid(width), itsForm(form) {}

	//	perform the conversion
	char*	convert(double_t val, char* buf) const {
				sprintf(buf, kFloatFormatList[static_cast<int>(itsForm)], itsWid, itsPrec, val);
				return buf;
			}

	uint16	getwidth() const { return itsWid; }
	uint16	getprecision() const { return itsPrec; }
	DlFloatFormatType	getformat() const { return itsForm; }

	void	setwidth(size_t w) { itsWid=w; }
	void	setprecision(size_t p) { itsPrec=p; }
	void	setformat(DlFloatFormatType t) { itsForm=t; }
	
private:

		//	number of digits past the decimal
		uint16				itsPrec;
		//	field width to consume (0 for whatever is needed)
		uint16				itsWid;
		//	true for fixed point format, false for exponential
		DlFloatFormatType	itsForm;
};

class	DlIntFormat {

public:
		DlIntFormat(size_t fwidth = 0, 
					DlIntFormatType form = DlIntFormatType::Decimal) :
				itsWid(fwidth), itsForm(form) {}

		//	perform the conversion d
		char*	convert(long val, char* buf) const {
					sprintf(buf, kIntFormatList[static_cast<int>(itsForm)], itsWid, val);
					return buf;
				}

		//	perform the conversion
		char*	convert(unsigned long val, char* buf) const {
					sprintf(buf, kIntFormatList[static_cast<int>(itsForm)], itsWid, val);
					return buf;
				}
	
		//	perform the conversion
		char*	convert(long long val, char* buf) const {
					sprintf(buf, kLongLongFormatList[static_cast<int>(itsForm)], itsWid, val);
					return buf;
				}

		//	perform the conversion
		char*	convert(unsigned long long val, char* buf) const {
					sprintf(buf, kLongLongFormatList[static_cast<int>(itsForm)], itsWid, val);
					return buf;
				}

		uint16	getwidth() const { return itsWid; }
		DlIntFormatType	getformat() const { return itsForm; }
	
private:

		//	field width
		uint16			itsWid;
		//	format type
		DlIntFormatType	itsForm;
};

//-------------------------------------------------------------------------------
//	class DlStrStore
//
//		char storage for string class
//
//-------------------------------------------------------------------------------
class	DlStrStore	: public DlStorage<char> {

	friend class DlString;

public:

	DlStrStore() {}

	DlStrStore(char* buf, size_t len) 
			: DlStorage<char>(buf, len) {}
	
	DlStrStore(const DlStorage<char>& st) 
			: DlStorage<char>(st) {}
	
	DlStrStore copy() const {
		return DlStorage<char>::copy();
	}

};

//-------------------------------------------------------------------------------
//	class DlString
//
//		Define string class
//
//-------------------------------------------------------------------------------
class	DlString {

#if USE_STREAMS
	friend ostream& operator << (ostream& s, const DlString& str);
	friend istream& operator >> (istream& s, DlString& str);
#endif

	static DlFloatFormat	sFloatFormat;
	static DlIntFormat		sIntFormat;

public:

	static	void 	SetFloatingFormat(const DlFloatFormat& form) { sFloatFormat = form; }
	static	void 	SetIntFormat(const DlIntFormat& form) {	sIntFormat = form; }

	static const DlFloatFormat& GetFloatingFormat() { return sFloatFormat; }
	static const DlIntFormat& GetIntFormat() { return sIntFormat; }
	
					//	default constructor
					DlString();	
					DlString(const DlStrStore& mem);
					DlString(const DlString& cpy);
					DlString(const char* str);
					DlString(short resID, short index);
	explicit		DlString(double_t value, const DlFloatFormat& form=DlString::sFloatFormat);
	explicit		DlString(long value, const DlIntFormat& form=DlString::sIntFormat);
	explicit		DlString(unsigned long value, const DlIntFormat& form=DlString::sIntFormat);
//					DlString(const unsigned char* str);

					//	assignment
	const DlString& operator= (const DlString& str);
	const DlString& operator =(const char* str);

					//	fill with character
	void			Fill(char fillchar = kFloatingOverflowChar);

					//	return length
	size_t			Length() const { return itsLength; }
	void			TruncateTo(size_t newLen);
	void			Clear() { itsLength = 0; }

					//	access
	char			operator [](size_t i) const;
	char& 			operator [](size_t i);

	const char*		get() const { return itsLength == 0 ? kEmptyString : itsBuf.ptr(); }
	operator const char* () const { return get(); }

					//	convert to double
	double_t 		getFloat() const;
	long			getLong() const;

	int				compareNoCase(const DlString& str) const { return DlStrICmp(get(), str.get()); }
	int				compareNoCase(const char* str) const { return DlStrICmp(get(), str); }
	int				compareNoCase(const unsigned char* str) const { return DlStrNICmp(get(), (const char*)(str+1), str[0]); }

					//	compare to string
	int				compare(const DlString& str) const { return strcmp(get(), str.get()); }
	int				compare(const char* str) const { return strcmp(get(), str); }
					
					//	string order
	int				operator > (const DlString& cmp) const { return compare(cmp) > 0; }
	int				operator > (const char* cmp) const { return compare(cmp) > 0; }
	int 			operator < (const DlString& cmp) const { return compare(cmp) < 0; }
	int 			operator < (const char* cmp) const { return compare(cmp) < 0; }

					//	operator ==
	int 			operator == (const DlString& cmp) const { return compare(cmp) == 0; }
	int 			operator == (const char* const cmp) const { return compare(cmp) == 0; }

					//	operator !=
	int 			operator != (const DlString& cmp) const { return compare(cmp) != 0; }
	int 			operator != (const char* const cmp) const { return compare(cmp) != 0; }

	const DlString& operator += (const DlString& str);
	const DlString& operator += (char c);

//	Assign
	const DlString& Assign(void* ptr, size_t len) { return AssignFromPtr(ptr, len); }

	//	number conversions
	const DlString& Assign(const double_t& value, const DlFloatFormat& form=DlString::sFloatFormat);
	const DlString& Assign(const float& value, const DlFloatFormat& form=DlString::sFloatFormat);
	const DlString& Assign(const long long& value, const DlIntFormat& form=DlString::sIntFormat);
	const DlString& Assign(const unsigned long long& value, const DlIntFormat& form=DlString::sIntFormat);
	const DlString& Assign(const long& value, const DlIntFormat& form=DlString::sIntFormat);
	const DlString& Assign(const short& value, const DlIntFormat& form=DlString::sIntFormat);
	const DlString& Assign(const unsigned long& value, const DlIntFormat& form=DlString::sIntFormat);
	const DlString& Assign(const unsigned short& value, const DlIntFormat& form=DlString::sIntFormat);

				
	const DlString& Assign(const char* str);
//	const DlString& Assign(const unsigned char* str);
	
	//	character
	const DlString& Assign(const char& value);
//	Insert

	const DlString& Insert(const char& value, size_t where);
	const DlString& Insert(const char* str, size_t where);
	const DlString& Insert(const DlString& str, size_t where);
	const DlString& Insert(const unsigned char* p, size_t where);

//	Append
	//	number conversions
	const DlString& Append(const double_t& value, const DlFloatFormat& form = DlString::sFloatFormat);
	const DlString& Append(const float& value, const DlFloatFormat& form = DlString::sFloatFormat);
	const DlString& Append(const long long& value, const DlIntFormat& form = DlString::sIntFormat);
	const DlString& Append(const unsigned long long& value, const DlIntFormat& form = DlString::sIntFormat);
	const DlString& Append(const long& value, const DlIntFormat& form = DlString::sIntFormat);
	const DlString& Append(const short& value, const DlIntFormat& form = DlString::sIntFormat);
	const DlString& Append(const unsigned long& value, const DlIntFormat& form = DlString::sIntFormat);
	const DlString& Append(const unsigned short& value, const DlIntFormat& form = DlString::sIntFormat);

	//	strings
	const DlString& Append(const char* str);
//	const DlString& Append(const unsigned char* str);
	//	character
	const DlString& Append(const char& value);
private:

	const DlString& AssignFromPtr(const void* mem, size_t len);
	const DlString& InsertFromPtr(const void* mem, size_t len, size_t where);

	const DlString& AppendFromPtr(const void* mem, size_t len);

	size_t					itsLength;	//	string length (not including null)
	DlStrStore				itsBuf;	//	storage
};

// inline functions

//	default constructor
inline 
DlString::DlString() 
	: itsLength(0) 
{
}								

//	construct empty string using mem
inline 
DlString::DlString(const DlStrStore& mem) 
	: itsLength(0)
	, itsBuf(mem)
{
}

//	copy constructor
inline 
DlString::DlString(const DlString& cpy) 
	: itsLength(cpy.itsLength)
	, itsBuf(cpy.itsBuf.owned() ? cpy.itsBuf : cpy.itsBuf.copy()) 
{
}

//	char* constructor
inline 
DlString::DlString(const char* str) 
{
	Assign(str); 
}

//	construct from resource
inline 
DlString::DlString(short resID, short index) 
{
	Assign(resID, index); 
}

//	construct from double
inline DlString::DlString(double_t value, const DlFloatFormat& form)
{
	Assign(value, form);
}

//	construct from long
inline DlString::DlString(long value, const DlIntFormat& form)
{
	Assign(value, form);
}

//	construct from long
inline DlString::DlString(unsigned long value, const DlIntFormat& form) 
{
	Assign(value, form);
}

//	construct from str255
//inline 
//DlString::DlString(const unsigned char* str) 
//{
//	Assign(str);
//}

//	copy assignment
inline const DlString& 
DlString::operator= (const DlString& str) 
{
	if (&str != this) {
		itsLength = str.itsLength;
		itsBuf = str.itsBuf.owned() ? str.itsBuf : str.itsBuf.copy();
	}
	return *this;
}								// assignment from DlString

inline const DlString&
DlString::operator= (const char* str) 
{
	return Assign(str);
}

inline char
DlString::operator[] (size_t i) const 
{	
	return static_cast<char>(i > itsLength ? 0 : itsBuf.get(i));
}

//	return reference to char
inline char& 			
DlString::operator[] (size_t i) 
{
	_DlAssert(i >= 0 && i < itsLength);
	return itsBuf.ptr()[i];
}

inline void			
DlString::TruncateTo(size_t newLen)
{
	if (newLen < itsLength) {
		itsBuf.ptr()[newLen] = 0;
		itsLength = newLen;
	}
}

inline double_t
DlString::getFloat() const
{
	double_t	val;
	sscanf(get(), "%lf", &val);
	return val;
}

//	convert to long
inline long
DlString::getLong() const
{
	long val;
	sscanf(get(), "%ld", &val);
	return val;
}

inline const DlString& 
DlString::operator+= (const DlString& str) {
	Append(str);
	return *this;
}

inline const DlString& 
DlString::operator+= (char c)
{
	Append(c);
	return *this;
}

inline const DlString& 
DlString::Assign(const double_t& value, const DlFloatFormat& form) 
{
	char	buf[256];
	return Assign(form.convert(value, buf));
}

inline const DlString& 
DlString::Assign(const float& value, const DlFloatFormat& form)
{
	char	buf[256];
	return Assign(form.convert(value, buf));
}

inline const DlString& 
DlString::Assign(const long long& value, const DlIntFormat& form)
{
	char	buf[256];
	return Assign(form.convert(value, buf));
}

inline const DlString& 
DlString::Assign(const unsigned long long& value, const DlIntFormat& form)
{
	char	buf[256];
	return Assign(form.convert(value, buf));
}

inline const DlString& 
DlString::Assign(const long& value, const DlIntFormat& form)
{
	char	buf[256];
	return Assign(form.convert(value, buf));
}

inline const DlString& 
DlString::Assign(const short& value, const DlIntFormat& form)
{
	char	buf[256];
	return Assign(form.convert((long)value, buf));
}

inline const DlString& 
DlString::Assign(const unsigned long& value, const DlIntFormat& form)
{
	char	buf[256];
	return Assign(form.convert(value, buf));
}

inline const DlString& 
DlString::Assign(const unsigned short& value, const DlIntFormat& form)
{
	char	buf[256];
	return Assign(form.convert((unsigned long)value, buf));
}

//	strings
inline const DlString& 
DlString::Assign(const char* str)
{
	return AssignFromPtr(str, strlen(str));
}

//inline const DlString& 
//DlString::Assign(const unsigned char* str)
//{
//	return AssignFromPtr(str + 1, str[0]);
//}

//	character
inline const DlString& 
DlString::Assign(const char& value)
{
	return AssignFromPtr(&value, 1);
}

inline const DlString& 
DlString::Insert(const char& value, size_t where) 
{
	return InsertFromPtr(&value, 1, where); 
}

inline const DlString& 
DlString::Insert(const char* str, size_t where) 
{
	return InsertFromPtr(str, strlen(str), where); 
}

inline const DlString& 
DlString::Insert(const DlString& str, size_t where) 
{ 
	return InsertFromPtr(str, str.Length(), where); 
}

inline const DlString& 
DlString::Insert(const unsigned char* p, size_t where) 
{ 
	return InsertFromPtr(p + 1, p[0], where); 
}

inline const DlString& 
DlString::Append(const double_t& value, const DlFloatFormat& form)
{
	char	buf[256];
	return Append(form.convert(value, buf));
}

inline const DlString& 
DlString::Append(const float& value, const DlFloatFormat& form)
{
	char	buf[256];
	return Append(form.convert(value, buf));
}

inline const DlString& 
DlString::Append(const long long& value, const DlIntFormat& form)
{
	char	buf[256];
	return Append(form.convert(value, buf));
}

inline const DlString& 
DlString::Append(const unsigned long long& value, const DlIntFormat& form)
{
	char	buf[256];
	return Append(form.convert(value, buf));
}

inline const DlString& 
DlString::Append(const long& value, const DlIntFormat& form)
{
	char	buf[256];
	return Append(form.convert(value, buf));
}

inline const DlString& 
DlString::Append(const short& value, const DlIntFormat& form)
{
	char	buf[256];
	return Append(form.convert((long)value, buf));
}

inline const DlString& 
DlString::Append(const unsigned long& value, const DlIntFormat& form)
{
	char	buf[256];
	return Append(form.convert(value, buf));
}

inline const DlString& 
DlString::Append(const unsigned short& value, const DlIntFormat& form)
{
	char	buf[256];
	return Append(form.convert((unsigned long)value, buf));
}


//	strings
inline const DlString& 
DlString::Append(const char* str)
{
	return AppendFromPtr(str, strlen(str));
}

//inline const DlString& 
//DlString::Append(const unsigned char* str)
//{
//	return AppendFromPtr(str + 1, str[0]);
//}

//	character
inline const DlString& 
DlString::Append(const char& value)
{
	return AppendFromPtr(&value, 1);
}

#pragma mark -
#pragma mark helper classes

//	template for stack based strings
template <size_t S> class DlStkStr : public DlString 
{
public:
	DlStkStr() : DlString(DlStrStore(s, S)) {}

	char		s[S];
};

//	template for stack based strings
template <size_t S> class DlStkPStr : public DlString 
{
public:
	DlStkPStr() : DlString(DlStrStore((char*)s + 1, S)) {}

	//	cast to pstring
	operator const unsigned char* () {
					s[0] = Length();
					return s;
				}

	unsigned char	s[S + 1];
};


#endif

//	eof
