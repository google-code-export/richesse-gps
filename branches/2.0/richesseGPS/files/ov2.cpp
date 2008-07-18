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

#include "../StdAfx.h"
#include "../RichesseGPS.h"
#include "files.h"
#include "../../share/file.h"
#include "../../share/helpers.h"


BOOL ReadOV2File(const CString &fileName, CList<CPoi *, CPoi *> &pois) {
	CBufferedFile file(16384);
	if (file.Create(fileName, GENERIC_READ, 0, OPEN_EXISTING, 0)) {
		do {
			BYTE hdr;
			DWORD size;
			int longitude, latitude;
			DWORD read;
			
			if (!file.Read(&hdr, sizeof(hdr), &read) || read < sizeof(hdr) || hdr != 2) break;
			if (!file.Read(&size, sizeof(size), &read) || read < sizeof(hdr)) break;
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
		} while (TRUE);
		file.Close();

		return TRUE;
	}
	else
		return FALSE;
}
