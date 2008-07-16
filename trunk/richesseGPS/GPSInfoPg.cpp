/**
 *  GPSInfoPg.cpp : implementation file
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
#include "GPSInfoPg.h"
#include "../share/helpers.h"
#include "gps/gps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGPSInfoPg property page

IMPLEMENT_DYNCREATE(CGPSInfoPg, CPropertyPage)

CGPSInfoPg::CGPSInfoPg() : CPropertyPage(CGPSInfoPg::IDD) {
	//{{AFX_DATA_INIT(CGPSInfoPg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
//	Gps = NULL;

	RefreshTimer = 1;

	m_ctlSatellites.SatsToShow = 12;
	m_ctlSatellites.ColSep = 3;
}

CGPSInfoPg::~CGPSInfoPg() {
}

void CGPSInfoPg::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGPSInfoPg)
	DDX_Control(pDX, IDC_SATELLITES, m_ctlSatellites);
	DDX_Control(pDX, IDC_STATUS, m_ctlStatus);
	DDX_Control(pDX, IDC_SAT_CNT, m_ctlSatCount);
	DDX_Control(pDX, IDC_LONGITUDE, m_ctlLongitude);
	DDX_Control(pDX, IDC_LATITUDE, m_ctlLatitude);
	DDX_Control(pDX, IDC_ALTITUDE, m_ctlAltitude);
	DDX_Control(pDX, IDC_C_SATELLITES, m_lblSatellites);
	DDX_Control(pDX, IDC_C_POSITION, m_lblPosition);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGPSInfoPg, CPropertyPage)
	//{{AFX_MSG_MAP(CGPSInfoPg)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGPSInfoPg message handlers

BOOL CGPSInfoPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	ShowInfo();	
	SetTimer(RefreshTimer, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGPSInfoPg::OnDestroy() {
	CPropertyPage::OnDestroy();
	
	KillTimer(RefreshTimer);
}

void CGPSInfoPg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == RefreshTimer) {
		ShowInfo();
	}
	
	CPropertyPage::OnTimer(nIDEvent);
}

void CGPSInfoPg::ShowInfo() {
	// display info
	CString sText;

	theApp.Gps.Lock();
	if (theApp.Gps.GpsOn && theApp.Gps.Quality > 0) {
		// position
		sText = FormatCoord(theApp.Gps.Latitude > 0 ? 'N': 'S', fabs(theApp.Gps.Latitude));
		m_ctlLatitude.SetWindowText(sText);

		sText = FormatCoord(theApp.Gps.Longitude > 0 ? 'E' : 'W', fabs(theApp.Gps.Longitude));
		m_ctlLongitude.SetWindowText(sText);

		// TODO: english units
		sText.Format(_T("%.1lf m"), theApp.Gps.Altitude);
		m_ctlAltitude.SetWindowText(sText);

		// satellites
		int used = 0;
		for (int i = 0; i < MAX_SATELLITES; i++) {
			m_ctlSatellites.m_arSatellites[i] = theApp.Gps.Satellites[i];
			m_ctlSatellites.m_arSatellitesForFix[i] = theApp.Gps.SatellitesForFix[i];

			if (m_ctlSatellites.m_arSatellitesForFix[i] > 10)
				used++;
		}
		m_ctlSatellites.Invalidate();

		sText.Format(IDS_SATELLITES, used, theApp.Gps.SatellitesInView);
		m_ctlSatCount.SetWindowText(sText);
	}
	else {
		sText.LoadString(IDS_UNKNOWN);
		m_ctlLatitude.SetWindowText(sText);
		m_ctlLongitude.SetWindowText(sText);
		m_ctlAltitude.SetWindowText(sText);

		m_ctlSatCount.SetWindowText(_T(""));

		// satellites
		for (int i = 0; i < MAX_SATELLITES; i++) {
			m_ctlSatellites.m_arSatellites[i].SNR = -1;
			m_ctlSatellites.m_arSatellitesForFix[i] = -1;
		}
		m_ctlSatellites.Invalidate();
	}

	if (theApp.Gps.IsRunning())
		sText.LoadString(IDS_RUNNING);
	else
		sText.LoadString(IDS_NOT_RUNNING);
	m_ctlStatus.SetWindowText(sText);

	theApp.Gps.Unlock();
}
