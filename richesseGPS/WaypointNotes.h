/**
 *  WaypointNotes.h : header file
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

#if !defined(AFX_WAYPOINTNOTES_H__218A911E_2422_4E3A_A85B_EEF362FA9056__INCLUDED_)
#define AFX_WAYPOINTNOTES_H__218A911E_2422_4E3A_A85B_EEF362FA9056__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CWaypointNotesPg dialog

class CWaypointNotesPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CWaypointNotesPg)

// Construction
public:
	CWaypointNotesPg();
	~CWaypointNotesPg();

// Dialog Data
	//{{AFX_DATA(CWaypointNotesPg)
	enum { IDD = IDD_ADD_NOTES };
	CCeEdit	m_ctlDescription;
	CString	m_strDescription;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWaypointNotesPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWaypointNotesPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnSetActive();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAYPOINTNOTES_H__218A911E_2422_4E3A_A85B_EEF362FA9056__INCLUDED_)
