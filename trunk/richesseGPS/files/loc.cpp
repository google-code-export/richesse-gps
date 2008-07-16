/**
 *  loc.cpp : implementation file
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

#include "../../libexpat/expat.h"


#define BUFSIZ							8192

static enum EWaypointField {
	None,
	Name,
	Coord,
	Type,
	Link
};

static XML_Parser Parser;
static EWaypointField WptState;
static CPoi *Poi;

static void XMLCALL
startElement(void *userData, const char *name, const char **attr) {
	if (strcmp(name, "waypoint") == 0) {
		delete Poi;
		Poi = new CPoi;
		Poi->Type = CPoi::Traditional;

		WptState = None;
	}
	else if (Poi != NULL) {
		if (strcmp(name, "name") == 0) {
			WptState = Name;

			for (int i = 0; attr[i]; i += 2) {
				if (strcmp(attr[i], "id") == 0) {
					Poi->Id = CharToWChar(attr[i + 1], -1, CP_UTF8);
				}
			}
		}
		else if (strcmp(name, "coord") == 0) {
			WptState = Coord;

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
		else if (strcmp(name, "type") == 0) {
			WptState = Type;
		}
		else if (strcmp(name, "link") == 0) {
			WptState = Link;
		}
	}
}

static void XMLCALL
endElement(void *userData, const char *name) {
	CList<CPoi *, CPoi *> *pois = (CList<CPoi *, CPoi *> *) userData;

	if (strcmp(name, "waypoint") == 0) {
		pois->AddTail(Poi);
		Poi = NULL;
	}

	WptState = None;
}

static void XMLCALL 
charDataHandler(void *userData, const XML_Char *s, int len) {
	if (Poi != NULL) {
		if (WptState == Name) {
			Poi->Name = CharToWChar(s, len, CP_UTF8);
		}
		else if (WptState == Link) {
			Poi->Link = CharToWChar(s, len, CP_UTF8);
		}
	}
}

BOOL ReadLocFile(const CString &fileName, CList<CPoi *, CPoi *> &pois) {
	char buf[BUFSIZ];
	BOOL done;

	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// reset temp node
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

