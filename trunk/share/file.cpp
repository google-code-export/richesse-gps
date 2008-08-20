/**
 *  file.cpp
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

#include <windows.h>
#include "file.h"

CBufferedFile::CBufferedFile(DWORD bufferSize/* = 8192*/) {
	File = NULL;

	BufferSize = bufferSize;
	Buffer = new BYTE [BufferSize];

	InitializeCriticalSection(&CS);
}

CBufferedFile::~CBufferedFile() {
	DeleteCriticalSection(&CS);

	if (File != NULL)
		this->Close();

	delete [] Buffer;
}

void CBufferedFile::Lock() {
	EnterCriticalSection(&CS);
}

void CBufferedFile::Unlock() {
	LeaveCriticalSection(&CS);
}

BOOL CBufferedFile::Create(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDispostion, DWORD dwFlagsAndAttributes) {
	File = CreateFile(lpFileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDispostion, dwFlagsAndAttributes, NULL);
	if (File == INVALID_HANDLE_VALUE)
		return FALSE;

	Access = dwDesiredAccess;
	EmptyCache();
	BufferStart = 0;

	return TRUE;
}

DWORD CBufferedFile::Seek(LONG lDistanceToMove, DWORD dwMoveMethod) {
	DWORD newPos = 0;
	switch (dwMoveMethod) {
		case FILE_CURRENT:
			newPos = BufferStart + BufferPos + lDistanceToMove;
			break;

		case FILE_BEGIN:
			newPos = lDistanceToMove;
			break;

		case FILE_END:
			newPos = GetFileSize(File, NULL) - lDistanceToMove;
			break;

		default:
			newPos = 0;
			break;
	}

	if (BufferStart <= newPos && newPos < BufferStart + BufferSize) {
		BufferPos = newPos - BufferStart;
	}
	else {
		EmptyCache();
		BufferStart = 0;

		SetFilePointer(File, newPos, NULL, FILE_BEGIN);
	}

	return newPos;
}

BOOL CBufferedFile::EmptyCache() {
	BufferPos = 0;
	EndOfFile = FALSE;
	BufferFilled = 0;

	return TRUE;
}

BOOL CBufferedFile::Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, DWORD *nBytesRead/* = NULL*/) {
	DWORD nRemain = nNumberOfBytesToRead;
	BYTE *lpDest = (BYTE *) lpBuffer;

	DWORD numberOfBytesRead = 0;

	BOOL ret = TRUE;
	while (nRemain > 0) {
		if (BufferPos + nRemain <= BufferFilled) {
			memcpy(lpDest, Buffer + BufferPos, nRemain);
			BufferPos += nRemain;
			numberOfBytesRead += nRemain;
			nRemain = 0;
		}
		else {
			int nToRead = BufferFilled - BufferPos;
			if (nToRead <= 0) {
				DWORD read;

				BufferStart = SetFilePointer(File, 0, NULL, FILE_CURRENT);
				ret = ReadFile(File, Buffer, BufferSize, &read, NULL);
				BufferFilled = read;
				BufferPos = 0;

				if (!ret || read == 0)	{
					// reading beyond the end of the file
					ret = FALSE;
					break;
				}
			}
			else {
				memcpy(lpDest, Buffer + BufferPos, nToRead);

				numberOfBytesRead += nToRead;
				nRemain -= nToRead;
				lpDest += nToRead;
				BufferPos += nToRead;
			}
		}
	}

	if (nBytesRead != NULL)
		*nBytesRead = numberOfBytesRead;

	return ret;
}

BOOL CBufferedFile::Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, DWORD *nNumberOfBytesWritten/* = NULL*/) {
	DWORD numberOfBytesWritten = 0;
	DWORD nRemain = nNumberOfBytesToWrite;
	BYTE *lpSrc = (BYTE *) lpBuffer;

	while (nRemain > 0) {
		if (BufferPos + nRemain < BufferSize) {
			memcpy(Buffer + BufferPos, lpSrc, nRemain);
			BufferPos += nRemain;
			numberOfBytesWritten += nRemain;
			nRemain = 0;
		}
		else {
			int nToWrite = BufferSize - BufferPos;
			memcpy(Buffer + BufferPos, lpSrc, nToWrite);
			BufferPos = 0;
			nRemain -= nToWrite;
			lpSrc += nToWrite;
			numberOfBytesWritten += nToWrite;

			BufferStart = SetFilePointer(File, 0, NULL, FILE_CURRENT);
			DWORD nWritten;
			WriteFile(File, Buffer, BufferSize, &nWritten, NULL);
		}
	}

	if (nNumberOfBytesWritten != NULL)
		*nNumberOfBytesWritten = numberOfBytesWritten;

	return TRUE;
}

BOOL CBufferedFile::Close() {
	// flush buffers
	if ((Access & GENERIC_WRITE) && Buffer != NULL && BufferPos > 0) {
		DWORD written;
		if (!WriteFile(File, Buffer, BufferPos, &written, NULL))
			return FALSE;
		BufferPos = 0;
	}

	BOOL ret = CloseHandle(File);
	File = NULL;

	return ret;
}

