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

#include "stdafx.h"
#include "RichesseGPS.h"
#include "config.h"

#include "MainFrm.h"
#include "ctrls/cedialog.h"
#include "ctrls/linkctrl.h"

#include "files/files.h"

#include <htmlctrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CCeDialog {
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CLinkCtrl	m_ctlHomePage;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();		// Added for WCE apps
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CCeDialog(CAboutDlg::IDD) {
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	SetMenu(IDR_CANCEL);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_HOMEPAGE, m_ctlHomePage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichesseGPSApp commands
// Added for WCE apps

BOOL CAboutDlg::OnInitDialog()  {
	CCeDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


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
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
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

