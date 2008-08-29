/**
 *  ChildView.h : interface of the CChildView class
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

#if !defined(AFX_CHILDVIEW_H__A0F65C54_CCD6_496C_B51D_53321832EBC0__INCLUDED_)
#define AFX_CHILDVIEW_H__A0F65C54_CCD6_496C_B51D_53321832EBC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CompassCtrl.h"
#include "ctrls/SatellitesCtrl.h"
#include "ctrls/Caption.h"
#include "ctrls/CeEdit.h"
#include "ctrls/CeStatic.h"
#include "ctrls/HtmlCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	CCompassCtrl m_ctlCompass;
	CCeStatic m_ctlWaypoint;

	CCaption m_lblAltitude;
	CStatic m_ctlAltitude;

	CCaption m_lblAzimuth;
	CStatic m_ctlAzimuth;

	CCaption m_lblDistance;
	CCaption m_ctlDistance;

	CCaption m_lblSpeed;
	CStatic m_ctlSpeed;

	CCaption m_lblHDOP;
	CStatic m_ctlHDOP;

	CCeStatic	m_ctlLongitude;
	CCeStatic	m_ctlLatitude;
	CSatellitesCtrl	m_ctlSatellites;

	CBitmapButton m_btnAdd, m_btnCompass, m_btnNotes, m_btnInfo;

	CCeEdit m_ctlNotes;
	CHTMLCtrl m_ctlInfo;

	enum EView {
		Compass,
		Notes,
		Info
	} View;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();

	void ShowInfo();
	void OnAdd();
	void LoadHtml(LPCTSTR fileName);

	// Generated message map functions
protected:
	CFont m_fntWaypoint;
	CFont m_fntBase;
	CFont m_fntSmall;
	CFont m_fntBold;

	void CreateFonts();

	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChangeNotes();
	//}}AFX_MSG

	void ShowCompassPg(BOOL show);
	void ShowNotesPg(BOOL show);
	void ShowInfoPg(BOOL show);

	void OnCompassView();
	void OnNotesView();
	void OnInfoView();

	afx_msg void OnWaypointCtx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPositionCtx(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnCopyWaypoint();
	afx_msg void OnCopyLongitude();
	afx_msg void OnCopyLatitude();

	afx_msg void OnDownloadInfo();
	afx_msg void OnMarkAs(UINT nID);

	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__A0F65C54_CCD6_496C_B51D_53321832EBC0__INCLUDED_)
