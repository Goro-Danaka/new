#pragma once

#include <string>
#include <algorithm>
#include "OddsInfo.h"
#include "BetSites\BetSite.h"

using namespace std;

//#define NO_ENGINE

#define BET_SUBTHREAD_MODE

#define OutDbgStrAA_1xbetchina		//OutDbgStrAA
#define OutDbgStrAA_GameData		//OutDbgStrAA

#define MAX_LEAGUE_NAME		0x80
#define MAX_TEAM_NAME		0x40
#define MAX_COUNTRY_NAME	0x20
#define MAX_STAGE_NAME		0x20
#define MAX_BOOKMAKER		0x20

#define BET_HISTORY_SAVE_LIMITCOUNT 50
#define REFRESH_TIME_SECOND		30

class CGameInfo;
class CGameData;
class CMainInfo;
struct SCORE;

enum
{
	MSG_UPDATE = (WM_USER + 1),
	MSG_ADD_TEAMNAME = (WM_USER + 2),
	MSG_CUR_BALANCE,
	MSG_LOGIN_STATUS,
	MSG_UPDATE_EVENT_LIST,
	MSG_UPDATE_EVENT_TREE,
	MSG_CHECK_TEAMNAME,
	MSG_CAPTCHA,
	MSG_DISP_BET_STATUS,
	MSG_BETTING,
};

enum SITE
{
	SITE_188bet = 0,
	SITE_18bet = 1,
	SITE_1bet,
	SITE_1xbetchina,
	SITE_babibet,
	SITE_bet3000,
	SITE_bet365,
	SITE_bet888vip,
	SITE_betballer,
	SITE_bwin,
	SITE_marathonbet,
	SITE_tomato333,
	SITE_COUNT,
};

enum GAME_CATEGORY
{
	GC_BADMINTON = 0,
	GC_BASEBALL = 1,
	GC_BASKETBALL,
	GC_BEACH_SOCCER,
	GC_BEACH_VOLLEYBALL,
	GC_BOWLS,
	GC_CRICKET,
	GC_DARTS,
	GC_E_SPORTS,
	GC_FIELD_HOCKEY,
	GC_FLOORBALL,
	GC_FOOTBALL,
	GC_HANDBALL,
	GC_ICE_HOCKEY,
	GC_RUGBY_LEAGUE,
	GC_RUGBY_SEVENS,
	GC_SNOOKER,
	GC_TABLE_TENNIS,
	GC_TENNIS,
	GC_VOLLEYBALL,
	GC_WATER_POLO,
	GC_COUNT,
};

enum BET_STRATEGY
{
	BS_SPATIAL = 0,
	BS_TEMPORAL = 1,
	BS_SPATIO_TEMPORAL,
	BS_COUNT,
};
enum
{
	COL_TYPE = 0,
	COL_HTWIN = 1,
	COL_DRAW,
	COL_ATWIN,
	COL_OVER,
	COL_UNDER,
	COL_ODD,
	COL_EVEN,
	COL_BOOKMAKER,
};

struct SCORE
{
	int nScore;//game score, ex: football(2:1), tennis(set score)
	int nScore1;//round score, ex: volleyball(12:18), tennis(game score)
	int nScore2;// only for tennis(score in game)

	SCORE()
	{
		nScore = -1;
		nScore1 = -1;
		nScore2 = -1;
	};

	SCORE(SCORE& score)
	{
		nScore = score.nScore;
		nScore1 = score.nScore1;
		nScore2 = score.nScore2;
	};

	SCORE(int score, int score1, int score2)
	{
		nScore = score;
		nScore1 = score1;
		nScore2 = score2;
	};

	SCORE& operator=(SCORE& score)
	{
		nScore = score.nScore;
		nScore1 = score.nScore1;
		nScore2 = score.nScore2;
		return *this;
	};

	void UpdateScore(SCORE& score)
	{
		if (nScore < score.nScore)
			nScore = score.nScore;
		if (nScore1 < score.nScore1)
			nScore1 = score.nScore1;
		if (nScore2 < score.nScore2)
			nScore2 = score.nScore2;
		return;
	};

	BOOL operator==(SCORE& score)
	{
		if (nScore != -1 && score.nScore != -1 && nScore != score.nScore)
			return FALSE;
		if (nScore1 != -1 && score.nScore1 != -1 && nScore1 != score.nScore1)
			return FALSE;
		if (nScore2 != -1 && score.nScore2 != -1 && nScore2 != score.nScore2)
			return FALSE;
		return TRUE;
		//return (nScore == score.nScore);
		//return (nScore == score.nScore && nScore1 == score.nScore1 && nScore2 == score.nScore2);
	}

