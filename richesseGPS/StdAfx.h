// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F7E5508F_040C_4A75_AE0E_E3A138F42A5F__INCLUDED_)
#define AFX_STDAFX_H__F7E5508F_040C_4A75_AE0E_E3A138F42A5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if (_WIN32_WCE <= 200)
#error : This project does not support MFCCE 2.00 or earlier, because it requires CControlBar, available only in MFCCE 2.01 or later
#endif


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#if defined(_WIN32_WCE) && (_WIN32_WCE >= 211) && (_AFXDLL)
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "../share/UIHelper.h"
#include "../share/shguim.h"

#include <afxtempl.h>


#define countof(a) (sizeof(a)/sizeof(a[0]))

#define PI								3.1415926535

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F7E5508F_040C_4A75_AE0E_E3A138F42A5F__INCLUDED_)
