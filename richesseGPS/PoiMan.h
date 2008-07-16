/**
 *  PoiMan.h : header file
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

#if !defined(AFX_POIMAN_H__FD9F75CE_7C56_4217_8E95_963DCCCD63C5__INCLUDED_)
#define AFX_POIMAN_H__FD9F75CE_7C56_4217_8E95_963DCCCD63C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Config.h"
#include "ctrls/CeDialog.h"
#include "ctrls/PoiListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPoiMan dialog

class CPoiMan : public CCeDialog
{
// Construction
public:
	CPoiMan(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPoiMan)
	enum { IDD = IDD_POI_MAN };
	CPoiListCtrl	m_ctlPoints;
	//}}AFX_DATA

	CPoi *NavigateTo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPoiMan)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// values must match to the order of column in the m_ctlPoitns control
	enum ESortColumn {
		Name = 0,
		Status = 1,
		CacheType = 2
	} SortColumn;

	enum ESortType {
		Ascending = 0,
		Descending = 1
	} SortType;

	CImageList m_ilCache;

	void InsertPoi(CPoi *poi);
	void SortPois();

	void UpdateControls();


	// Generated message map functions
	//{{AFX_MSG(CPoiMan)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnImport();
	afx_msg void OnInfo();
	afx_msg void OnRemove();
	afx_msg void OnSelect();
	afx_msg void OnAdd();
	afx_msg void OnItemchangedPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMarkasCompleted();
	afx_msg void OnMarkasNotcompleted();
	afx_msg void OnMarkasNotfound();
	afx_msg void OnClickPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSortbyAscending();
	afx_msg void OnSortbyDescending();
	afx_msg void OnSortbyName();
	afx_msg void OnSortbyStatus();
	afx_msg void OnSortbyCacheType();
	afx_msg void OnExport();
	afx_msg void OnExportKML();
	afx_msg void OnSelectall();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDownloadInfo();
	//}}AFX_MSG
	void OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult);
	void SetPoiImage(int item, CPoi::EType type);

	void OnUpdateSort(CCmdUI *pCmdUI);
	void OnUpdateSortCol(CCmdUI *pCmdUI);
	void OnUpdateMarkAs(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()

	friend int CALLBACK PointsCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POIMAN_H__FD9F75CE_7C56_4217_8E95_963DCCCD63C5__INCLUDED_)
