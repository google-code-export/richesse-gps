/**
 *  PoiListCtrl.h : header file
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

#if !defined(AFX_POILISTCTRL_H__93035D17_5F74_48FA_9377_4B6A002841A3__INCLUDED_)
#define AFX_POILISTCTRL_H__93035D17_5F74_48FA_9377_4B6A002841A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPoiListCtrl window

class CPoiListCtrl : public CListCtrl
{
// Construction
public:
	CPoiListCtrl();

// Attributes
public:

protected:
	UINT TapAndHoldTimer;
	CPoint LastCursorPos;
	int LastItem;
	int SelectionStart;
	BOOL Selecting;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoiListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPoiListCtrl();
	void SelectAllItems();
	void DeselectAllItems();

	// Generated message map functions
protected:
	CImageList m_ilStatus;

	//{{AFX_MSG(CPoiListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POILISTCTRL_H__93035D17_5F74_48FA_9377_4B6A002841A3__INCLUDED_)
