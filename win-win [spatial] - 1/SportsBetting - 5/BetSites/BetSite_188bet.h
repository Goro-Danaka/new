#pragma once

#include "BetSite.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

enum
{
	REQ_GetBySelectionId = 0,
	REQ_PlaceBetNew = 1,
	REQ_GetRemainSelectionId,
};

class CGameData;
class CBetSite_188bet :	public CBetSite
{
public:
	CBetSite_188bet();
	~CBetSite_188bet();

	//CMap<int, int, CGameData*, CGameData*> m_mapGameID;
	TCHAR m_szCache_sb188[0x100];
	TCHAR m_szCache_a188[0x100];
	TCHAR m_szSession_sb188[0x100];
	TCHAR m_szSession_fptoken[0x40];
	TCHAR m_szSession_fptoken_val[0x100];
	TCHAR m_szReferer[0x200];

	TCHAR m_szRedirectUrl[0x100];
	TCHAR m_szPassport[0x100];

	int m_cid;
	char m_did[0x100];

	UINT64 m_dwOddID;
	TCHAR m_szHVal[0x10];
	int m_hs;
	int m_as;

public:
	int DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo);
	virtual int MainProc();
	virtual BOOL SendRequest(int nReqKind);
	virtual int SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind);

	virtual BOOL AnalyzeResData();
	virtual BOOL AnalyzeResBalance();
	BOOL AnalyzeResLogIn();
	BOOL AnalyzeResLogOut();
	BOOL AnalyzeResLive2();
	BOOL AnalyzeResLive4();

	BOOL AnalyzeBetGetBySelectionId();
	BOOL AnalyzeBetGetRemainSelectionId();
	int AnalyzeBetPlaceBetNew();

	virtual int GetGameCategory(char* szGameCategory);
	virtual CString GetGameCategory(int nGameCategory);

	virtual BOOL UpdateTree(int nIsExitThread);

	void ArrayProc(int nKind, CGameData* pGameData, rapidjson::Value& val);
	BOOL GetSessionVal_a188(TCHAR* buffer);
	BOOL GetSessionVal_sb188(TCHAR* buffer);
	BOOL GetSessionVal_fptoken(TCHAR* buffer);


	UINT64 GetOddID(int nBetObjNo, COddsInfo* pOddsInfo);
	string GetHVal(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo);
	COddsInfo* MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3);
};
