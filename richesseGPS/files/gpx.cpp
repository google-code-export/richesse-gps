/**
 *  gpx.cpp : implementation file
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
#include "..\..\share\helpers.h"

#include "..\..\libexpat\expat.h"


#define BUFSIZ							8192

static enum EWaypointField {
	None,
	Name,
	Desc,
	Url,
	GS_Cache,
	GS_Type,
	GS_ShortDescription,
	GS_LongDescription,
	GS_Name,
	GS_Container,
	GS_Difficulty,
	GS_Terrain,
	GS_PlacedBy,
	GS_Hints,
	GS_Logs,
	GS_Log,
	GS_LogEntry,
	GS_LogDate,
	GS_LogType,
	GS_LogText,
	GS_LogFinder,
	GS_TBs,
	GS_TB,
	GS_TB_Name
};

static XML_Parser Parser;
static EWaypointField WptState;
static CPoi *Poi;
//static CString ShortDescription, LongDescription;
//static CString Type;
static BOOL SwitchNameDesc;

static void XMLCALL
startElement(void *userData, const char *name, const char **attr) {
	if (strcmp(name, "gpx") == 0) {
		for (int i = 0; attr[i]; i += 2) {
			if (strcmp(attr[i], "creator") == 0) {
				if (strstr(attr[i + 1], "http://www.geocaching.cz") != NULL)
					SwitchNameDesc = TRUE;				// GPX files from www.geocaching.cz have name and Desc tag switched
				else
					SwitchNameDesc = FALSE;
			}
		}
	}
	else if (strcmp(name, "wpt") == 0) {
//		ShortDescription.Empty();
//		LongDescription.Empty();
//		Type.Empty();

		delete Poi;
		Poi = new CPoi;
		Poi->Type = CPoi::Traditional;

		WptState = None;

		//
		for (int i = 0; attr[i]; i += 2) {
			if (strcmp(attr[i], "lat") == 0) {
				double lat;
				sscanf(attr[i + 1], "%lf", &lat);
				Poi->Latitude = lat;
			}
			else if (strcmp(attr[i], "lon") == 0) {
				double lon;
				sscanf(attr[i + 1], "%lf", &lon);
				Poi->Longitude = lon;
			}
		}
	}
	else if (Poi != NULL) {
		if (strcmp(name, "name") == 0) WptState = Name;
		else if (strcmp(name, "desc") == 0) WptState = Desc;
		else if (strcmp(name, "url") == 0) WptState = Url;
/*		else if (strcmp(name, "groundspeak:short_description") == 0) {
			WptState = GS_ShortDescription;
		}
		else if (strcmp(name, "groundspeak:long_description") == 0) {
			WptState = GS_LongDescription;
		}
*/		else if (strcmp(name, "groundspeak:type") == 0) WptState = GS_Type;
		else if (strcmp(name, "groundspeak:name") == 0) WptState = GS_Name;
		else if (strcmp(name, "groundspeak:placed_by") == 0) WptState = GS_PlacedBy;
		else if (strcmp(name, "groundspeak:container") == 0) WptState = GS_Container;
		else if (strcmp(name, "groundspeak:difficulty") == 0) WptState = GS_Difficulty;
		else if (strcmp(name, "groundspeak:terrain") == 0) WptState = GS_Terrain;
		else if (strcmp(name, "groundspeak:short_description") == 0) WptState = GS_ShortDescription;
		else if (strcmp(name, "groundspeak:long_description") == 0) WptState = GS_LongDescription;
		else if (strcmp(name, "groundspeak:encoded_hints") == 0) WptState = GS_Hints;
		else if (strcmp(name, "groundspeak:logs") == 0) WptState = GS_Logs;
		else if (strcmp(name, "groundspeak:travelbugs") == 0) WptState = GS_TBs;
	}
	else if (WptState == GS_Logs) {
		if (strcmp(name, "groundspeak:log") == 0) WptState = GS_Log;
	}
	else if (WptState == GS_TBs) {
		if (strcmp(name, "groundspeak:travelbug") == 0) WptState = GS_TB;
	}
	else if (WptState == GS_Log) {
		if (strcmp(name, "groundspeak:date") == 0) WptState = GS_LogDate;
		else if (strcmp(name, "groundspeak:type") == 0) WptState = GS_LogType;
		else if (strcmp(name, "groundspeak:finder") == 0) WptState = GS_LogFinder;
		else if (strcmp(name, "groundspeak:text") == 0) WptState = GS_LogText;
	}
}

