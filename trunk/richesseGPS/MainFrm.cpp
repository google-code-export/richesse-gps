/**
 *  MainFrm.cpp : implementation of the CMainFrame class
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

#include "MainFrm.h"

#include "ctrls/CePropertySheet.h"
#include "SetupPg.h"
#include "SetupGpsPg.h"
#include "GPSInfoPg.h"
#include "Config.h"
#include "PoiMan.h"

#include "AddWaypointDlg.h"
#include "WaypointNotes.h"

#include "RadarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const DWORD dwAdornmentFlags = 0; // exit button

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_MENU_OPTIONS, OnMenuOptions)
	ON_COMMAND(ID_MENU_GPSINFO, OnMenuGpsinfo)
	ON_COMMAND(ID_GPS_ON, OnGpsOn)
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_POIMANAGER, OnMenuPoimanager)
	ON_COMMAND(ID_MENU_RADAR, OnMenuRadar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	RefreshTimer = 1;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	// menu bar
	BOOL fSuccess;
	SHMENUBARINFO mbi = { 0 };

	mbi.cbSize = sizeof(mbi);
	mbi.dwFlags = SHCMBF_HMENU;
	mbi.nToolBarId = IDR_MAINFRAME;
	mbi.hInstRes = AfxGetInstanceHandle();
	mbi.hwndParent = GetSafeHwnd();

	fSuccess = SHCreateMenuBar(&mbi);
	m_hwndCmdBar = mbi.hwndMB;

	m_wndView.ShowInfo();
	SetTimer(RefreshTimer, 1000, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnMenuOptions() {
	CCePropertySheet sheet(IDS_OPTIONS);

	CSetupGeneralPg pgGeneral;
	CSetupGpsPg pgGps;
	sheet.AddPage(&pgGeneral);
	sheet.AddPage(&pgGps);
	sheet.SetMenu(IDR_CANCEL);
	if (sheet.DoModal() == IDOK) {
	}
}

void CMainFrame::OnMenuGpsinfo() {
	CGPSInfoPg pgInfo;
//	pgInfo.Gps = &Gps;
	CCePropertySheet sheet(IDS_GPS_INFO);
	sheet.AddPage(&pgInfo);
	sheet.SetMenu(IDR_CANCEL);
	sheet.DoModal();	
}

BOOL CMainFrame::DestroyWindow() {
	
	return CFrameWnd::DestroyWindow();
}

void CMainFrame::SetMenuCaption(UINT nID) {
	CString sText;
	sText.LoadString(nID);
	
	TBBUTTONINFO tbButtonInfo = { 0 };
	tbButtonInfo.cbSize = sizeof(TBBUTTONINFO);
	tbButtonInfo.dwMask = TBIF_TEXT;

	tbButtonInfo.pszText = (LPTSTR) (LPCTSTR) sText;
	tbButtonInfo.cchText = sText.GetLength();
	::SendMessage(m_hwndCmdBar, TB_SETBUTTONINFO, ID_GPS_ON, (LPARAM) &tbButtonInfo);
}

void CMainFrame::OnGpsOn() {
	if (theApp.Gps.GpsOn) {
		theApp.Gps.Off();
		m_wndView.m_ctlCompass.Stop();
		m_wndView.m_ctlCompass.StopNavigate();
		m_wndView.ShowInfo();
		SetMenuCaption(IDS_GPS_ON);
	}
	else {
		// setup gps params
		theApp.Gps.SetPort(Config.Port);
		theApp.Gps.SetBaudRate(Config.BaudRate);

		if (theApp.Gps.On())
			SetMenuCaption(IDS_GPS_OFF);
		else
			AfxMessageBox(IDS_GPS_CONNECT_FAILED);
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) {
	if (nIDEvent == RefreshTimer) {
		if (theApp.Gps.GpsOn)
			m_wndView.ShowInfo();
	}

	CWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnMenuPoimanager() {
	CPoiMan dlg;
	if (dlg.DoModal() == IDOK) {
		theApp.Destination = dlg.NavigateTo;
	}

	CPoi *poi = theApp.Destination;
	if (poi != NULL) {
		m_wndView.m_ctlWaypoint.SetWindowText(poi->Name);
		m_wndView.m_ctlNotes.SetWindowText(poi->Notes);

		CWaitCursor wait;
		CString fileName;
		fileName.Format(_T("%s\\cache\\%s.html"), Config.InstallDir, poi->Id);
		m_wndView.LoadHtml(fileName);
	}
	else {
		m_wndView.m_ctlCompass.StopNavigate();
		m_wndView.m_ctlWaypoint.SetWindowText(_T(""));
		m_wndView.m_ctlNotes.SetWindowText(_T(""));
		m_wndView.LoadHtml(NULL);
	}

	theApp.SavePois();
}

void CMainFrame::OnMenuRadar() {
	CRadarDlg dlg;
	if (dlg.DoModal() == IDOK) {
		theApp.Destination = dlg.NavigateTo;

		CPoi *poi = theApp.Destination;
		if (poi != NULL) {
			m_wndView.m_ctlWaypoint.SetWindowText(poi->Name);
			m_wndView.m_ctlNotes.SetWindowText(poi->Notes);

			CWaitCursor wait;
			CString fileName;
			fileName.Format(_T("%s\\cache\\%s.html"), Config.InstallDir, poi->Id);
			m_wndView.LoadHtml(fileName);
		}
		else {
			m_wndView.m_ctlCompass.StopNavigate();
			m_wndView.m_ctlWaypoint.SetWindowText(_T(""));
			m_wndView.m_ctlNotes.SetWindowText(_T(""));
			m_wndView.LoadHtml(NULL);
		}
	}


}
