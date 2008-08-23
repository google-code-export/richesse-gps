/**
 *  PoiMan.cpp : implementation file
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

#include <windows.h>

#include "StdAfx.h"
#include "RichesseGPS.h"
#include "PoiMan.h"
#include "ctrls/BrowseDlg.h"
#include "AddWaypointDlg.h"
#include "WaypointNotes.h"
#include "../share/file.h"
#include "../share/helpers.h"
#include "DownloadManagerDlg.h"

#include "files/files.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ICON_NOT_CHECKED					0
#define ICON_CHECKED						1

/////////////////////////////////////////////////////////////////////////////
// CImport Dialog
//

class CImportDlg : public CBrowseDlg {
public:
	CImportDlg(CWnd *pParent = NULL);

	// Generated message map functions
	//{{AFX_MSG(CImportDlg)
	afx_msg void OnImport();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

CImportDlg::CImportDlg(CWnd *pParent/* = NULL*/) : CBrowseDlg(TRUE, pParent) {
	m_nMenuID = IDR_IMPORT;

//	m_bNewFolderBtn = FALSE;
	m_strCaption.LoadString(IDS_FILE_TO_IMPORT);
	m_strExts = _T("loc;gpx;ov2;rp");
}

BEGIN_MESSAGE_MAP(CImportDlg, CBrowseDlg)
	//{{AFX_MSG_MAP(CImportDlg)
	ON_COMMAND(ID_IMPORT, OnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CImportDlg::OnImport() {
	UpdateData();
	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// CExport Dialog
//

class CExportDlg : public CBrowseDlg {
public:
	CExportDlg(CWnd *pParent = NULL);

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
	afx_msg void OnExport();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

CExportDlg::CExportDlg(CWnd *pParent/* = NULL*/) : CBrowseDlg(FALSE, pParent) {
	m_nMenuID = IDR_EXPORT;

//	m_bNewFolderBtn = FALSE;
	m_strCaption.LoadString(IDS_SELECT_LOCATION);
}

BEGIN_MESSAGE_MAP(CExportDlg, CBrowseDlg)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_COMMAND(ID_EXPORT, OnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExportDlg::OnExport() {
	UpdateData();
	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////

static int CALLBACK 
PointsCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CPoiMan *pg = (CPoiMan *) lParamSort;
	CPoi *poi1 = (CPoi *) lParam1;
	CPoi *poi2 = (CPoi *) lParam2;

	switch (pg->SortColumn) {
		case CPoiMan::Name: {
			if (pg->SortType == CPoiMan::Ascending)
				return poi1->Name.CompareNoCase(poi2->Name);
			else
				return poi2->Name.CompareNoCase(poi1->Name);
			} break;

		case CPoiMan::Status: {
			if (pg->SortType == CPoiMan::Ascending)
				return poi1->Status - poi2->Status;
			else
				return poi2->Status - poi1->Status;
			} break;

		case CPoiMan::CacheType: {
			if (pg->SortType == CPoiMan::Ascending)
				return poi1->Type - poi2->Type;
			else
				return poi2->Type - poi1->Type;
			} break;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPoiMan dialog


CPoiMan::CPoiMan(CWnd* pParent /*=NULL*/)
	: CCeDialog(CPoiMan::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPoiMan)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetMenu(IDR_POI_MAN);

	SortColumn = (ESortColumn) Config.SortColumn;			// by defualt, sort by complete
	SortType = (ESortType) Config.SortType;

	NavigateTo = NULL;
}


void CPoiMan::DoDataExchange(CDataExchange* pDX)
{
	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPoiMan)
	DDX_Control(pDX, IDC_POIS, m_ctlPoints);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPoiMan, CCeDialog)
	//{{AFX_MSG_MAP(CPointsPg)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENU_IMPORT, OnImport)
	ON_COMMAND(ID_MENU_EDIT, OnInfo)
	ON_COMMAND(ID_MENU_DELETE, OnRemove)
	ON_COMMAND(ID_MENU_ADDPOI, OnAdd)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_POIS, OnItemchangedPoints)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_POIS, OnColumnclickPoints)
	ON_NOTIFY(LVN_KEYDOWN, IDC_POIS, OnKeydownPoints)
	ON_COMMAND(ID_POI_MARKAS_COMPLETED, OnMarkasCompleted)
	ON_COMMAND(ID_POI_MARKAS_NOTCOMPLETED, OnMarkasNotcompleted)
	ON_COMMAND(ID_POI_MARKAS_NOTFOUND, OnMarkasNotfound)
	ON_NOTIFY(NM_CLICK, IDC_POIS, OnClickPoints)
	ON_COMMAND(ID_MENU_EXPORT, OnExport)
	ON_COMMAND(ID_MENU_EXPORT_KML, OnExportKML)
	ON_COMMAND(ID_NAVIGATE, OnSelect)
	ON_COMMAND(ID_POI_SORTBY_ASCENDING, OnSortbyAscending)
	ON_COMMAND(ID_POI_SORTBY_DESCENDING, OnSortbyDescending)
	ON_COMMAND(ID_POI_SORTBY_NAME, OnSortbyName)
	ON_COMMAND(ID_POI_SORTBY_STATUS, OnSortbyStatus)
	ON_COMMAND(ID_POI_SORTBY_CACHETYPE, OnSortbyCacheType)
	ON_COMMAND(ID_POI_SELECTALL, OnSelectall)
	ON_COMMAND(ID_MENU_DOWNLOADINFO, OnDownloadInfo)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(GN_CONTEXTMENU, IDC_POIS, OnContextMenu)
	ON_UPDATE_COMMAND_UI_RANGE(ID_POI_SORTBY_ASCENDING, ID_POI_SORTBY_DESCENDING, OnUpdateSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_POI_SORTBY_NAME, ID_POI_SORTBY_CACHETYPE, OnUpdateSortCol)
	ON_UPDATE_COMMAND_UI_RANGE(ID_POI_MARKAS_NOTCOMPLETED, ID_POI_MARKAS_NOTFOUND, OnUpdateMarkAs)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoiMan message handlers

BOOL CPoiMan::OnInitDialog() {
	CCeDialog::OnInitDialog();
	
	CRect rc;
	m_ctlPoints.GetClientRect(rc);
	rc.right -= ::GetSystemMetrics(SM_CXVSCROLL);

//	m_ctlPoints.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	m_ctlPoints.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	CString s;

//	m_ctlPoints.SetExtendedStyle(LVS_EX_FULLROWSELECT);
//	m_ctlPoints.InsertColumn(0, _T(""), LVCFMT_LEFT, SCALEX(20));
//	s.LoadString(IDS_ID);   m_ctlPoints.InsertColumn(1, s, LVCFMT_LEFT, SCALEX(60));
	s.LoadString(IDS_NAME);
	m_ctlPoints.InsertColumn(0, s, LVCFMT_LEFT, rc.Width());

	CDC *pDC = GetDC();
	if (pDC->GetDeviceCaps(LOGPIXELSX) > 130)
		m_ilCache.Create(IDB_CACHE_HI, SCALEX(16), 0, RGB(255, 0, 255));
	else
		m_ilCache.Create(IDB_CACHE, SCALEX(16), 0, RGB(255, 0, 255));
	ReleaseDC(pDC);

	m_ctlPoints.SetImageList(&m_ilCache, LVSIL_SMALL);

	POSITION pos = theApp.POIs.GetHeadPosition();
	while (pos != NULL) {
		CPoi *poi = theApp.POIs.GetNext(pos);
		InsertPoi(poi);
	}

	SortPois();
	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPoiMan::SortPois() {
	m_ctlPoints.SortItems(PointsCompareProc, (DWORD) this);
}

void CPoiMan::SetPoiImage(int item, CPoi::EType type) {
	LVITEM lvi = { 0 };
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = item;
	lvi.iImage = GetCacheImage(type);
	m_ctlPoints.SetItem(&lvi);
}

void CPoiMan::InsertPoi(CPoi *poi) {
	LVFINDINFO info;
	info.flags = LVFI_PARTIAL|LVFI_STRING;
	CString str;
	info.psz = poi->Name;
	int nIndex;
	if ((nIndex = m_ctlPoints.FindItem(&info))!= -1) {
//		MessageBox(NULL,L"Multiple POI item",L"POI warning",MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		int n = m_ctlPoints.GetItemCount();
		int item = m_ctlPoints.InsertItem(n, poi->Name, poi->Status == CPoi::Completed ? ICON_CHECKED : ICON_NOT_CHECKED);

	SetPoiImage(item, poi->Type);
//	m_ctlPoints.SetItemText(item, 1, poi->Id);
//	m_ctlPoints.SetItemText(item, 1, poi->Name);
//	m_ctlPoints.SetCheck(item, poi->Completed);

	m_ctlPoints.SetItemData(item, (DWORD) poi);
	}
}

void CPoiMan::OnDestroy() {
	CCeDialog::OnDestroy();
	
	// save points
	theApp.POIs.RemoveAll();
	for (int i = 0; i < m_ctlPoints.GetItemCount(); i++) {
		CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(i);
		theApp.POIs.AddTail(poi);
	}

	// save a part of GUI
	Config.SortColumn = SortColumn;
	Config.SortType = SortType;
}

void CPoiMan::UpdateControls() {
/*	if (m_ctlPoints.GetSelectedCount() >= 1) {
		m_btnRemove.EnableWindow();
	}
	else {
		m_btnRemove.EnableWindow(FALSE);
	}

	if (m_ctlPoints.GetSelectedCount() == 1) {
		m_btnInfo.EnableWindow();
		m_btnSelect.EnableWindow();
	}
	else {
		m_btnInfo.EnableWindow(FALSE);
		m_btnSelect.EnableWindow(FALSE);
	}
*/
}



void CPoiMan::OnImport() {
	CImportDlg dlg(this);
	dlg.m_strPath = Config.LastImportDir;

	if (dlg.DoModal() == IDOK) {
		// load the file
		BOOL ret;
		CList<CPoi *, CPoi *> pois;
		{
			CWaitCursor wait;
			CString ext = dlg.m_strPath.Right(4);
			if (ext.CompareNoCase(_T(".loc")) == 0)
				ret = ReadLocFile(dlg.m_strPath, pois);
			else if (ext.CompareNoCase(_T(".gpx")) == 0)
				ret = ReadGpxFile(dlg.m_strPath, pois);
			else if (ext.CompareNoCase(_T(".ov2")) == 0)
				ret = ReadOV2File(dlg.m_strPath, pois);
			else if (ext.Right(3).CompareNoCase(_T(".rp")) == 0)
				ret = ReadRPFile(dlg.m_strPath, pois);
		}

		if (ret) {
			m_ctlPoints.SetRedraw(FALSE);

			POSITION pos = pois.GetHeadPosition();
			while (pos != NULL) {
				CPoi *poi = pois.GetNext(pos);
				if (poi != NULL)
					InsertPoi(poi);
			}

			m_ctlPoints.SetRedraw(TRUE);
		}
		else {
			AfxMessageBox(IDS_IMPORT_FAILED);
		}

		// save the path of the last imported file
		CString sPath = dlg.m_strPath;
		int nPos = sPath.ReverseFind('\\');
		if (nPos != -1)
			sPath = sPath.Left(nPos + 1);

		Config.LastImportDir = sPath;

		SortPois();
	}
}

void CPoiMan::OnInfo() {
	int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
	if (nSelItem != -1) {
		CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(nSelItem);

		CWaypointPg pgWaypoint;
		CWaypointNotesPg pgNotes;

		CString strTitle;
		if (poi->Id.GetLength() > 0)
			strTitle.Format(IDS_WAYPOINT_INFO_ID, poi->Id);
		else
			strTitle.Format(IDS_WAYPOINT_INFO);

		CWaypointSheet sheet(strTitle, this);
		sheet.AddPage(&pgWaypoint);
		sheet.AddPage(&pgNotes);

		sheet.SetMenu(IDR_INFO);

		pgWaypoint.m_strName = poi->Name;
		pgWaypoint.m_nType = poi->Type;
		pgWaypoint.m_dLatitude = poi->Latitude;
		pgWaypoint.m_dLongitude = poi->Longitude;
		pgWaypoint.m_bCompleted = poi->Status == CPoi::Completed;
		pgWaypoint.m_stCompleted = poi->TimeCompleted;
		pgNotes.m_strDescription = poi->Notes;

		if (sheet.DoModal()) {
			poi->Name = pgWaypoint.m_strName;
			poi->Latitude = pgWaypoint.m_dLatitude;
			poi->Longitude = pgWaypoint.m_dLongitude;
			poi->Type = (CPoi::EType) pgWaypoint.m_nType;
			poi->Notes = pgNotes.m_strDescription;

			m_ctlPoints.SetItemText(nSelItem, 0, poi->Name);
			SetPoiImage(nSelItem, poi->Type);
			SortPois();
		}
	}
}

void CPoiMan::OnRemove() {
	int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);

	if (nSelItem != -1 && AfxMessageBox(IDS_ARE_YOU_SERIOUS, MB_YESNO) == IDYES) {
		m_ctlPoints.SetRedraw(FALSE);

//		int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
		while (nSelItem != -1) {
			CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(nSelItem);

//			EnterCriticalSection(&CSGpsInfo);
			if (theApp.Destination == poi)
				theApp.Destination = NULL;
//			LeaveCriticalSection(&CSGpsInfo);

			delete poi;
			m_ctlPoints.DeleteItem(nSelItem);

			nSelItem = m_ctlPoints.GetNextItem(nSelItem - 1, LVNI_SELECTED);
		}

		m_ctlPoints.SetRedraw();
	}	
}

void CPoiMan::OnSelect() {
	int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
	if (nSelItem != -1) {
		CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(nSelItem);
		NavigateTo = poi;
		EndDialog(IDOK);
	}
}

void CPoiMan::OnAdd() {
	CWaypointPg pgWaypoint;
	CWaypointNotesPg pgNotes;
	CWaypointSheet sheet(IDS_ADD_WAYPOINT, this);

	sheet.AddPage(&pgWaypoint);
	sheet.AddPage(&pgNotes);
	sheet.SetMenu(IDR_ADD);

	// GPS fixed
	if (theApp.Gps.Quality > 0) {
		// pre fill the dialog
		pgWaypoint.m_dLatitude = theApp.Gps.Latitude;
		pgWaypoint.m_dLongitude = theApp.Gps.Longitude;
	}

	pgWaypoint.m_nType = CPoi::MyPOI;

	if (sheet.DoModal()) {
		CPoi *poi = new CPoi();
		poi->Name = pgWaypoint.m_strName;
		poi->Id = _T("");
		poi->Latitude = pgWaypoint.m_dLatitude;
		poi->Longitude = pgWaypoint.m_dLongitude;
		poi->Type = (CPoi::EType) pgWaypoint.m_nType;
		poi->Notes = pgNotes.m_strDescription;
		
		InsertPoi(poi);
		SortPois();
	}
}

void CPoiMan::OnItemchangedPoints(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	UpdateControls();
	
	*pResult = 0;
}

void CPoiMan::OnColumnclickPoints(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView != NULL) {
/*		if (pNMListView->iSubItem != SortColumn) {
			// change sort column (keep sort order as is)
			switch (pNMListView->iSubItem) {
/*				case 0:
					SortColumn = Complete;
					break;
*/
/*				case 1:
					SortColumn = Id;
					break;
*/
/*				case 0:
					SortColumn = Name;
					break;
			}
		}
		else {
*/
			// just change sort order
			if (SortType == Ascending)
				SortType = Descending;
			else
				SortType = Ascending;
//		}

		SortPois();
	}

	*pResult = 0;
}

void CPoiMan::OnMarkasCompleted() {
	int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
	while (nSelItem != -1) {
		CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(nSelItem);
		poi->Status = CPoi::Completed;
		GetLocalTime(&(poi->TimeCompleted));
		nSelItem = m_ctlPoints.GetNextItem(nSelItem, LVNI_SELECTED);
	}	
	m_ctlPoints.Invalidate();
	SortPois();
}

void CPoiMan::OnMarkasNotcompleted() {
	int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
	while (nSelItem != -1) {
		CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(nSelItem);
		poi->Status = CPoi::NotCompleted;
		nSelItem = m_ctlPoints.GetNextItem(nSelItem, LVNI_SELECTED);
	}	
	m_ctlPoints.Invalidate();
	SortPois();
}

void CPoiMan::OnMarkasNotfound() {
	int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
	while (nSelItem != -1) {
		CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(nSelItem);
		poi->Status = CPoi::NotFound;
		nSelItem = m_ctlPoints.GetNextItem(nSelItem, LVNI_SELECTED);
	}	
	m_ctlPoints.Invalidate();
	SortPois();
}

void CPoiMan::OnClickPoints(NMHDR* pNMHDR, LRESULT* pResult) {
	NMLISTVIEW *pnmlv = (LPNMLISTVIEW) pNMHDR;	

	if (pnmlv->iItem != -1)
		m_ctlPoints.SetItemState(pnmlv->iItem, LVIS_SELECTED, LVIS_SELECTED);

	*pResult = 0;
}

void CPoiMan::OnKeydownPoints(NMHDR* pNMHDR, LRESULT* pResult) {
	NMLVKEYDOWN *pnmkd = (NMLVKEYDOWN *) pNMHDR;	

	if (pnmkd->wVKey == VK_RETURN) {
		OnSelect();
	}

	*pResult = 0;
}

void CPoiMan::OnContextMenu(NMHDR *pNMHDR, LRESULT *pResult) {
	PNMRGINFO pInfo = (PNMRGINFO) pNMHDR;

	CMenu mnu;
	mnu.LoadMenu(IDR_POI_MAN_CTX);

	CMenu *popup = mnu.GetSubMenu(0);
	popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pInfo->ptAction.x, pInfo->ptAction.y, this);

	*pResult = TRUE;
}

