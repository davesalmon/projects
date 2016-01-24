/*+
 *	File:		DlGraphTypes.h
 *
 *	Contains:	graphics type interface
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
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

#ifndef _H_DlGraphTypes
#define _H_DlGraphTypes

#include "DlTypes.h"

struct	DlPoint2d {
	DlPoint2d() : x(0), y(0) {}
	DlPoint2d(DlFloat x_, DlFloat y_) : x(x_), y(y_) {}
	DlFloat	x;
	DlFloat	y;
};

struct DlRect2d {
	DlRect2d() : l(0), t(0), r(0), b(0) {}
	DlRect2d(const DlRect2d& r_) : l(r_.l), t(r_.t), r(r_.r), b(r_.b) {}
	DlRect2d(DlFloat l_, DlFloat t_, DlFloat r_, DlFloat b_)
		: l(l_), t(t_), r(r_), b(b_) {}

	DlInt32 Width() const { return DlInt32(r - l); }
	DlInt32 Height() const { return DlInt32(t - b); }

	DlFloat l;
	DlFloat t;
	DlFloat r;
	DlFloat b;
};

struct DlScreenPoint2d {
	DlScreenPoint2d() : x(0), y(0) {}
	DlScreenPoint2d(DlInt32 x_, DlInt32 y_) : x(x_), y(y_) {}
	DlInt32 x;
	DlInt32 y;
};

struct DlScreenRect2d {
	DlScreenRect2d() : l(0), t(0), r(0), b(0) {}
	DlScreenRect2d(const DlScreenRect2d& r_) : l(r_.l), t(r_.t), r(r_.r), b(r_.b) {}
	DlScreenRect2d(DlInt32 l_, DlInt32 t_, DlInt32 r_, DlInt32 b_)
		: l(l_), t(t_), r(r_), b(b_) {}
	
	DlInt32 Width() const { return r - l; }
	DlInt32 Height() const { return b - t; }
	
	DlInt32 l;
	DlInt32 t;
	DlInt32 r;
	DlInt32 b;
};

typedef struct rgbColor {
	DlUInt8	r, g, b;
} rgbColor;

class DlGraph2d {
public:
	DlGraph2d() : _xscale(1), _yscale(-1), _world(-1, -1, 1, 1) { update(); }

	DlGraph2d(DlFloat xscale, DlFloat yscale, const DlRect2d& wr)
		: _xscale(xscale)
		, _yscale(yscale)
		, _world(wr)
	{
		if ((_world.t > _world.b && _yscale > 0) || (_world.t < _world.b && _yscale < 0))
			_yscale = -_yscale;
		update();
	}
	
	DlPoint2d	ScreenToWorld(const DlScreenPoint2d& p) const {
		return DlPoint2d(p.x / _xscale + _world.l, 
					(p.y - _screenSize.y) / _yscale + _world.b);
	}
	
	DlScreenPoint2d WorldToScreen(const DlPoint2d& p) const {
		return DlScreenPoint2d(DlInt32((p.x - _world.l) * _xscale), 
					DlInt32((p.y - _world.b) * _yscale + _screenSize.y));
	}
	
	const DlRect2d& World() const { return _world; }
	const DlScreenPoint2d& ScreenSize() const { return _screenSize; }
	
	void SetWorld(const DlRect2d& world) {
		_world = world;
		update();
	}
	
	void SetScale(DlFloat xscale, DlFloat yscale) {
		_xscale = xscale;
		_yscale = yscale;
		update();
	}
	
	private:
		
		void update()
		{
			_screenSize.x = DlInt32((_world.r - _world.l) * _xscale);
			_screenSize.y = DlInt32((_world.b - _world.t) * _yscale);				
		}
	
		DlScreenPoint2d	_screenSize;
		DlRect2d 		_world; 
		DlFloat			_xscale;
		DlFloat 		_yscale;
		
};

#endif
