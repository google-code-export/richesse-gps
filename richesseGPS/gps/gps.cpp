/**
 *  gps.cpp : implementation file
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
#include "..\config.h"
#include "gps.h"

//#define LOG_GPS

static double NmeaToDegrees(double coord) {
	// ddmm(degress).ssss(decimal) -> dd.dddd (whole decimal)

	int degrees = (int) coord / 100;					// degrees
	int mins = (int) coord - (degrees * 100);			// min
	double secs = (coord - ((int) coord));				// secs

	return degrees + (mins / 60.0) + (secs / 60.0);
}

// NMEA parsing
void CGPS::ParseGGA(char *buffer, int len) {
	// fields
	double utcTime;
	double latitude;
	char latDir;
	double longitude;
	char lonDir;
	int quality = 0;
	int satellitesInView;
	double dilution;
	double altitude;
	char altUnit;
	double geoSep;
	char geoSepUnit;

	sscanf(buffer, "%lf,%lf,%c,%lf,%c,%d,%d,%lf,%lf,%c,%lf,%c,",
		&utcTime, &latitude, &latDir, &longitude, &lonDir, &quality, &satellitesInView,
		&dilution, &altitude, &altUnit, &geoSep, &geoSepUnit);

	// to degress
	latitude = NmeaToDegrees(latitude);
	longitude = NmeaToDegrees(longitude);

	if (latDir == 'S') latitude = -latitude;
	if (lonDir == 'W') longitude = -longitude;

	Lock();
	if (quality > 0) {
		Latitude = latitude;
		Longitude = longitude;
		Altitude = altitude;
		SatellitesInView = satellitesInView;
		Quality = quality;
		HDOP = dilution;
	}
	Unlock();
}

void CGPS::ParseGSV(char *buffer, int len) {
	int totalMsg, msgNo, satellitesInView;
	char s[1024] = { 0 };
	sscanf(buffer, "%d,%d,%d,%s", &totalMsg, &msgNo, &satellitesInView, s);

	static gsaIdx = 0;

	if (msgNo == 1)
		gsaIdx = 0;

	char *c = s;
	char *st = s;
	int idx = 0;			// index to the array of values
	int val[4] = { 0 };		// array of values
	while (*c != '\0' && *c != '*') {
		if (*c == ',') {
			*c = '\0';		// terminate token
			if (st != NULL && *st != '\0')
				sscanf(st, "%d", &(val[idx]));
			else
				val[idx] = -1;
			st = c + 1;		// start of the following token

			idx = (idx + 1) % 4;

			if (idx == 0) {
				// got all info
				Satellites[gsaIdx].Id = val[0];
				Satellites[gsaIdx].Elevation = val[1];
				Satellites[gsaIdx].Azimuth = val[2];
				Satellites[gsaIdx].SNR = val[3];

				gsaIdx = (gsaIdx + 1) % 12;
			}
		}

		c++;
	}
}

void CGPS::ParseRMC(char *buffer, int len) {

	double utcTime;
	char status;
	double latitude;
	char latDir;
	double longitude;
	char lonDir;
	double speed;
	double track;
	int date;

	sscanf(buffer, "%lf,%c,%lf,%c,%lf,%c,%lf,%lf,%d", &utcTime, &status,
		&latitude, &latDir, &longitude, &lonDir, &speed, &track, &date);

	Lock();
	Speed = speed;
	Track = track;
	Unlock();
}

#ifdef LOG_GPS
HANDLE hLogFile;
#endif

void CGPS::ParseGSA(char *buffer, int len) {
	int mode;
	char sm;
	char s[1024] = { 0 };
	sscanf(buffer, "%c,%d,%s", &sm, &mode, s);

	char *c = s;
	char *st = s;
	int idx = 0;
	while (*c != '\0' && *c != '*' && idx < MAX_SATELLITES) {
		if (*c == ',') {
			*c = '\0';		// terminate token
			SatellitesForFix[idx] = -1;
			if (st != NULL && *st != '\0') {
				int v = -1;
				if (sscanf(st, "%d", &v) == 1)
					SatellitesForFix[idx] = v;
			}
			st = c + 1;		// start of the following token

			idx = idx + 1;
		}

		c++;
	}
}

void CGPS::ProcessNmeaLine(char *buffer, int len) {
	if (len < 7)
		return;

	if (buffer[0] != '$')
		return;		// incomplete line

	if (buffer[1] != 'G' &&
		buffer[2] != 'P')
		return;		// not a GPS

	if (strncmp(buffer + 3, "GGA", 3) == 0)
		ParseGGA(buffer + 7, len - 7);
	else if (strncmp(buffer + 3, "GSA", 3) == 0)
		ParseGSA(buffer + 7, len - 7);
	else if (strncmp(buffer + 3, "RMC", 3) == 0)
		ParseRMC(buffer + 7, len - 7);
	else if (strncmp(buffer + 3, "GSV", 3) == 0)
		ParseGSV(buffer + 7, len - 7);
}

CGPS::CGPS() {
	InitializeCriticalSection(&CSGpsInfo);
	HCommPort = NULL;
	HGpsThread = NULL;
	State = STATE_OK;
}

CGPS::~CGPS() {
	DeleteCriticalSection(&CSGpsInfo);
}

void CGPS::SetPort(const CString &port) {
	Port.Format(_T("%s:"), port);
}

void CGPS::SetBaudRate(int bauds) {
	BaudRate = bauds;
}

void CGPS::ReadSerialData(HANDLE hPort) {
	BYTE buffer[256] = { 0 };
	int idx = 0;

	while (State == STATE_OK) {
		DWORD dwBytesTransferred;
		BYTE ch;
		// Read data from the serial port
		dwBytesTransferred = 0;
		if (ReadFile(hPort, &ch, 1, &dwBytesTransferred, 0)) {
			// process the data
			if (dwBytesTransferred == 1) {
				buffer[idx] = ch;
				idx++;

				if (idx >= 2 && buffer[idx - 2] == '\r' && buffer[idx - 1] == '\n') {
#ifdef LOG_GPS
					DWORD written;
					WriteFile(hLogFile, buffer, idx, &written, NULL);
#endif

					// end of the row -> process it
					ProcessNmeaLine((char *) buffer, idx);
					idx = 0;
				}
			}
			else
				Sleep(250);
		}
		else {
			State = STATE_READ_ERROR;
#ifdef LOG_GPS
			DWORD written;
			WriteFile(hLogFile, "failed", 6, &written, NULL);
#endif
		}
	}
}

// background thread
DWORD WINAPI GpsThreadProc(LPVOID lpParam) {
	CGPS *gps = (CGPS *) lpParam;
	return gps->ReadThread();
}

DWORD CGPS::ReadThread() {
#ifdef LOG_GPS
	hLogFile = CreateFile(_T("\\richesse-gps.log"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif

	ReadSerialData(HCommPort);

	// TODO: reconnect

	//
	CloseHandle(HGpsThread);
	HGpsThread = NULL;


#ifdef LOG_GPS
	CloseHandle(hLogFile);
#endif

	return 0;
}

BOOL CGPS::On() {
	// open COM port
	HCommPort = CreateFile(Port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (HCommPort != INVALID_HANDLE_VALUE) {
		// init
		DCB dcb;
		dcb.DCBlength = sizeof(dcb);
		if (GetCommState(HCommPort, &dcb)) {
			dcb.BaudRate = BaudRate;
			dcb.ByteSize = 8;
			dcb.Parity = NOPARITY;
			dcb.StopBits = ONESTOPBIT;

			if (SetCommState(HCommPort, &dcb)) {
				// we are connected
				GpsOn = TRUE;
				State = STATE_OK;

				// start
				DWORD threadId;
				HGpsThread = CreateThread(NULL, 0, GpsThreadProc, this, 0, &threadId);

				if (Config.PreventSuspend)
					SuspendKiller.Start();
			}
			else {
				// SetCommMask failed
				State = STATE_CANNOT_INIT;
			}
		}
		else {
			// GetCommMask failed
			State = STATE_CANNOT_INIT;
		}
	}
	else {
		// Can not open serial port
		State = STATE_CANNOT_OPEN;
	}

	return State == STATE_OK;
}

void CGPS::Off() {
	CloseHandle(HCommPort);
	HCommPort = NULL;

	if (HGpsThread != NULL)
		WaitForSingleObject(HGpsThread, INFINITE);

	GpsOn = FALSE;
}

void CalcDistanceDir(double srcLat, double srcLong, double destLat, double destLong, double &distance, double &dir) {

		double lat1 = (PI / 180.0) * srcLat;
		double lon1 = (PI / 180.0) * srcLong;

		double lat2 = (PI / 180.0) * destLat;
		double lon2 = (PI / 180.0) * destLong;
//		LeaveCriticalSection(&CSGpsInfo);

		// distance
		double r = 6371; // earth's mean radius in km
		double dLat = lat2 - lat1;
		double dLon = fabs(lon2 - lon1);
		double dPhi = log(tan(lat2/2 + PI/4) / tan(lat1/2 + PI/4));
		double q;
		if (dPhi == 0)
			q = cos(lat1);
		else
			q = dLat / dPhi;
		// if dLon over 180° take shorter rhumb across 180° meridian:
		if (dLon > PI) dLon = 2 * PI - dLon;
		double d = sqrt(dLat * dLat + q * q * dLon * dLon); 
		distance = d * r;

		// direction
		dLon = lon2 - lon1;
//		dPhi = log(tan(lat2 / 2 + PI / 4) / tan(lat1 / 2 + PI / 4));
		if (fabs(dLon) > PI)
			dLon = dLon > 0 ? -(2 * PI - dLon) : (2 * PI + dLon);
		
		dir = atan2(dLon, dPhi);
		if (dir < 0)
			dir = 2 * PI + dir;
}