	BOOL operator!=(SCORE& score)
	{
		if (nScore != -1 && score.nScore != -1 && nScore != score.nScore)
			return TRUE;
		if (nScore1 != -1 && score.nScore1 != -1 && nScore1 != score.nScore1)
			return TRUE;
		if (nScore2 != -1 && score.nScore2 != -1 && nScore2 != score.nScore2)
			return TRUE;
		return FALSE;
		//return (nScore != score.nScore);
		//return (nScore != score.nScore || nScore1 != score.nScore1 || nScore2 != score.nScore2);
	}
};

struct BetValue
{
	BOOL bValid;
	int nBookMaker;
	DWORD dwGameID;

	float oVal;
	float oVal_orig;
	float oVal_cur;
	int nBettingMoney;

	int nResult;
};

struct BetInfo
{
	int nCategory;
	int nOddsKind;
	float hVal1;

	char szHTeam[MAX_TEAM_NAME];
	char szATeam[MAX_TEAM_NAME];
	char szLeague[MAX_LEAGUE_NAME];

	SCORE HTScore;
	SCORE ATScore;

#ifdef NO_ENGINE
	BetValue BetVal[5];//ht - 0, at - 1, draw - 2
					   //odd - 0, even - 1
					   //over - 0, under - 1
#else
	BetValue BetVal[3];//ht - 0, at - 1, draw - 2
					   //odd - 0, even - 1
					   //over - 0, under - 1
#endif
	BetInfo()
	{
	};

	~BetInfo()
	{
	}
	void Init()
	{
		nCategory = -1;
		nOddsKind = -1;
		hVal1 = 0;
		strcpy_s(szHTeam, "");
		strcpy_s(szATeam, "");
		strcpy_s(szLeague, "");
		for (int i = 0; i < 3; i++)
		{
			BetVal[i].bValid = FALSE;
		}
	}
};
typedef CArray<BetInfo*, BetInfo*> CArrayBetInfo;

struct BetStatusDisplay
{
	char szHTeam[MAX_TEAM_NAME];
	char szATeam[MAX_TEAM_NAME];
	char szLeague[MAX_LEAGUE_NAME];
	int nCategory;
	int nOddKind;
	float fHVal;
	float fOVal_orig;
	float fOVal_cur;
	int nObj;
	int nBookMaker;
	int nBettingMoney;
	float fBalance;
	int nResult;
	DWORD dwEngineTime;
	DWORD dwBettingTime;
	int nRepeat;
};

struct NamePair
{
	char* strTeam1;
	char* strTeam2;

	NamePair()
	{
		strTeam1 = new char[MAX_TEAM_NAME];
		strTeam2 = new char[MAX_TEAM_NAME];
	};

	~NamePair()
	{
		delete strTeam1;
		delete strTeam2;
	};
};
typedef CArray<NamePair*, NamePair*> CArrayNamePair;

typedef CMap<CStringA, LPCSTR, CStringA, LPCSTR> CMapName;
typedef CArray<char*, char*> CArrayMainName;

typedef CArray<CGameInfo*, CGameInfo*> CArrayGameInfo;
extern BetInfo* g_pBetInfo;

extern char* g_szSiteName[12];
extern TCHAR* g_wszSiteName[12];
extern float g_fBalance[12];
extern float g_fBalanceOld[12];

extern char* g_szGameCategory[21];
extern TCHAR* g_wszGameCategory[21];

extern TCHAR* g_wszBetResult[26];
extern TCHAR* g_wszBetStrategy[3];
extern TCHAR* g_wszOddKind[6];

extern CBetSite* g_site[SITE_COUNT];

extern SYSTEMTIME g_EngineStartTime;
extern SYSTEMTIME g_BettingStartTime;
extern SYSTEMTIME g_BettingEndTime;

//extern CArrayNamePair g_arySameNamePair;
extern CArrayNamePair g_aryDiffNamePair;

extern CMapName g_mapName;
//extern CArrayMainName g_aryMainName;

extern CArrayGameInfo g_aryGameInfo;

extern CRITICAL_SECTION g_csGameInfoArray;
extern CRITICAL_SECTION g_csTeamNameWnd;
extern CRITICAL_SECTION g_csSameName;
extern CRITICAL_SECTION g_csDiffName;

