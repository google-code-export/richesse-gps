/**
 *  PoiListCtrl.cpp : implementation file
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

#include "../StdAfx.h"
#include "../RichesseGPS.h"
#include "PoiListCtrl.h"
#include "../Config.h"
#include "../../share/helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPoiListCtrl

CPoiListCtrl::CPoiListCtrl() {
	Selecting = FALSE;
	SelectionStart = -1;	
	TapAndHoldTimer = 1;

	CDC *pDC = GetDC();
	if (pDC->GetDeviceCaps(LOGPIXELSX) > 130)
		m_ilStatus.Create(IDB_POI_STATUS_HI, SCALEX(16), 0, RGB(255, 0, 255));
	else
		m_ilStatus.Create(IDB_POI_STATUS, SCALEX(16), 0, RGB(255, 0, 255));
	ReleaseDC(pDC);
}

CPoiListCtrl::~CPoiListCtrl() {
}


BEGIN_MESSAGE_MAP(CPoiListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CPoiListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
//	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoiListCtrl message handlers

void CPoiListCtrl::SelectAllItems() {
	SetRedraw(FALSE);
	for (int i = 0; i < GetItemCount(); i++)
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	SetRedraw(TRUE);
}

void CPoiListCtrl::DeselectAllItems() {
//	CListCtrl &ctlList = GetListCtrl();
	// deselect all
	SetRedraw(FALSE);
	for (int i = 0; i < GetItemCount(); i++)
		SetItemState(i, 0, LVIS_SELECTED);
	SetRedraw(TRUE);
}

void CPoiListCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
	SetFocus();
	SetCapture();

	Selecting = FALSE;

	UINT flags = 0;
	int item = HitTest(point, &flags);
	if (item != -1) {
		UINT state = GetItemState(item, LVIS_SELECTED);
		if (state & LVIS_SELECTED) {
		}
		else {
			DeselectAllItems();
		}
		SetItemState(item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}
	SelectionStart = item;
	LastItem = item;
	LastCursorPos = point;

	KillTimer(TapAndHoldTimer);	// for sure
	SetTimer(TapAndHoldTimer, 100, NULL);
}

void CPoiListCtrl::OnMouseMove(UINT nFlags, CPoint point) {
	CListCtrl::OnMouseMove(nFlags, point);

	UINT flags = 0;
	int item = HitTest(point, &flags);

	if (Selecting) {
		CRect rcClient;
		GetClientRect(&rcClient);
		CPoint ptClient = point;

		if (PtInRect(rcClient, ptClient)) {
			if (item != -1 && item != LastItem && SelectionStart != -1) {
				// selection interval
				int selStart = -1;
				int selEnd = -1;
				if (item < SelectionStart) {
					selStart = item;
					selEnd = SelectionStart;
				}
				else {
					selStart = SelectionStart;
					selEnd = item;
				}

				// select items
				SetRedraw(FALSE);
				int i;
				for (i = 0; i < selStart; i++)
					SetItemState(i, 0, LVIS_SELECTED);
				for (i = selStart; i <= selEnd; i++)
					SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				for (i = selEnd + 1; i < GetItemCount(); i++)
					SetItemState(i, 0, LVIS_SELECTED);
				SetRedraw(TRUE);
				UpdateWindow();

				LastItem = item;
			}
		}
		else {
			// scroll
			CRect rcItem;
			GetItemRect(0, &rcItem, LVIR_BOUNDS);

			if (ptClient.y < rcClient.top) {
				SetRedraw(FALSE);
				// scroll up
				Scroll(CSize(0, -rcItem.Height()));
				// select scrolled items
				for (int i = GetTopIndex(); i < SelectionStart; i++)
					SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				SetRedraw(TRUE);
				UpdateWindow();
			}
			else {
				SetRedraw(FALSE);
				// scroll down
				Scroll(CSize(0, rcItem.Height()));
				// select scrolled items
				int nLast = GetTopIndex() + GetCountPerPage() + 1;
				for (int i = SelectionStart; i < nLast; i++)
					SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				SetRedraw(TRUE);
				UpdateWindow();
			}
		}

		LastCursorPos = point;
	}
	else {
		if (abs(LastCursorPos.x - point.x) > 2 || abs(LastCursorPos.y - point.y) > 2) {
			Selecting = TRUE;
			KillTimer(TapAndHoldTimer);
		}
	}
}

void CPoiListCtrl::OnLButtonUp(UINT nFlags, CPoint point) {
	ReleaseCapture();

	CListCtrl::OnLButtonUp(nFlags, point);

	//
	if (Selecting) {
	}
	else {
		// deselect all but the active item
		SetRedraw(FALSE);
		for (int i = 0; i < GetItemCount(); i++) {
			if (i == SelectionStart) 
				SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			else
				SetItemState(i, 0, LVIS_SELECTED);
		}
		SetRedraw(TRUE);
	}
	Selecting = FALSE;
}

void CPoiListCtrl::OnTimer(UINT nIDEvent) {
	if (nIDEvent == TapAndHoldTimer) {
		KillTimer(TapAndHoldTimer);

		ReleaseCapture();
		Selecting = FALSE;

		// tap and hold
		SHRGINFO  shrg;
		shrg.cbSize = sizeof(shrg);
		shrg.hwndClient = GetSafeHwnd();
		shrg.ptDown.x = LastCursorPos.x;
		shrg.ptDown.y = LastCursorPos.y;
		shrg.dwFlags = SHRG_NOTIFYPARENT | SHRG_RETURNCMD;

		::SHRecognizeGesture(&shrg);
	}
		
	CListCtrl::OnTimer(nIDEvent);
}

void CPoiListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
	CDC* pDC;
	pDC = CDC::FromHandle(lpDIS->hDC);
	int nSave = pDC->SaveDC();

	CHeaderCtrl *hdrCtrl = GetHeaderCtrl();
	for (int col = 0; col < hdrCtrl->GetItemCount(); col++) {
		LV_ITEM lvi;
		lvi.mask = LVIF_PARAM | LVIF_IMAGE;
		lvi.iItem = lpDIS->itemID;
		lvi.iSubItem = col;
		VERIFY(GetItem(&lvi));

		CString strTitle = GetItemText(lpDIS->itemID, col);
		
		LVCOLUMN lvCol;
		lvCol.mask = LVCF_FMT;
		GetColumn(col, &lvCol);

		CRect rcItem;
		GetSubItemRect(lpDIS->itemID, col, LVIR_BOUNDS, rcItem);

		// draw background
		if (lpDIS->itemState & ODS_SELECTED) {
			pDC->FillSolidRect(&rcItem, GetSysColor(COLOR_HIGHLIGHT));
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else {
			pDC->FillSolidRect(&rcItem, GetSysColor(COLOR_WINDOW));
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		}

		if (col == 0) {
			// draw icon
			CDC dcSrc;
			dcSrc.CreateCompatibleDC(pDC);
			CBitmap bmp;
			bmp.CreateCompatibleBitmap(pDC, SCALEX(16 + 1), SCALEY(16 + 1));
			dcSrc.SelectObject(&bmp);
			CRect rcIcon(0, 0, SCALEX(16 + 1), SCALEY(16 + 1));
			if (lpDIS->itemState & ODS_SELECTED) {
				dcSrc.FillSolidRect(&rcIcon, GetSysColor(COLOR_HIGHLIGHT));
			}
			else {
				dcSrc.FillSolidRect(&rcIcon, GetSysColor(COLOR_WINDOW));
			}

			CImageList *pImageList = GetImageList(LVSIL_SMALL);

			IMAGELISTDRAWPARAMS drawing;
			drawing.cbSize = sizeof(IMAGELISTDRAWPARAMS);
			drawing.himl = pImageList->GetSafeHandle();
			drawing.i = lvi.iImage;
			drawing.hdcDst = dcSrc.GetSafeHdc();
			drawing.x = 0;
			drawing.y = 0;
			drawing.cx = 0;
			drawing.cy = 0;
			drawing.xBitmap = 0;
			drawing.yBitmap = 0;
			drawing.rgbBk = CLR_NONE;
			drawing.rgbFg = CLR_DEFAULT;
			if (lpDIS->itemState & ODS_SELECTED)
				drawing.fStyle = ILD_BLEND50;
			else
				drawing.fStyle = ILD_NORMAL;
			drawing.dwRop = SRCCOPY;
			ImageList_DrawIndirect(&drawing);

			pDC->StretchBlt(rcItem.left + SCALEX(2), rcItem.top, rcItem.Height(), rcItem.Height(),
				&dcSrc, 0, 0, SCALEX(16 + 1), SCALEY(16 + 1), SRCCOPY);

			CPoi *poi = (CPoi *) lvi.lParam;
			IMAGELISTDRAWPARAMS drStatus;
			drStatus.cbSize = sizeof(IMAGELISTDRAWPARAMS);
			drStatus.himl = m_ilStatus.GetSafeHandle();
			switch (poi->Status) {
				case CPoi::Completed:    drStatus.i = 1; break;
				case CPoi::NotFound:     drStatus.i = 2; break;
				default:
				case CPoi::NotCompleted: drStatus.i = 0; break;
			}
			drStatus.hdcDst = dcSrc.GetSafeHdc();
			drStatus.x = 0;
			drStatus.y = 0;
			drStatus.cx = 0;
			drStatus.cy = 0;
			drStatus.xBitmap = 0;
			drStatus.yBitmap = 0;
			drStatus.rgbBk = CLR_NONE;
			drStatus.rgbFg = CLR_DEFAULT;
			if (lpDIS->itemState & ODS_SELECTED)
				drStatus.fStyle = ILD_BLEND50;
			else
				drStatus.fStyle = ILD_NORMAL;
			drStatus.dwRop = SRCCOPY;
			ImageList_DrawIndirect(&drStatus);

			pDC->StretchBlt(rcItem.left + SCALEX(2), rcItem.top, rcItem.Height(), rcItem.Height(),
				&dcSrc, 0, 0, SCALEX(16 + 1), SCALEY(16 + 1), SRCCOPY);

			bmp.DeleteObject();
			dcSrc.DeleteDC();
		}

		// draw text
		CRect rcLabel;
		GetSubItemRect(lpDIS->itemID, col, LVIR_LABEL, rcLabel);
		UINT uFormat =  DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
		if (lvCol.fmt & LVCFMT_LEFT)
			uFormat |= DT_LEFT;
		else if (lvCol.fmt & LVCFMT_RIGHT)
			uFormat |= DT_RIGHT;
		else if (lvCol.fmt & LVCFMT_CENTER)
			uFormat |= DT_CENTER;
		else
			uFormat |= DT_LEFT;

		rcLabel.DeflateRect(SCALEX(4), 0, SCALEX(4), 0);
		DrawTextEndEllipsis(*pDC, strTitle, rcLabel, uFormat);
	}

	pDC->RestoreDC(nSave);
}

/*
void CPoiListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	switch (nChar) {
		case VK_RETURN:
			break;

		default:
//			CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/