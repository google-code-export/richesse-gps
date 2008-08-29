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

#include "../StdAfx.h"
#include "../RichesseGPS.h"
#include "files.h"
#include "../../share/file.h"
#include "../../share/helpers.h"

#include "../../libexpat/expat.h"


#define BUFSIZ							8192

static enum EMainState {
	Gpx,
	Wpt,
	GS_Logs,
	GS_TBs,
	GCAU_Cache,
	GCAU_Logs
};

static enum EWaypointField {
	None,
	Name,
	Desc,
	Url,
	UrlName,
	Time,
	GS_Cache,
	GS_Type,
	GS_ShortDescription,
	GS_LongDescription,
	GS_Name,
	GS_Owner,
	GS_Container,
	GS_Difficulty,
	GS_Terrain,
	GS_PlacedBy,
	GS_Hints,
	GS_Log,
	GS_LogDate,
	GS_LogType,
	GS_LogText,
	GS_LogFinder,
	GS_TB,
	GS_TB_Name,
	X_Unsupported
};

struct CLogEntry {
	SYSTEMTIME Date;
	CString Text;
	BOOL Encoded;			// TRUE if HTML
	CString Type;			// TODO: use enum
	CString Finder;

	CLogEntry() {
		memset(&Date, 0, sizeof(Date));
		Encoded = TRUE;
	}
};

struct CTravelBug {
//	CString Id;
//	CString Ref;
	CString Name;
};

static XML_Parser Parser;
static EWaypointField WptState;
static EMainState WptMainState;
static CPoi *Poi;
static BOOL SwitchNameDesc;

static SYSTEMTIME GsTime;
static CString GsName;
static CString GsPlacedBy;
static CString GsOwner;
static CString GsType;
static CString GsContainer;
static CString GsDifficulty;
static CString GsTerrain;
static CString GsShortDesc;
static CString GsLongDesc;
static CString GsHints;
static BOOL GsEncoded;

static CLogEntry *GsLogEntry;
static CList<CLogEntry *, CLogEntry *> GsLog;
static CTravelBug *GsTravelBug;
static CList<CTravelBug *, CTravelBug *> GsTravelBugs;

// Reset module variables (prepare for next cache to read)
static void ResetVars() {
	GsName.Empty();
	GsPlacedBy.Empty();
	GsOwner.Empty();
	GsType.Empty();
	GsContainer.Empty();
	GsDifficulty.Empty();
	GsTerrain.Empty();
	GsShortDesc.Empty();
	GsLongDesc.Empty();
	GsHints.Empty();

	while (!GsLog.IsEmpty())
		delete GsLog.RemoveHead();
	while (!GsTravelBugs.IsEmpty())
		delete GsTravelBugs.RemoveHead();
}

static BOOL WriteString(HANDLE hFile, const CString &str) {
	int len = WideCharToMultiByte(CP_UTF8, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	char *conv = new char [len];
	WideCharToMultiByte(CP_UTF8, 0, str, str.GetLength(), conv, len, NULL, NULL);
	DWORD written;
	BOOL res = WriteFile(hFile, conv, len, &written, NULL);
	delete [] conv;
	return res;
}

static BOOL WriteTime(HANDLE hFile, SYSTEMTIME *time) {
	TCHAR str[512] = { 0 };
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, time, NULL, str, 512);
	return WriteString(hFile, str);
}

