/**
 *  SetupPg.cpp : implementation file
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
#include "SetupPg.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupGeneralPg property page

IMPLEMENT_DYNCREATE(CSetupGeneralPg, CPropertyPage)

CSetupGeneralPg::CSetupGeneralPg() : CPropertyPage(CSetupGeneralPg::IDD)
{
	//{{AFX_DATA_INIT(CSetupGeneralPg)
	m_bPreventSuspend = Config.PreventSuspend;
	//}}AFX_DATA_INIT
}

CSetupGeneralPg::~CSetupGeneralPg()
{
}

void CSetupGeneralPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupGeneralPg)
	DDX_Control(pDX, IDC_COORDS, m_ctlCoords);
	DDX_Control(pDX, IDC_UNITS, m_ctlUnits);
	DDX_Control(pDX, IDC_C_COORDS, m_lblCoords);
	DDX_Control(pDX, IDC_C_SUSPEND_MODE, m_lblSuspendMode);
	DDX_Check(pDX, IDC_PREVENT_SUSPEND, m_bPreventSuspend);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupGeneralPg, CPropertyPage)
	//{{AFX_MSG_MAP(CSetupGeneralPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupGeneralPg message handlers

BOOL CSetupGeneralPg::OnInitDialog() {
	CPropertyPage::OnInitDialog();

	int i;

	// coords
	struct SItem {
		CString S;
		int Data;
		SItem(const CString &s, int data) {
			S = s;
			Data = data;
		}
		SItem(UINT nID, int data) {
			S.LoadString(nID);
			Data = data;
		}
	};
	
	SItem Items[] = {
		SItem(_T("DD.DDDDDD"), Degrees),
		SItem(_T("DD MM.MMMM"), DegMin),
		SItem(_T("DD MM SS.SS"), DegMinSec)
	};

	for (i = 0; i < countof(Items); i++) {
		int item = m_ctlCoords.AddString(Items[i].S);
		m_ctlCoords.SetItemData(item, Items[i].Data);

		if (Config.CoordType == Items[i].Data)
			m_ctlCoords.SetCurSel(item);
	}

	if (m_ctlCoords.GetCurSel() == CB_ERR)
		m_ctlCoords.SetCurSel(0);

	// units
	SItem Units[] = {
		SItem(IDS_METRIC, Metric),
		SItem(IDS_ENGLISH, English)
	};

	for (i = 0; i < countof(Units); i++) {
		int item = m_ctlUnits.AddString(Units[i].S);
		m_ctlUnits.SetItemData(item, Units[i].Data);

		if (Config.UnitsType == Units[i].Data)
			m_ctlUnits.SetCurSel(item);
	}

	if (m_ctlUnits.GetCurSel() == CB_ERR)
		m_ctlUnits.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSetupGeneralPg::OnApply() {
	CString s;

	Config.CoordType = (ECoordType) m_ctlCoords.GetItemData(m_ctlCoords.GetCurSel());
	Config.UnitsType = (EUnitsType) m_ctlUnits.GetItemData(m_ctlUnits.GetCurSel());
	Config.PreventSuspend = m_bPreventSuspend;

	return CPropertyPage::OnApply();
}
