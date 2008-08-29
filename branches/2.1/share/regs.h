/**
 *  regs.h
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _REGS_H_
#define _REGS_H_

void RegWriteDword(HKEY hKey, LPCTSTR valueName, DWORD value);
void RegWriteString(HKEY hKey, LPCTSTR valueName, LPCTSTR value);
DWORD RegReadDword(HKEY hKey, LPCTSTR valueName, DWORD defaultValue);
CString RegReadString(HKEY hKey, LPCTSTR valueName, LPCTSTR defaultValue);

#endif