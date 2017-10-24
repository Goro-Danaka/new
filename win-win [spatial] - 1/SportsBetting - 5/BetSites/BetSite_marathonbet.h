#pragma once
#include "BetSite.h"

class CBetSite_marathonbet : public CBetSite
{
public:
	CBetSite_marathonbet();
	~CBetSite_marathonbet();
public:
	TCHAR m_puid[0x80];

public:
	virtual int MainProc();
	virtual BOOL SendRequestEx(int nReqKind);
	virtual BOOL AnalyzeResData();
	virtual BOOL AnalyzeResBalance();
	BOOL AnalyzeResLive();
	BOOL AnalyzeResLogIn();
	BOOL AnalyzeResLogOut();

	BOOL GetPUIDVal(TCHAR* buffer);

};

