/**
 *  setup.cpp : Defines the entry point for the DLL application.
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
#include <projects.h>
#include <ce_setup.h>
#include "..\share\defs.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

codeINSTALL_INIT
Install_Init(HWND hwndParent, BOOL fFirstCall, BOOL fPreviouslyInstalled, LPCTSTR pszInstallDir) {
	return codeINSTALL_INIT_CONTINUE;
}

codeINSTALL_EXIT
Install_Exit(HWND hwndParent, LPCTSTR pszInstallDir, WORD cFailedDirs, WORD cFailedFiles,
	WORD cFailedRegKeys, WORD cFailedRegVals, WORD cFailedShortcuts) {

	HKEY hApp;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, REG_APP, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hApp, &dwDisposition) != ERROR_SUCCESS)
		return codeINSTALL_EXIT_UNINSTALL;

	// write install path
	if (RegSetValueEx(hApp, REG_INSTALL_DIR, 0, REG_SZ, (LPBYTE) pszInstallDir, sizeof(TCHAR) * (lstrlen(pszInstallDir) + 1)) != ERROR_SUCCESS) {
		RegCloseKey(hApp);
		return codeINSTALL_EXIT_UNINSTALL;
	}

	RegCloseKey(hApp);

	return codeINSTALL_EXIT_DONE;
}

codeUNINSTALL_INIT
Uninstall_Init(HWND hwndParent, LPCTSTR pszInstallDir) {
	RegDeleteKey(HKEY_CURRENT_USER, REG_APP);

	return codeUNINSTALL_INIT_CONTINUE;
}

codeUNINSTALL_EXIT
Uninstall_Exit(HWND hwndParent) {

	return codeUNINSTALL_EXIT_DONE;
}
