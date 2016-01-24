//
//  DlAppSupportFiles.h
//
//  Created by David Salmon on 3/21/14.
//  Copyright © 2014-2016 David C. Salmon. All Rights Reserved.
//
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

#ifndef Common_Util_DlAppSupportFiles_h
#define Common_Util_DlAppSupportFiles_h

#include <string>

constexpr const char* const kBundleIdFolder = "bundle";

// return the path to the specified folder in the application support folder.
// if subFolder is null, the path to the base app support folder is returned.
std::string DlGetApplicationSupportFolder(const char* subFolder, bool create=true);

// remove the specified folder from the application support folder.
void DlRemoveFolderFromApplicationSupport(const char* subFolder);

// return the standard path for application logs ~/Library/Logs/bundleid/app name.log
std::string DlGetApplicationLogPath(const char* defaultName=nullptr, bool appendDate = false);

// return the bundle id for the module containing this code.
std::string DlGetBundleId();

#endif	// Common_Util_DlAppSupportFiles_h
