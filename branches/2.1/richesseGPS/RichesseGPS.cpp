/**
 *  RichesseGPS.cpp : Defines the class behaviors for the application.
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

#include "StdAfx.h"
#include "RichesseGPS.h"
#include "Config.h"

#include "MainFrm.h"
#include "ctrls/CePropertySheet.h"
#include "AboutPg.h"
#include "LicensePg.h"
#include "CreditsPg.h"

#include "files/files.h"

#include <htmlctrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRichesseGPSApp

BEGIN_MESSAGE_MAP(CRichesseGPSApp, CWinApp)
	//{{AFX_MSG_MAP(CRichesseGPSApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichesseGPSApp construction

CRichesseGPSApp::CRichesseGPSApp()
	: CWinApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRichesseGPSApp object

CRichesseGPSApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRichesseGPSApp initialization

BOOL CRichesseGPSApp::InitInstance() {
	// Standard initialization
	HIDPI_InitScaling();

	//
	if (!InitHTMLControl(AfxGetInstanceHandle()))
		return 0;

	// Initialize the control
	INITCOMMONCONTROLSEX icce;
	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_DATE_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_UPDOWN_CLASS | ICC_TREEVIEW_CLASSES | ICC_TOOLTIP_CLASSES | ICC_TAB_CLASSES;
	InitCommonControlsEx(&icce);

	SHInitExtraControls();


	// Standard initialization
	Config.Load();
	//
	LoadPois();

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);


	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	return TRUE;
}

// App command to run the dialog
void CRichesseGPSApp::OnAppAbout() {
	CAboutPg pgAbout;
	CLicensePg pgLicense;
	CCreditsPg pgCredits;

	CCePropertySheet sheet(IDS_ABOUT);
	sheet.AddPage(&pgAbout);
	sheet.AddPage(&pgLicense);
	sheet.AddPage(&pgCredits);
	sheet.SetMenu(IDR_CANCEL);
	sheet.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CRichesseGPSApp message handlers



#define POI_DB_FILENAME						_T(".pois")

BOOL CRichesseGPSApp::LoadPois() {
	CString fileName;
	fileName.Format(_T("%s\\%s"), Config.InstallDir, POI_DB_FILENAME);

	return ReadRPFile(fileName, POIs);
}

BOOL CRichesseGPSApp::SavePois() {
	CString fileName;
	fileName.Format(_T("%s\\%s"), Config.InstallDir, POI_DB_FILENAME);

	return WriteRPFile(fileName, POIs);
}

int CRichesseGPSApp::ExitInstance() {
	SavePois();
	Config.Save();

	return CWinApp::ExitInstance();
}

