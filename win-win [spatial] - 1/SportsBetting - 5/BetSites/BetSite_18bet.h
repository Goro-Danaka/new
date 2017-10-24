#pragma once
#include "BetSite.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

enum
{
	REQ_PLACE_BET = 0,
	REQ_UPDATE_BETSLIP = 1,
	REQ_GET_ODD,
	REQ_REMOVE_ODD,
	REQ_BET_BALANCE,
};

class CBetSite_18bet :
	public CBetSite
{
public:
	CBetSite_18bet();
	~CBetSite_18bet();

	TCHAR m_szSV[0x100];
	UINT64 m_dwOddID;

public:
	int DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo);
	virtual int MainProc();
	virtual BOOL SendRequest(int nReqKind);
	virtual int SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind);

	virtual BOOL AnalyzeResData();
	virtual BOOL AnalyzeResBalance();
	BOOL AnalyzeResLogIn();
	BOOL AnalyzeResLogOut();

	int AnalyzeBetREQ_GET_ODD();
	int AnalyzeBetREQ_UPDATE_BETSLIP(BetInfo* pBetInfo, int nBetObjNo, float& coeff);
	int AnalyzeBetREQ_PLACE_BET();
	BOOL AnalyzeBetREQ_REMOVE_ODD();
	BOOL AnalyzeBetResBalance();

	BOOL GetDELASESS(TCHAR* buffer);
	BOOL GetSV(TCHAR* buffer);

	virtual int GetGameCategory(char* szGameCategory);
	virtual CString GetGameCategory(int nGameCategory);

	virtual BOOL UpdateTree(int nIsExitThread);

	void GetOddVals(int nGameCategory, CGameData* pGameData, rapidjson::Value& info, char* szItem);
	void GetBalance(char* szAmount);

	UINT64 GetOddID(int nBetObjNo, COddsInfo* pOddsInfo);
	COddsInfo* MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3);
};

