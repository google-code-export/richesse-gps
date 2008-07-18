/**
 *  SetupGpsPg.h : header file
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

#if !defined(AFX_SETUPGPSPG_H__928CFD6F_29FD_414D_98F1_B2A6BD463A37__INCLUDED_)
#define AFX_SETUPGPSPG_H__928CFD6F_29FD_414D_98F1_B2A6BD463A37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/Caption.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupGpsPg dialog

class CSetupGpsPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSetupGpsPg)

// Construction
public:
	CSetupGpsPg();
	~CSetupGpsPg();

// Dialog Data
	//{{AFX_DATA(CSetupGpsPg)
	enum { IDD = IDD_SETUP_GPS };
	BOOL	m_bAutoConnect;
	BOOL	m_bReconnect;
	CComboBox	m_ctlBaudRate;
	CComboBox	m_ctlPort;
	int		m_nPort;
	CString	m_strBaudRate;
	CCaption m_ctlGpsConn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSetupGpsPg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSetupGpsPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAutodetect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPGPSPG_H__928CFD6F_29FD_414D_98F1_B2A6BD463A37__INCLUDED_)
