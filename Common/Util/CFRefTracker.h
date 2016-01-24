/*+
 *
 *  CFRefTracker.h
 *
 *  Copyright © 2014-2016 David C. Salmon. All Rights Reserved.
 *
 *  track CFRef objects -- retain and release.
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

#ifndef Common_Util_CFRefTracker_h
#define Common_Util_CFRefTracker_h

#define TRACK_TYPES 0

#if TRACK_TYPES
#include <typeinfo>
#endif

template <typename T> class CFRefTracker
{
public:
	CFRefTracker();
	CFRefTracker(T val);
	
	CFRefTracker(const CFRefTracker<T>& val);
	~CFRefTracker();
	
	// assignment
	CFRefTracker<T>& operator=(const CFRefTracker<T>& val);
	
	// cast
	operator T() const;
	
	bool operator !() const;
	
	T& ref();
	
private:
	T _ref;
};

//----------------------------------------------------------------------------------------
//  CFRefTracker
//
//      construct ref tracker.
//
//
//----------------------------------------------------------------------------------------
template <typename T>
inline
CFRefTracker<T>::CFRefTracker()
	: _ref(0)
{
}

//----------------------------------------------------------------------------------------
//  CFRefTracker
//
//      construct ref tracker.
//
//  T val                      -> reference to track
//----------------------------------------------------------------------------------------
template <typename T>
inline
CFRefTracker<T>::CFRefTracker(T val)
	: _ref(val)
{
#if TRACK_TYPES
	printf("constructor retain count for ptr %p (%s) is %ld\n", val, typeid(*this).name(),
		   CFGetRetainCount(_ref));
#endif
}

//----------------------------------------------------------------------------------------
//  CFRefTracker
//
//      construct ref tracker.
//
//  const CFRefTracker<T>& val         -> tracker to copy
//
//----------------------------------------------------------------------------------------
template <typename T>
inline
CFRefTracker<T>::CFRefTracker(const CFRefTracker<T>& val)
{
	_ref = val._ref;
	if (_ref != 0)
		CFRetain(_ref);
}

//----------------------------------------------------------------------------------------
//  operator=
//
//      construct ref tracker.
//
//  const CFRefTracker<T>& val         -> tracker to set this to.
//
//	returns <- reference to this.
//
//----------------------------------------------------------------------------------------
template <typename T>
inline
CFRefTracker<T>&
CFRefTracker<T>::operator=(const CFRefTracker<T>& val)
{
	if (val._ref != _ref)
	{
		if (_ref != 0)
			CFRelease(_ref);
		_ref = val._ref;
		if (_ref != 0)
			CFRetain(_ref);
		
	}
	return *this;
}

//----------------------------------------------------------------------------------------
//  ~CFRefTracker
//
//      destruct ref tracker.
//
//----------------------------------------------------------------------------------------
template <typename T>
inline
CFRefTracker<T>::~CFRefTracker()
{
	if (_ref != 0) {
#if TRACK_TYPES
		printf("destructor retain count for ptr %p (%s) is %ld\n", _ref, typeid(*this).name(),
			   CFGetRetainCount(_ref));
#endif
		CFRelease(_ref);
	}
}

//----------------------------------------------------------------------------------------
//  operator T()
//
//      cast operator to T
//
//  returns <- the reference being tracker
//----------------------------------------------------------------------------------------
template <typename T>
inline
CFRefTracker<T>::operator T() const
{
	return _ref;
}

//----------------------------------------------------------------------------------------
//  ref
//
//      explicit access to the reference being tracked.
//
//  returns <- the reference
//----------------------------------------------------------------------------------------
template <typename T>
inline
T& CFRefTracker<T>::ref()
{
	return _ref;
}

//----------------------------------------------------------------------------------------
//  operator !()
//
//      return true if the reference is not null.
//
//  returns <- true if there is a reference being tracked.
//----------------------------------------------------------------------------------------
template <typename T>
inline
bool
CFRefTracker<T>::operator !() const
{
	return !_ref;
}

#endif // Common_Util_CFRefTracker_h