static void XMLCALL
endElement(void *userData, const char *name) {
	CList<CPoi *, CPoi *> *pois = (CList<CPoi *, CPoi *> *) userData;

	if (strcmp(name, "wpt") == 0) {
/*		CString s;
		if (!ShortDescription.IsEmpty()) {
			s.Format(_T("Short Description:\r\n%s"), ShortDescription);
			Poi->Notes += s;
		}

		if (!LongDescription.IsEmpty()) {
			if (!ShortDescription.IsEmpty()) {
				Poi->Notes += _T("\r\n\r\n");
			}

			s.Format(_T("Long Description:\r\n%s"), LongDescription);
			Poi->Notes += s;
		}
*/

		pois->AddTail(Poi);
		Poi = NULL;
	}
	else if (strcmp(name, "groundspeak:log") == 0) WptState = GS_Logs;
	else if (strcmp(name, "groundspeak:travelbug") == 0) WptState = GS_TBs;

	WptState = None;
}

static void XMLCALL 
charDataHandler(void *userData, const XML_Char *s, int len) {
	if (Poi != NULL) {
		if (WptState == Name) {
			if (SwitchNameDesc)
				Poi->Name = CharToWChar(s, len, CP_UTF8);
			else
				Poi->Id = CharToWChar(s, len, CP_UTF8);
		}
		else if (WptState == Desc) {
			if (SwitchNameDesc)
				Poi->Id = CharToWChar(s, len, CP_UTF8);
			else
				Poi->Name = CharToWChar(s, len, CP_UTF8);
		}
		else if (WptState == Url) {
			Poi->Link = CharToWChar(s, len, CP_UTF8);
		}
		else if (WptState == GS_Type) {
			CString type = CharToWChar(s, len, CP_UTF8);
			type.MakeLower();
			if (type.CompareNoCase(_T("traditional cache")) == 0)
				Poi->Type = CPoi::Traditional;
			else if (type.Find(_T("multi-cache")) != -1)
				Poi->Type = CPoi::MultiCache;
			else if (type.Find(_T("letterbox")) != -1)
				Poi->Type = CPoi::LetterBox;
			else if (type.Find(_T("earth")) != -1)
				Poi->Type = CPoi::EarthCache;
			else if (type.Find(_T("event")) != -1)
				Poi->Type = CPoi::EventCache;
			else if (type.Find(_T("mystery")) != -1)
				Poi->Type = CPoi::MysteryCache;
			else if (type.Find(_T("virtual cache")) != -1)
				Poi->Type = CPoi::Virtual;
			else if (type.Find(_T("unknown cache")) != -1)
				Poi->Type = CPoi::Unknown;
		}
/*		else if (WptState == GS_ShortDescription) {
			if (len > 0)
				ShortDescription = CharToWChar(s, len, CP_UTF8);
		}
		else if (WptState == GS_LongDescription) {
			if (len > 0)
				LongDescription = CharToWChar(s, len, CP_UTF8);
		}
*/	}
}

BOOL ReadGpxFile(const CString &fileName, CList<CPoi *, CPoi *> &pois) {
	char buf[BUFSIZ];
	BOOL done;

	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// reset temp node
	SwitchNameDesc = FALSE;
	Poi = NULL;
	BOOL ret = TRUE;
	Parser = XML_ParserCreate(NULL);
	XML_SetUserData(Parser, &pois);
	XML_SetXmlDeclHandler(Parser, NULL);
	XML_SetElementHandler(Parser, startElement, endElement);
	XML_SetCharacterDataHandler(Parser, charDataHandler);

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	// main load loop with the parsing
	do {
		DWORD read;
		done = FALSE;
		if (ReadFile(hFile, buf, BUFSIZ, &read, NULL)) {
			done = read < BUFSIZ;
			if (XML_Parse(Parser, buf, read, done) == XML_STATUS_ERROR) {
				CloseHandle(hFile);
				ret = FALSE;
				break;
			}
		}
	} while (!done);

	XML_ParserFree(Parser);

	CloseHandle(hFile);

	return ret;
}

