#pragma once

#include "afxinet.h"

using namespace std;
#include "..\OddsInfo.h"
#include <inttypes.h>

//#define SAVE_TEXT_FILE

#define READ_SIZE (4 * 1024) // 4K
#define GZIP_SIZE (1024 * 1024) // 1M
#define TEXT_SIZE (16 * 1024 * 1024) // 16M

#define MAX_ID				0x40
#define MAX_PSWD			0x40
#define MAX_ENC_PSWD		0x80

enum LOGIN_STATE
{
	LS_INIT = 0,
	LS_SUCCESS = 1,
	LS_FAIL,
	LS_LIVE,
};

enum
{
	REQ_LOGIN = 0,
	REQ_LOGOUT = 1,
	REQ_LIVE,
	REQ_LIVE1,
	REQ_LIVE2,
	REQ_LIVE3,
	REQ_LIVE4,
	REQ_LIVE5,
	REQ_LIVE6,
	REQ_LOGIN2,
	REQ_BALANCE,
	REQ_DATA,
	REQ_DATA1,
	REQ_HB,
	REQ_BET,
	REQ_BETSLIP,
	REQ_LOGIN_REDIRECT,
	REQ_GET_CATEGORY_COUNT,
};

//g_wszBetResult
enum
{
	BET_SUCCESS = 0,
	BET_SUCCESS1 = 1,
	BET_FAIL,
	BET_ALREADY_PLACED,
	BET_CHECK_OK,
	BET_GAME_REMOVED,
	BET_VALUE_CHANGED,
	BET_VALUE_DECREASED,
	BET_VALUE_DECREASED1,
	BET_ODDS_NOT_VALID,
	BET_ODDS_NOT_AVAILABLE,
	BET_GET_ODD_FAIL,
	REQ_UPDATE_BETSLIP_FAIL,
	REQ_PLACE_BET_FAIL,
	BET_ODD_REMOVED,
	BET_TIMEOUT,
	BET_PARSE_FAIL,
	BET_BLOCKED,
	BET_CHECK_EVENT_FAIL,
	BET_UNABLE,
	BET_ANALYZE_ERROR1,
	BET_ANALYZE_ERROR2,
	BET_ANALYZE_ERROR3,
	BET_ANALYZE_ERROR4,
	BET_ANALYZE_ERROR5,
	BET_ANALYZE_ERROR6,
};

class COddsInfo;
class CGameData;
class CGameInfo;
class CMainInfo;
struct BetInfo;

DWORD WINAPI ThreadProc(PVOID pParam);

class CBetSite
{
public:
	CBetSite();
	virtual ~CBetSite();

public:
	CRITICAL_SECTION m_csThread;
	CRITICAL_SECTION m_csBetObjCount;

	CHttpFile* m_HttpFile;
	CHttpFile* m_HttpFileBet;
	HANDLE m_hThread;
	BOOL m_bExitThreadFlg;
	BOOL m_bDoLogOut;
	int m_nTimePeriod;
	int m_nBettingTimePeriod;
	
	BYTE* m_bufRead;
	BYTE* m_bufGZip;
	char* m_szText;
	BYTE* m_bufReadBet;
	BYTE* m_bufGZipBet;
	char* m_szTextBet;

	int m_nGZipSize;
	int m_nTextSize;
	int m_nGZipSizeBet;
	int m_nTextSizeBet;

	int m_nReqKindID;
	int m_nReqNo;
	int m_nTimeOut;

	ULONGLONG m_uuLastBettingTime;

	TCHAR m_szGZipFileName[MAX_PATH];
	TCHAR m_szTextFileName[MAX_PATH];

	int m_nBookMaker;

	char m_szID[MAX_ID];
	char m_szPswd[MAX_PSWD];
	char m_szCaptcha[20];

	int m_nLogInStatus;

	CInternetSession* m_pSession;
	CInternetSession* m_pSessionBet;
	TCHAR m_szSession[0x100];

	int m_nMinBetMoney;
	int m_nMaxBetMoney;

	int m_repeat, m_repeat1;

	CMap<int, int, CGameData*, CGameData*> m_mapGameID;

public:
	virtual void AnalyzeResText();

	void ReadResponse();
	int ReadResponse(CHttpFile* HttpFile, BYTE* bufGZip, BYTE* bufRead, char* szText);

	void StartThread(char* szID, char* szPswd);
	void ExitThread();

	DWORD ThreadProc();
	virtual void CreateSubThread();

	virtual int GetGameCategory(char* szGameCategory);
	virtual CString GetGameCategory(int nGameCategory);

	void CopyGameDataProc(CGameData* pGameData);
	CGameInfo* CreateGameInfo(CMainInfo* pMainInfo, int nEventID);
	BOOL GetGameInfoProc(CMainInfo* pMainInfo, CGameInfo** ppGameInfo, int nEventID/* = -1*/);
	BOOL AddOddInfo(COddsInfo* pOddsInfo, CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3);
	CGameInfo* CheckGameInfo(int nBookMaker, CMainInfo* pMainInfo, BOOL& bSameOrSimilar, BOOL& bHAChanged, int nEventID);
	CGameInfo* GetGameInfo(int nBookMaker, CMainInfo& info, BOOL& bSameOrSimilar, BOOL& bHAChanged, int nEventID);

	int ZipStream(char *a, int sa, char *b, int sb);
	int UnzipStream(char *input, int nSizeIn, char *output, int nSizeOut);
	int Unzip(TCHAR* szGZfileName, BYTE* buf);

	virtual int MainProc();
	BOOL SendRequestEx(int nReqKind);
	virtual BOOL SendRequest(int nReqKind);
	int SendBetRequestEx(BetInfo* pBetInfo, int nBetObjNo, int nReqKind);
	virtual int SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind);

	virtual BOOL UpdateTree(int nIsExitThread);
	BOOL UpdateTree_AllInfoDownAtOnce(int nIsExitThread);

	int DoBetting(BetInfo* pBetInfo, int nBetObjNo);
	virtual int DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo);
	virtual void RefreshData();
	int CheckBetInfo(BetInfo* pBetInfo, int nBetObjNo, CGameData* pGameData, COddsInfo** ppOddsInfo);

	BOOL IsNeedGetBalance();
};