// Sort ////

void CPoiMan::OnSortbyAscending() {
	if (SortType != Ascending) {
		SortType = Ascending;
		SortPois();
	}
}

void CPoiMan::OnSortbyDescending() {
	if (SortType != Descending) {
		SortType = Descending;
		SortPois();
	}
}

void CPoiMan::OnSortbyName() {
	if (SortColumn != Name) {
		SortColumn = Name;
		SortPois();
	}
}

void CPoiMan::OnSortbyStatus() {
	if (SortColumn != Status) {
		SortColumn = Status;
		SortPois();
	}
}

void CPoiMan::OnSortbyCacheType() {
	if (SortColumn != CacheType) {
		SortColumn = CacheType;
		SortPois();
	}
}

void CPoiMan::OnExport() {
	CExportDlg dlg;
	dlg.m_strPath = Config.LastExportDir;

	if (dlg.DoModal() == IDOK) {
		Config.LastExportDir = dlg.m_strPath;

		CList<CPoi *, CPoi *> pois;
		int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
		while (nSelItem != -1) {
			CPoi *p = (CPoi *) m_ctlPoints.GetItemData(nSelItem);
			pois.AddTail(p);
			nSelItem = m_ctlPoints.GetNextItem(nSelItem, LVNI_SELECTED);
		}

		if (pois.GetCount() <= 0)
			return;

		CString outFileName;
		outFileName.Format(_T("%s\\my-poi.rp"), dlg.m_strPath);

		BOOL export = FALSE;
		HANDLE hFile = CreateFile(outFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			DWORD dwError = GetLastError();
			if (dwError == ERROR_FILE_EXISTS || dwError == ERROR_ALREADY_EXISTS) {
				CString s;
				s.Format(IDS_OVERWRITE_FILE, outFileName);
				if (AfxMessageBox(s, MB_YESNO) == IDYES) {
					CloseHandle(hFile);
					DeleteFile(outFileName);
					export = TRUE;
				}
			}
			else {
				SysError(IDS_CANNOT_CREATE_FILE_EXPORT, GetLastError(), FALSE);
			}
		}
		else {
			export = TRUE;
			CloseHandle(hFile);
		}

		if (export) {
			CString sText;
			if (WriteRPFile(outFileName, pois))
				AfxMessageBox(IDS_EXPORT_SUCCESSFUL);
			else
				AfxMessageBox(IDS_EXPORT_UNSUCCESSFUL);
		}
	}
}

