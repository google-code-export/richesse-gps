/**
 *  AddWaypointDlg.cpp : implementation file
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
#include "AddWaypointDlg.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaypointSheet Dialog
//

CWaypointSheet::CWaypointSheet(UINT nIDCaption, CWnd* pParentWnd/* = NULL*/, UINT iSelectPage/* = 0*/) :
	CCePropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CWaypointSheet::CWaypointSheet(LPCTSTR pszCaption, CWnd* pParentWnd/* = NULL*/, UINT iSelectPage/* = 0*/) :
	CCePropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

BEGIN_MESSAGE_MAP(CWaypointSheet, CCePropertySheet)
	//{{AFX_MSG_MAP(CWaypointSheet)
	ON_COMMAND(ID_SAVE, OnSave)
	ON_COMMAND(ID_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWaypointSheet::OnSave() {
	GetActivePage()->UpdateData();
	GetActivePage()->OnApply();

	EndDialog(IDOK);
}

void CWaypointSheet::OnAdd() {
	GetActivePage()->UpdateData();
	GetActivePage()->OnApply();

	EndDialog(IDOK);
}


/////////////////////////////////////////////////////////////////////////////
// CWaypointPg dialog


CWaypointPg::CWaypointPg()
	: CPropertyPage(CWaypointPg::IDD)
{
	//{{AFX_DATA_INIT(CWaypointPg)
	m_strName = _T("");
	m_dLatitude = 0.0;
	m_dLongitude = 0.0;
	m_nType = -1;
	//}}AFX_DATA_INIT
	memset(&m_stCompleted, 0, sizeof(m_stCompleted));
}


void CWaypointPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaypointPg)
	DDX_Control(pDX, IDC_TYPE, m_ctlType);
	DDX_Control(pDX, IDC_LONG_SEC, m_ctlLongSec);
	DDX_Control(pDX, IDC_LONG_MIN, m_ctlLongMin);
	DDX_Control(pDX, IDC_LONG_DEG, m_ctlLongDeg);
	DDX_Control(pDX, IDC_LAT_SEC, m_ctlLatSec);
	DDX_Control(pDX, IDC_LAT_MIN, m_ctlLatMin);
	DDX_Control(pDX, IDC_LAT_DEG, m_ctlLatDeg);
	DDX_Control(pDX, IDC_LONG_DIR, m_ctlLongDir);
	DDX_Control(pDX, IDC_LAT_DIR, m_ctlLatDir);
	DDX_Control(pDX, IDC_NAME, m_ctlName);
//	DDX_Control(pDX, IDC_C_TITLE, m_lblTitle);
	DDX_Control(pDX, IDC_COMPLETED_LBL, m_lblCompleted);
	DDX_Control(pDX, IDC_COMPLETED, m_ctlCompleted);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_CBIndex(pDX, IDC_TYPE, m_nType);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate) {
		CString sDegs, sMins, sSecs;
		int deg, min;
		double dmin, sec;

		switch (Config.CoordType) {
			default:
			case Degrees:
				m_ctlLatDeg.GetWindowText(sDegs);
				swscanf(sDegs, _T("%lf"), &m_dLatitude);

				m_ctlLongDeg.GetWindowText(sDegs);
				swscanf(sDegs, _T("%lf"), &m_dLongitude);
				break;

			case DegMin:
				m_ctlLatDeg.GetWindowText(sDegs);
				m_ctlLatMin.GetWindowText(sMins);
				swscanf(sDegs, _T("%d"), &deg);
				swscanf(sMins, _T("%lf"), &dmin);
				m_dLatitude = deg + (dmin / 60.0);

				m_ctlLongDeg.GetWindowText(sDegs);
				m_ctlLongMin.GetWindowText(sMins);
				swscanf(sDegs, _T("%d"), &deg);
				swscanf(sMins, _T("%lf"), &dmin);
				m_dLongitude = deg + (dmin / 60.0);
				break;

			case DegMinSec:
				m_ctlLatDeg.GetWindowText(sDegs);
				m_ctlLatMin.GetWindowText(sMins);
				m_ctlLatSec.GetWindowText(sSecs);
				swscanf(sDegs, _T("%d"), &deg);
				swscanf(sMins, _T("%d"), &min);
				swscanf(sSecs, _T("%lf"), &sec);
				m_dLatitude = deg + (min / 60.0) + (sec / 3600.0);

				m_ctlLongDeg.GetWindowText(sDegs);
				m_ctlLongMin.GetWindowText(sMins);
				m_ctlLongSec.GetWindowText(sSecs);
				swscanf(sDegs, _T("%d"), &deg);
				swscanf(sMins, _T("%d"), &min);
				swscanf(sSecs, _T("%lf"), &sec);
				m_dLongitude = deg + (min / 60.0) + (sec / 3600.0);
				break;
		}

		CString s;
		m_ctlLatDir.GetWindowText(s);
		if (s.CompareNoCase(_T("S")) == 0)
			m_dLatitude = -m_dLatitude;

		m_ctlLongDir.GetWindowText(s);
		if (s.CompareNoCase(_T("W")) == 0)
			m_dLongitude = -m_dLongitude;
	}
}


BEGIN_MESSAGE_MAP(CWaypointPg, CPropertyPage)
	//{{AFX_MSG_MAP(CWaypointPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

