/**
 *  SetupGpsPg.cpp : implementation file
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
#include "SetupGpsPg.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int baudRates[] = {
	4800, 9600, 14400, 19200, 38400, 11500, 57600
};

/////////////////////////////////////////////////////////////////////////////
// CSetupGpsPg property page

IMPLEMENT_DYNCREATE(CSetupGpsPg, CPropertyPage)

CSetupGpsPg::CSetupGpsPg() : CPropertyPage(CSetupGpsPg::IDD)
{
	//{{AFX_DATA_INIT(CSetupGpsPg)
	m_nPort = -1;
	m_strBaudRate = _T("");
	m_bAutoConnect = Config.AutoConnectOnStartup;
	m_bReconnect = Config.ReconnectAfterLost;
	//}}AFX_DATA_INIT
}

CSetupGpsPg::~CSetupGpsPg()
{
}

void CSetupGpsPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupGpsPg)
	DDX_Check(pDX, IDC_AUTO_CONNECT, m_bAutoConnect);
	DDX_Control(pDX, IDC_BAUD_RATE, m_ctlBaudRate);
	DDX_Control(pDX, IDC_PORT, m_ctlPort);
	DDX_CBIndex(pDX, IDC_PORT, m_nPort);
	DDX_CBString(pDX, IDC_BAUD_RATE, m_strBaudRate);
	DDX_Check(pDX, IDC_RECONNECT, m_bReconnect);
	DDX_Control(pDX, IDC_C_GPS_CONN, m_ctlGpsConn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupGpsPg, CPropertyPage)
	//{{AFX_MSG_MAP(CSetupGpsPg)
	ON_BN_CLICKED(IDC_AUTODETECT, OnAutodetect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupGpsPg message handlers

BOOL CSetupGpsPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	CString s;
	
	// insert com ports
	for (int i = 0; i <= 12; i++) {
		s.Format(_T("COM%d"), i);
		m_ctlPort.AddString(s);
	}
	
	m_ctlPort.SelectString(-1, Config.Port);

	// baud rate
	for (i = 0; i < countof(baudRates); i++) {
		s.Format(_T("%d"), baudRates[i]);
		m_ctlBaudRate.AddString(s);
	}

	s.Format(_T("%d"), Config.BaudRate);
	m_ctlBaudRate.SelectString(-1, s);

	return TRUE;
}

BOOL CSetupGpsPg::OnApply() {
	CString s;
	
	m_ctlPort.GetWindowText(s);
	Config.Port = s;

	m_ctlBaudRate.GetWindowText(s);
	int rate;
	if (swscanf(s, _T("%d"), &rate) == 1) {
		Config.BaudRate = rate;
	}

	Config.AutoConnectOnStartup = m_bAutoConnect;
	Config.ReconnectAfterLost = m_bReconnect;

	return CPropertyPage::OnApply();
}

void CSetupGpsPg::OnAutodetect() {
	AfxMessageBox(_T("Not yet implemented"));
}

