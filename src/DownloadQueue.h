// This file is part of the aMule Project
//
// Copyright (c) 2003-2004 aMule Project ( http://www.amule-project.net )
// Copyright (C) 2002 Merkur ( merkur-@users.sourceforge.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef DOWNLOADQUEUE_H
#define DOWNLOADQUEUE_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DownloadQueue.h"
#endif

#include "types.h"		// Needed for uint8, uint16, uint32 and uint64
#include "CTypedPtrList.h"
#include "CMD4Hash.h"
#include <deque>
#include <list>

class CSharedFileList;
class CSearchFile;
class CPartFile;
class CUpDownClient;
class CServer;
class CSafeMemFile;
class CKnownFile;
class CED2KLink;
class CED2KFileLink;
class CED2KServerLink;
class CED2KServerListLink;


struct Hostname_Entry {
		CMD4Hash fileid;
		wxString strHostname;
		uint16 port;
};

class CDownloadQueue
{
	friend class CServerSocket;
public:
	CDownloadQueue(CSharedFileList* in_sharedfilelist);
	~CDownloadQueue();
	void	Process();
	void	Init();
	void	AddSearchToDownload(CSearchFile* toadd, uint8 category);
	bool	IsFileExisting(const CMD4Hash& fileid);
	bool	IsPartFile(const CKnownFile* totest);
	CPartFile *GetFileByID(const CMD4Hash& filehash);
	CPartFile *GetFileByIndex(unsigned int idx);
	void    CheckAndAddSource(CPartFile* sender,CUpDownClient* source);
	void    CheckAndAddKnownSource(CPartFile* sender,CUpDownClient* source);
	bool	RemoveSource(CUpDownClient* toremove, bool updatewindow = true, bool bDoStatsUpdate = true); // delete later ->{ return RemoveSource(toremove,NULL,updatewindow);}
	void	DeleteAll();
	void	RemoveFile(CPartFile* toremove);
	float	GetKBps() const						{return datarate/1024.0;}
	void	SortByPriority();
	void	StopUDPRequests();
	CServer*	cur_udpserver;
	void	GetDownloadStats(uint32 results[]);
	void	AddDownload(CPartFile* newfile, bool paused, uint8 category);
	CUpDownClient* 	GetDownloadClientByIP(uint32 dwIP);
	void	StartNextFile();
	void	AddLinksFromFile();

	unsigned int GetFileCount()				{return filelist.size();}
	void	ResetCatParts(int cat);
	void	SavePartFiles(bool del = false);	// InterCeptor
	void	SetCatPrio(int cat, uint8 newprio);
	void	SetCatStatus(int cat, int newstatus);
	void	SendLocalSrcRequest(CPartFile* sender);
	void	RemoveLocalServerRequest(CPartFile* pFile);
	void	ResetLocalServerRequests();
	uint16	GetDownloadingFileCount();
	uint16	GetPausedFileCount();
	// Kry - HostNameSources
	void AddToResolve(const CMD4Hash& fileid, const wxString& pszHostname, uint16 port);
	bool OnHostnameResolved(uint32 ip);
	std::deque<Hostname_Entry*> m_toresolve;
	
	void SaveSourceSeeds();
	void LoadSourceSeeds();
	
	bool	AddED2KLink( const wxString& link, int category = 0 );
	bool	AddED2KLink( const CED2KLink* link, int category = 0 );
	bool	AddED2KLink( const CED2KFileLink* link, int category = 0 );
	bool	AddED2KLink( const CED2KServerLink* link );
	bool	AddED2KLink( const CED2KServerListLink* link );
	
private:
	void	CheckDiskspace( const wxString& path );
	bool	SendNextUDPPacket();
	void	ProcessLocalRequests();
	int	GetMaxFilesPerUDPServerPacket() const;
	bool	SendGlobGetSourcesUDPPacket(CSafeMemFile& data);

	void AddSearchToDownloadCommon(CPartFile *newfile, uint8 category);

	std::deque<CPartFile*> filelist;
	std::list<CPartFile*> m_localServerReqQueue;
	CSharedFileList* sharedfilelist;
	uint16	filesrdy;
	uint32	datarate;

	CPartFile*	lastfile;
	uint32		lastcheckdiskspacetime; // (Creteil) checkDiskspace
	uint32		lastudpsearchtime;
	uint32		lastudpstattime;
	uint32		m_nLastED2KLinkCheck;
	uint8		m_cRequestsSentToServer;
	uint32		m_dwNextTCPSrcReq;
	int		m_iSearchedServers;
	uint8		udcounter;

	bool do_not_sort_please;
};

#endif // DOWNLOADQUEUE_H