UINT CacheTypeStr[] = {
	IDS_TRADITIONAL,
	IDS_MULTICACHE,
	IDS_LETTERBOX,
	IDS_EARTHCACHE,
	IDS_EVENTCACHE,
	IDS_MYSTERYCACHE,
	IDS_MY_POI,
	IDS_UNKNOWN,
	IDS_VIRTUAL
};

/////////////////////////////////////////////////////////////////////////////
// CWaypointPg message handlers

BOOL CWaypointPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	
//	m_lblTitle.SetWindowText(m_strTitle);

/*	// menu bar
	BOOL fSuccess;
	SHMENUBARINFO mbi = { 0 };

	mbi.cbSize = sizeof(mbi);
	mbi.dwFlags = SHCMBF_HMENU;
	mbi.nToolBarId = m_nMenuID;
	mbi.hInstRes = AfxGetInstanceHandle();
	mbi.hwndParent = GetSafeHwnd();

	fSuccess = SHCreateMenuBar(&mbi);
	m_hwndCmdBar = mbi.hwndMB;	
*/
	
	for (int i = 0; i < countof(CacheTypeStr); i++) {
		CString sType;
		sType.LoadString(CacheTypeStr[i]);
		int item = m_ctlType.AddString(sType);
		if (i == m_nType)
			m_ctlType.SetCurSel(i);
	}

	//
	m_ctlLatDir.AddString(_T("N"));
	m_ctlLatDir.AddString(_T("S"));
	if (m_dLatitude >= 0)
		m_ctlLatDir.SelectString(-1, _T("N"));
	else {
		m_ctlLatDir.SelectString(-1, _T("S"));
		m_dLatitude = -m_dLatitude;
	}

	m_ctlLongDir.AddString(_T("E"));
	m_ctlLongDir.AddString(_T("W"));
	if (m_dLongitude >= 0)
		m_ctlLongDir.SelectString(-1, _T("E"));
	else {
		m_ctlLongDir.SelectString(-1,  _T("W"));
		m_dLongitude = -m_dLongitude;
	}

	int latDegrees = (int) m_dLatitude;
	double latM = (m_dLatitude - latDegrees) * 60;
	int latMins = (int) latM;
	double latSecs = (latM - latMins) * 60;

	int longDegrees = (int) m_dLongitude;
	double longM = (m_dLongitude - longDegrees) * 60;
	int longMins = (int) longM;
	double longSecs = (longM - longMins) * 60;

	CString s;
	switch (Config.CoordType) {
		default:
		case Degrees:
			m_ctlLatDeg.SetWindowPos(NULL, 0, 0, SCALEX(115), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%.6lf"), m_dLatitude); m_ctlLatDeg.SetWindowText(s);
			m_ctlLatMin.ShowWindow(SW_HIDE);
			m_ctlLatSec.ShowWindow(SW_HIDE);

			m_ctlLongDeg.SetWindowPos(NULL, 0, 0, SCALEX(115), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%.6lf"), m_dLongitude); m_ctlLongDeg.SetWindowText(s);
			m_ctlLongMin.ShowWindow(SW_HIDE);
			m_ctlLongSec.ShowWindow(SW_HIDE);
			break;

		case DegMin:
			m_ctlLatDeg.SetWindowPos(NULL, 0, 0, SCALEX(30), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%d"), latDegrees); m_ctlLatDeg.SetWindowText(s);
			m_ctlLatMin.SetWindowPos(NULL, 0, 0, SCALEX(75), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%.4lf"), latM); m_ctlLatMin.SetWindowText(s);
			m_ctlLatSec.ShowWindow(SW_HIDE);

			m_ctlLongDeg.SetWindowPos(NULL, 0, 0, SCALEX(30), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%d"), longDegrees); m_ctlLongDeg.SetWindowText(s);
			m_ctlLongMin.SetWindowPos(NULL, 0, 0, SCALEX(75), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%.4lf"), longM); m_ctlLongMin.SetWindowText(s);
			m_ctlLongSec.ShowWindow(SW_HIDE);
			break;

		case DegMinSec:
			m_ctlLatDeg.SetWindowPos(NULL, 0, 0, SCALEX(30), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%d"), latDegrees); m_ctlLatDeg.SetWindowText(s);
			m_ctlLatMin.SetWindowPos(NULL, 0, 0, SCALEX(30), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%d"), latMins); m_ctlLatMin.SetWindowText(s);
			m_ctlLatSec.SetWindowPos(NULL, 0, 0, SCALEX(42), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%02.2lf"), latSecs); m_ctlLatSec.SetWindowText(s);

			m_ctlLongDeg.SetWindowPos(NULL, 0, 0, SCALEX(30), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%d"), longDegrees); m_ctlLongDeg.SetWindowText(s);
			m_ctlLongMin.SetWindowPos(NULL, 0, 0, SCALEX(30), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%d"), longMins); m_ctlLongMin.SetWindowText(s);
			m_ctlLongSec.SetWindowPos(NULL, 0, 0, SCALEX(42), SCALEY(20), SWP_NOMOVE | SWP_NOZORDER);
			s.Format(_T("%02.2lf"), longSecs); m_ctlLongSec.SetWindowText(s);
			break;
	}	

	CString time;
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &m_stCompleted, NULL, time.GetBufferSetLength(256), 255);
	if (m_bCompleted)
		m_ctlCompleted.SetWindowText(time);
	else
		m_ctlCompleted.SetWindowText(_T("---"));

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CWaypointPg::UpdateControls() {
}
