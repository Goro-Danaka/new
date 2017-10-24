#pragma once
#include "BetSite.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

enum
{
	REQ_ORDER_SELECT = 100,
	REQ_ORDER_BET = 101,
	REQ_BALANCE_BB,
	REQ_BET_BALANCE_BB,
};

class CBetSite_bet888vip : public CBetSite
{
public:
	CBetSite_bet888vip();
	~CBetSite_bet888vip();

public:
	int DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo);
	virtual int MainProc();
	virtual BOOL SendRequest(int nReqKind);
	virtual int SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind);

	virtual BOOL AnalyzeResData();
	virtual BOOL AnalyzeResBalance();
	BOOL AnalyzeResLogIn();
	BOOL AnalyzeResLogOut();
	BOOL AnalyzeResLive();
	BOOL AnalyzeResLive1();
	BOOL AnalyzeResLive2();

	int AnalyzeOrderSelect();
	int AnalyzeOrderBet();
	BOOL AnalyzeBalanceBB();
	BOOL AnalyzeBetBalanceBB();

	void DisplayCaptcha(BYTE* buf, int len, HANDLE hEvent);

	virtual BOOL UpdateTree(int nIsExitThread);
	virtual int GetGameCategory(char* szGameCategory);
	string MyGetGameCategory(int nGameCategory);

	BOOL GetBBSESSID(TCHAR* buffer);

	TCHAR m_szIOIPL[0x100];
	TCHAR m_szIBCACHE[0x100];
	TCHAR m_szBBSESSID[0x100];

	char m_SS[0x100];
	char m_SR[0x100];
	char m_TS[0x100];
	char m_SID[0x100];

	char m_unique[0x100];
	char m_data_pool[0x100];
	char m_verify[0x100];
	char m_secret[0x100];
	char m_ratio[0x100];
	char m_concede[0x100];

	char m_order_auth[0x100];
	char m_ioratio[0x100];
	char m_strong[0x100];

	char m_szOddID[8];

	UINT64 GetOddIntID(char* szOddID);
	void GetOddIntStr(UINT64 nOddID, char* szOddID);
	void GetOddID(int nBetObjNo, COddsInfo* pOddsInfo, char* szOddID);
	COddsInfo* MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, char* szOddID1, char* szOddID2, char* szOddID3);
};
