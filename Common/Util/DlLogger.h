//
//	DlLogger.h
//
//	DlLogger logger to file or console.
//
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

#include "DlListener.h"
#include <string>

class DlLogger
{

public:

	constexpr static const char* const ConsoleLog { "console" };
	
	// create a new logger with the given path. If the path is null,
	//	the logger logs to stderr.
	DlLogger(const char* logFilePath);
	
	// move a logger instance to this one.
	DlLogger(DlLogger&& l);
	
	void SetLogPath(const char* path);
	void SetLogInPreferences(const char* name);

	void Log(const char* fmt, ...) const;
	void Log(const char* fmt, va_list ap) const;
	
	static const DlLogger& getConsole();
	
private:

	class FILETracker {
	public:
		
		FILETracker() : _f(stderr), _isConsole(false), _opened(false) {}
		
		FILETracker(const std::string& path);
		
		// FileTracker can't be deleted.
		FILETracker(const FILETracker& t) = delete;
        FILETracker& operator=(const FILETracker& t) = delete;
		
		FILETracker(FILETracker&& t);
		FILETracker& operator=(FILETracker&& t);
		
		~FILETracker();
		
		void putMsg(const char* msg) const;

	private:
		
		FILE* _f;
		bool _isConsole;
		bool _opened;
	};
	
	std::string _path;
	FILETracker _tracker;
	
	static DlLogger console;
};
