/**
 *  Config.h: interface for the CConfig class.
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

#if !defined(AFX_CONFIG_H__3AC5942A_61A5_4469_ADFE_264704448F3C__INCLUDED_)
#define AFX_CONFIG_H__3AC5942A_61A5_4469_ADFE_264704448F3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../share/defs.h"

// config
#define CONFIG_DEFAULT_INSTALL_DIR			_T("\\")

#define CONFIG_DEFAULT_PORT					_T("COM1")
#define CONFIG_DEFAULT_BAUD_RATE			9600
#define CONFIG_DEFAULT_AUTOCONNECTONSTARTUP	FALSE
#define CONFIG_DEFAULT_RECONNECTAFTERLOST	FALSE

#define CONFIG_DEFAULT_COORD_TYPE			DegMin
#define CONFIG_DEFAULT_UNITS_TYPE           Metric
#define CONFIG_DEFAULT_PREVENT_SUSPEND		FALSE

#define CONFIG_DEFAULT_GC_URL				_T("http://www.geocaching.com/seek/cache_details.aspx?wp=%s&pf=y&decrypt=y")

#define CONFIG_DEFAULT_LAST_IMPORT_DIR		_T("\\")

#define CONFIG_DEFAULT_SORT_COLUMN			0
#define CONFIG_DEFAULT_SORT_TYPE			0


/////////////////////////////////////////////////////////////////////

enum ECoordType {
	Degrees = 0,
	DegMin = 1,
	DegMinSec = 2
};

enum EUnitsType {
	Metric = 0,
	English = 1
};

/////////////////////////////////////////////////////////////////////

// point of interest
class CPoi {
public:
	enum EStatus {
		NotCompleted = 0,
		Completed = 1,
		NotFound = 2
	};

	enum EType {
		Traditional = 0,
		MultiCache,
		LetterBox,
		EarthCache,
		EventCache,
		MysteryCache,
		MyPOI,
		Unknown,
		Virtual
	};

	CString Id;
	CString Name;
	double Longitude;
	double Latitude;
	CString Link;
	CString Notes;
	EStatus Status;
	EType Type;
	SYSTEMTIME TimeCompleted;

	CPoi();

//	void Save(HKEY hKey);
//	void Load(HKEY hKey);
};

/////////////////////////////////////////////////////////////////////

class CConfig {
public:
	CString InstallDir;

	// gps
	CString Port;
	int BaudRate;
	BOOL AutoConnectOnStartup;
	BOOL ReconnectAfterLost;

	//
	ECoordType CoordType;
	EUnitsType UnitsType;
	BOOL PreventSuspend;

	CString GCUrl;

	// GUI
	CString LastImportDir;
	int SortColumn;
	int SortType;
	CString LastExportDir;


	CConfig();
	virtual ~CConfig();

	void Save();
	void Load();
};

extern CConfig Config;
extern LPCTSTR szConfig;

///////////////////////////////////////////////////////////////////////////////

void RegWriteDword(HKEY hKey, LPCTSTR valueName, DWORD value);
void RegWriteString(HKEY hKey, LPCTSTR valueName, LPCTSTR value);
DWORD RegReadDword(HKEY hKey, LPCTSTR valueName, DWORD defaultValue);
CString RegReadString(HKEY hKey, LPCTSTR valueName, LPCTSTR defaultValue);


#endif // !defined(AFX_CONFIG_H__3AC5942A_61A5_4469_ADFE_264704448F3C__INCLUDED_)
