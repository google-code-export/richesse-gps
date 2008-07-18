/**
 *  SatellitesCtrl.h : header file
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

#if !defined(AFX_SATELLITESCTRL_H__B3D65574_4BEF_4735_B78C_753470CB8DA3__INCLUDED_)
#define AFX_SATELLITESCTRL_H__B3D65574_4BEF_4735_B78C_753470CB8DA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../gps/gps.h"

/////////////////////////////////////////////////////////////////////////////
// CSatellitesCtrl window

class CSatellitesCtrl : public CStatic
{
// Construction
public:
	CSatellitesCtrl();

// Attributes
public:
	CSatellitePos m_arSatellites[MAX_SATELLITES];		// GSP can see only 12 satellites
	int m_arSatellitesForFix[MAX_SATELLITES];

	int SatsToShow;
	int ColSep;

protected:
	CBitmap *m_bmp;
	CDC *m_dcMem;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSatellitesCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSatellitesCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSatellitesCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SATELLITESCTRL_H__B3D65574_4BEF_4735_B78C_753470CB8DA3__INCLUDED_)
