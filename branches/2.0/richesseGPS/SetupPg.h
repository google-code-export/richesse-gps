/**
 *  SetupPg.h : header file
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

#if !defined(AFX_SETUPPG_H__89BC9F78_AAAA_4D8F_82BF_F34F013AD76D__INCLUDED_)
#define AFX_SETUPPG_H__89BC9F78_AAAA_4D8F_82BF_F34F013AD76D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/Caption.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupGeneralPg dialog

class CSetupGeneralPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSetupGeneralPg)

// Construction
public:
	CSetupGeneralPg();
	~CSetupGeneralPg();

// Dialog Data
	//{{AFX_DATA(CSetupGeneralPg)
	enum { IDD = IDD_SETUP_GENERAL };
	CComboBox	m_ctlCoords;
	CComboBox	m_ctlUnits;
	CCaption	m_lblCoords;
	CCaption	m_lblSuspendMode;
	BOOL	m_bPreventSuspend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSetupGeneralPg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSetupGeneralPg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPPG_H__89BC9F78_AAAA_4D8F_82BF_F34F013AD76D__INCLUDED_)