void GlobConvertAnsiToUnicode(PCSTR source, PWSTR obj, UINT codePage);
void GlobConvertUnicodeToAscii(PCWSTR source, PSTR obj, UINT codePage);
void ToAscii(PCWSTR src, PSTR dest, UINT codePage);
void ToUnicode(PCSTR src, PWSTR dest, UINT codePage);

void GlobGetRandomFileNameA(char* szFileName, int size, char* szExt);
void GlobGetRandomFileNameW(WCHAR* szFileName, int size, WCHAR* szExt);
void GlobGetRandomTempFilePathA(char* szFilePath, int nFileNameLen, char* szExt);
void GlobGetRandomTempFilePathW(WCHAR* szFilePath, int nFileNameLen, WCHAR* szExt);

void OutDbgStrAA(char* szFmtStr, ...);
void OutDbgStrWW(LPCWSTR szFmtStr, ...);
void WriteHex(LPVOID pBuffer, int nLen);

int GetTime(const char* strStage, int nGameCategory);

bool replace(std::string& str, const std::string& from, const std::string& to);
bool replaceW(std::wstring& str, const std::wstring& from, const std::wstring& to);

BOOL IsEqualGameName(CMainInfo& info1, CMainInfo& info2, int nSameOrSimilar, int nHAChanged);
BOOL IsEqualTeamName(char* a, char* b);
BOOL IsSimilarTeamName(char* aa, char* bb);
BOOL IsEqualTime(int a, int b);
BOOL IsDiffTeamName(char* aa, char* bb);

BOOL IsExceptionGame(char* ht, char* at, char* lg, char* stage);

BOOL Split(char* str, char c, float& fVal1, float& fVal2);
BOOL SplitS(char* str, char c, string& s1, string& s2);

int GetGameCategory(char* szGameCategory);
CString GetGameCategory(int nGameCategory);

CString F2SW(float f);
CStringA F2SA(float f);
CString MakeHandicapLabel(COddsInfo* pOddsInfo);
CString MakeGoalLabel(COddsInfo* pOddsInfo);

char *ltrim(char *s, char c);
char *rtrim(char *s, char c);
char *trim(char *s, char c);

char *ltrim(char *s, char* c);
char *rtrim(char *s, char* c);
char *trim(char *s, char* c);

void  strproc1(char* aa, char* bb);

BOOL IsEqualF(float f1, float f2);
void SetClipboard(CStringA textToclipboard);
CString GetClipboard();

int GetImageNo(int nCat);

void WritePrivateProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, int Value, LPCWSTR lpFileName);
void WritePrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, int Value, LPCSTR lpFileName);

string GetStage(char* s, int& t);

void SetListItemText(CListCtrl& listCtrl, int col, int row, LPCTSTR szText);

void PeekMessageLoop();
void WaitForThreadToTerminate(HANDLE hThread);

void AddToArray(CArrayGameInfo& aryGameInfo, CGameInfo* pGameInfo);

void GetEncPswd(char* szPswd, char* szPswd1);
void GetDecPswd(char* szPswd, char* szPswd1);

void WriteResData(TCHAR* szFile, char* szText);

BOOL IsMasterName(char* name);
BOOL IsSlaveName(char* name);
void SetMasterName(char* name);
void SetSlaveName(char* slave, char* main);

void CopyGameInfo(CArrayGameInfo& aryGameInfo1, CArrayGameInfo& aryGameInfo2);
CGameData* GetGameID(CArrayGameInfo& aryGameInfo, int nCategory, int nBookMaker, char* szHTeam, char* szATeam);

DWORD GetTimeStamp();
CStringA GetTimeStampA();
CString GetTimeStampW();
CStringA GetTimeStampLongA();
CString GetTimeStampLongW();

CGameInfo* GetGameInfoFromBetInfo(BetInfo* pBetInfo, int nBetObjNo);

BOOL GetSessionValue(TCHAR* buffer, TCHAR* szSession, TCHAR* szVal);
BOOL GetSessionValue(TCHAR* buffer, TCHAR* szSession1, TCHAR* szSession, TCHAR* szVal);

DWORD GetRelativeTime(SYSTEMTIME time1, SYSTEMTIME time2);

string PlusProc(char* s);
string GenUUID();

BOOL GetOddVal(char* str, string& sHval, float& fOval);
string GetStageMin(char* str, int& nMin);
string GetTeamName(char* szName);
void GetScore(char* str, int& hs, int& as);