// save the HTML file with read info
static void DumpHtmlFile(const CString &id) {
	CString strCacheDir;
	strCacheDir.Format(_T("%s\\cache"), Config.InstallDir);
	CreateDirectory(strCacheDir, NULL);

	CString fileName;
	fileName.Format(_T("%s\\%s.html"), strCacheDir, id);

	HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		WriteString(hFile, _T("<html>\n"));
		WriteString(hFile, _T("<head>\n"));
		WriteString(hFile, _T("<title>"));
		WriteString(hFile, GsName);
		WriteString(hFile, _T("</title>\n"));
		WriteString(hFile, _T("<style><!--\n"));
		WriteString(hFile, _T("#cache-type { background-color: #369; padding: 4px 8px; color: #fff; }\n"));
		WriteString(hFile, _T(".body { padding: 4px 8px; }\n"));
		WriteString(hFile, _T("#cache-log { background-color: #69C; padding: 4px 8px; color: #fff; }\n"));
		WriteString(hFile, _T("#cb { padding-bottom: 4px; }\n"));
		WriteString(hFile, _T("#s-desc { padding: 4px 0px; }\n"));
		WriteString(hFile, _T("#l-desc, .txt { padding: 4px 0px; }\n"));
		WriteString(hFile, _T("#hints { padding: 4px 0px; }\n"));
		WriteString(hFile, _T("--></style>\n"));
		WriteString(hFile, _T("</head>\n"));

		WriteString(hFile, _T("<body>\n"));
		// type
		WriteString(hFile, _T("<div id=\"cache-type\">"));
		WriteString(hFile, GsType);
		WriteString(hFile, _T("</div>\n"));

		WriteString(hFile, _T("<div class=\"body\">"));
		// name
		WriteString(hFile, _T("<div id=\"cb\">A cache by "));
		WriteString(hFile, GsPlacedBy);
		WriteString(hFile, _T("</div>\n"));
		// size
		WriteString(hFile, _T("<div>"));
		WriteString(hFile, _T("<strong>Size:</strong> "));
		WriteString(hFile, GsContainer);
		WriteString(hFile, _T(", "));
		// hidden
		WriteString(hFile, _T("<strong>Hidden:</strong> "));
		WriteTime(hFile, &GsTime);
		WriteString(hFile, _T("</div>\n"));
		// difficulty
		WriteString(hFile, _T("<div>"));
		WriteString(hFile, _T("<strong>Difficulty:</strong> "));
		WriteString(hFile, GsDifficulty);
		WriteString(hFile, _T(", "));
		// Terrain
		WriteString(hFile, _T("<strong>Terrain:</strong> "));
		WriteString(hFile, GsTerrain);
		WriteString(hFile, _T("</div>\n"));

		// short description
		WriteString(hFile, _T("<div id=\"s-desc\">\n"));
		WriteString(hFile, GsShortDesc);
		WriteString(hFile, _T("</div>\n"));

		// long description
		WriteString(hFile, _T("<div id=\"l-desc\">\n"));
		WriteString(hFile, GsLongDesc);
		WriteString(hFile, _T("</div>\n"));

		// hints
		if (!GsHints.IsEmpty()) {
			WriteString(hFile, _T("<div id=\"hints\"><strong>Hints:</strong> "));
			WriteString(hFile, GsHints);
			WriteString(hFile, _T("</div>\n"));
		}

		if (GsTravelBugs.GetCount() > 0) {
			// travel bugs
			WriteString(hFile, _T("<div id=\"inventory\">\n"));
			WriteString(hFile, _T("<strong>Inventory:</strong>\n"));

			WriteString(hFile, _T("<ul>\n"));
			POSITION pos = GsTravelBugs.GetHeadPosition();
			while (pos != NULL) {
				CTravelBug *tb = GsTravelBugs.GetNext(pos);

				WriteString(hFile, _T("<li>"));
				WriteString(hFile, tb->Name);
				WriteString(hFile, _T("</li>\n"));
			}
			WriteString(hFile, _T("</ul>\n"));
			WriteString(hFile, _T("</div>\n"));
		}

		WriteString(hFile, _T("</div>\n"));

		// log entries
		WriteString(hFile, _T("<div id=\"cache-log\">Cache Logs:</div>\n"));
		WriteString(hFile, _T("<div class=\"body\">\n"));
		if (GsLog.GetCount() > 0) {
			POSITION pos = GsLog.GetHeadPosition();
			while (pos != NULL) {
				CLogEntry *le = GsLog.GetNext(pos);

				// title
				WriteString(hFile, _T("<div><strong>"));
				WriteTime(hFile, &(le->Date));
				WriteString(hFile, _T(" by "));
				WriteString(hFile, le->Finder);
				WriteString(hFile, _T("</strong></div>\n"));
				// body
				WriteString(hFile, _T("<div class=\"txt\">"));
				if (!le->Encoded) {
					le->Text.Replace(_T("\n"), _T("<br/>"));
				}
				WriteString(hFile, le->Text);
				WriteString(hFile, _T("</div>\n"));
			}
		}
		else {
			WriteString(hFile, _T("<div>No log entries</div>\n"));
		}
		WriteString(hFile, _T("</div>\n"));

		// the end
		WriteString(hFile, _T("</body>\n"));
		WriteString(hFile, _T("</html>\n"));

		CloseHandle(hFile);
	}
}

