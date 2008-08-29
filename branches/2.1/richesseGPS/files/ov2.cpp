/**
 *  ov2.cpp : implementation file
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

// See http://search.cpan.org/src/HPA/Geo-Ov2-0.91/lib/Geo/Ov2.pm for details

#include "../StdAfx.h"
#include "../RichesseGPS.h"
#include "files.h"
#include "../../share/file.h"
#include "../../share/helpers.h"


BOOL ReadOV2File(const CString &fileName, CList<CPoi *, CPoi *> &pois) {
	CBufferedFile file(16384);
	if (file.Create(fileName, GENERIC_READ, 0, OPEN_EXISTING, 0)) {
		do {
			BYTE type;
			DWORD size;
			int longitude, latitude;
			DWORD read;

			if (!file.Read(&type, sizeof(type), &read) || read < sizeof(type)) break;

			if (type == 0x01) {
				if (file.Seek(20, FILE_CURRENT) == INVALID_SET_FILE_POINTER) break;
			}
			else if (type == 0x02) {
				if (!file.Read(&size, sizeof(size), &read) || read < sizeof(size)) break;
				if (!file.Read(&longitude, sizeof(longitude), &read) || read < sizeof(longitude)) break;
				if (!file.Read(&latitude, sizeof(latitude), &read) || read < sizeof(latitude)) break;

				int len = size - 13;
				char *name = new char [len];
				if (!file.Read(name, len, &read) || read < sizeof(len)) {
					delete [] name;
					break;
				}

				CPoi *poi = new CPoi();
				poi->Name = CharToWChar(name, len);
				poi->Latitude = longitude / 1e5;
				poi->Longitude = latitude / 1e5;
				pois.AddTail(poi);
			}
			else if (type == 0x04) {
				if (file.Seek(6, FILE_CURRENT) == INVALID_SET_FILE_POINTER) break;
			}
			else if (type == 0x05 || type == 0x15) {
				if (file.Seek(8, FILE_CURRENT) == INVALID_SET_FILE_POINTER) break;
			}
			else if (type == 0x06) {
				if (file.Seek(9, FILE_CURRENT) == INVALID_SET_FILE_POINTER) break;
			}
			else if (type == 0x07 || type == 0x08 || type == 0x18 || type == 0x09 || type == 0x19 || type == 0x0a || type == 0x1a || type == 0x0c) {
				BYTE sz;
				if (!file.Read(&sz, sizeof(sz), &read) || read < sizeof(sz)) break;
				if (file.Seek(6 + sz, FILE_CURRENT) == INVALID_SET_FILE_POINTER) break;
			}
			else
				break;

		} while (TRUE);
		file.Close();

		return TRUE;
	}
	else
		return FALSE;
}
