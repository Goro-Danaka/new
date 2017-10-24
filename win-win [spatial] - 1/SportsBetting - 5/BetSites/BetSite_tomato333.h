#pragma once
#include "BetSite.h"
#include "..\global.h"

class CBetSite_tomato333 :
	public CBetSite
{
public:
	CBetSite_tomato333();
	~CBetSite_tomato333();

	TCHAR m_szOdnb[0x100];

public:
	virtual int MainProc();
	virtual BOOL SendRequestEx(int nReqKind);
	virtual BOOL AnalyzeResData();
	virtual BOOL AnalyzeResBalance();
	void AnalyzeResLogIn();
	void AnalyzeResLogOut();

	void GetSessionVal(TCHAR* buffer);
	void GetOdnbVal(TCHAR* buffer);
	void GetRedirectUrl(TCHAR* buffer);

	char m_szCategory[GC_COUNT][0x40];
};

