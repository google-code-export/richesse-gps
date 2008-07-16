/**
 *  defs.h : Application-wide defines
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

#ifndef _DEFS_H_
#define _DEFS_H_

struct Pt {
	double X;
	double Y;
};

//
#define REG_APP									_T("Software\\DaProfik\\RichesseGPS")
#define REG_INSTALL_DIR							_T("InstallDir")

// user-defined messages

#define UWM_ERROR								(WM_APP + 0x0001)
#define UWM_UPDATE_MENU							(WM_APP + 0x0002)
#define UWM_ICON_NOTIFY							(WM_APP + 0x0003)

#endif