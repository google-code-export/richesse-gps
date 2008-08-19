/**
 *  DownloadManagerDlg.cpp : implementation file
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
#include "DownloadManagerDlg.h"
#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD WINAPI DownManThreadProc(LPVOID lpParam) {
	CDownloadManagerDlg *dlg = (CDownloadManagerDlg *) lpParam;
	dlg->ThreadProc();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadManagerDlg dialog


CDownloadManagerDlg::CDownloadManagerDlg(CWnd* pParent /*=NULL*/)
	: CCeDialog(CDownloadManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDownloadManagerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	HThread = NULL;
	Terminated = FALSE;

	SetMenu(IDR_CANCEL);
}


void CDownloadManagerDlg::DoDataExchange(CDataExchange* pDX) {
	CCeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownloadManagerDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_ITEMS, m_ctlItems);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDownloadManagerDlg, CCeDialog)
	//{{AFX_MSG_MAP(CDownloadManagerDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownloadManagerDlg message handlers

BOOL CDownloadManagerDlg::OnInitDialog() {
	CCeDialog::OnInitDialog();

	CRect rc;
	m_ctlItems.GetClientRect(rc);
	rc.right -= ::GetSystemMetrics(SM_CXVSCROLL);

	m_ctlItems.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_ctlItems.InsertColumn(0, _T("Item"), LVCFMT_LEFT, rc.Width() - SCALEX(60));
	m_ctlItems.InsertColumn(1, _T("Status"), LVCFMT_LEFT, SCALEX(60));

	// insert items
	int i = 0;
	POSITION pos = Pois.GetHeadPosition();
	while (pos != NULL) {
		CPoi *poi = Pois.GetNext(pos);

		CString s;
		s.Format(_T("%s: %s"), poi->Id, poi->Name);
		int item = m_ctlItems.InsertItem(i++, s);
		m_ctlItems.SetItemText(item, 1, _T("waiting"));
	}

	DWORD pid;
	HThread = CreateThread(NULL, 0, DownManThreadProc, this, 0, &pid);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDownloadManagerDlg::OnCancel() {
	Terminated = TRUE;

	CCeDialog::OnCancel();
}

BOOL PeekAndPump() {
	static MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!AfxGetApp()->PumpMessage()) {
			::PostQuitMessage(0);
			return FALSE;
		}
	}

	return TRUE;
}

DWORD CDownloadManagerDlg::DownloadFile(const CString &url, const CString &strFileName) {
	DWORD ret = 0;

	CInternetSession session(_T("RichesseGPS Download Agent"), 1, PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, 0);

	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 5);
	session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);

	DWORD serviceType;
	CString hostName, object;
	INTERNET_PORT port;
	AfxParseURL(url, serviceType, hostName, object, port);

	CHttpConnection *conn = NULL;
	CHttpFile *file = NULL;
	DWORD dwRet;

	conn = session.GetHttpConnection(hostName, port);
	if (conn != NULL) {
		file = conn->OpenRequest(CHttpConnection::HTTP_VERB_GET, object);

		if (file != NULL) {
			file->SendRequest();
			file->QueryInfoStatusCode(dwRet);

			if (dwRet == HTTP_STATUS_OK) {
				char buf[256];
				int numRead;

				m_ctlProgress.SetPos(0);

				CFile myfile(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
				while ((numRead = file->Read(buf, sizeof(buf) - 1 )) > 0) {
					buf[numRead] = '\0';
					myfile.Write(buf, numRead);

					m_ctlProgress.StepIt();

					PeekAndPump();
				}
				m_ctlProgress.SetPos(100);

				myfile.Close();

				ret = 1;
			}

			file->Close();
			delete file;
		}

		conn->Close();
		delete conn;

	}

	session.Close();

	return ret;
}

void CDownloadManagerDlg::ThreadProc() {
	CString strCacheDir;
	strCacheDir.Format(_T("%s\\cache"), Config.InstallDir);
	CreateDirectory(strCacheDir, NULL);

	m_ctlProgress.SetRange(0, 100);
	m_ctlProgress.SetStep(1);

	int i = 0;
	POSITION pos = Pois.GetHeadPosition();
	while (pos != NULL && !Terminated) {
		CPoi *poi = Pois.GetNext(pos);

		m_ctlItems.SetItemText(i, 1, _T("downloading"));

		CString filePath;
		filePath.Format(_T("%s\\%s.html"), strCacheDir, poi->Id);

		CString url;
		url.Format(Config.GCUrl, poi->Id);
		DWORD ret = DownloadFile(url, filePath);

		if (ret == 0)
			m_ctlItems.SetItemText(i, 1, _T("failed"));
		else
			m_ctlItems.SetItemText(i, 1, _T("ok"));

		i++;
	}

	m_ctlProgress.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
	CRect rc;
	GetClientRect(rc);
	m_ctlItems.SetWindowPos(NULL, 0, SCALEX(24), rc.Width(), rc.Height() - SCALEY(24), SWP_NOZORDER);

	CloseHandle(HThread);
	HThread = NULL;
	UpdateMenu();
}

void CDownloadManagerDlg::OnSize(UINT nType, int cx, int cy) {
	CCeDialog::OnSize(nType, cx, cy);

	m_ctlItems.SetWindowPos(NULL, 0, SCALEX(24), cx, cy - SCALEY(24) - SCALEY(20), SWP_NOZORDER);
	m_ctlProgress.SetWindowPos(NULL, 0, cy - SCALEX(20), cx, SCALEY(20), SWP_NOZORDER);
}

void CDownloadManagerDlg::UpdateMenu() {
	TBBUTTONINFO tbbi;
	tbbi.cbSize = sizeof(tbbi);
	tbbi.dwMask = TBIF_TEXT;
	if (HThread == NULL)
		tbbi.pszText = _T("Done");
	else
		tbbi.pszText = _T("Cancel");
	::SendMessage(m_hwndCmdBar, TB_SETBUTTONINFO, (WPARAM) IDCANCEL, (LPARAM) &tbbi);
}