// XML handlers ///////////////////////////////////////////////////////////////

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
		WptMainState = Gpx;
	}
	else if (strcmp(name, "wpt") == 0) {
		delete Poi;
		Poi = new CPoi;
		Poi->Type = CPoi::Traditional;

		WptMainState = Wpt;

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
	else if (WptMainState == Wpt) {
		if (strcmp(name, "name") == 0) WptState = Name;
		else if (strcmp(name, "desc") == 0) WptState = Desc;
		else if (strcmp(name, "url") == 0) WptState = Url;
		else if (strcmp(name, "time") == 0) WptState = Time;
		// groundspeak tags
		else if (strcmp(name, "groundspeak:type") == 0) WptState = GS_Type;
		else if (strcmp(name, "groundspeak:name") == 0) WptState = GS_Name;
		else if (strcmp(name, "groundspeak:owner") == 0) WptState = GS_Owner;
		else if (strcmp(name, "groundspeak:placed_by") == 0) WptState = GS_PlacedBy;
		else if (strcmp(name, "groundspeak:container") == 0) WptState = GS_Container;
		else if (strcmp(name, "groundspeak:difficulty") == 0) WptState = GS_Difficulty;
		else if (strcmp(name, "groundspeak:terrain") == 0) WptState = GS_Terrain;
		else if (strcmp(name, "groundspeak:short_description") == 0) WptState = GS_ShortDescription;
		else if (strcmp(name, "groundspeak:long_description") == 0) WptState = GS_LongDescription;
		else if (strcmp(name, "groundspeak:encoded_hints") == 0) WptState = GS_Hints;
		// main state
		else if (strcmp(name, "groundspeak:logs") == 0) WptMainState = GS_Logs;
		else if (strcmp(name, "groundspeak:travelbugs") == 0) WptMainState = GS_TBs;
		else if (strcmp(name, "geocache") == 0) {
			// geocaching.com.au geocache tag
			for (int i = 0; attr[i]; i += 2) {
				if (strcmp(attr[i], "xmlns") == 0 && strstr(attr[i + 1], "http://geocaching.com.au/geocache") != NULL) {
					WptMainState = GCAU_Cache;
				}
			}
		}
	}
	else if (WptMainState == GCAU_Cache) {
		// geocaching.com.au schema-style geocache tag support
		if (strcmp(name, "name") == 0) WptState = GS_Name;
		else if (strcmp(name, "owner") == 0) WptState = GS_PlacedBy;
		else if (strcmp(name, "locale") == 0) WptState = X_Unsupported;
		else if (strcmp(name, "state") == 0) WptState = X_Unsupported;
		else if (strcmp(name, "country") == 0) WptState = X_Unsupported;
		else if (strcmp(name, "type") == 0) WptState = GS_Type;
		else if (strcmp(name, "container") == 0) WptState = GS_Container;
		else if (strcmp(name, "difficulty") == 0) WptState = GS_Difficulty;
		else if (strcmp(name, "terrain") == 0) WptState = GS_Terrain;
		else if (strcmp(name, "summary") == 0) WptState = GS_ShortDescription;
		else if (strcmp(name, "description") == 0) WptState = GS_LongDescription;
		else if (strcmp(name, "hints") == 0) WptState = GS_Hints;
		else if (strcmp(name, "licence") == 0) WptState = X_Unsupported;
		else if (strcmp(name, "logs") == 0) WptMainState = GCAU_Logs;

	}
	else if (WptMainState == GCAU_Logs) {
		if (strcmp(name, "log") == 0) {
			for (int i = 0; attr[i]; i += 2) {
				if (strcmp(attr[i], "id") == 0) {
					int id;
					sscanf(attr[i + 1], "%d", &id);
					if (id > 0) {
						GsLogEntry = new CLogEntry;
					}
					else {
						GsLogEntry = NULL;
					}
				}
			}
		}
		else if (strcmp(name, "time") == 0) WptState = GS_LogDate;
		else if (strcmp(name, "type") == 0) WptState = GS_LogType;
		else if (strcmp(name, "geocacher") == 0) WptState = GS_LogFinder;
		else if (strcmp(name, "text") == 0) {
			WptState = GS_LogText;
			// no log encoding support in GCAU schema
			if (GsLogEntry != NULL)
				GsLogEntry->Encoded = FALSE;
		}
	}
	else if (WptMainState == GS_Logs) {
		if (strcmp(name, "groundspeak:log") == 0) {
			for (int i = 0; attr[i]; i += 2) {
				if (strcmp(attr[i], "id") == 0) {
					int id;
					sscanf(attr[i + 1], "%d", &id);
					if (id > 0) {
						GsLogEntry = new CLogEntry;
					}
					else {
						GsLogEntry = NULL;
					}
				}
			}
		}
		else if (strcmp(name, "groundspeak:date") == 0) WptState = GS_LogDate;
		else if (strcmp(name, "groundspeak:type") == 0) WptState = GS_LogType;
		else if (strcmp(name, "groundspeak:finder") == 0) WptState = GS_LogFinder;
		else if (strcmp(name, "groundspeak:text") == 0) {
			BOOL encoded = FALSE;
			for (int i = 0; attr[i]; i += 2) {
				if (strcmp(attr[i], "encoded") == 0 && _stricmp(attr[i + 1], "true") == 0)
					encoded = TRUE;
			}
			WptState = GS_LogText;

			if (GsLogEntry != NULL)
				GsLogEntry->Encoded = encoded;
		}
	}
	else if (WptMainState == GS_TBs) {
		if (strcmp(name, "groundspeak:travelbug") == 0) {
			GsTravelBug = new CTravelBug;
		}
		else if (strcmp(name, "groundspeak:name") == 0) WptState = GS_TB_Name;
	}
}

