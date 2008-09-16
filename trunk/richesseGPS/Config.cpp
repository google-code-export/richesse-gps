/**
 *  Config.cpp: implementation of the CConfig class.
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

#include "StdAfx.h"
#include "Config.h"
#include "../share/defs.h"
#include "../share/regs.h"

//////////////////////////////////////////////////////////////////////

LPCTSTR szPort = _T("Port");
LPCTSTR szBaudRate = _T("Baud Rate");
LPCTSTR szAutoConnectOnStartup = _T("AutoConnect On Startup");
LPCTSTR szReconnectAfterLost = _T("Reconnect After Lost");
LPCTSTR szPreventSuspend = _T("Prevent Suspend");
LPCTSTR szCoordType = _T("Coord Type");
LPCTSTR szUnitsType = _T("Units Type");

LPCTSTR szLastImportDir = _T("Last Import Dir");
LPCTSTR szSortColumn = _T("Sort Column");
LPCTSTR szSortType = _T("Sort Type");
LPCTSTR szLastExportDir = _T("Last Export Dir");

LPCTSTR szGCURL = _T("GC URL");

/*
LPCTSTR szPois = _T("POIs");
LPCTSTR szId = _T("ID");
LPCTSTR szName = _T("Name");
LPCTSTR szLongitude = _T("Longitude");
LPCTSTR szLatitude = _T("Latitude");
LPCTSTR szLink = _T("Link");
LPCTSTR szNotes = _T("Notes");
LPCTSTR szStatus = _T("Status");
LPCTSTR szType = _T("Type");
*/

CConfig Config;

//////////////////////////////////////////////////////////////////////

CPoi::CPoi() {
	Latitude = 0;
	Longitude = 0;
	Status = NotCompleted;
	Type = Traditional;
	memset(&TimeCompleted, 0, sizeof(TimeCompleted));
}

/*
void CPoi::Save(HKEY hKey) {
	CString s;

	RegWriteString(hKey, szId, Id);
	RegWriteString(hKey, szName, Name);
	RegWriteDword(hKey, szType, Type);
	RegWriteString(hKey, szLink, Link);
	RegWriteString(hKey, szNotes, Notes);

	s.Format(_T("%lf"), Latitude);
	RegWriteString(hKey, szLatitude, s);

	s.Format(_T("%lf"), Longitude);
	RegWriteString(hKey, szLongitude, s);

	RegWriteDword(hKey, szStatus, Status);
}

void CPoi::Load(HKEY hKey) {
	CString s;

	Id = RegReadString(hKey, szId, _T(""));
	Name = RegReadString(hKey, szName, _T(""));
	Type = (EType) RegReadDword(hKey, szType, Traditional);
	Link = RegReadString(hKey, szLink, _T(""));
	Notes= RegReadString(hKey, szNotes, _T(""));

	s = RegReadString(hKey, szLatitude, _T("0.0"));
	swscanf(s, _T("%lf"), &Latitude);
	
	s = RegReadString(hKey, szLongitude, _T("0.0"));
	swscanf(s, _T("%lf"), &Longitude);	

	Status = (EStatus) RegReadDword(hKey, szStatus, (DWORD) NotCompleted);
}
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig() {
	Port = CString(CONFIG_DEFAULT_PORT);
	BaudRate = CONFIG_DEFAULT_BAUD_RATE;
	CoordType = CONFIG_DEFAULT_COORD_TYPE;

	LastImportDir = CONFIG_DEFAULT_LAST_IMPORT_DIR;
	SortColumn = CONFIG_DEFAULT_SORT_COLUMN;
	SortType = CONFIG_DEFAULT_SORT_TYPE;

	GCUrl = CONFIG_DEFAULT_GC_URL;
}

CConfig::~CConfig() {
//	while (!Points.IsEmpty())
//		delete Points.RemoveHead();
}

/*
void CConfig::SavePois() {
	DWORD dwDisposition;
	HKEY hApp;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, szRoot, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hApp, &dwDisposition) == ERROR_SUCCESS) {
		// POIs
		RegDeleteKey(hApp, szPois);
		HKEY hPois;
		if (RegCreateKeyEx(hApp, szPois, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hPois, &dwDisposition) == ERROR_SUCCESS) {
			int idx = 1;
			POSITION pos = Points.GetHeadPosition();
			while (pos != NULL) {
				CPoi *poi = Points.GetNext(pos);

				CString s;
				s.Format(_T("%d"), idx);
				HKEY hP;
				if (RegCreateKeyEx(hPois, s, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hP, &dwDisposition) == ERROR_SUCCESS) {
					poi->Save(hP);
					RegCloseKey(hP);
					idx++;
				}
			}

			RegCloseKey(hPois);
		}
		RegCloseKey(hApp);
	}
}


void CConfig::LoadPois() {
	HKEY hApp;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, szRoot, 0, 0, &hApp) == ERROR_SUCCESS) {
		// POIS
		HKEY hPois;
		if (RegOpenKeyEx(hApp, szPois, 0, 0, &hPois) == ERROR_SUCCESS) {
			DWORD count = 0;
			RegQueryInfoKey(hPois, NULL, NULL, NULL, &count, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			for (UINT i = 1; i <= count; i++) {
				CString s;
				s.Format(_T("%d"), i);

				HKEY hP;
				if (RegOpenKeyEx(hPois, s, 0, 0, &hP) == ERROR_SUCCESS) {
					CPoi *poi = new CPoi();
					poi->Load(hP);
					RegCloseKey(hP);

					Points.AddTail(poi);
				}
			}

			RegCloseKey(hPois);
		}

		RegCloseKey(hApp);
	}
}
*/

