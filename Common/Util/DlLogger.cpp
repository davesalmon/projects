/*+
 *
 *  DlLogger.cpp
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  logger to file or console.
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

#include "DlPlatform.h"
#include "DlLogger.h"
#include "stdarg.h"

#include <Foundation/NSObjCRuntime.h>

DlLogger DlLogger::console(DlLogger::ConsoleLog);

//----------------------------------------------------------------------------------------
//  DlLogger::getConsole
//
//      return the consol logger.
//
//  returns const DlLogger&    <- 
//----------------------------------------------------------------------------------------
const DlLogger&
DlLogger::getConsole()
{
	return console;
}

// create a new logger with the given path. If the path is null,
//	the logger logs to stderr.
//----------------------------------------------------------------------------------------
//  DlLogger::DlLogger                                                        constructor
//
//      construct logger.
//
//  const char* logFilePath    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlLogger::DlLogger(const char* logFilePath)
		: _path(logFilePath ? logFilePath : ""), _tracker(logFilePath) {
}

DlLogger::DlLogger(DlLogger&& l) : _path(l._path), _tracker(std::move(l._tracker)) {};

//----------------------------------------------------------------------------------------
//  DlLogger::SetLogPath
//
//      set a file path for the logger.
//
//  const char* path   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlLogger::SetLogPath(const char* path) {
	_tracker = FILETracker(path);
}

//----------------------------------------------------------------------------------------
//  DlLogger::SetLogInPreferences
//
//      log to the specified name in the preference folder.
//
//  const char* name   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlLogger::SetLogInPreferences(const char* name) 
{
	FSRef	parent;
	DlUInt8	pathBuf[2048];
	OSErr theErr = ::FSFindFolder(kOnAppropriateDisk, kPreferencesFolderType, true, &parent);
	if (theErr == noErr) {
		theErr = FSRefMakePath(&parent, pathBuf, DlArrayElements(pathBuf)-1);
		if (theErr == noErr) {
			_tracker = FILETracker((const char*)pathBuf);
		}
	}
}

//----------------------------------------------------------------------------------------
//  DlLogger::Log
//
//      log the specified message.
//
//  const char* fmt    -> the message format string.
//  ...                -> the arguments.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlLogger::Log(const char* fmt, ...) const {
	va_list ap;
	va_start(ap, fmt);
	Log(fmt, ap);
}

//----------------------------------------------------------------------------------------
//  DlLogger::Log
//
//      log the specified message.
//
//  const char* fmt    -> message format string.
//  va_list ap         -> argument list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlLogger::Log(const char* fmt, va_list ap) const
{
	const size_t DEFAULT_SIZE = 2048;
	const size_t MAXIMUM_SIZE = (DEFAULT_SIZE << 4);
	
	char buf[DEFAULT_SIZE];
	int count = vsnprintf(buf, DlArrayElements(buf), fmt, ap);
	if (count >= DlArrayElements(buf)) {
		size_t bufSize = DEFAULT_SIZE << 1;
		
		// try to get a large enough buffer. If the buffer required is larger than 16 times
		//	the default, then just send the truncated message.
		while(true)
		{
			std::auto_ptr<char> mBuf((char*)OPERATORNEW(bufSize));
			if (vsnprintf(mBuf.get(), bufSize, fmt, ap) >= bufSize && bufSize < MAXIMUM_SIZE)
			{
				bufSize <<= 1;
			}
			else
			{
				_tracker.putMsg(mBuf.get());
			}
		}
		
	} else {
		_tracker.putMsg(buf);
	}

}

//----------------------------------------------------------------------------------------
//  DlLogger::FILETracker::putMsg
//
//      write the message to the target.
//
//  const char* msg    -> the message.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlLogger::FILETracker::putMsg(const char* msg) const {
	if (!_isConsole) {
		time_t now = time(0);
		tm timeBuf = *localtime(&now);
		char dateBuffer[64];
		strftime(dateBuffer, DlArrayElements(dateBuffer), "%D %T", &timeBuf);
		fprintf(_f, "%s %s", dateBuffer, msg);
		if ((msg[0] == 0) || (msg[strlen(msg)-1] != '\n'))
		{
			fputc('\n', _f);
		}
		fflush(_f);
	} else {
		NSLog(@"%s\n", msg);
	}
}

//----------------------------------------------------------------------------------------
//  DlLogger::FILETracker::FILETracker                                        constructor
//
//      construct file tracker.
//
//  const std::string& path    -> the file path.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlLogger::FILETracker::FILETracker(const std::string& path)
		: FILETracker() {
	if (path == DlLogger::ConsoleLog)
		_isConsole = true;
	else {
		if (!path.empty()) {
			_f = fopen(path.c_str(), "a");
		}
		
		if (!_f)
			_f = stderr;
	}
}

//----------------------------------------------------------------------------------------
//  DlLogger::FILETracker::FILETracker                                        constructor
//
//      move file tracker from t to this one. t subsequently logs to stderr.
//
//  FILETracker&& t    -> the tracker.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlLogger::FILETracker::FILETracker(FILETracker&& t)
		: _f(t._f), _isConsole(t._isConsole), _opened(t._opened)
{
	t._f = 0;
}

//----------------------------------------------------------------------------------------
//  DlLogger::FILETracker::operator=                                        constructor
//
//      move file tracker from t to this one. t subsequently logs to whatever this used to.
//
//  FILETracker&& t    -> the tracker.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlLogger::FILETracker&
DlLogger::FILETracker::operator=(FILETracker&& t)
{
	std::swap(_f, t._f);
	_opened = t._opened;
	_isConsole = t._isConsole;
	
	return *this;
}

//----------------------------------------------------------------------------------------
//  DlLogger::FILETracker::~FILETracker                                        destructor
//
//      destruct file tracker.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlLogger::FILETracker::~FILETracker()
{
	if (!_isConsole) {
		if (_opened)
			fclose(_f);
		else
			fflush(_f);
	}
}
