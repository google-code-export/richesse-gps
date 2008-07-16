/**
 *  GPSInfoPg.h : header file
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

#if !defined(AFX_GPSINFOPG_H__INCLUDED_)
#define AFX_GPSINFOPG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/Caption.h"
#include "ctrls/SatellitesCtrl.h"

class CGPS;

/////////////////////////////////////////////////////////////////////////////
// CGPSInfoPg dialog

class CGPSInfoPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CGPSInfoPg)

// Construction
public:
	CGPSInfoPg();
	~CGPSInfoPg();

// Dialog Data
	//{{AFX_DATA(CGPSInfoPg)
	enum { IDD = IDD_INFO };
	CSatellitesCtrl	m_ctlSatellites;
	CStatic	m_ctlStatus;
	CStatic	m_ctlSatCount;
	CStatic	m_ctlLongitude;
	CStatic	m_ctlLatitude;
	CStatic	m_ctlAltitude;
	CCaption	m_lblSatellites;
	CCaption	m_lblPosition;
	//}}AFX_DATA
	UINT RefreshTimer;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGPSInfoPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ShowInfo();
	// Generated message map functions
	//{{AFX_MSG(CGPSInfoPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GPSInfoPg_H__178356EF_B1C4_4DE0_B0DC_8655435B44CE__INCLUDED_)
