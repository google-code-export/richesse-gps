/**
 *  binfile.h
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

#ifndef _BINFILE_H_INCLUDED_
#define _BINFILE_H_INCLUDED_

#include "../../share/file.h"
#include "../../share/helpers.h"

struct CArchiveFileChunkHeader {
	char Id[4];
	DWORD Size;

	BOOL Load(CBufferedFile &file) {
		if (!file.Read(Id, 4))
			return FALSE;
		if (!file.Read(&Size, 4))
			return FALSE;

		return TRUE;
	}

	BOOL Save(CBufferedFile &file) {
		if (!file.Write(Id, 4))
			return FALSE;
		return file.Write(&Size, sizeof(DWORD));
	}
};

struct CArchiveFileChunk {
	CArchiveFileChunkHeader Header;
	char *Data;
	CList<CArchiveFileChunk *, CArchiveFileChunk*> Childs;

	CArchiveFileChunk(char id[]) {
//		assert(strlen(id) == 4);
		strncpy(Header.Id, id, 4);
		Header.Size = 0;

		Data = NULL;
	}

	void Add(CArchiveFileChunk *chunk) {
//		LOG1(5, "CArchiveFileChunk::Add(%p) - chunk", chunk);

		Childs.AddTail(chunk);
	}

	void Add(char id[], const CString &str) {
//		LOG2(5, "CArchiveFileChunk::Add('%s', '%S')", id, str);

		CArchiveFileChunk *chunk = new CArchiveFileChunk(id);
		chunk->Data = WCharToChar(str, CP_UTF8);
		chunk->Header.Size = strlen(chunk->Data);

		Add(chunk);
	}

	void Add(char id[], int number) {
//		LOG2(5, "CArchiveFileChunk::Add('%s', %d)", id, number);

		CArchiveFileChunk *chunk = new CArchiveFileChunk(id);
		chunk->Data = new char [sizeof(int)];
		memcpy(chunk->Data + 0, &number, sizeof(number));
		chunk->Header.Size = sizeof(int);
		Add(chunk);
	}

	void Add(char id[], double number) {
//		LOG2(5, "CArchiveFileChunk::Add('%s', %d)", id, number);

		CArchiveFileChunk *chunk = new CArchiveFileChunk(id);
		chunk->Data = new char [sizeof(double)];
		memcpy(chunk->Data + 0, &number, sizeof(number));
		chunk->Header.Size = sizeof(double);
		Add(chunk);
	}


	void Add(char id[], SYSTEMTIME &st) {
//		LOG1(5, "CArchiveFileChunk::Add('%s', ) - systime", id);

		CArchiveFileChunk *chunk = new CArchiveFileChunk(id);

		FILETIME ft;
		SystemTimeToFileTime(&st, &ft);

		chunk->Data = new char [sizeof(DWORD) * 2];
		memcpy(chunk->Data + 0, &ft.dwHighDateTime, sizeof(DWORD));
		memcpy(chunk->Data + 4, &ft.dwLowDateTime, sizeof(DWORD));
		chunk->Header.Size = sizeof(DWORD) * 2;

		Add(chunk);
	}

	~CArchiveFileChunk() {
		delete [] Data;
		while (!Childs.IsEmpty())
			delete Childs.RemoveHead();
	}

	void CalcSize() {
//		LOG0(5, "CArchiveFileChunk::CalcSize()");
		
		if (Childs.GetCount() > 0) {
			Header.Size = 0;
			POSITION pos = Childs.GetHeadPosition();
			while (pos != NULL) {
				CArchiveFileChunk *fc = Childs.GetNext(pos);
				fc->CalcSize();
				Header.Size += fc->Header.Size + 8;
			}
		}
	}

	void Save(CBufferedFile &file) {
//		LOG0(5, "CArchiveFileChunk::Save()");

		Header.Save(file);
		if (Childs.GetCount() > 0) {
			POSITION pos = Childs.GetHeadPosition();
			while (pos != NULL) {
				CArchiveFileChunk *fc = Childs.GetNext(pos);
				fc->Save(file);
			}
		}
		else {
			if (Data != NULL)
				file.Write(Data, Header.Size);
		}
	}
};

BOOL ReadString(CBufferedFile &file, int len, CString &str);
BOOL ReadNumber(CBufferedFile &file, int &number);
BOOL ReadNumber(CBufferedFile &file, DWORD &number);
BOOL ReadNumber(CBufferedFile &file, double &number);
BOOL ReadSystemTime(CBufferedFile &file, SYSTEMTIME &st);


#endif