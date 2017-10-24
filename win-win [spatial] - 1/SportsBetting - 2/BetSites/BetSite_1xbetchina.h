#pragma once
#include "BetSite.h"
#include "..\GameData.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

enum
{
	REQ_PUTS_BETCOMMON = 0,
	REQ_PUTS_BETCOMMON1 = 1,
	REQ_SET_RDATA,
	REQ_SET_RDATA1,
	REQ_SET_RDATA2,
	REQ_SET_RDATA3,
	REQ_BET_BALANCE1,
	REQ_UPDATE_COUPON,
	REQ_EXPRESS,
	REQ_BET_GETDATA,
};

class CBetSite_1xbetchina :
	public CBetSite
{
public:
	CBetSite_1xbetchina();
	~CBetSite_1xbetchina();

	TCHAR m_szHash[0x80];
	TCHAR m_szUserID[0x80];
	char m_szBetGUID[0x80];
	TCHAR m_che_h[0x80];
	TCHAR m_che_i[0x80];
	TCHAR m_che_r[0x80];

	CStringA m_strNameGroup, m_strNameBet, m_strSobID, m_dopV;
	int m_idGroup, m_type;

	int DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo);
	virtual int MainProc();
	virtual BOOL SendRequest(int nReqKind);
	virtual int SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind);
	virtual void RefreshData();

	BOOL AnalyzeResData();
	BOOL AnalyzeResLogIn();
	BOOL AnalyzeResLogOut();
	BOOL AnalyzeResBalance();
	BOOL AnalyzeResRedirect();

	int AnalyzeBetPutBet(int& waitTime);
	int AnalyzeBetPutBet1();
	BOOL AnalyzeBetSetRData();
	BOOL AnalyzeBetUpdateCoupon(BetInfo* pBetInfo, int nBetObjNo, float& coeff);
	BOOL AnalyzeBetResBalance(int nStep, float& fMoney);

	void GetNameInfo(BetInfo* pBetInfo, int nBetObjNo, CStringA& strNameGroup, CStringA& strNameBet, int& idGroup, int& type);
	void GetSobID(BetInfo* pBetInfo, int type, int nGameID, CStringA& strSobID, CStringA& dopV);

	BOOL GetSessionVal(TCHAR* buffer);
	BOOL GetHash(TCHAR* buffer);
	BOOL GetCheH(TCHAR* buffer);

	virtual BOOL UpdateTree(int nIsExitThread);

	COddsInfo* GetOdds(CGameData* pGameData, int nOddsKind, float fP);
	UINT64 GetOddID(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo);

	int GetCorrespondOdd(CGameData* pGameData, int nOddsKind, float fp);
	void MainOddsProc(CGameData* pGameData, rapidjson::Value& E, int index, int& nCount);
	void UpdateOddInfo(CGameData* pGameData, COddsInfo* pOddsInfo, int nOddsKind, float fOval1, float fOval2, float fOval3, DWORD dwOddID);

	COddsInfo* MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3);
};