void CConfig::Save() {
	DWORD dwDisposition;
	HKEY hApp;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, REG_APP, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hApp, &dwDisposition) == ERROR_SUCCESS) {
		// gps
		RegWriteString(hApp, szPort, Port);
		RegWriteDword(hApp, szBaudRate, BaudRate);
		RegWriteDword(hApp, szAutoConnectOnStartup, AutoConnectOnStartup);
		RegWriteDword(hApp, szReconnectAfterLost, ReconnectAfterLost);

		// general
		RegWriteDword(hApp, szCoordType, CoordType);
		RegWriteDword(hApp, szUnitsType, UnitsType);
		RegWriteDword(hApp, szPreventSuspend, PreventSuspend);
		
		// GUI
		RegWriteString(hApp, szLastImportDir, LastImportDir);
		RegWriteDword(hApp, szSortColumn, SortColumn);
		RegWriteDword(hApp, szSortType, SortType);
		RegWriteString(hApp, szLastExportDir, LastExportDir);

		RegCloseKey(hApp);
	}
	
	if (Config.PreventSuspend)
		SuspendKiller.Stop();
}

void CConfig::Load() {
	HKEY hApp;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_APP, 0, 0, &hApp) == ERROR_SUCCESS) {
		InstallDir = RegReadString(hApp, REG_INSTALL_DIR, CONFIG_DEFAULT_INSTALL_DIR);

		// GPS
		Port = RegReadString(hApp, szPort, CONFIG_DEFAULT_PORT);
		BaudRate = RegReadDword(hApp, szBaudRate, CONFIG_DEFAULT_BAUD_RATE);
		AutoConnectOnStartup = RegReadDword(hApp, szAutoConnectOnStartup, CONFIG_DEFAULT_AUTOCONNECTONSTARTUP);
		ReconnectAfterLost = RegReadDword(hApp, szReconnectAfterLost, CONFIG_DEFAULT_RECONNECTAFTERLOST);
		
		// general
		CoordType = (ECoordType) RegReadDword(hApp, szCoordType, CONFIG_DEFAULT_COORD_TYPE);
		UnitsType = (EUnitsType) RegReadDword(hApp, szUnitsType, CONFIG_DEFAULT_UNITS_TYPE);
		PreventSuspend = RegReadDword(hApp, szPreventSuspend, CONFIG_DEFAULT_PREVENT_SUSPEND);

		GCUrl = RegReadString(hApp, szGCURL, CONFIG_DEFAULT_GC_URL);

		//
		SortColumn = RegReadDword(hApp, szSortColumn, CONFIG_DEFAULT_SORT_COLUMN);
		if (SortColumn < 0 || SortColumn > 2)
			SortColumn = 0;
		SortType = RegReadDword(hApp, szSortType, CONFIG_DEFAULT_SORT_TYPE);

		// GUI
		LastImportDir = RegReadString(hApp, szLastImportDir, CONFIG_DEFAULT_LAST_IMPORT_DIR);
		
		TCHAR sMyDocs[MAX_PATH] = { 0 };
		SHGetSpecialFolderPath(NULL, sMyDocs, CSIDL_PERSONAL, FALSE);
		LastExportDir = RegReadString(hApp, szLastExportDir, sMyDocs);

		RegCloseKey(hApp);
	}
}
