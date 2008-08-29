/**
 *  DownloadManagerDlg.h : header file
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

#if !defined(AFX_DOWNLOADMANAGERDLG_H__662D05A2_32F5_49A2_9A44_164349F0CA9D__INCLUDED_)
#define AFX_DOWNLOADMANAGERDLG_H__662D05A2_32F5_49A2_9A44_164349F0CA9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Config.h"
#include "ctrls/CeDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CDownloadManagerDlg dialog

class CDownloadManagerDlg : public CCeDialog
{
// Construction
public:
	CDownloadManagerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDownloadManagerDlg)
	enum { IDD = IDD_DOWNLOAD };
	CProgressCtrl	m_ctlProgress;
	CListCtrl	m_ctlItems;
	//}}AFX_DATA

	CList<CPoi *, CPoi *> Pois;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloadManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HANDLE HThread;
	BOOL Terminated;

	void ThreadProc();
	DWORD DownloadFile(const CString &url, const CString &strFileName);
	void UpdateMenu();

	// Generated message map functions
	//{{AFX_MSG(CDownloadManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

	friend DWORD WINAPI DownManThreadProc(LPVOID lpParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNLOADMANAGERDLG_H__662D05A2_32F5_49A2_9A44_164349F0CA9D__INCLUDED_)
