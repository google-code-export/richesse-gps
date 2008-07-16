/**
 *  helpers.cpp : implementation file
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

//#include "..\stdafx.h"
#include <windows.h>
#include <afxwin.h>
#include "helpers.h"
#include "../richesseGPS/Config.h"	

CString FormatSysError(DWORD errCode) {
	TCHAR errMsg[1024];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errCode,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errMsg, 1024, NULL);

	CString strMessage;
	strMessage.Format(_T("(%d) %s"), errCode, errMsg);

	return strMessage;
}

BOOL SysError(int resID, DWORD err, BOOL quiet, ...) {
	if (!quiet) {
		TCHAR msg[1024];
		msg[0] = 0;
		va_list arglist;
		va_start(arglist, quiet);

		CString strFmt;
		strFmt.LoadString(resID);
		vswprintf(msg, strFmt, arglist);
		va_end(arglist);

		CString strMessage;
		if (err != ERROR_SUCCESS)
			strMessage.Format(_T("%s\n\n%s"), msg, FormatSysError(err));
		else
			strMessage.Format(_T("%s"), msg);
		AfxMessageBox(strMessage);
	}

	return FALSE;
}

char *WCharToChar(const CString &str, UINT codePage/* = CP_ACP*/) {
	int len = WideCharToMultiByte(codePage, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	char *conv = new char [len + 1];
	WideCharToMultiByte(codePage, 0, str, str.GetLength(), conv, len, NULL, NULL);
	conv[len] = '\0';
	return conv;
}

CString CharToWChar(const char *str, int length, UINT codepage/* = CP_ACP*/) {
	int wLen = MultiByteToWideChar(codepage, 0, str, length, NULL, 0);
	TCHAR *wStr = new TCHAR [wLen + 1];
	MultiByteToWideChar(codepage, 0, str, -1, wStr, wLen + 1);
	wStr[wLen] = '\0';
	CString s;
	s.Format(_T("%s"), wStr);
	delete [] wStr;
	return s;
}

///////////////////////////////////////////////////////////////////////////////

CString FormatCoord(char dir, double coord) {
	int degrees = (int) coord;
	double m = (coord - degrees) * 60;
	int mins = (int) m;
	double secs = (m - mins) * 60;

	CString str;
	switch (Config.CoordType) {
		case Degrees:	str.Format(_T("%c %3.6lf°"), dir, coord); break;
		case DegMin:	str.Format(_T("%c %3d° %0.4lf'"), dir, degrees, m); break;
		case DegMinSec:	str.Format(_T("%c %3d° %02d' %02.2lf\""), dir, degrees, mins, secs); break;
	}

	return str;
}

////

int DrawTextEndEllipsis(CDC &dc, const CString &strText, CRect &rc, UINT uFormat) {
	int nWidth = rc.Width();

	CRect rcTemp = rc;
	dc.DrawText(strText, rcTemp, uFormat | DT_CALCRECT);
	if (rcTemp.Width() > nWidth) {
		// Text doesn't fit in rect. We have to truncate it and add ellipsis to the end.
		CString strTemp;

		for (int i = strText.GetLength(); i >= 0; i--) {
			strTemp.Format(_T("%s..."), strText.Left(i));

			rcTemp = *rc;
			dc.DrawText(strTemp, rcTemp, uFormat | DT_CALCRECT);
			if (rcTemp.Width() < nWidth) {
				// Gotcha!
				break;
			}
		}
		return dc.DrawText(strTemp, rc, uFormat);
	}

	return dc.DrawText(strText, rc, uFormat);
}


BOOL CopyTextToClipboard(HWND hWnd, const CString &strText) {
	int ret = TRUE;

	if (hWnd != NULL && ::OpenClipboard(hWnd)) {
		::EmptyClipboard();

		HGLOBAL hClipboardData = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(TCHAR) * (strText.GetLength() + 1));
		if (hClipboardData != NULL) {
			TCHAR *pchData = (TCHAR *) GlobalLock(hClipboardData);
			lstrcpy(pchData, (LPCTSTR) strText);
			GlobalUnlock(hClipboardData);
			
			::SetClipboardData(CF_UNICODETEXT, hClipboardData);
		}
		else {
			ret = FALSE;
		}

		::CloseClipboard();
	}
	else {
		// clipboard opening failed
		ret = FALSE;
	}

	return ret;
}

int GetCacheImage(CPoi::EType type) {
	switch (type) {
		case CPoi::Traditional:  return 0;
		case CPoi::MultiCache:   return 1;
		case CPoi::LetterBox:    return 2;
		case CPoi::EarthCache:   return 3;
		case CPoi::EventCache:   return 4;
		case CPoi::MysteryCache: return 5;
		case CPoi::MyPOI:        return 6;
		case CPoi::Unknown:      return 5;
		case CPoi::Virtual:      return 7;
		default:				 return 0;
	}
}
