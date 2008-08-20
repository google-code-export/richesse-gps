/**
 *  PoiMan.cpp : implementation file
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
#include "RadarDlg.h"
#include "../share/helpers.h"
#include "units.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static int CALLBACK
PointsCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CRadarDlg *pg = (CRadarDlg *) lParamSort;
	CPoi *poi1 = (CPoi *) lParam1;
	CPoi *poi2 = (CPoi *) lParam2;

	double dist1, dir1;
	double dist2, dir2;

	if (theApp.Gps.Quality > 0) {
		CalcDistanceDir(theApp.Gps.Latitude, theApp.Gps.Longitude, poi1->Latitude, poi1->Longitude, dist1, dir1);
		CalcDistanceDir(theApp.Gps.Latitude, theApp.Gps.Longitude, poi2->Latitude, poi2->Longitude, dist2, dir2);

		if (dist1 > dist2)
			return 1;
		else
			return -1;
	}
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CRadarDlg dialog


CRadarDlg::CRadarDlg(CWnd* pParent /*=NULL*/)
	: CCeDialog(CRadarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRadarDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetMenu(IDR_RADAR);
	RefreshTimer = 1;

	NavigateTo = NULL;
}


void CRadarDlg::DoDataExchange(CDataExchange* pDX)
{
	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadarDlg)
	DDX_Control(pDX, IDC_POIS, m_ctlPoints);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRadarDlg, CCeDialog)
	//{{AFX_MSG_MAP(CPointsPg)
	ON_WM_DESTROY()
	ON_COMMAND(ID_NAVIGATE, OnSelect)
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadarDlg message handlers

BOOL CRadarDlg::OnInitDialog() {
	CCeDialog::OnInitDialog();

	CRect rc;
	m_ctlPoints.GetClientRect(rc);

	rc.right -= ::GetSystemMetrics(SM_CXVSCROLL);

//	m_ctlPoints.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	m_ctlPoints.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	CString s;

//	m_ctlPoints.SetExtendedStyle(LVS_EX_FULLROWSELECT);
//	m_ctlPoints.InsertColumn(0, _T(""), LVCFMT_LEFT, SCALEX(20));
//	s.LoadString(IDS_ID);   m_ctlPoints.InsertColumn(1, s, LVCFMT_LEFT, SCALEX(60));
	s.LoadString(IDS_NAME);
	m_ctlPoints.InsertColumn(0, s, LVCFMT_LEFT, rc.Width() - SCALEX(60));

	s.LoadString(IDS_DISTANCE);
	m_ctlPoints.InsertColumn(1, s, LVCFMT_RIGHT, SCALEX(60));

	CDC *pDC = GetDC();
	if (pDC->GetDeviceCaps(LOGPIXELSX) > 130)
		m_ilCache.Create(IDB_CACHE_HI, SCALEX(16), 0, RGB(255, 0, 255));
	else
		m_ilCache.Create(IDB_CACHE, SCALEX(16), 0, RGB(255, 0, 255));
	ReleaseDC(pDC);

	m_ctlPoints.SetImageList(&m_ilCache, LVSIL_SMALL);

	POSITION pos = theApp.POIs.GetHeadPosition();
	while (pos != NULL) {
		CPoi *poi = theApp.POIs.GetNext(pos);
		InsertPoi(poi);
	}
	UpdateColumnWidths();

	SetTimer(RefreshTimer, 3000, NULL);

	SortPois();
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRadarDlg::SortPois() {
	m_ctlPoints.SortItems(PointsCompareProc, (DWORD) this);
}

void CRadarDlg::SetPoiImage(int item, CPoi::EType type) {
	LVITEM lvi = { 0 };
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = item;
	lvi.iImage = GetCacheImage(type);
	m_ctlPoints.SetItem(&lvi);
}

void CRadarDlg::InsertPoi(CPoi *poi) {
	int n = m_ctlPoints.GetItemCount();
//	int item = m_ctlPoints.InsertItem(n, poi->Name, poi->Completed ? ICON_CHECKED : ICON_NOT_CHECKED);
	int item = m_ctlPoints.InsertItem(n, poi->Name);

	SetPoiImage(item, poi->Type);
//	m_ctlPoints.SetItemText(item, 1, poi->Id);
//	m_ctlPoints.SetItemText(item, 1, poi->Name);
//	m_ctlPoints.SetCheck(item, poi->Completed);

	m_ctlPoints.SetItemData(item, (DWORD) poi);

	UpdateDistance(item);
}

void CRadarDlg::UpdateDistance(int item) {
	CString sDist;
	if (theApp.Gps.Quality > 0) {
		CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(item);
		double dist, dir;

		CalcDistanceDir(theApp.Gps.Latitude, theApp.Gps.Longitude, poi->Latitude, poi->Longitude, dist, dir);
		FormatDistance(sDist, dist);
	}
	else {
		sDist = _T("???");
	}
	m_ctlPoints.SetItemText(item, 1, sDist);
}

void CRadarDlg::OnDestroy() {
	CCeDialog::OnDestroy();

	KillTimer(RefreshTimer);
}

void CRadarDlg::UpdateControls() {
/*	if (m_ctlPoints.GetSelectedCount() >= 1) {
		m_btnRemove.EnableWindow();
	}
	else {
		m_btnRemove.EnableWindow(FALSE);
	}

	if (m_ctlPoints.GetSelectedCount() == 1) {
		m_btnInfo.EnableWindow();
		m_btnSelect.EnableWindow();
	}
	else {
		m_btnInfo.EnableWindow(FALSE);
		m_btnSelect.EnableWindow(FALSE);
	}
*/
}


void CRadarDlg::OnSelect() {
	int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
	if (nSelItem != -1) {
		CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(nSelItem);
		NavigateTo = poi;
		EndDialog(IDOK);
	}
}

void CRadarDlg::OnSize(UINT nType, int cx, int cy) {
	CCeDialog::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(&rc);

	m_ctlPoints.SetWindowPos(NULL, 0, SCALEX(24), cx, cy - SCALEY(24), SWP_NOZORDER);
}

void CRadarDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == RefreshTimer) {
		if (theApp.Gps.Quality > 0) {
			SortPois();
			m_ctlPoints.SetRedraw(FALSE);
			for (int i = 0; i < m_ctlPoints.GetItemCount(); i++) {
				UpdateDistance(i);
			}
			m_ctlPoints.SetRedraw(TRUE);
			UpdateColumnWidths();
		}
	}

	CCeDialog::OnTimer(nIDEvent);
}

void CRadarDlg::UpdateColumnWidths() {
	CRect rc;
	m_ctlPoints.GetClientRect(rc);

	rc.right -= ::GetSystemMetrics(SM_CXVSCROLL);

	m_ctlPoints.SetColumnWidth(1, LVSCW_AUTOSIZE);
	int col1wd = m_ctlPoints.GetColumnWidth(1);
	m_ctlPoints.SetColumnWidth(0, rc.Width() - col1wd);
}
