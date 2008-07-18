/**
 *  RichesseGPS.h : main header file for the RICHESSEGPS application
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

#if !defined(AFX_RICHESSEGPS_H__0013B90D_14F9_4F87_8CCF_7E6075AD3356__INCLUDED_)
#define AFX_RICHESSEGPS_H__0013B90D_14F9_4F87_8CCF_7E6075AD3356__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Config.h"       // main symbols
#include "gps/gps.h"

/////////////////////////////////////////////////////////////////////////////
// CRichesseGPSApp:
// See RichesseGPS.cpp for the implementation of this class
//

class CRichesseGPSApp : public CWinApp
{
public:
	CRichesseGPSApp();

	CGPS Gps;
	// POIs
	CList<CPoi *, CPoi *> POIs;
	// navigate to
	CPoi *Destination;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichesseGPSApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	BOOL SavePois();
	BOOL LoadPois();

// Implementation
protected:

	//{{AFX_MSG(CRichesseGPSApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

extern CRichesseGPSApp theApp;


//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHESSEGPS_H__0013B90D_14F9_4F87_8CCF_7E6075AD3356__INCLUDED_)
