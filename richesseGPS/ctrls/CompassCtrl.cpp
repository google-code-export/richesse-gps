/**
 *  CompassCtrl.cpp : implementation file
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
#include "CompassCtrl.h"
#include "..\..\share\defs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompassCtrl

CCompassCtrl::CCompassCtrl() {
	NorthDirection = CurNorthDirection = 0;
	NavDirection = CurNavDirection = 0;
	NavigateTo = FALSE;
//	Set = FALSE;

	m_bmp = NULL;
	m_dcMem = NULL;

	m_clrFrame = RGB(0x66, 0x66, 0x66);
	m_clrBkGnd = RGB(0xf0, 0xf0, 0xf0);
	m_clrRed = RGB(0xcc, 0x00, 0x00);

	RotTimer = 1;
	NavTimer = 2;
}

CCompassCtrl::~CCompassCtrl() {
	if (m_dcMem != NULL && m_bmp != NULL)
		m_dcMem->SelectObject(m_bmp);	// select original bitmap
	delete m_dcMem;
}


BEGIN_MESSAGE_MAP(CCompassCtrl, CStatic)
	//{{AFX_MSG_MAP(CCompassCtrl)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompassCtrl message handlers

void CCompassCtrl::CreateFonts() {
}

int CCompassCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

//	KillTimer(RotTimer);
	SetTimer(RotTimer, 100, NULL);

	return 0;
}

void CCompassCtrl::OnPaint() {
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(rc);

	if (m_dcMem == NULL) {
		m_dcMem = new CDC;

		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());

		m_dcMem->CreateCompatibleDC(&dc);
		m_bmp = m_dcMem->SelectObject(&bmp);
	}

	int i, j;
	int saveDC = m_dcMem->SaveDC();

	CRect rcEll = rc;
	rcEll.DeflateRect(1, 1, 1, 1);

	// clear bkgnd
	m_dcMem->FillSolidRect(rc, GetSysColor(COLOR_WINDOW));
	
	CPen penFr(PS_SOLID, 1, m_clrFrame);
	CBrush brBk(m_clrBkGnd);
	m_dcMem->SelectObject(&penFr);
	m_dcMem->SelectObject(&brBk);

	m_dcMem->Ellipse(rcEll);

	int l = ((rcEll.bottom - rcEll.top) / 2);
	CPoint center((rcEll.left + rcEll.right) / 2, l);


	// units
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	m_dcMem->SelectObject(&pen);

	Pt dC[] = {
		{ 0, l - 1 },
		{ 0, l - SCALEY(4) }
	};	
	Pt dCL[] = {
		{ 0, l - 1 },
		{ 0, l - SCALEY(8) }
	};	

//	NorthDirection = 0;
	for (int alfa = 0; alfa < 360; alfa += 10) {
		double theta = PI - ((PI / 180.0) * (alfa + CurNorthDirection));

		Pt dp[2];
		if (alfa % 30 == 0) {
			memcpy(dp, dCL, sizeof(dCL));
		}
		else {
			memcpy(dp, dC, sizeof(dC));
		}

		CPoint pts[2];
		for (j = 0; j < countof(pts); j++) {
			pts[j].x = (int) (center.x + ( dp[j].X * cos(theta)) + (dp[j].Y * sin(theta)));
			pts[j].y = (int) (center.y - (-dp[j].X * sin(theta)) + (dp[j].Y * cos(theta)));
		}

		m_dcMem->MoveTo(pts[0].x, pts[0].y);
		m_dcMem->LineTo(pts[1].x, pts[1].y);
	}

	//
	Pt dLetter = { SCALEX(3), l - SCALEY(7) };	
	TCHAR letter[] = { 'N', 'E', 'S', 'W' };
//	m_dcMem->SetBkMode(TRANSPARENT);
	for (i = 0; i < countof(letter); i++) {
		CFont fnt;

		int alfa = (i * 90 + CurNorthDirection) % 360;
		double theta = PI - ((PI / 180.0) * (alfa));


		HGDIOBJ hObj = ::GetStockObject(SYSTEM_FONT);
		LOGFONT lf;
		::GetObject(hObj, sizeof(LOGFONT), (LPVOID) &lf);
		lf.lfHeight = SCALEY(13) + 1;
		lf.lfOrientation = lf.lfEscapement = (360 - alfa) * 10;
		lf.lfWeight = FW_BOLD;
		fnt.CreateFontIndirect(&lf);

		CPoint pt;
		pt.x = (int) (center.x + ( dLetter.X * cos(theta)) + (dLetter.Y * sin(theta)));
		pt.y = (int) (center.y + (-dLetter.X * sin(theta)) + (dLetter.Y * cos(theta)));

		CString s(letter[i]);
		CFont *oldFont = m_dcMem->SelectObject(&fnt);
		if (i == 0) {
			m_dcMem->SetBkColor(m_clrBkGnd);
			m_dcMem->SetTextColor(m_clrRed);
		}
		else {
			m_dcMem->SetTextColor(RGB(0x00, 0x00, 0x00));
			m_dcMem->SetBkColor(m_clrBkGnd);
		}
		m_dcMem->ExtTextOut(pt.x, pt.y, ETO_OPAQUE, NULL, s, NULL);

		m_dcMem->SelectObject(oldFont);
		fnt.DeleteObject();
	}

	// arrow
	if (NavigateTo) {
		Pt dPts[] = {
			{ -SCALEX(20), -l + SCALEY(30) },
			{  0,           l - SCALEY(25) },
			{  SCALEX(20), -l + SCALEY(30) },
			{  0,          -l + SCALEX(40) }
		};

		double theta = 2 * PI - ((PI / 180.0) * CurNavDirection);
		CPoint pts[4];
		for (i = 0; i < countof(dPts); i++) {
			double x = dPts[i].X;
			double y = dPts[i].Y;

			pts[i].x = (int) (center.x + ( ( x * cos(theta)) + (y * sin(theta))));
			pts[i].y = (int) (center.y - ( (-x * sin(theta)) + (y * cos(theta))));
		}
		CPen pen(PS_SOLID, SCALEX(1), RGB(0x11, 0x44, 0x77));
		CBrush brush;
		brush.CreateSolidBrush(RGB(0x33, 0x66, 0x99));

		CPen *oldPen = m_dcMem->SelectObject(&pen);
		CBrush *oldbrush = m_dcMem->SelectObject(&brush);

		m_dcMem->Polygon(pts, 4);

		m_dcMem->SelectObject(oldPen);
		m_dcMem->SelectObject(oldbrush);
	}

	// copy to screen
	dc.BitBlt(0, 0, rc.Width(), rc.Height(), m_dcMem, 0, 0, SRCCOPY);

	m_dcMem->RestoreDC(saveDC);

	ValidateRect(NULL);

	// Do not call CStatic::OnPaint() for painting messages
}

void CCompassCtrl::SetNorthDirection(int dir) {
//	CurNorthDirection = NorthDirection;
	NorthDirection = 360 - dir;
	int deg = ((NorthDirection - CurNorthDirection) + 360) % 360;
	if (deg > 0) {
		if (deg <= 180) NorthDir = 1;
		else NorthDir = -1;

//		KillTimer(RotTimer);
//		SetTimer(RotTimer, 100, NULL);

		Invalidate();
	}
}

void CCompassCtrl::SetDirection(int dir) {
	NavigateTo = TRUE;
	CurNavDirection = NavDirection = dir;
/*	int deg = ((NavDirection - CurNavDirection) + 360) % 360;
	if (deg > 0) {
		if (deg <= 180) NavDir = 1;
		else NavDir = -1;

//		KillTimer(NavTimer);
//		SetTimer(NavTimer, 100, NULL);

		Invalidate();
	}
*/
}

