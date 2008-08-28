/*

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF

  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO

  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A

  PARTICULAR PURPOSE.



    This is "Sample Code" and is distributable subject to the terms of the end user license agreement.

*/

#include <afx.h>
#include "Connection.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const GUID IID_ConnPrv_IProxyExtension =
	{ 0xaf96b0bd, 0xa481, 0x482c, { 0xa0, 0x94, 0xa8, 0x44, 0x87, 0x67, 0xa0, 0xc0 } };

// {436EF144-B4FB-4863-A041-8F905A62C572}
const GUID IID_DestNetInternet =
	{ 0x436ef144, 0xb4fb, 0x4863, { 0xa0, 0x41, 0x8f, 0x90, 0x5a, 0x62, 0xc5, 0x72 } };

CConnection Connection;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnection::CConnection() {
	m_gNetwork = IID_DestNetInternet;
}

CConnection::~CConnection() {
	HangupConnection();
}

//
// Can we get to this resource 'quickly'
//
HRESULT CConnection::IsAvailable() {
	HRESULT hr;

	BOOL bAvailable = FALSE;
	HANDLE hConn = NULL;

	CONNMGR_CONNECTIONINFO ci = { 0 };
	ci.cbSize           = sizeof(ci);
	ci.dwParams         = CONNMGR_PARAM_GUIDDESTNET | CONNMGR_PARAM_MAXCONNLATENCY;
	ci.dwFlags          = CONNMGR_FLAG_PROXY_HTTP | CONNMGR_FLAG_PROXY_SOCKS4 | CONNMGR_FLAG_PROXY_SOCKS5;
	ci.ulMaxConnLatency = 4000;         // 4 second
	ci.bExclusive       = FALSE;
	ci.bDisabled        = TRUE;
	ci.dwPriority       = CONNMGR_PRIORITY_USERINTERACTIVE;
	ci.guidDestNet      = GetNetworkGuid();

	if (SUCCEEDED(hr = ConnMgrEstablishConnection(&ci, &hConn))) {
		DWORD dwResult = WaitForSingleObject(hConn, 400);

		DWORD dwStatus;
		switch (dwResult) {
			case WAIT_OBJECT_0:
				if (SUCCEEDED(ConnMgrConnectionStatus(hConn, &dwStatus)) &&
					((dwStatus == CONNMGR_STATUS_CONNECTED) || (dwStatus == CONNMGR_STATUS_CONNECTIONDISABLED)))
				{
					hr = S_OK;
					CheckForRequiredProxy(hConn);
				}
				else
					hr = S_FALSE;
				break;

			case WAIT_TIMEOUT:
				hr = E_FAIL;
				break;
		}
		ConnMgrReleaseConnection(hConn, FALSE);
	}
	return hr;
}

//
// Attempts to connect to the network
// overried the Do* methods to see what's happening.
//
HRESULT CConnection::AttemptConnect() {
	HRESULT hr;
	if (SUCCEEDED(hr = HangupConnection())) {
		CONNMGR_CONNECTIONINFO connInfo = { 0 };
		connInfo.cbSize = sizeof(connInfo);
		connInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET;
		connInfo.dwFlags = CONNMGR_FLAG_PROXY_HTTP | CONNMGR_FLAG_PROXY_SOCKS4 | CONNMGR_FLAG_PROXY_SOCKS5;
		connInfo.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;
		connInfo.guidDestNet = GetNetworkGuid();

		DWORD dwStatus;
		HRESULT hr = ConnMgrEstablishConnectionSync(&connInfo, &m_hConnection, 25000, &dwStatus); // wait for 25 seconds
		if (hr == S_OK) {
			if (dwStatus == CONNMGR_STATUS_CONNECTED)
				hr = S_OK;
			else
				hr = S_FALSE;
		}
		else {
			hr = S_FALSE;
		}
	}
	return hr;
}

HRESULT CConnection::HangupConnection() {
	// Release the connection, caching if we should.
	if (m_hConnection) {
		ConnMgrReleaseConnection(m_hConnection, FALSE);
		m_hConnection = NULL;
	}

	return S_OK;
}

HRESULT CConnection::GetNetworkFromPath(LPCTSTR pszPath) {
	if (pszPath == NULL)
		return E_FAIL;
	else
		return ConnMgrMapURL(pszPath, &m_gNetwork, 0);
}

BOOL CConnection::CheckForRequiredProxy(HANDLE hConn) {
	m_bProxyRequired = FALSE;

	ZeroMemory(&m_ProxyInfo, sizeof(m_ProxyInfo));
	m_ProxyInfo.dwType = CONNMGR_FLAG_PROXY_HTTP | CONNMGR_FLAG_PROXY_SOCKS4 | CONNMGR_FLAG_PROXY_SOCKS5;
	if (SUCCEEDED(ConnMgrProviderMessage(hConn, &IID_ConnPrv_IProxyExtension, NULL, 0, 0, (PBYTE) &m_ProxyInfo, sizeof(m_ProxyInfo)))) {
		m_bProxyRequired = TRUE;
	}
	return m_bProxyRequired;
}