static void XMLCALL
endElement(void *userData, const char *name) {
	CList<CPoi *, CPoi *> *pois = (CList<CPoi *, CPoi *> *) userData;

	if (strcmp(name, "wpt") == 0) {
		// if <groundspeak:name> was specified, use it
		if (!GsName.IsEmpty()) Poi->Name = GsName;

		pois->AddTail(Poi);
		// if <groundspeak:XXX> tags were specified, dump an HTML file to the cache
		if (!GsName.IsEmpty())
			DumpHtmlFile(Poi->Id);

		ResetVars();
		WptMainState = Gpx;
		Poi = NULL;
		GsLogEntry = NULL;
		GsTravelBug = NULL;
	}
	else if (strcmp(name, "groundspeak:log") == 0) {
		if (GsLogEntry != NULL) {
			GsLog.AddTail(GsLogEntry);
			GsLogEntry = NULL;
		}
	}
	else if (strcmp(name, "groundspeak:logs") == 0) {
		WptMainState = Wpt;
	}
	else if (strcmp(name, "groundspeak:travelbug") == 0) {
		if (GsTravelBug != NULL) {
			GsTravelBugs.AddTail(GsTravelBug);
			GsTravelBug = NULL;
		}
	}
	else if (strcmp(name, "groundspeak:travelbugs") == 0) {
		WptMainState = Wpt;
	}
	else if (strcmp(name, "geocache") == 0 && WptMainState == GCAU_Cache) {
		WptMainState = Wpt;
	}
	else if (strcmp(name, "logs") == 0 && WptMainState == GCAU_Logs) {
		WptMainState = GCAU_Cache;
	}
	else if (strcmp(name, "log") == 0 && WptMainState == GCAU_Logs) {
		if (GsLogEntry != NULL) {
			GsLog.AddTail(GsLogEntry);
			GsLogEntry = NULL;
		}
	}
	WptState = None;
}

static void XMLCALL
charDataHandler(void *userData, const XML_Char *s, int len) {
	if ((WptMainState == Wpt || WptMainState == GCAU_Cache) && Poi != NULL) {
		if (WptState == Name) {
			if (SwitchNameDesc) Poi->Name = CharToWChar(s, len, CP_UTF8);
			else Poi->Id = CharToWChar(s, len, CP_UTF8);
		}
		else if (WptState == Desc) {
			if (SwitchNameDesc) Poi->Id = CharToWChar(s, len, CP_UTF8);
			else Poi->Name = CharToWChar(s, len, CP_UTF8);
		}
		else if (WptState == Url) {
			Poi->Link = CharToWChar(s, len, CP_UTF8);
		}
		else if (WptState == Time) {
			int year, month, day;
			sscanf(s, "%d-%d-%d", &year, &month, &day);
			GsTime.wYear = year;
			GsTime.wMonth = month;
			GsTime.wDay = day;
		}
		else if (WptState == GS_Type) {
			GsType = CharToWChar(s, len, CP_UTF8);
			CString type = GsType;
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
			/* GCAU types... */
			else if (type.Find(_T("traditional")) != -1)
				Poi->Type = CPoi::Traditional;
		}
		else if (WptState == GS_Cache) ;
		else if (WptState == GS_ShortDescription) GsShortDesc += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_LongDescription) GsLongDesc += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_Name) GsName += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_Owner) GsOwner += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_Container) GsContainer += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_Difficulty) GsDifficulty += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_Terrain) GsTerrain += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_PlacedBy) GsPlacedBy += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_Hints) GsHints += CharToWChar(s, len, CP_UTF8);
	}
	else if ((WptMainState == GS_Logs || WptMainState == GCAU_Logs) && GsLogEntry != NULL) {
		if (WptState == GS_LogDate) {
			int year, month, day;
			sscanf(s, "%d-%d-%d", &year, &month, &day);
			GsLogEntry->Date.wYear = year;
			GsLogEntry->Date.wMonth = month;
			GsLogEntry->Date.wDay = day;
		}
		else if (WptState == GS_LogType) GsLogEntry->Type += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_LogText) GsLogEntry->Text += CharToWChar(s, len, CP_UTF8);
		else if (WptState == GS_LogFinder) GsLogEntry->Finder += CharToWChar(s, len, CP_UTF8);
	}
	else if (WptMainState == GS_TBs && GsTravelBug != NULL) {
		if (WptState == GS_TB_Name) GsTravelBug->Name += CharToWChar(s, len, CP_UTF8);
	}
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
	GsLogEntry = NULL;
	GsTravelBug = NULL;

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

