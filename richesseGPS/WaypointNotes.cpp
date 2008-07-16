/**
 *  WaypointNotes.cpp : implementation file
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
#include "richessegps.h"
#include "WaypointNotes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaypointNotesPg property page

IMPLEMENT_DYNCREATE(CWaypointNotesPg, CPropertyPage)

CWaypointNotesPg::CWaypointNotesPg() : CPropertyPage(CWaypointNotesPg::IDD)
{
	//{{AFX_DATA_INIT(CWaypointNotesPg)
	m_strDescription = _T("");
	//}}AFX_DATA_INIT
}

CWaypointNotesPg::~CWaypointNotesPg()
{
}

void CWaypointNotesPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaypointNotesPg)
	DDX_Control(pDX, IDC_DESCRIPTION, m_ctlDescription);
	DDX_Text(pDX, IDC_DESCRIPTION, m_strDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaypointNotesPg, CPropertyPage)
	//{{AFX_MSG_MAP(CWaypointNotesPg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaypointNotesPg message handlers

void CWaypointNotesPg::OnSize(UINT nType, int cx, int cy) {
	CPropertyPage::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(rc);
	rc.DeflateRect(SCALEX(4), SCALEY(4), SCALEX(4), SCALEY(4));

	m_ctlDescription.SetWindowPos(NULL, 0, 0, rc.Width() - SCALEX(6), rc.Height() - SCALEX(4), SWP_NOZORDER | SWP_NOMOVE);
}

BOOL CWaypointNotesPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	return TRUE;
}

BOOL CWaypointNotesPg::OnSetActive() {
	CString sText;
	m_ctlDescription.GetWindowText(sText);
	m_ctlDescription.PostMessage(EM_SETSEL, sText.GetLength(), sText.GetLength());

	return CPropertyPage::OnSetActive();
}
