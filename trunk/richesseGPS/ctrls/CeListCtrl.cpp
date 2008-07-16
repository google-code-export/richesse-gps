/**
 *  CeListCtrl.cpp : implementation file
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

#include "../stdafx.h"
#include "../richessegps.h"
#include "CeListCtrl.h"
//#include "..\Appearance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCeListCtrl

CCeListCtrl::CCeListCtrl(UINT nID/* = IDS_NO_ITEMS_TO_DISPLAY*/) {
	m_nID = nID;
	KeyCtxTimer = 1;
	CreateFont();
}

CCeListCtrl::~CCeListCtrl() {
	m_oFont.DeleteObject();
}


BEGIN_MESSAGE_MAP(CCeListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCeListCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCeListCtrl message handlers

void CCeListCtrl::CreateFont() {
	HFONT hFont = (HFONT) GetStockObject(SYSTEM_FONT);
	CFont *pFont;
	pFont = CFont::FromHandle(hFont);
	LOGFONT lf = { 0 };
	pFont->GetLogFont(&lf);
//	lf.lfHeight = (lf.lfHeight * 8) / 10;
	lf.lfHeight = SCALEY(12) + 1;
	m_oFont.CreateFontIndirect(&lf);
}

void CCeListCtrl::OnPaint() {
    Default();
    if (GetItemCount() <= 0) {
        COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);
        COLORREF clrTextBk = ::GetSysColor(COLOR_WINDOW);

        CDC *pDC = GetDC();
        int nSavedDC = pDC->SaveDC();

        CRect rc;
        GetWindowRect(&rc);
        ScreenToClient(&rc);

        CHeaderCtrl *pHC = GetHeaderCtrl();
        if (pHC != NULL) {
            CRect rcH;
            pHC->GetItemRect(0, &rcH);
            rc.top += rcH.bottom;
        }
        rc.top += SCALEX(8);

        CString strText;
		strText.LoadString(m_nID);

        pDC->SetTextColor(clrText);
        pDC->SetBkColor(clrTextBk);
        pDC->FillRect(rc, &CBrush(clrTextBk));
        pDC->SelectObject(&m_oFont);
        pDC->DrawText(strText, rc, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);

        pDC->RestoreDC(nSavedDC);
        ReleaseDC(pDC);
    }
	// Do not call CListCtrl::OnPaint() for painting messages
}

void CCeListCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
	// The code below fixes wrong MFC behaviour on Pocket PC 2002 devices
	// (running circles appears twice while you tap and hold
	Default();
}

void CCeListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	switch (nChar) {
		case VK_RETURN:
			KillTimer(KeyCtxTimer);
			SetTimer(KeyCtxTimer, 100, NULL);
			break;
	}

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCeListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	switch (nChar) {
		case VK_RETURN:
			KillTimer(KeyCtxTimer);
			break;
	}

	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCeListCtrl::OnTimer(UINT nIDEvent) {
	if (nIDEvent == KeyCtxTimer) {
		KillTimer(KeyCtxTimer);
	}

	CListCtrl::OnTimer(nIDEvent);
}
