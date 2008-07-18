/**
 *  AddWaypointDlg.h : header file
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

#if !defined(AFX_ADDWAYPOINTDLG_H__E4475C9C_CEB7_4AF1_BCB8_22022BAEE5A9__INCLUDED_)
#define AFX_ADDWAYPOINTDLG_H__E4475C9C_CEB7_4AF1_BCB8_22022BAEE5A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/Caption.h"
#include "ctrls/CeEdit.h"
#include "ctrls/CePropertySheet.h"

/////////////////////////////////////////////////////////////////////////////
// CWaypointPg dialog

class CWaypointPg : public CPropertyPage
{
// Construction
public:
	CWaypointPg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaypointPg)
	enum { IDD = IDD_ADD };
	CComboBox	m_ctlType;
	CCeEdit	m_ctlLongSec;
	CCeEdit	m_ctlLongMin;
	CCeEdit	m_ctlLongDeg;
	CCeEdit	m_ctlLatSec;
	CCeEdit	m_ctlLatMin;
	CCeEdit	m_ctlLatDeg;
	CComboBox	m_ctlLongDir;
	CComboBox	m_ctlLatDir;
	CCeEdit	m_ctlName;
//	CCaption	m_lblTitle;
	CStatic	m_lblCompleted;
	CStatic	m_ctlCompleted;
	CString	m_strName;
	int		m_nType;
	//}}AFX_DATA
//	CString m_strTitle;
//	UINT m_nMenuID;

	double	m_dLatitude;
	double	m_dLongitude;
	SYSTEMTIME m_stCompleted;
	BOOL m_bCompleted;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaypointPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HWND m_hwndCmdBar;

	// Generated message map functions
	//{{AFX_MSG(CWaypointPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	void UpdateControls();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWaypointSheet Dialog
//

class CWaypointSheet : public CCePropertySheet {
public:
	CWaypointSheet(UINT nIDCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0);
	CWaypointSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	// Generated message map functions
	//{{AFX_MSG(CWaypointSheet)
	afx_msg void OnAdd();
	afx_msg void OnSave();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDWAYPOINTDLG_H__E4475C9C_CEB7_4AF1_BCB8_22022BAEE5A9__INCLUDED_)
