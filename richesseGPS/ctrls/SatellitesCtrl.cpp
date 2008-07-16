/**
 *  SatellitesCtrl.cpp : implementation file
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

#include "..\stdafx.h"
#include "..\RichesseGPS.h"
#include "satellitesctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSatellitesCtrl

CSatellitesCtrl::CSatellitesCtrl() {
	m_bmp = NULL;
	m_dcMem = NULL;

	SatsToShow = 8;
	ColSep = 1;

	for (int i = 0; i < MAX_SATELLITES; i++)
		m_arSatellites[i].SNR = -1;
}

CSatellitesCtrl::~CSatellitesCtrl() {
	if (m_dcMem != NULL && m_bmp != NULL)
		m_dcMem->SelectObject(m_bmp);	// select original bitmap
	delete m_dcMem;
}


BEGIN_MESSAGE_MAP(CSatellitesCtrl, CStatic)
	//{{AFX_MSG_MAP(CSatellitesCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSatellitesCtrl message handlers

void CSatellitesCtrl::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CRect rc, rcClient;
	GetClientRect(rcClient);
	rc = rcClient;
	rc.bottom--;

	if (m_dcMem == NULL) {
		m_dcMem = new CDC;

		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());

		m_dcMem->CreateCompatibleDC(&dc);
		m_bmp = m_dcMem->SelectObject(&bmp);
	}
	
	int saveDC = m_dcMem->SaveDC();
	
	// clear bkgnd
	m_dcMem->FillSolidRect(rcClient, GetSysColor(COLOR_WINDOW));

	CBrush br(GetSysColor(COLOR_WINDOW));
	CPen penFr(PS_SOLID, 1, RGB(0, 0, 0));

	m_dcMem->SelectObject(&br);
	m_dcMem->SelectObject(&penFr);
	int colWd = rc.Width() / SatsToShow;
	for (int i = 0; i < SatsToShow; i++) {
		CRect r(
			rc.left + (i * colWd), rc.top, 
			rc.left + ((i + 1) * colWd) - SCALEX(ColSep), rc.bottom);
		m_dcMem->Rectangle(r);
	}

	int j = 0;
	CBrush brBk(RGB(0x33, 0x66, 0x99));
	CPen penNull(PS_NULL, 1, RGB(0, 0, 0));

	m_dcMem->SelectObject(&brBk);
	m_dcMem->SelectObject(&penNull);
	for (int k = 0; j < SatsToShow && k < MAX_SATELLITES; k++) {
		for (int i = 0; i < MAX_SATELLITES; i++) {
			if (m_arSatellitesForFix[k] == m_arSatellites[i].Id) {
				CRect r(
					rc.left + (j * colWd), rc.top, 
					rc.left + ((j + 1) * colWd) - SCALEX(ColSep), rc.bottom);

				r.DeflateRect(1, 1, 1, 1);

				int ht = (int) (r.Height() * (m_arSatellites[i].SNR / 99.0));
				r.top = r.bottom - ht;
				m_dcMem->Rectangle(r);
				j++;
			}
		}
	}


	// copy to screen
	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), m_dcMem, 0, 0, SRCCOPY);

	m_dcMem->RestoreDC(saveDC);

	ValidateRect(NULL);
	
	// Do not call CStatic::OnPaint() for painting messages
}
