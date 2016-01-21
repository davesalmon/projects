/*++++++++++++++++++++++++++
 **	DlArray.h
 **
 **	Purpose:
 **
 **		Define iterators for valarray objects. Also define slice iterators that
 **		allow iteration through matrices.
 **
 *	Copyright:	COPYRIGHT (C) 1991-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 **	Author:		David C. Salmon
 **	Created:	Sat, Mar 2, 1991
 */
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

#ifndef _H_DlArray
#define _H_DlArray

#include <valarray>

namespace DlArray
{

	const DlInt32 kDefaultArrayOffset = 1;

/* ----------------------------------------------------------------------------
 * DlArrayIter
 *
 * supports only operator []
---------------------------------------------------------------------------- */
	template <class T> class DlArrayIter
	{
	public:
		// construct
		DlArrayIter(std::valarray<T>& arr) : _arr(arr), _index(0) {}
		// copy construct
		DlArrayIter(const DlArrayIter<T>& arr) : _arr(arr._arr), _index(arr._index) {}
		
		// return element reference
		T& operator*() 					{ return _arr[_index]; }
		T& operator[] (DlUInt32 i) 		{ return _arr[i];}
		
		// return const element reference
		const T& operator*() const				{ return _arr[_index]; }
		const T& operator[] (DlUInt32 i) const	{ return _arr[i];}

		// increment - move pointer.
		DlArrayIter& operator++() 		{ ++_index; return *this; }
		void operator+=(DlUInt32 ofs) 	{ _index += ofs; }
		void operator-=(DlUInt32 ofs) 	{ _index -= ofs; }

		// reset iterator.
		void reset()					{ _index = 0; }

		// create an iterator that points to the end of the underlyign array.
		DlArrayIter end()				{ DlArrayIter t = *this;
											t._index = _arr.size();
											return t; }
		// size of array
		DlUInt32 size() const { return static_cast<DlUInt32>(_arr.size()); }
	private:
		std::valarray<T>&	_arr;
		DlUInt32			_index;
	};

	template <class T> class DlArrayConstIter
	{
	public:
		DlArrayConstIter(const std::valarray<T>& arr) : _arr(arr), _index(0) {}
		DlArrayConstIter(const DlArrayConstIter<T>& arr) : _arr(arr._arr), _index(arr._index) {}
		
		const T& operator*() const 			{ return _arr[_index]; }
		const T& operator[] (DlUInt32 i) const { return _arr[i];}

		DlArrayConstIter& operator++() 	{ ++_index; return *this; }
		void operator+=(DlUInt32 ofs) 	{ _index += ofs; }
		void operator-=(DlUInt32 ofs) 	{ _index -= ofs; }

		void reset()					{ _index = 0; }

		DlArrayConstIter end() const 	{ DlArrayConstIter t = *this;
											t._index = _arr.size();
											return t; }
		
		DlUInt32 size() const { return static_cast<DlUInt32>(_arr.size()); }
	private:
		const std::valarray<T>& _arr;
		DlUInt32				_index;
	};

/* ----------------------------------------------------------------------------
 * DlOneBasedIterator
 *
 * supports only operator []
---------------------------------------------------------------------------- */
	template <class T> class DlOneBasedIter
	{
	public:
		DlOneBasedIter(std::valarray<T>& arr) : _arr(arr) {}
		
		T& operator[] (DlUInt32 index)				{ return _arr[index - 1]; }
		const T& operator[] (DlUInt32 index) const	{ return _arr[index - 1]; }
		
		DlUInt32 size() const				{ return static_cast<DlUInt32>(_arr.size()); }
		
		const std::valarray<T>& array() const { return _arr; }
		
	private:
		std::valarray<T>& _arr;
	};

	template <class T> class DlOneBasedConstIter
	{
	public:
		DlOneBasedConstIter(const std::valarray<T>& arr) : _arr(arr) {}
		
		const T& operator[] (DlUInt32 index) const  { return _arr[index - 1]; }
		DlUInt32 size() const 			{ return static_cast<DlUInt32>(_arr.size()); }
	private:
		const std::valarray<T>& _arr;
	};

/* ----------------------------------------------------------------------------
 * slice
 *
 * keep track of matrix slice
---------------------------------------------------------------------------- */
	class slice
	{
	public:
		slice(DlUInt32 start, DlUInt32 len, DlUInt32 stride)
			: _start(start)
			, _len(len)
			, _stride(stride)
		{}
		
		DlUInt32 _start;
		DlUInt32 _len;
		DlUInt32 _stride;
	};

/* ----------------------------------------------------------------------------
 * DlSliceIter
 *
 *  Allows construction of slices for matrice. slice contains the start,
 *	length, and stride for the slice. offset is the index of the first element
 *	in the slice (typically 0 or 1).
---------------------------------------------------------------------------- */
	template <class T, DlUInt32 OFFSET=kDefaultArrayOffset> class DlSliceIter
	{
	public:
		DlSliceIter(std::valarray<T>& arr, const slice& slice)
			: _arr(arr)
			, _slice(slice)
			, _curr(slice._start)
		{
//			printf("construct\n");
		}
				
		T& operator[] (DlUInt32 i) 	{ return ref(i); }
		const T& operator[] (DlUInt32 i) const	{ return ref(i); }
		
		DlSliceIter& operator++() 	{ _curr += _slice._stride; return *this; }
		
		T& operator*() 				{ return _arr[_curr]; }
		const T& operator*() const 	{ return _arr[_curr]; }
		
		DlSliceIter end() 			{ DlSliceIter t = *this;
										t._curr = _slice._start + _slice._len * _slice._stride;
										return t; }
		
		void reset() 				{ _curr = _slice._start; }
		DlUInt32 size() const		{ return _slice._len; }
	private:
		T& ref(DlUInt32 i) const 	{ return _arr[_slice._start +
										(i-OFFSET) * _slice._stride]; }
	
		std::valarray<T>& 	_arr;
		slice				_slice;
		DlUInt32			_curr;
	};

	template <class T, DlUInt32 OFFSET=kDefaultArrayOffset> class DlSliceConstIter
	{
	public:
		DlSliceConstIter(const std::valarray<T>& arr, const slice& slice)
			: _arr(arr)
			, _slice(slice)
			, _curr(slice._start)
		{
		}
		
		const T& operator[] (DlUInt32 i) const { return ref(i); }
		
		DlSliceConstIter& operator++() 	{ _curr += _slice._stride; return *this; }
		
		const T& operator*() const		{ return _arr[_curr]; }
		
		DlSliceConstIter end() const	{ DlSliceConstIter t = *this;
											t._curr = _slice._start + _slice._len * _slice._stride;
											return t; }
		
		void reset() 					{ _curr = _slice._start; }
		DlUInt32 size() const 			{ return _slice._len; }
	private:
		const T& ref(DlUInt32 i) const	{ return _arr[_slice._start +
											(i - OFFSET) * _slice._stride]; }
	
		const std::valarray<T>& 	_arr;
		slice						_slice;
		DlUInt32					_curr;
	};
	
}

#endif

