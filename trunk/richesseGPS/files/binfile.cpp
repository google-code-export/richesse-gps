/**
 *  binfile.cpp : implementation file
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

#include "..\stdafx.h"
#include "..\RichesseGPS.h"
#include "binfile.h"

// Load/Save


BOOL ReadString(CBufferedFile &file, int len, CString &str) {
	if (len > 0) {
		char *buffer = new char[len + 1];
		memset(buffer, 0, sizeof(char) * (len + 1));
		if (!file.Read(buffer, len))
			return FALSE;
		str = CharToWChar(buffer, -1, CP_UTF8);
		delete [] buffer;
	}
	else
		str.Empty();
	
	return TRUE;
}

BOOL ReadNumber(CBufferedFile &file, int &number) {
	int i;
	if (!file.Read(&i, sizeof(i)))
		return FALSE;
	number = i;
	return TRUE;
}

BOOL ReadNumber(CBufferedFile &file, DWORD &number) {
	DWORD i;
	if (!file.Read(&i, sizeof(i)))
		return FALSE;
	number = i;
	return TRUE;
}

BOOL ReadNumber(CBufferedFile &file, double &number) {
	double d;
	if (!file.Read(&d, sizeof(d)))
		return FALSE;
	number = d;
	return TRUE;
}

BOOL ReadSystemTime(CBufferedFile &file, SYSTEMTIME &st) {
	FILETIME ft;
	char buffer[8];
	if (!file.Read(buffer, sizeof(buffer)))
		return FALSE;
	memcpy(&ft.dwHighDateTime, buffer + 0, sizeof(DWORD));
	memcpy(&ft.dwLowDateTime, buffer + 4, sizeof(DWORD));
	FileTimeToSystemTime(&ft, &st);
	return TRUE;
}
