#pragma once
#include "BetSite.h"

class CBetSite_bet365 :	public CBetSite
{
public:
	CBetSite_bet365();
	~CBetSite_bet365();

public:
	virtual int MainProc();
	virtual BOOL SendRequest(int nReqKind);

	virtual BOOL AnalyzeResData();
	virtual BOOL AnalyzeResBalance();
	BOOL AnalyzeResLogIn();
	BOOL AnalyzeResLogOut();
	BOOL AnalyzeResLive();
	BOOL AnalyzeResLive1();
	BOOL AnalyzeResLive2();
	BOOL AnalyzeResLive4();
	BOOL AnalyzeResLive5();

	BOOL GetPstk(TCHAR* buffer);
	BOOL GetRmbs(TCHAR* buffer);

	BOOL GetUrls(char* szText);

	void GetPremlpDataPacket(int nPacketNoPremlp, char* strFormData);

public:
	int m_nPacketNoPremlp;
	int m_nPacketNoPshudlp;

	TCHAR m_wszPstk[0x40];
	char m_szPstk[0x40];
	TCHAR m_wszAps[0x40];
	TCHAR m_wszRmbs[0x40];

	TCHAR m_wszUrlPremlp[0x40];
	TCHAR m_wszUrlPshudlp[0x40];

	TCHAR m_wszPremlpID[0x40];
	TCHAR m_wszPshudlpID[0x40];
	char m_szPremlpID[0x40];
	char m_szPshudlpID[0x40];
};

