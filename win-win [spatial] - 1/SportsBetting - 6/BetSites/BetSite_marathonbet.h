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
	virtual BOOL SendRequest(int nReqKind);
	virtual BOOL AnalyzeResData();
	virtual BOOL AnalyzeResBalance();
	BOOL AnalyzeResLive();
	BOOL AnalyzeResLogIn();
	BOOL AnalyzeResLogOut();

	BOOL GetPUIDVal(TCHAR* buffer);
	BOOL CheckCatLabel(int index, char* szValue);
	BOOL CheckLeagueLabel(int index, char* szValue);
	BOOL CheckGameID(int index, char* szValue);
	BOOL CheckEventLabel(int index, char* szValue);
	BOOL CheckScoreLabel(int index, char* szValue, int nGameCategory);
	BOOL CheckTimeLabel(int index, char* szValue);
	BOOL CheckLabel(int index, char* szLabel, char* szValue, char* szEndMark = "</");
	BOOL CheckLabel1(int index, char* szLabel, char* szValue, char* szEndMark = "</");

	void GetScore1(char* szValue, int nGameCategory, int& hs, int& hs1, int& hs2, int& as, int& as1, int& as2, string& stage);
	void GetTimeStage(char* szValue, int nGameCategory, int& ipt, string& stage);

	virtual BOOL UpdateTree(int nIsExitThread);
};

