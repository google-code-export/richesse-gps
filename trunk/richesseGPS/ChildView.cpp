/**
 *  ChildView.cpp : implementation of the CChildView class
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
#include "RichesseGPS.h"
#include "ChildView.h"
#include "../share/helpers.h"

#include "AddWaypointDlg.h"
#include "WaypointNotes.h"
#include "DownloadManagerDlg.h"
#include "units.h"
#include "net/Connection.h"

#include "../share/file.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	View = Compass;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_ADD_BTN, OnAdd)
	ON_COMMAND(ID_MENU_ADD_NEW_POI, OnAdd)
	ON_BN_CLICKED(IDC_COMPASS_BTN, OnCompassView)
	ON_COMMAND(ID_MENU_COMPASS_VIEW, OnCompassView)
	ON_BN_CLICKED(IDC_NOTES_BTN, OnNotesView)
	ON_COMMAND(ID_MENU_NOTEPAD_VIEW, OnNotesView)
	ON_BN_CLICKED(IDC_INFO_BTN, OnInfoView)
	ON_COMMAND(ID_MENU_POIINFO_VIEW, OnInfoView)
	ON_EN_CHANGE(IDC_NOTES, OnChangeNotes)
	//}}AFX_MSG_MAP
	ON_NOTIFY(GN_CONTEXTMENU, IDC_WAYPOINT, OnWaypointCtx)
	ON_NOTIFY(GN_CONTEXTMENU, IDC_LONGITUDE, OnPositionCtx)
	ON_NOTIFY(GN_CONTEXTMENU, IDC_LATITUDE, OnPositionCtx)

	ON_COMMAND(ID_COPY_WAYPOINT, OnCopyWaypoint)
	ON_COMMAND(ID_COPY_LONGITUDE, OnCopyLongitude)
	ON_COMMAND(ID_COPY_LATITUDE, OnCopyLatitude)

	ON_COMMAND_RANGE(ID_POI_MARKAS_NOTCOMPLETED, ID_POI_MARKAS_NOTFOUND, OnMarkAs)
	ON_COMMAND(ID_MENU_DOWNLOADINFO, OnDownloadInfo)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
		NULL, HBRUSH(COLOR_WINDOW+1), NULL);

	CreateFonts();

	return TRUE;
}

void CChildView::OnPaint() {
	CPaintDC dc(this); // device context for painting

	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::CreateFonts() {
	HGDIOBJ hObj = ::GetStockObject(SYSTEM_FONT);
	LOGFONT lf;
	::GetObject(hObj, sizeof(LOGFONT), (LPVOID) &lf);
	lf.lfHeight = SCALEY(12) + 1;
	m_fntBase.CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	lf.lfHeight = SCALEY(12) + 1;
	m_fntBold.CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	lf.lfHeight = SCALEY(13);
	m_fntWaypoint.CreateFontIndirect(&lf);

	lf.lfWeight = FW_NORMAL;
//	lf.lfHeight = SCALEY(11) + 1;
	lf.lfHeight = SCALEY(12) + 1;
	m_fntSmall.CreateFontIndirect(&lf);
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;

	m_ctlWaypoint.Create(_T("Waypoint Name"), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY | SS_NOPREFIX, rc, this, IDC_WAYPOINT);
	m_ctlWaypoint.SetFont(&m_fntWaypoint);

	m_lblAltitude.Create(_T("Altitude"), WS_CHILD | WS_VISIBLE, rc, this, IDC_STATIC);
	m_lblAltitude.m_fntFont = &m_fntSmall;
	m_lblAltitude.m_clrText = RGB(0x99, 0x99, 0x99);

	m_ctlAltitude.Create(_T("800.1 m"), WS_CHILD | WS_VISIBLE | SS_RIGHT, rc, this, IDC_STATIC);
	m_ctlAltitude.SetFont(&m_fntBold);

	m_lblAzimuth.Create(_T("Azimuth"), WS_CHILD | WS_VISIBLE, rc, this, IDC_STATIC);
	m_lblAzimuth.m_fntFont = &m_fntSmall;
	m_lblAzimuth.m_clrText = RGB(0x99, 0x99, 0x99);

	m_ctlAzimuth.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_RIGHT, rc, this, IDC_STATIC);
	m_ctlAzimuth.SetFont(&m_fntBold);

	m_lblSpeed.Create(_T("Speed"), WS_CHILD | WS_VISIBLE | SS_RIGHT, rc, this, IDC_STATIC);
	m_lblSpeed.m_fntFont = &m_fntSmall;
	m_lblSpeed.m_clrText = RGB(0x99, 0x99, 0x99);

	m_lblHDOP.Create(_T("HDOP"), WS_CHILD | WS_VISIBLE | SS_RIGHT, rc, this, IDC_STATIC);
	m_lblHDOP.m_fntFont = &m_fntSmall;
	m_lblHDOP.m_clrText = RGB(0x99, 0x99, 0x99);

	m_ctlHDOP.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_RIGHT, rc, this, IDC_STATIC);
	m_ctlHDOP.SetFont(&m_fntBold);

	m_ctlSpeed.Create(_T("16 km/h"), WS_CHILD | WS_VISIBLE | SS_RIGHT, rc, this, IDC_STATIC);
	m_ctlSpeed.SetFont(&m_fntBold);


	m_ctlSatellites.Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this, IDC_STATIC);
	m_ctlLongitude.Create(_T("N 13 50\"50"), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rc, this, IDC_LONGITUDE);
	m_ctlLongitude.SetFont(&m_fntSmall);
	m_ctlLatitude.Create(_T("W 106 50\"80"), WS_CHILD | WS_VISIBLE | SS_NOTIFY, rc, this, IDC_LATITUDE);
	m_ctlLatitude.SetFont(&m_fntSmall);

	m_lblDistance.Create(_T("Distance"), WS_CHILD | WS_VISIBLE | SS_RIGHT, rc, this, IDC_STATIC);
	m_lblDistance.m_fntFont = &m_fntSmall;
	m_lblDistance.m_clrText = RGB(0x99, 0x99, 0x99);

	m_ctlDistance.Create(_T("1001 m"), WS_CHILD | WS_VISIBLE | SS_RIGHT, rc, this, IDC_STATIC);
	m_ctlDistance.m_clrText = RGB(0x99, 0x00, 0x0);
	m_ctlDistance.m_fntFont = &m_fntBold;

	m_ctlCompass.Create(_T("Compass"), WS_CHILD | WS_VISIBLE, rc, this, IDC_COMPASS);

	/// btns
	m_btnAdd.Create(NULL, WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, rc, this, IDC_ADD_BTN);
	m_btnCompass.Create(NULL, WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, rc, this, IDC_COMPASS_BTN);
	m_btnNotes.Create(NULL, WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, rc, this, IDC_NOTES_BTN);
	m_btnInfo.Create(NULL, WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, rc, this, IDC_INFO_BTN);

	CDC *pDC = GetDC();
	if (pDC->GetDeviceCaps(LOGPIXELSX) > 130) {
		m_btnAdd.LoadBitmaps(MAKEINTRESOURCE(IDB_BTN_ADD_HI));
		m_btnCompass.LoadBitmaps(MAKEINTRESOURCE(IDB_BTN_COMPASS_HI));
		m_btnNotes.LoadBitmaps(MAKEINTRESOURCE(IDB_BTN_NOTES_HI));
		m_btnInfo.LoadBitmaps(MAKEINTRESOURCE(IDB_BTN_INFO_HI));
	}
	else {
		m_btnAdd.LoadBitmaps(MAKEINTRESOURCE(IDB_BTN_ADD));
		m_btnCompass.LoadBitmaps(MAKEINTRESOURCE(IDB_BTN_COMPASS));
		m_btnNotes.LoadBitmaps(MAKEINTRESOURCE(IDB_BTN_NOTES));
		m_btnInfo.LoadBitmaps(MAKEINTRESOURCE(IDB_BTN_INFO));
	}
	ReleaseDC(pDC);

	m_ctlNotes.Create(WS_CHILD | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | WS_BORDER, rc, this, IDC_NOTES);

	//
	m_ctlInfo.Create(WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER, rc, GetSafeHwnd(), IDC_INFO);
	::SetWindowLong(m_ctlInfo.GetHwnd(), GWL_ID, 12321);
	::SendMessage(m_ctlInfo.GetHwnd(), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) _T(""));

	m_ctlInfo.EnableContextMenu(TRUE);

	m_ctlInfo.EnableScripting(FALSE);
	m_ctlInfo.ZoomLevel(1);

	m_ctlInfo.Clear();
	m_ctlInfo.AddText(L"<html>");
	m_ctlInfo.AddText(L"<center>No waypoint selected</center>");
	m_ctlInfo.AddText(L"</html>");

	m_ctlInfo.EndOfSource();

	return 0;
}

void CChildView::OnSize(UINT nType, int cx, int cy) {
	CWnd ::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(&rc);

	m_ctlWaypoint.SetWindowPos(NULL, SCALEX(2), SCALEY(4), rc.Width() - SCALEX(4), SCALEY(14), SWP_NOZORDER);

	m_ctlCompass.SetWindowPos(NULL, SCALEX(40), SCALEY(35), SCALEX(160), SCALEY(160), SWP_NOZORDER);

	m_lblAltitude.SetWindowPos(NULL, SCALEX(6), SCALEY(20), SCALEX(58), SCALEY(12), SWP_NOZORDER);
	m_ctlAltitude.SetWindowPos(NULL, SCALEX(6), SCALEY(32), SCALEX(58), SCALEY(12), SWP_NOZORDER);

	m_lblAzimuth.SetWindowPos(NULL, SCALEX(6), SCALEY(47), SCALEX(40), SCALEY(12), SWP_NOZORDER);
	m_ctlAzimuth.SetWindowPos(NULL, SCALEX(6), SCALEY(59), SCALEX(35), SCALEY(12), SWP_NOZORDER);

	m_lblSpeed.SetWindowPos(NULL, rc.right - SCALEX(58) - SCALEX(8), SCALEY(20), SCALEX(58), SCALEY(12), SWP_NOZORDER);
	m_ctlSpeed.SetWindowPos(NULL, rc.right - SCALEX(58) - SCALEX(8), SCALEY(33), SCALEX(58), SCALEY(12), SWP_NOZORDER);

	m_lblHDOP.SetWindowPos(NULL, rc.right - SCALEX(35) - SCALEX(8), SCALEY(47), SCALEX(35), SCALEY(12), SWP_NOZORDER);
	m_ctlHDOP.SetWindowPos(NULL, rc.right - SCALEX(35) - SCALEX(8), SCALEY(59), SCALEX(35), SCALEY(12), SWP_NOZORDER);

	m_lblDistance.SetWindowPos(NULL, rc.right - SCALEX(58) - SCALEX(8), SCALEY(190), SCALEX(58), SCALEY(12), SWP_NOZORDER);
	m_ctlDistance.SetWindowPos(NULL, rc.right - SCALEX(68) - SCALEX(8), SCALEY(203), SCALEX(68), SCALEY(16), SWP_NOZORDER);

	m_ctlSatellites.SetWindowPos(NULL, SCALEX(6), SCALEY(171), SCALEX(50), SCALEY(24), SWP_NOZORDER);
	m_ctlLatitude.SetWindowPos(NULL,   SCALEX(6), SCALEY(197), SCALEX(80), SCALEY(12), SWP_NOZORDER);
	m_ctlLongitude.SetWindowPos(NULL,  SCALEX(6), SCALEY(208), SCALEX(80), SCALEY(12), SWP_NOZORDER);

	// btns
	m_btnAdd.SetWindowPos(NULL, SCALEX(6), SCALEY(230), SCALEX(32), SCALEY(32), SWP_NOZORDER);
	m_btnCompass.SetWindowPos(NULL, rc.right - SCALEX(96 + 8) - SCALEX(8), SCALEY(230), SCALEX(32), SCALEY(32), SWP_NOZORDER);
	m_btnNotes.SetWindowPos(NULL, rc.right - SCALEX(64 + 4) - SCALEX(8), SCALEY(230), SCALEX(32), SCALEY(32), SWP_NOZORDER);
	m_btnInfo.SetWindowPos(NULL, rc.right - SCALEX(32) - SCALEX(8), SCALEY(230), SCALEX(32), SCALEY(32), SWP_NOZORDER);

	m_ctlNotes.SetWindowPos(NULL, SCALEX(0), SCALEY(22), rc.Width(), SCALEY(200), SWP_NOZORDER);
//	m_ctlNotes.SetWindowPos(NULL, SCALEX(0), SCALEY(22), rc.Width(), cy - SCALEY(30 + 40), SWP_NOZORDER);

	::SetWindowPos(m_ctlInfo.GetHwnd(), NULL, SCALEX(0), SCALEY(22), rc.Width(), SCALEY(200), SWP_NOZORDER);
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point) {
//	DEBUG:
//	m_ctlCompass.SetNorthDirection(170);
//	Default();

//	CWnd ::OnLButtonDown(nFlags, point);
}

void CChildView::ShowInfo() {
	CString s;

	if (View == Compass) {
		if (theApp.Gps.GpsOn && theApp.Gps.Quality > 0) {
			s = FormatCoord(theApp.Gps.Latitude > 0 ? 'N': 'S', fabs(theApp.Gps.Latitude));
			m_ctlLatitude.SetWindowText(s);

			s = FormatCoord(theApp.Gps.Longitude > 0 ? 'E' : 'W', fabs(theApp.Gps.Longitude));
			m_ctlLongitude.SetWindowText(s);

			// speed
			FormatSpeed(s, theApp.Gps.Speed);
			m_ctlSpeed.SetWindowText(s);

			m_ctlCompass.SetNorthDirection((int) theApp.Gps.Track);
			m_ctlCompass.Invalidate();

			FormatAltitude(s, theApp.Gps.Altitude);
			m_ctlAltitude.SetWindowText(s);

			/////////
			s.Format(_T("%5.0lf"), theApp.Gps.Track);
			m_ctlAzimuth.SetWindowText(s);

			s.Format(_T("%5.2lf"), theApp.Gps.HDOP);
			m_ctlHDOP.SetWindowText(s);
			/////////

			for (int i = 0; i < MAX_SATELLITES; i++) {
				m_ctlSatellites.m_arSatellites[i] = theApp.Gps.Satellites[i];
				m_ctlSatellites.m_arSatellitesForFix[i] = theApp.Gps.SatellitesForFix[i];
			}
	//		m_ctlSatellites.ShowWindow(SW_SHOW);
			m_ctlSatellites.Invalidate();
		}
		else {
//			m_ctlWaypoint.SetWindowText(_T(""));
			s.LoadString(IDS_NO_WAYPOINT_SELECTED);
			m_ctlWaypoint.SetWindowText(s);

			m_ctlAltitude.SetWindowText(_T("---"));
			m_ctlAzimuth.SetWindowText(_T("---"));
			m_ctlSpeed.SetWindowText(_T("---"));
			m_ctlHDOP.SetWindowText(_T("---"));
			m_ctlDistance.SetWindowText(_T("---"));

			m_ctlLatitude.SetWindowText(_T("---"));
			m_ctlLongitude.SetWindowText(_T("---"));

			// satellites
	//		m_ctlSatellites.ShowWindow(SW_HIDE);
			for (int i = 0; i < MAX_SATELLITES; i++) {
				m_ctlSatellites.m_arSatellites[i].SNR = 0;
				m_ctlSatellites.m_arSatellitesForFix[i] = -1;
			}
			m_ctlSatellites.Invalidate();
		}

		if (theApp.Destination != NULL) {
			m_ctlWaypoint.SetWindowText(theApp.Destination->Name);

	//		EnterCriticalSection(&CSGpsInfo);
			double distance, dir;
			CalcDistanceDir(theApp.Gps.Latitude, theApp.Gps.Longitude, theApp.Destination->Latitude, theApp.Destination->Longitude, distance, dir);

	//		theApp.Track = 275;
	//		double dAzimuth = (theApp.Track * (PI / 180.0)) - dir;

			///////////
	//		s.Format(_T("%5.2lf"), dir * 180.0 / PI);
	//		m_ctlAzimuth2.SetWindowText(s);
			///////////

			if (theApp.Gps.Quality > 0) {
				CString sDist;
				FormatDistance(sDist, distance);
				m_ctlDistance.SetWindowText(sDist);

				m_ctlCompass.SetDirection((int) (theApp.Gps.Track - (dir * 180.0 / PI)));
				m_ctlCompass.Invalidate();
			}
			else {
	//			s.LoadString(IDS_UNKNOWN);
				m_ctlDistance.SetWindowText(_T("---"));
	//			m_ctlDir.SetWindowText(_T(""));
			}
		}
		else {
	//		s.LoadString(IDS_NO_TARGET_SELECTED);
	//		m_ctlWaypoint.SetWindowText(s);
	//		m_ctlAzimuth2.SetWindowText(_T(""));
			s.LoadString(IDS_NO_WAYPOINT_SELECTED);
			m_ctlWaypoint.SetWindowText(s);
		}
	}
}


void CChildView::OnAdd() {
	CWaypointPg pgWaypoint;
	CWaypointNotesPg pgNotes;
	CWaypointSheet sheet(IDS_ADD_WAYPOINT, this);

	sheet.AddPage(&pgWaypoint);
	sheet.AddPage(&pgNotes);
	sheet.SetMenu(IDR_ADD);

	// GPS fixed
	if (theApp.Gps.Quality > 0) {
		// pre fill the dialog
		pgWaypoint.m_dLatitude = theApp.Gps.Latitude;
		pgWaypoint.m_dLongitude = theApp.Gps.Longitude;
	}

	pgWaypoint.m_nType = CPoi::MyPOI;

	if (sheet.DoModal()) {
		CPoi *poi = new CPoi();
		poi->Name = pgWaypoint.m_strName;
		poi->Id = _T("");
		poi->Latitude = pgWaypoint.m_dLatitude;
		poi->Longitude = pgWaypoint.m_dLongitude;
		poi->Type = (CPoi::EType) pgWaypoint.m_nType;
		poi->Notes = pgNotes.m_strDescription;

		theApp.POIs.AddTail(poi);
	}
}

void CChildView::ShowCompassPg(BOOL show) {
	int cmd = show ? SW_SHOW : SW_HIDE;

	m_ctlCompass.ShowWindow(cmd);

	m_lblAltitude.ShowWindow(cmd);
	m_ctlAltitude.ShowWindow(cmd);

	m_lblAzimuth.ShowWindow(cmd);
	m_ctlAzimuth.ShowWindow(cmd);

	m_lblSpeed.ShowWindow(cmd);
	m_ctlSpeed.ShowWindow(cmd);

	m_lblHDOP.ShowWindow(cmd);
	m_ctlHDOP.ShowWindow(cmd);

	m_lblDistance.ShowWindow(cmd);
	m_ctlDistance.ShowWindow(cmd);

	m_ctlSatellites.ShowWindow(cmd);
	m_ctlLatitude.ShowWindow(cmd);
	m_ctlLongitude.ShowWindow(cmd);
}

void CChildView::ShowNotesPg(BOOL show) {
	int cmd = show ? SW_SHOW : SW_HIDE;

	m_ctlNotes.ShowWindow(cmd);
}

void CChildView::ShowInfoPg(BOOL show) {
	int cmd = show ? SW_SHOW : SW_HIDE;

	::ShowWindow(m_ctlInfo.GetHwnd(), cmd);
}

void CChildView::OnCompassView() {
	View = Compass;

	ShowNotesPg(FALSE);
	ShowInfoPg(FALSE);
	ShowCompassPg(TRUE);
}

void CChildView::OnNotesView() {
	View = Notes;

	ShowNotesPg(TRUE);
	ShowInfoPg(FALSE);
	ShowCompassPg(FALSE);

	m_ctlNotes.SetFocus();
}

void CChildView::OnInfoView() {
	View = Info;

	ShowNotesPg(FALSE);
	ShowInfoPg(TRUE);
	ShowCompassPg(FALSE);

	::SetFocus(m_ctlInfo.GetHwnd());
}

void CChildView::OnChangeNotes() {
	if (theApp.Destination != NULL) {
		CString s;
		m_ctlNotes.GetWindowText(s);
		theApp.Destination->Notes = s;
	}
}

void CChildView::LoadHtml(LPCTSTR fileName) {
	CBufferedFile file;
	if (file.Create(fileName, GENERIC_READ, 0, OPEN_EXISTING, 0)) {
		m_ctlInfo.Clear();

		char buffer[2048];
		DWORD r;
		BOOL ok;
		do {
			ok = file.Read(buffer, 2047, &r);
			buffer[r] = '\0';
			CString s = CharToWChar(buffer, r, CP_UTF8);
			m_ctlInfo.AddText(s);
		} while (ok);

		m_ctlInfo.EndOfSource();

		file.Close();
	}
	else {
		m_ctlInfo.Clear();
		m_ctlInfo.AddText(L"<html>");
		m_ctlInfo.AddText(L"<center>Nothing cached</center>");
		m_ctlInfo.AddText(L"</html>");
		m_ctlInfo.EndOfSource();
	}

}

void CChildView::OnWaypointCtx(NMHDR *pNMHDR, LRESULT *pResult) {
	// GN_CONTEXTMENU sends NMRGINFO structure through notify struct parameter
	PNMRGINFO pInfo = (PNMRGINFO) pNMHDR;

	if (theApp.Destination != NULL) {
		CMenu mnu;
		mnu.LoadMenu(IDR_WAYPOINT_CTX);

		CMenu *popup = mnu.GetSubMenu(0);

		CString s;
		s.Format(IDS_COPY_STR, theApp.Destination->Id);
		popup->InsertMenu(0, MF_STRING, ID_COPY_WAYPOINT, s);

		popup->TrackPopupMenu(TPM_LEFTALIGN, pInfo->ptAction.x, pInfo->ptAction.y, this);
	}

	*pResult = TRUE; // This is important!
}

void CChildView::OnPositionCtx(NMHDR *pNMHDR, LRESULT *pResult) {
	// GN_CONTEXTMENU sends NMRGINFO structure through notify struct parameter
	PNMRGINFO pInfo = (PNMRGINFO) pNMHDR;

	if (theApp.Gps.GpsOn && theApp.Gps.Quality > 0) {
		CMenu popup;
		CString s, sPos;

		popup.CreatePopupMenu();

		sPos.Format(_T("%.6lf"), theApp.Gps.Latitude);
		s.Format(IDS_COPY_STR, sPos);
		popup.AppendMenu(MF_STRING, ID_COPY_LATITUDE, s);

		sPos.Format(_T("%.6lf"), theApp.Gps.Longitude);
		s.Format(IDS_COPY_STR, sPos);
		popup.AppendMenu(MF_STRING, ID_COPY_LONGITUDE, s);

		popup.TrackPopupMenu(TPM_LEFTALIGN, pInfo->ptAction.x, pInfo->ptAction.y, this);
	}

	*pResult = TRUE; // This is important!
}

void CChildView::OnCopyWaypoint() {
	CopyTextToClipboard(GetSafeHwnd(), theApp.Destination->Id);
}

void CChildView::OnCopyLongitude() {
	CString s;
	s.Format(_T("%.6lf"), theApp.Gps.Longitude);
	CopyTextToClipboard(GetSafeHwnd(), s);
}

void CChildView::OnCopyLatitude() {
	CString s;
	s.Format(_T("%.6lf"), theApp.Gps.Latitude);
	CopyTextToClipboard(GetSafeHwnd(), s);
}

void CChildView::OnDownloadInfo() {
	CDownloadManagerDlg dlg;

	if (theApp.Destination != NULL) {
		dlg.Pois.AddTail(theApp.Destination);
		dlg.DoModal();
	}
}

void CChildView::OnMarkAs(UINT nID) {
	if (theApp.Destination != NULL) {
		CPoi *poi = theApp.Destination;
		switch (nID) {
			case ID_POI_MARKAS_NOTCOMPLETED:
				poi->Status = CPoi::NotCompleted;
				break;

			case ID_POI_MARKAS_NOTFOUND:
				poi->Status = CPoi::NotFound;
				break;

			case ID_POI_MARKAS_COMPLETED:
				poi->Status = CPoi::Completed;
				GetLocalTime(&(poi->TimeCompleted));
				break;
		}
	}
}

BOOL CChildView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
	NMHDR *pnmh = (LPNMHDR) lParam;
	if (pnmh != NULL) {
		NM_HTMLVIEW *pnmHTML = (NM_HTMLVIEW *) lParam;

		if (pnmh->code == NM_HOTSPOT) {
			*pResult = 1;
			return 1;
		}
		else if (pnmh->code == NM_CONTEXTMENU) {
			*pResult = 1;
			return 1;
		}
		else if (pnmh->code == NM_INLINE_IMAGE) {
			if (Connection.IsAvailable() == S_OK) {
				// got connection -> use default handling of an image
				*pResult = 0;
				return 0;
			}
			else {
				// no connection -> prevent loading of not cached images
				::SendMessage(m_ctlInfo.GetHwnd(), DTM_IMAGEFAIL, 0, (LPARAM) (INLINEIMAGEINFO*) pnmHTML->dwCookie);
				*pResult = 1;
				return 1;
			}
		}
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}
