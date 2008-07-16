/**
 *  CompassCtrl.h : header file
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

#if !defined(AFX_COMPASSCTRL_H__B213292A_5805_4A5A_858F_C94FB1D48784__INCLUDED_)
#define AFX_COMPASSCTRL_H__B213292A_5805_4A5A_858F_C94FB1D48784__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCompassCtrl window

class CCompassCtrl : public CStatic
{
// Construction
public:
	CCompassCtrl();

// Attributes
protected:
	CBitmap *m_bmp;
	CDC *m_dcMem;

// Operations
public:
	void SetNorthDirection(int dir);
	void SetDirection(int dir);

	void Stop();
	void StopNavigate();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompassCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCompassCtrl();

	// Generated message map functions
protected:
	void CreateFonts();

	COLORREF m_clrBkGnd, m_clrFrame;
	COLORREF m_clrRed;

	//{{AFX_MSG(CCompassCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	int CurNorthDirection;
	int NorthDir;
	int NorthDirection;

	int CurNavDirection;
	int NavDir;
	int NavDirection;
	BOOL NavigateTo;

	UINT RotTimer, NavTimer;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPASSCTRL_H__B213292A_5805_4A5A_858F_C94FB1D48784__INCLUDED_)