void CCompassCtrl::OnTimer(UINT nIDEvent) {
	if (nIDEvent == RotTimer) {
		if (NorthDirection != CurNorthDirection) {
			if (abs(((CurNorthDirection + (NorthDir * 5) + 360) % 360) - NorthDirection) < 5) {
				CurNorthDirection = NorthDirection;
			}
			else
				CurNorthDirection += (NorthDir * 5);
			if (CurNorthDirection < 0) CurNorthDirection += 360;
			CurNorthDirection %= 360;

//			if (CurNorthDirection == NorthDirection)
//				KillTimer(RotTimer);
		}

/*		if (CurNavDirection != NavDirection) {
			if (abs(((CurNavDirection + (NavDir * 5) + 360) % 360) - NavDirection) < 5) {
				CurNavDirection = NavDirection;
			}
			else
				CurNavDirection += (NavDir * 5);
			if (CurNavDirection < 0) CurNavDirection += 360;
			CurNavDirection %= 360;

//			if (CurNavDirection == NavDirection)
//				KillTimer(NavTimer);
		}
*/
		Invalidate();
	}
//	else if (nIDEvent == NavTimer) {
//	}

	CStatic:: OnTimer(nIDEvent);
}

void CCompassCtrl::Stop() {
	NorthDirection = CurNorthDirection;
//	KillTimer(RotTimer);

}

void CCompassCtrl::StopNavigate() {
	NavDirection = CurNavDirection;
//	KillTimer(NavTimer);

	NavigateTo = FALSE;
}
