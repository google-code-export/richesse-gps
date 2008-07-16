/**
 *  rp.cpp : implementation file
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
#include "files.h"
#include "..\..\share\file.h"
#include "binfile.h"

BOOL ReadRPFile(const CString &fileName, CList<CPoi *, CPoi *> &pois) {
	CBufferedFile file;
	if (!file.Create(fileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL))
		return FALSE;

	DWORD remain;
	CArchiveFileChunkHeader header;
	if (!header.Load(file) || strncmp(header.Id, "RGPS", 4) != 0)
		return FALSE;

	// load POIS
	if (!header.Load(file) || strncmp(header.Id, "POIS", 4) != 0)
		return FALSE;

	BOOL ok = TRUE;
	DWORD remainItems = header.Size;
	while (remainItems > 0) {
		CArchiveFileChunkHeader hdrItem;
		if (!hdrItem.Load(file) || strncmp(hdrItem.Id, "POI ", 4) != 0)
			return FALSE;
		remainItems -= 8;

		// load item
		BOOL itemOK = TRUE;
		CPoi *poi = new CPoi();
		remain = hdrItem.Size;
		while (remain > 0 && itemOK) {
			CArchiveFileChunkHeader hdr;
			if (!hdr.Load(file)) {
				itemOK = FALSE;
				break;
			}
			remain -= 8;

			if (strncmp(hdr.Id, "ID  ", 4) == 0)
				ReadString(file, hdr.Size, poi->Id);
			else if (strncmp(hdr.Id, "NAME", 4) == 0)
				ReadString(file, hdr.Size, poi->Name);
			else if (strncmp(hdr.Id, "LONG", 4) == 0)
				ReadNumber(file, poi->Longitude);
			else if (strncmp(hdr.Id, "LAT ", 4) == 0)
				ReadNumber(file, poi->Latitude);
			else if (strncmp(hdr.Id, "LINK", 4) == 0)
				ReadString(file, hdr.Size, poi->Link);
			else if (strncmp(hdr.Id, "NOTE", 4) == 0)
				ReadString(file, hdr.Size, poi->Notes);
			else if (strncmp(hdr.Id, "STAT", 4) == 0)
				ReadNumber(file, (DWORD &) poi->Status);
			else if (strncmp(hdr.Id, "TYPE", 4) == 0)
				ReadNumber(file, (DWORD &) poi->Type);
			else if (strncmp(hdr.Id, "COMP", 4) == 0)
				ReadSystemTime(file, poi->TimeCompleted);
			else
				file.Seek(hdr.Size, FILE_CURRENT);
			
			remain -= hdr.Size;
		} // while

		if (itemOK) {
			pois.AddTail(poi);
		}
		else {
			delete poi;
			ok = FALSE;
			break;
		}

		remainItems -= hdrItem.Size;
	}


	file.Close();

	return ok;
}

BOOL WriteRPFile(const CString &fileName, CList<CPoi *, CPoi *> &pois) {
	CBufferedFile file;
	if (!file.Create(fileName, GENERIC_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL))
		return FALSE;

	// id
	CArchiveFileChunk *root = new CArchiveFileChunk("RGPS");

	if (pois.GetCount() > 0) {
		// POIs
		CArchiveFileChunk *poisChunk = new CArchiveFileChunk("POIS");
		POSITION pos = pois.GetHeadPosition();
		while (pos != NULL) {
			CPoi *poi = pois.GetNext(pos);

			CArchiveFileChunk *poiChunk = new CArchiveFileChunk("POI ");
			poiChunk->Add("ID  ", poi->Id);
			poiChunk->Add("NAME", poi->Name);
			poiChunk->Add("LONG", poi->Longitude);
			poiChunk->Add("LAT ", poi->Latitude);
			poiChunk->Add("LINK", poi->Link);
			poiChunk->Add("NOTE", poi->Notes);
			poiChunk->Add("STAT", poi->Status);
			poiChunk->Add("TYPE", poi->Type);
			poiChunk->Add("COMP", poi->TimeCompleted);

			poisChunk->Add(poiChunk);
		}

		root->Add(poisChunk);
	}

	root->CalcSize();
	root->Save(file);

	delete root;

	file.Close();

	return TRUE;
}