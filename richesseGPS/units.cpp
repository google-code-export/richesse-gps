/**
 *  units.cpp
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
#include "units.h"
#include "Config.h"

void FormatAltitude(CString &s, double altitude) {
	switch (Config.UnitsType) {
		case English: s.Format(_T("%.1lf ft"), altitude); break;
		default: s.Format(_T("%.1lf m"), altitude * 3.2808399); break;
	}
}

void FormatSpeed(CString &s, double speed) {
	// speed is in knots
	switch (Config.UnitsType) {
		case English: s.Format(_T("%.1lf mph"), speed * 1.15077945); break;
		default: s.Format(_T("%.1lf kph"), speed * 1.85200); break;
	}
}

void FormatDistance(CString &s, double distance) {
	// distnace is in km
	switch (Config.UnitsType) {
		case English:
			if (distance < 1) s.Format(_T("%.0lf yards"), distance * 1093.6133);
			else s.Format(_T("%.2lf miles"), distance * 0.621371192);
			break;

		default:
			if (distance < 2) s.Format(_T("%.0lf m"), distance * 1000);
			else s.Format(_T("%.2lf km"), distance);
			break;
	}

}
