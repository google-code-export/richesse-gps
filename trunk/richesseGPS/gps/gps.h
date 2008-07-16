/**
 *  gps.h
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

#ifndef _GPS_H_INCLUDED_
#define _GPS_H_INCLUDED_

#define STATE_OK						0
#define STATE_TERMINATE					1
#define STATE_READ_ERROR				2
#define STATE_CANNOT_OPEN				3
#define STATE_CANNOT_INIT				4

#include "..\misc\suspendkiller.h"

#define MAX_SATELLITES					12

struct CSatellitePos {
	int Id;
	int Elevation;
	int Azimuth;
	int SNR;
};


class CGPS {
public:
	CGPS();
	~CGPS();

	BOOL On();
	void Off();

	void SetPort(const CString &port);
	void SetBaudRate(int bauds);

	void Lock() { EnterCriticalSection(&CSGpsInfo); }
	void Unlock() { LeaveCriticalSection(&CSGpsInfo); }

	BOOL IsRunning() { return HCommPort != NULL; }

// data
	BOOL GpsOn;

	// gps info
	double Latitude;		// in degress
	double Longitude;		// in degress
	double Altitude;
	int SatellitesInView;
	int Quality;			// 0 - fix not available, 1 - GPS fix, 2 - Differential GPS fix
	double HDOP;			// Horizontal Dilution of Precision (HDOP)

	//
	double Speed;
	double Track;			// azimuth? (in degress)

	CSatellitePos Satellites[MAX_SATELLITES];
	int SatellitesForFix[MAX_SATELLITES];

protected:
	DWORD ReadThread();
	friend DWORD WINAPI GpsThreadProc(LPVOID lpParam);

	void ReadSerialData(HANDLE hPort);
	
	HANDLE HCommPort;
	int State;
	HANDLE HGpsThread;
	CRITICAL_SECTION CSGpsInfo;

	// port settings
	CString Port;
	int BaudRate;

	CSuspendKiller SuspendKiller;

	void ParseGGA(char *buffer, int len);
	void ParseGSV(char *buffer, int len);
	void ParseRMC(char *buffer, int len);
	void ParseGSA(char *buffer, int len);
	void ProcessNmeaLine(char *buffer, int len);
};


void CalcDistanceDir(double srcLat, double srcLong, double destLat, double destLong, double &distance, double &dir);


#endif