void CPoiMan::OnExportKML() {
	CExportDlg dlg;
	dlg.m_strPath = Config.LastExportDir;

	if (dlg.DoModal() == IDOK) {
		Config.LastExportDir = dlg.m_strPath;

		CList<CPoi *, CPoi *> pois;
		int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
		while (nSelItem != -1) {
			CPoi *p = (CPoi *) m_ctlPoints.GetItemData(nSelItem);
			pois.AddTail(p);
			nSelItem = m_ctlPoints.GetNextItem(nSelItem, LVNI_SELECTED);
		}

		if (pois.GetCount() <= 0)
			return;

		CString outFileName;
		outFileName.Format(_T("%s\\my-poi.kml"), dlg.m_strPath);

		BOOL export = FALSE;
		HANDLE hFile = CreateFile(outFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			DWORD dwError = GetLastError();
			if (dwError == ERROR_FILE_EXISTS || dwError == ERROR_ALREADY_EXISTS) {
				CString s;
				s.Format(IDS_OVERWRITE_FILE, outFileName);
				if (AfxMessageBox(s, MB_YESNO) == IDYES) {
					CloseHandle(hFile);
					DeleteFile(outFileName);
					export = TRUE;

					hFile = CreateFile(outFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
				}
			}
			else {
				SysError(IDS_CANNOT_CREATE_FILE_EXPORT, GetLastError(), FALSE);
			}
		}
		else {
			export = TRUE;
		}

		if (export) {
			// export
			char buffer[1024] = { 0 };
			DWORD written;

			sprintf(buffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			WriteFile(hFile, buffer, strlen(buffer), &written, NULL);
			sprintf(buffer, "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n");
			WriteFile(hFile, buffer, strlen(buffer), &written, NULL);

			POSITION pos = pois.GetHeadPosition();
			while (pos != NULL) {
				CPoi *poi = pois.GetNext(pos);
				char *str = NULL;

				sprintf(buffer, "\t<Placemark>\n");
				WriteFile(hFile, buffer, strlen(buffer), &written, NULL);
				
				str = WCharToChar(poi->Name, CP_UTF8);
				sprintf(buffer, "\t\t<name>%s</name>\n", str);
				WriteFile(hFile, buffer, strlen(buffer), &written, NULL);
				delete [] str;

				sprintf(buffer, "\t\t<description>%s</description>\n", "geocache");
				WriteFile(hFile, buffer, strlen(buffer), &written, NULL);

				sprintf(buffer, "\t\t<Point>\n");
				WriteFile(hFile, buffer, strlen(buffer), &written, NULL);
				sprintf(buffer, "\t\t\t<coordinates>%lf,%lf</coordinates>\n", poi->Longitude, poi->Latitude);
				WriteFile(hFile, buffer, strlen(buffer), &written, NULL);
				sprintf(buffer, "\t\t</Point>\n");
				WriteFile(hFile, buffer, strlen(buffer), &written, NULL);
				sprintf(buffer, "\t</Placemark>\n");
				WriteFile(hFile, buffer, strlen(buffer), &written, NULL);
			}

			sprintf(buffer, "</kml>\n");
			WriteFile(hFile, buffer, strlen(buffer), &written, NULL);

			CloseHandle(hFile);

			AfxMessageBox(IDS_EXPORT_SUCCESSFUL);
		}
	}
}

void CPoiMan::OnSelectall() {
	m_ctlPoints.SelectAllItems();
}

void CPoiMan::OnSize(UINT nType, int cx, int cy) {
	CCeDialog::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(&rc);

/*	m_btnAdd.SetWindowPos(NULL, SCALEX(6), rc.bottom - SCALEY(25), SCALEX(21), SCALEY(19), SWP_NOZORDER);
	m_btnRemove.SetWindowPos(NULL, SCALEX(30), rc.bottom - SCALEY(25), SCALEX(21), SCALEY(19), SWP_NOZORDER);
	m_btnInfo.SetWindowPos(NULL, SCALEX(54), rc.bottom - SCALEY(25), SCALEX(30), SCALEY(19), SWP_NOZORDER);
	m_btnImport.SetWindowPos(NULL, SCALEX(87), rc.bottom - SCALEY(25), SCALEX(45), SCALEY(19), SWP_NOZORDER);
	m_btnSelect.SetWindowPos(NULL, SCALEX(169), rc.bottom - SCALEY(25), SCALEX(65), SCALEY(19), SWP_NOZORDER);
*/
	m_ctlPoints.SetWindowPos(NULL, 0, SCALEX(24), cx, cy - SCALEY(24), SWP_NOZORDER);
}

void CPoiMan::OnUpdateSort(CCmdUI *pCmdUI) {
	if (pCmdUI->m_pMenu != NULL)
		pCmdUI->m_pMenu->CheckMenuRadioItem(ID_POI_SORTBY_ASCENDING, ID_POI_SORTBY_DESCENDING, ID_POI_SORTBY_ASCENDING + SortType, MF_BYCOMMAND);
}

void CPoiMan::OnUpdateSortCol(CCmdUI *pCmdUI) {
	if (pCmdUI->m_pMenu != NULL)
		pCmdUI->m_pMenu->CheckMenuRadioItem(ID_POI_SORTBY_NAME, ID_POI_SORTBY_CACHETYPE, ID_POI_SORTBY_NAME + SortColumn, MF_BYCOMMAND);
}

void CPoiMan::OnUpdateMarkAs(CCmdUI *pCmdUI) {
	if (m_ctlPoints.GetSelectedCount() == 1) {
		int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
		if (nSelItem != -1) {
			CPoi *poi = (CPoi *) m_ctlPoints.GetItemData(nSelItem);
			if (pCmdUI->m_pMenu != NULL)
				pCmdUI->m_pMenu->CheckMenuRadioItem(ID_POI_MARKAS_NOTCOMPLETED, ID_POI_MARKAS_NOTFOUND, ID_POI_MARKAS_NOTCOMPLETED + poi->Status, MF_BYCOMMAND);
		}
	}
}

void CPoiMan::OnDownloadInfo() {
	CDownloadManagerDlg dlg;

	// gather selected POIs
	int nSelItem = m_ctlPoints.GetNextItem(-1, LVNI_SELECTED);
	while (nSelItem != -1) {
		CPoi *p = (CPoi *) m_ctlPoints.GetItemData(nSelItem);
		dlg.Pois.AddTail(p);
		nSelItem = m_ctlPoints.GetNextItem(nSelItem, LVNI_SELECTED);
	}

	if (dlg.Pois.GetCount() <= 0)
		return;

	dlg.DoModal();
}
