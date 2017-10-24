#pragma once
#include "BetSite.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "..\global.h"

//#define SUB_THREAD_MODE
#ifdef SUB_THREAD_MODE
#define SUBTHREAD_COUNT	8
#endif

enum
{
	REQ_PlaceBetNewBwin = 0,
	REQ_BetSlipSave = 1,
};

struct ODD_INF
{
	UINT64 m_dwPriceID;
	UINT64 m_dwMarketID;
	char* m_szTitle;
	char* m_szPriceFormatted;
	char* m_szMarketDescription;

	ODD_INF()
	{
		m_szTitle = new char[MAX_TEAM_NAME * 3];
		m_szPriceFormatted = new char[0x20];
		m_szMarketDescription = new char[0x40];
		strcpy_s(m_szTitle, MAX_TEAM_NAME * 3, "");
		strcpy_s(m_szPriceFormatted, 0x20, "");
		strcpy_s(m_szMarketDescription, 0x40, "");
	}
	~ODD_INF()
	{
		delete m_szTitle;
		delete m_szPriceFormatted;
		delete m_szMarketDescription;
	};
};

class CBetSite_bwin :
	public CBetSite
{
public:
	CBetSite_bwin();
	~CBetSite_bwin();

public:
	int DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo);
	virtual int MainProc();
	virtual BOOL SendRequest(int nReqKind);
	virtual int SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind);

	virtual BOOL AnalyzeResData();
	virtual BOOL AnalyzeResBalance();
#ifdef SUB_THREAD_MODE
	BOOL AnalyzeResData1(char* szText);
#endif
	BOOL AnalyzeResLogOut();
	BOOL AnalyzeResLive();
	BOOL AnalyzeBetSlipSave();
	BOOL AnalyzePlaceBetNew();

	BOOL CheckUserName(TCHAR* buffer);

	void GetScore(char* score, int& hs, int& as);
	BOOL GetBalance(char* szAmount);
	void GetLoginResult(TCHAR* szStr);

#ifdef SUB_THREAD_MODE
	void CreateSubThread();
	DWORD SubThread(LPVOID lpParameter);
#endif

	virtual BOOL UpdateTree(int nIsExitThread);
	ODD_INF* GetOddInf(int nBetObjNo, COddsInfo* pOddsInfo, UINT64& dwOddID);

	char m_szAuthToken[0x40];
	TCHAR m_szUserInfo[0x100];
#ifdef SUB_THREAD_MODE
	HANDLE m_hCompletionPort;
#endif
	CMap<UINT64, UINT64&, ODD_INF*, ODD_INF*> m_mapOddInf;

	COddsInfo* MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, 
		UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3, UINT64 dwPriceID1, UINT64 dwPriceID2, UINT64 dwPriceID3, UINT64 dwMarketID1, UINT64 dwMarketID2, UINT64 dwMarketID3,
		char* szTitle, char* szMarket, char* price_fmt);
};
