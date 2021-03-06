/*
FTP File YM plugin
Copyright (C) 2007-2010 Jan Holub

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "stdafx.h"
#include "job_generic.h"
#include "../../libs/zlib/src/zip.h"

class PackerJob : public GenericJob
{
private:
	static const int MAX_RUNNING_JOBS = 1;

	UINT64 m_uiFileSize;
	UINT64 m_uiReaded;
	time_t m_startTS;
	time_t m_lastUpdateTick;

	static Event jobDone;
	static mir_cs mutexJobCount;
	static int iRunningJobCount;

	static void __cdecl waitingThread(PackerJob *job);
	static uLong getFileTime(wchar_t *file, tm_zip *tmzip, uLong *dt);

	void pack(); 
	void getZipFilePath();
	int createZipFile();
	void updateStats();
	bool isCanceled();

public:
	PackerJob(MCONTACT hContact, int m_iFtpNum, EMode mode);
	virtual ~PackerJob() {};

	virtual void start();
	virtual void pause();
	virtual void resume();
	virtual void cancel();
	virtual void addToUploadDlg();
	virtual void pauseHandler();

	virtual void refreshTab(bool bTabChanged);
	virtual void closeTab();
	virtual void closeAllTabs();
	virtual void createToolTip();
};
