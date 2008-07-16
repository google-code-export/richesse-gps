/**
 *  helpers.h
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

#ifndef _HELPERS_H_
#define _HELPERS_H_

#include "../richesseGPS/Config.h"

BOOL SysError(int resID, DWORD err, BOOL quiet, ...);

char *WCharToChar(const CString &str, UINT codepage = CP_ACP);
CString CharToWChar(const char *str, int length, UINT codepage = CP_ACP);

CString FormatCoord(char dir, double coord);

int DrawTextEndEllipsis(CDC &dc, const CString &strText, CRect &rc, UINT uFormat);

BOOL CopyTextToClipboard(HWND hWnd, const CString &strText);

int GetCacheImage(CPoi::EType type);

#endif