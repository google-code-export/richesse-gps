/**
 *  CeStatic.cpp : implementation file
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

#include "..\stdafx.h"
#include "..\RichesseGPS.h"
#include "CeStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCeStatic

CCeStatic::CCeStatic() {
}

CCeStatic::~CCeStatic() {
}


BEGIN_MESSAGE_MAP(CCeStatic, CStatic)
	//{{AFX_MSG_MAP(CCeStatic)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCeStatic message handlers


void CCeStatic::OnLButtonDown(UINT nFlags, CPoint point) {
//	Default();
    SHRGINFO shrgi = {0};

    shrgi.cbSize        = sizeof(SHRGINFO);
    shrgi.hwndClient    = m_hWnd;
    shrgi.ptDown.x      = point.x;
    shrgi.ptDown.y      = point.y;
    shrgi.dwFlags       = SHRG_NOTIFYPARENT;

    if (::SHRecognizeGesture(&shrgi) == 0)
        Default();
}
