/**
 *  RadeadDlg.h : header file
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

#if !defined(AFX_RADARDLG_H__FD9F75CE_7C56_4217_8E95_963DCCCD63C5__INCLUDED_)
#define AFX_RADARDLG_H__FD9F75CE_7C56_4217_8E95_963DCCCD63C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Config.h"
#include "ctrls/CeDialog.h"
#include "ctrls/PoiListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CRadarDlg dialog

class CRadarDlg : public CCeDialog
{
// Construction
public:
	CRadarDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRadarDlg)
	enum { IDD = IDD_RADAR };
	CPoiListCtrl	m_ctlPoints;
	//}}AFX_DATA

	CPoi *NavigateTo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// values must match to the order of column in the m_ctlPoitns control
	CImageList m_ilCache;

	void InsertPoi(CPoi *poi);
	void SortPois();

	void UpdateControls();


	// Generated message map functions
	//{{AFX_MSG(CRadarDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelect();
	afx_msg void OnKeydownPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	void OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult);
	void SetPoiImage(int item, CPoi::EType type);
	void UpdateDistance(int item);

	void UpdateColumnWidths();

	UINT RefreshTimer;

	DECLARE_MESSAGE_MAP()

	friend int CALLBACK PointsCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POIMAN_H__FD9F75CE_7C56_4217_8E95_963DCCCD63C5__INCLUDED_)
