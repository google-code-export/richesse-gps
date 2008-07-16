/**
 *  AboutPg.cpp
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
#include "AboutPg.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutPg property page

IMPLEMENT_DYNCREATE(CAboutPg, CCePropertyPage)

CAboutPg::CAboutPg() : CCePropertyPage(CAboutPg::IDD) {
	//{{AFX_DATA_INIT(CAboutPg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAboutPg::~CAboutPg() {
}

void CAboutPg::DoDataExchange(CDataExchange* pDX) {
	CCePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutPg)
	DDX_Control(pDX, IDC_HOMEPAGE_LINK, m_ctlHomepageLink);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutPg, CCePropertyPage)
	//{{AFX_MSG_MAP(CAboutPg)
	ON_BN_CLICKED(IDC_HOMEPAGE_LINK, OnHomepageLink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutPg message handlers

void CAboutPg::OnOK() {
	CCePropertyPage::OnOK();
}

BOOL CAboutPg::OnInitDialog() {
	CCePropertyPage::OnInitDialog();

	return TRUE;
}

void CAboutPg::OnHomepageLink() {
	SHELLEXECUTEINFO ei = { 0 };
	ei.cbSize = sizeof(ei);
	ei.fMask = SEE_MASK_NOCLOSEPROCESS;
	ei.lpFile = _T("iexplore.exe");
	CString strLink;
	m_ctlHomepageLink.GetWindowText(strLink);
	ei.lpParameters = strLink;
	ShellExecuteEx(&ei);
}

