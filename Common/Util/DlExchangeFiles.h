//
//  DlExchangeFiles.h
//
//  Created by David Salmon on 2/12/14.
//  Copyright © 2014-2016 David C. Salmon. All Rights Reserved.
//
//	Define file exchange method that safely exchanges the contents of
//	f1 with f2. If a backup char is specified, create backup.
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

#ifndef Common_Util_ExchangeFiles_h
#define Common_Util_ExchangeFiles_h

class DlFileSpec;

// replace file at f1 with content at f2, optionally creating a backup
// file
void DlExchangeFiles(const DlFileSpec& f1, const DlFileSpec f2, char backupChar);

#endif
