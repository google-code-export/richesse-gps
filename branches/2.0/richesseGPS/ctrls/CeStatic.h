/**
 *  CeStatic.h : header file
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

#if !defined(AFX_CESTATIC_H__INCLUDED_)
#define AFX_CESTATIC_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCeStatic window

class CCeStatic : public CStatic
{
// Construction
public:
	CCeStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCeStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCeStatic();

	// Generated message map functions
protected:
	void ContextMenu(CPoint point);

	//{{AFX_MSG(CCeStatic)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
//	afx_msg BOOL OnContextMenu(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEEDIT_H__B4848F28_54CD_4385_A299_DC1B9905C3D8__INCLUDED_)
