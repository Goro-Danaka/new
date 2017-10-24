
#include "stdafx.h"
#include "global.h"
#include "GameData.h"
#include "GameInfo.h"
#include "OddsInfo.h"

#include <string>
#include <algorithm>
#include <time.h>

CRITICAL_SECTION g_csGameInfoArray;
CRITICAL_SECTION g_csTeamNameWnd;
CRITICAL_SECTION g_csSameName;
CRITICAL_SECTION g_csDiffName;

char* g_szSiteName[12] =
{
	"marathonbet",
	"188bet",
	"18bet",
	"1bet",
	"1xbetchina",
	"babibet",
	"bet3000",
	"bet365",
	"bet888vip",
	"betballer",
	"bwin",
	"tomato333"
};

TCHAR* g_wszSiteName[12] =
{
	_T("marathonbet"),
	_T("188bet"),
	_T("18bet"),
	_T("1bet"),
	_T("1xbetchina"),
	_T("babibet"),
	_T("bet3000"),
	_T("bet365"),
	_T("bet888vip"),
	_T("betballer"),
	_T("bwin"),
	_T("tomato333")
};

float g_fBalance[12];
float g_fBalanceOld[12];

int g_nCatImage[21] =
{
	6, 11, 5, 1, 2, 8, 10, 13, 15, 9, 9, 1, 12, 9, 4, 4, 14, 7, 3, 2, 12
};

char* g_szGameCategory[21] =
{
	"Badminton",
	"Baseball",
	"Basketball",
	"Beach Soccer",
	"Beach Volleyball",
	"Bowls",
	"Cricket",
	"Darts",
	"e-Sports",
	"Field Hockey",
	"Floorball",
	"Football",
	"Handball",
	"Ice Hockey",
	"Rugby League",
	"Rugby Sevens",
	"Snooker",
	"Table Tennis",
	"Tennis",
	"Volleyball",
	"Water Polo"
};

TCHAR* g_wszGameCategory[21] =
{
	_T("Badminton"),
	_T("Baseball"),
	_T("Basketball"),
	_T("Beach Soccer"),
	_T("Beach Volleyball"),
	_T("Bowls"),
	_T("Cricket"),
	_T("Darts"),
	_T("e-Sports"),
	_T("Field Hockey"),
	_T("Floorball"),
	_T("Football"),
	_T("Handball"),
	_T("Ice Hockey"),
	_T("Rugby League"),
	_T("Rugby Sevens"),
	_T("Snooker"),
	_T("Table Tennis"),
	_T("Tennis"),
	_T("Volleyball"),
	_T("Water Polo")
};

TCHAR* g_wszBetResult[26] =
{
	_T("BET_SUCCESS"),
	_T("BET_SUCCESS1"),
	_T("BET_FAIL"),
	_T("BET_ALREADY_PLACED"),
	_T("BET_CHECK_OK"),
	_T("BET_GAME_REMOVED"),
	_T("BET_VALUE_CHANGED"),
	_T("BET_VALUE_DECREASED"),
	_T("BET_VALUE_DECREASED1"),
	_T("BET_ODDS_NOT_VALID"),
	_T("BET_ODDS_NOT_AVAILABLE"),
	_T("BET_GET_ODD_FAIL"),
	_T("REQ_UPDATE_BETSLIP_FAIL"),
	_T("REQ_PLACE_BET_FAIL"),
	_T("BET_ODD_REMOVED"),
	_T("BET_TIMEOUT"),
	_T("BET_PARSE_FAIL"),
	_T("BET_BLOCKED"),
	_T("BET_CHECK_EVENT_FAIL"),
	_T("BET_UNABLE"),
	_T("BET_ANALYZE_ERROR1"),
	_T("BET_ANALYZE_ERROR2"),
	_T("BET_ANALYZE_ERROR3"),
	_T("BET_ANALYZE_ERROR4"),
	_T("BET_ANALYZE_ERROR5"),
	_T("BET_ANALYZE_ERROR6"),
};

TCHAR* g_wszBetStrategy[3] =
{
	_T("Spatial"),
	_T("Temporal"),
	_T("Spatio-Temporal")
};

TCHAR* g_wszOddKind[6] =
{
	_T("Main"),
	_T("Handicap"),
	_T("Over/Under"),
	_T("Odd/Even"),
	_T("HTeam Over/Under"),
	_T("ATeam Over/Under")
};

CArrayNamePair g_aryDiffNamePair;

CMapName g_mapName;

CArrayGameInfo g_aryGameInfo;
BetInfo* g_pBetInfo = NULL;

CBetSite*	g_site[SITE_COUNT];

SYSTEMTIME g_EngineStartTime;
SYSTEMTIME g_BettingStartTime;
SYSTEMTIME g_BettingEndTime;

void GlobConvertAnsiToUnicode(PCSTR source, PWSTR obj, UINT codePage)
{
	int nLength = 0;

	nLength = MultiByteToWideChar(codePage, 0, source, -1, NULL, 0);
	MultiByteToWideChar(codePage, 0, source, -1, (LPWSTR)obj, nLength);
}

void GlobConvertUnicodeToAscii(PCWSTR source, PSTR obj, UINT codePage)
{
	int nLength = 0;

	nLength = WideCharToMultiByte(codePage, 0, (LPCWSTR)source, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(codePage, 0, (LPCWSTR)source, -1, obj, nLength, NULL, NULL);
}

void ToAscii(PCWSTR src, PSTR dest, UINT codePage)
{
	GlobConvertUnicodeToAscii(src, dest, codePage);
}

void ToUnicode(PCSTR src, PWSTR dest, UINT codePage)
{
	GlobConvertAnsiToUnicode(src, dest, codePage);
}

void GlobGetRandomFileNameA(char* szFileName, int size, char* szExt)
{
	srand(GetTickCount() * size);
	for (int i = 0; i < size; i++)
	{
		szFileName[i] = 'a' + (rand() * (size + i)) % ('z' - 'a');
	}
	szFileName[size] = 0;

	if (szExt)
	{
		strcat_s(szFileName, MAX_PATH, szExt);
	}
}

void GlobGetRandomFileNameW(WCHAR* szFileName, int size, WCHAR* szExt)
{
	srand(GetTickCount() * size);
	for (int i = 0; i < size; i++)
	{
		szFileName[i] = L'a' + (rand() * (size + i)) % (L'z' - L'a');
	}
	szFileName[size] = 0;

	if (szExt)
	{
		wcscat_s(szFileName, MAX_PATH, szExt);
	}
}

void GlobGetRandomTempFilePathA(char* szFilePath, int nFileNameLen, char* szExt)
{
	char szFileName[MAX_PATH];

	GetTempPathA(MAX_PATH, szFilePath);
	GlobGetRandomFileNameA(szFileName, nFileNameLen, szExt);
	strcat_s(szFilePath, MAX_PATH, szFileName);
	DeleteFileA(szFilePath);
}

void GlobGetRandomTempFilePathW(WCHAR* szFilePath, int nFileNameLen, WCHAR* szExt)
{
	WCHAR szFileName[MAX_PATH];

	GetTempPathW(MAX_PATH, szFilePath);
	GlobGetRandomFileNameW(szFileName, nFileNameLen, szExt);
	wcscat_s(szFilePath, MAX_PATH, szFileName);
	DeleteFileW(szFilePath);
}

void OutDbgStrAA(char* szFmtStr, ...)
{
	char szDebug[1024];
	va_list arg_list;
	va_start(arg_list, szFmtStr);
	_vsnprintf_s((char*)szDebug, sizeof(szDebug), _TRUNCATE, szFmtStr, arg_list);
	OutputDebugStringA(szDebug);
	va_end(arg_list);
}

void OutDbgStrWW(LPCWSTR szFmtStr, ...)
{
	WCHAR wszDebug[1024];
	memset(wszDebug, 0, 2048);
	va_list arg_list;
	va_start(arg_list, szFmtStr);
	_vsnwprintf_s(wszDebug, 1024, _TRUNCATE, szFmtStr, arg_list);

	OutputDebugStringW(wszDebug);

	va_end(arg_list);
}

#define MAX_LOG_HEADER_LEN	100
#define MAX_LINE_LEN	1000
#define MAX_LOG_LEN		5000
#define MAX_LINE_BYTES  16 

void HexToStr(LPVOID pHexBuffer, int nLen, char* szBuffer, size_t size)
{
	char cBytes[3];

	char* pBuffer;
	if (pHexBuffer == NULL || nLen <= 0 || szBuffer == NULL)
		return;

	pBuffer = (char*)pHexBuffer;
	szBuffer[0] = '\0';

	for (int iByte = 0; iByte < nLen; iByte++)
	{
		if (iByte > 0)
			strcat_s(szBuffer, MAX_LINE_LEN, " ");

		cBytes[2] = '\0';
		cBytes[1] = pBuffer[iByte] & 0x0F;

		//first 4 bits
		if (cBytes[1] >= 0x0A)
			cBytes[1] = (cBytes[1] - 0x0A) + 'A';
		else
			cBytes[1] = cBytes[1] + '0';

		cBytes[0] = (BYTE)pBuffer[iByte] >> 4;
		//last 4 bits
		if (cBytes[0] >= 0x0A)
			cBytes[0] = (cBytes[0] - 0x0A) + 'A';
		else
			cBytes[0] = cBytes[0] + '0';

		strcat_s(szBuffer, MAX_LINE_LEN, cBytes);
	}
}

void WriteHex(LPVOID pBuffer, int nLen)
{
	BYTE* pBuf;
	int   nLineLen;
	char  g_szLine[MAX_LINE_LEN] = "";

	if (pBuffer == NULL || nLen <= 0)
		return;
	pBuf = (BYTE*)pBuffer;

	int i;
	for (i = 0; i < nLen; i++)
	{
		if (pBuf[i] != 0)
			break;
	}
	if (i == nLen)
		return;
	OutDbgStrAA("---------------------------------------\r");

	while (nLen > 0)
	{
		//tab string if this is receive string
		nLineLen = (nLen < MAX_LINE_BYTES) ? nLen : MAX_LINE_BYTES;

		//one-line
		HexToStr(pBuf, nLineLen, g_szLine, sizeof(g_szLine));
		OutDbgStrAA("\r\r\r%s", g_szLine);

		pBuf += nLineLen;
		nLen -= nLineLen;
	}
}

int GetTime(const char* strStage, int nGameCategory)
{
	if (strcmp(strStage, "") == 0)
		return 0;
	int nColonCount = 0, i = 0, nColonPos = 0, nColPos = 0;
	char* temp = (char*)strStage;
	char* szColonPos;
	while (1)
	{
		temp += nColonPos;
		szColonPos = strchr(temp, ':');
		if (szColonPos == NULL)
			break;
		nColonPos = szColonPos - temp + 1;
		if (nColonCount == 0)
			nColPos = nColonPos;
		nColonCount++;
	}

	if (nColonCount != 1)
		ASSERT(0);

	char szMin[8], szSec[8];
	strncpy_s(szMin, strStage, nColPos - 1);
	szMin[nColPos - 1] = 0;
	strncpy_s(szSec, strStage + nColPos, strlen(strStage) - nColPos + 1);

	int nMin, nSec;
	sscanf_s(szMin, "%d", &nMin);
	sscanf_s(szSec, "%d", &nSec);
	return nMin * 60 + nSec;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	bool bRet = false;
	string str1 = str;
	size_t start_pos = str.find(from);
	while (start_pos != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		str1 = str.c_str() + start_pos + 1;
		start_pos = str.find(from);
		bRet = true;
	}
	return bRet;
}

bool replaceW(std::wstring& str, const std::wstring& from, const std::wstring& to) {
	bool bRet = false;
	wstring str1 = str;
	size_t start_pos = str.find(from);
	while (start_pos != std::wstring::npos)
	{
		str.replace(start_pos, from.length(), to);
		str1 = str.c_str() + start_pos + 1;
		start_pos = str.find(from);
		bRet = true;
	}
	return bRet;
}

void remove_bracket(std::string& str)
{
	string s = str;
	while (1)
	{
		size_t pos1 = s.find("(");
		size_t pos2 = s.find(")");
		ASSERT((pos1 == string::npos && pos2 == string::npos) || (pos1 != string::npos && pos2 != string::npos));
		if (pos1 == string::npos)
			break;
		s.replace(pos1, pos2 - pos1 + 1, "");
	}
	str = s;
}

BOOL IsUnderNumber(char* s)
{
	int n = 0, n1 = 0;
	sscanf_s(s, "u%d", &n);
	if (n != 0 || n1 != 0)
		return TRUE;
	return FALSE;
}

BOOL IsSpecial(char* s)
{
	if (IsUnderNumber(s))
		return TRUE;
	if (strcmp(s, "town") == 0 || strcmp(s, "city") == 0 || strcmp(s, "united") == 0 || strcmp(s, "club") == 0 || strcmp(s, "country") == 0 || strstr(s, "youth") != 0)
		return TRUE;
	return FALSE;
}

BOOL SubStrMatch(char* a, char* b)
{
	int n1 = strlen(a);
	int n2 = strlen(b);
	char* a1 = new char[n1 + 1];
	strcpy_s(a1, n1 + 1, a);
	char* b1 = new char[n2 + 1];
	strcpy_s(b1, n2 + 1, b);

	char* a2 = new char[n1 + 1];
	strcpy_s(a2, n1 + 1, a);
	char* b2 = new char[n2 + 1];
	strcpy_s(b2, n2 + 1, b);

	char *strToken = NULL;
	char *next_token = NULL;
	strToken = strtok_s(a1, " ", &next_token);
	if (strToken == NULL)
		goto NEXT;
	if (strlen(strToken) >= 4 && !IsSpecial(strToken) && strstr(b1, strToken) != 0)
		goto RET_TRUE;
	while (strToken != NULL)
	{
		strToken = strtok_s(NULL, " ", &next_token);
		if (strToken == NULL)
			goto NEXT;
		if (strlen(strToken) >= 4 && !IsSpecial(strToken) && strstr(b1, strToken) != 0)
			goto RET_TRUE;
	}
NEXT:
	strToken = NULL;
	next_token = NULL;
	strToken = strtok_s(b2, "- _", &next_token);
	if (strToken == NULL)
		goto RET_FALSE;
	if (strlen(strToken) >= 4 && !IsSpecial(strToken) && strstr(a2, strToken) != 0)
		goto RET_TRUE;
	while (strToken != NULL)
	{
		strToken = strtok_s(NULL, "- _", &next_token);
		if (strToken == NULL)
			goto RET_FALSE;
		if (strlen(strToken) >= 4 && !IsSpecial(strToken) && strstr(a2, strToken) != 0)
			goto RET_TRUE;
	}
RET_FALSE:
	delete a1;
	delete a2;
	delete b1;
	delete b2;
	return FALSE;
RET_TRUE:
	delete a1;
	delete a2;
	delete b1;
	delete b2;
	return TRUE;
}

void  strproc1(char* aa, char* bb)
{
	int n1 = strlen(aa);

	int i = 0, j = 0;
	for (i = 0; i < n1 + 1; i++)
	{
		if (aa[i] != '-' && aa[i] != '_' && aa[i] != '.')
		{
			if (aa[i] >= 'A' && aa[i] <= 'Z')
				bb[j] = aa[i] + 0x20;
			else
				bb[j] = aa[i];
		}
		else if (aa[i] == '-' || aa[i] == '_')
		{
			bb[j] = ' ';
		}
		j++;
	}
	return;
}

BOOL IsEqualGameName(CMainInfo& info1, CMainInfo& info2, int nSameOrSimilar, int nHAChanged)
{
	/*
	nSameOrSimilar: 1->same, -1->similar, 0->same or similar
	nHAChanged: 1->nochanged, -1->changed, 0->nochange or change
	*/
	if (nSameOrSimilar == 0)
		return IsEqualGameName(info1, info2, 1, nHAChanged) || IsEqualGameName(info1, info2, -1, nHAChanged);
	if (nHAChanged == 0)
		return IsEqualGameName(info1, info2, nSameOrSimilar, 1) || IsEqualGameName(info1, info2, nSameOrSimilar, -1);

	::EnterCriticalSection(&g_csSameName);

	//if (strstr(info1.m_szHTName, "ortuga") != 0 && strstr(info2.m_szHTName, "ortuga") != 0)
	//	_asm int 3
	char* h1 = info1.m_szhtName;
	char* h2 = info2.m_szhtName;
	char* a1 = info1.m_szatName;
	char* a2 = info2.m_szatName;
	if (nSameOrSimilar == 1)
	{
		if (nHAChanged == 1 && IsEqualTeamName(h1, h2) && IsEqualTeamName(a1, a2))
			goto L_TRUE;
		if (nHAChanged == -1 && IsEqualTeamName(h1, a2) && IsEqualTeamName(a1, h2))
			goto L_TRUE;
		goto L_FALSE;
	}

	if (nHAChanged == 1)
	{
		if (IsDiffTeamName(h1, h2) || IsDiffTeamName(a1, a2))
			goto L_FALSE;
	}
	else if (nHAChanged == -1)
	{
		if (IsDiffTeamName(h1, a2) || IsDiffTeamName(a1, h2))
			goto L_FALSE;
	}

	if (nHAChanged == 1)
	{
		if (IsEqualTeamName(h1, h2) && IsEqualTeamName(a1, a2))
			goto L_TRUE;
		if (IsSimilarTeamName(h1, h2) && IsSimilarTeamName(a1, a2))
			goto L_TRUE;
		//if ((IsSimilarTeamName(h1, h2) || IsSimilarTeamName(a1, a2)) && info1.m_HTScore == info2.m_HTScore && info1.m_ATScore == info2.m_ATScore)
		//	goto L_TRUE;
	}
	else if (nHAChanged == -1)
	{
		if (IsEqualTeamName(h1, a2) && IsEqualTeamName(a1, h2))
			goto L_TRUE;
		if (IsSimilarTeamName(h1, a2) && IsSimilarTeamName(a1, h2))
			goto L_TRUE;
		//if ((IsSimilarTeamName(h1, a2) || IsSimilarTeamName(a1, h2)) && info1.m_HTScore == info2.m_ATScore && info1.m_ATScore == info2.m_HTScore)
		//	goto L_TRUE;
	}
L_FALSE:
	::LeaveCriticalSection(&g_csSameName);
	return FALSE;
L_TRUE:
	::LeaveCriticalSection(&g_csSameName);
	if (nHAChanged == 1 && info1.m_HTScore != info2.m_HTScore && info1.m_ATScore != info2.m_ATScore)
		return FALSE;
	if (nHAChanged == -1 && info1.m_HTScore != info2.m_ATScore && info1.m_ATScore != info2.m_HTScore)
		return FALSE;
	return TRUE;
}

BOOL IsDiffTeamName(char* aa, char* bb)
{
	if (strcmp(aa, bb) == 0)
		return FALSE;
	if ((strstr(aa, " corner") != 0 && strstr(bb, " corner") == 0) ||
		(strstr(aa, " corner") == 0 && strstr(bb, " corner") != 0) ||
		(strstr(aa, "handicap") != 0 && strstr(bb, "handicap") == 0) ||
		(strstr(aa, "handicap") == 0 && strstr(bb, "handicap") != 0) ||
		(strstr(aa, ":") != 0 && strstr(bb, ":") == 0) ||
		(strstr(aa, ":") == 0 && strstr(bb, ":") != 0))
		return TRUE;
	//::EnterCriticalSection(&g_csDiffName);
	for (int i = 0; i < g_aryDiffNamePair.GetSize(); i++)
	{
		NamePair* pNamePare = g_aryDiffNamePair[i];
		if ((strcmp(pNamePare->strTeam1, aa) == 0 && strcmp(pNamePare->strTeam2, bb) == 0) ||
			(strcmp(pNamePare->strTeam1, bb) == 0 && strcmp(pNamePare->strTeam2, aa) == 0))
		{
			//::LeaveCriticalSection(&g_csDiffName);
			return TRUE;
		}
	}
	//::LeaveCriticalSection(&g_csDiffName);
	return FALSE;
}

BOOL SameName(char* a2, char* b2)
{
	char* a1 = new char[strlen(a2) + 1];
	char* b1 = new char[strlen(b2) + 1];
	memcpy(a1, a2, strlen(a2) + 1);
	memcpy(b1, b2, strlen(b2) + 1);

	CStringA strA[0x10], strB[0x10];
	int nSegCountA = 0, nSegCountB = 0;

	char *strToken = NULL;
	char *next_token = NULL;
	strToken = strtok_s(a1, " ", &next_token);
	while (strToken != NULL)
	{
		if (strlen(strToken) >= 4)
		{
			strA[nSegCountA] = strToken;
			nSegCountA++;
		}
		strToken = strtok_s(NULL, " ", &next_token);
	}

	strToken = NULL;
	next_token = NULL;
	strToken = strtok_s(b1, " ", &next_token);
	while (strToken != NULL)
	{
		if (strlen(strToken) >= 4)
		{
			strB[nSegCountB] = strToken;
			nSegCountB++;
		}
		strToken = strtok_s(NULL, " ", &next_token);
	}

	for (int i = 0; i < nSegCountA; i++)
	{
		for (int j = 0; j < nSegCountB; j++)
		{
			if (strA[i] == strB[j] && !IsSpecial(strA[i].GetBuffer()))
				return TRUE;
		}
	}
	delete a1;
	delete b1;
	return FALSE;
}

BOOL IsEqualTeamName(char* a1, char* b1)
{
	if (strcmp(a1, b1) == 0)
		return TRUE;

	if ((strstr(a1, " corner") != 0 && strstr(b1, " corner") == 0) ||
		(strstr(a1, " corner") == 0 && strstr(b1, " corner") != 0) ||
		(strstr(a1, "handicap") != 0 && strstr(b1, "handicap") == 0) ||
		(strstr(a1, "handicap") == 0 && strstr(b1, "handicap") != 0) ||
		(strstr(a1, ":") != 0 && strstr(b1, ":") == 0) ||
		(strstr(a1, ":") == 0 && strstr(b1, ":") != 0))
		return FALSE;

	int num1 = -1, num2 = -1;
	CStringA a2, b2;
	BOOL f1 = FALSE, f2 = FALSE;
	if (f1 = g_mapName.Lookup(a1, a2))
	{
		if (a2 == b1)
			return TRUE;
	}

	if (f2 = g_mapName.Lookup(b1, b2))
	{
		if (b2 == a1)
			return TRUE;
	}

	if (f1 && f2 && a2 == b2)
		return TRUE;

	if (SameName(a1, b1))
		return TRUE;
	return FALSE;
}

BOOL IsSimilarTeamName(char* a1, char* b1)
{
	if (strcmp(a1, b1) == 0)
		return FALSE;
	if ((strstr(a1, " corner") != 0 && strstr(b1, " corner") == 0) ||
		(strstr(a1, " corner") == 0 && strstr(b1, " corner") != 0) ||
		(strstr(a1, "handicap") != 0 && strstr(b1, "handicap") == 0) ||
		(strstr(a1, "handicap") == 0 && strstr(b1, "handicap") != 0) ||
		(strstr(a1, ":") != 0 && strstr(b1, ":") == 0) ||
		(strstr(a1, ":") == 0 && strstr(b1, ":") != 0))
		return FALSE;
	//if (IsEqualTeamName(a1, b1) || IsDiffTeamName(a1, b1))
	//	return FALSE;

	if (strstr(a1, b1) || strstr(b1, a1) || SubStrMatch(a1, b1))
		return TRUE;

	return FALSE;
}

BOOL IsEqualTime(int a, int b)
{
	return TRUE;
}

BOOL Split(char* str, char c, float& fVal1, float& fVal2)
{
	if (strchr(str, c) == 0)
		return FALSE;

	int nColonCount = 0, i = 0, nColonPos = 0, nColPos = 0;
	char* temp = (char*)str;
	char* szColonPos;
	while (1)
	{
		temp += nColonPos;
		szColonPos = strchr(temp, c);
		if (szColonPos == NULL)
			break;
		nColonPos = szColonPos - temp + 1;
		if (nColonCount == 0)
			nColPos = nColonPos;
		nColonCount++;
	}

	if (nColonCount != 1)
		ASSERT(0);

	char szMin[8], szSec[8];
	strncpy_s(szMin, str, nColPos - 1);
	szMin[nColPos - 1] = 0;
	strncpy_s(szSec, str + nColPos, strlen(str) - nColPos + 1);

	sscanf_s(szMin, "%f", &fVal1);
	sscanf_s(szSec, "%f", &fVal2);
	return TRUE;
}

BOOL SplitS(char* str, char c, string& s1, string& s2)
{
	if (strchr(str, c) == 0)
		return FALSE;

	int nColonCount = 0, i = 0, nColonPos = 0, nColPos = 0;
	char* temp = (char*)str;
	char* szColonPos;
	while (1)
	{
		temp += nColonPos;
		szColonPos = strchr(temp, c);
		if (szColonPos == NULL)
			break;
		nColonPos = szColonPos - temp + 1;
		if (nColonCount == 0)
			nColPos = nColonPos;
		nColonCount++;
	}

	if (nColonCount != 1)
		ASSERT(0);

	char szMin[0x40], szSec[0x40];
	memcpy(szMin, str, nColPos - 1);
	szMin[nColPos - 1] = 0;
	memcpy(szSec, str + nColPos, strlen(str) - nColPos + 1);
	::rtrim(szSec, '\n');

	s1 = szMin;
	s2 = szSec;
	return TRUE;
}

int GetGameCategory(char* szGameCategory)
{
	int nCount = sizeof(g_szGameCategory)/4;
	for (int i = 0; i < nCount; i++)
	{
		if (_strcmpi(szGameCategory, g_szGameCategory[i]) == 0)
			return i;
	}
	return -1;
}

CString GetGameCategory(int nGameCategory)
{
	return g_wszGameCategory[nGameCategory];
}

CString F2SW(float f)
{
	CString s;
	s.Format(_T("%.2f"), f);
	s.TrimRight(_T('0'));
	s.TrimRight(_T('.'));
	//s.Format(_T("%.2lf"), f);
	return s;
}

CStringA F2SA(float f)
{
	CStringA s;
	s.Format("%.2f", f);
	s.TrimRight('0');
	s.TrimRight('.');
	//s.Format(_T("%.2lf"), f);
	return s;
}

CString MakeHandicapLabel(COddsInfo* pOddsInfo)
{
	CString str = _T("");
	if (pOddsInfo->m_nOddsInfoKind != OI_HANDICAP)
		return str;
	if (pOddsInfo->m_hVal1 >= 0)
		str.Format(_T("+%s-"), F2SW(abs(pOddsInfo->m_hVal1)));
	else if (pOddsInfo->m_hVal1 < 0)
		str.Format(_T("-%s+"), F2SW(abs(pOddsInfo->m_hVal1)));
	return str;
}

CString MakeGoalLabel(COddsInfo* pOddsInfo)
{
	CString str = _T("");
	if (pOddsInfo->m_nOddsInfoKind != OI_GOAL_OU && pOddsInfo->m_nOddsInfoKind != OI_TEAM1_GOAL_OU && pOddsInfo->m_nOddsInfoKind != OI_TEAM2_GOAL_OU)
		return str;
	str.Format(_T("%s"), F2SW(pOddsInfo->m_hVal1));

	return str;
}

char *ltrim(char *s, char c)
{
	while (*s == c) s++;
	return s;
}

char *ltrim(char *s, char* c)
{
	int n = strlen(c);
	for (int i = 0; i < n; i++)
	{
		s = ltrim(s, c[i]);
	}
	return s;
}

char *rtrim(char *s, char c)
{
	char* back;
	int len = strlen(s);

	if (len == 0)
		return(s);

	back = s + len;
	while (*--back == c);
	*(back + 1) = '\0';
	return s;
}

char *rtrim(char *s, char* c)
{
	int n = strlen(c);
	for (int i = 0; i < n; i++)
	{
		s = rtrim(s, c[i]);
	}
	return s;
}

char *trim(char *s, char c)
{
	return rtrim(ltrim(s, c), c);
}

char *trim(char *s, char* c)
{
	int n = strlen(c);
	for (int i = 0; i < n; i++)
	{
		s = trim(s, c[i]);
	}
	return s;
}

BOOL IsEqualF(float f1, float f2)
{
	if (int(f1 * 100) == int(f2 * 100))
		return TRUE;
	return FALSE;
}

void SetClipboard(CStringA textToclipboard)
{
	if (OpenClipboard(NULL)){

		EmptyClipboard();
		HGLOBAL hClipboardData;
		int nStrLen = textToclipboard.GetLength();
		char* szText = (LPSTR)(LPCSTR)textToclipboard;
		TCHAR* wszText = new TCHAR[nStrLen + 1];
		
		size_t size = (nStrLen + 1) * sizeof(TCHAR);
		hClipboardData = GlobalAlloc(NULL, size);
		TCHAR* pchData = (TCHAR*)GlobalLock(hClipboardData);
		
		::ToUnicode(szText, wszText, CP_ACP);
		memcpy(pchData, wszText, size);
		
		SetClipboardData(CF_UNICODETEXT, hClipboardData);
		GlobalUnlock(hClipboardData);
		CloseClipboard();

		delete wszText;
	}
}

CString GetClipboard()
{
	CString str;
	str.Empty();
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
		return str;

	if (!OpenClipboard(NULL))
		return str;

	HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
	if (hMem == NULL)
	{
		CloseClipboard();
		return str;
	}
	LPTSTR lpStr = (LPTSTR)GlobalLock(hMem);
	if (lpStr == NULL)
	{
		CloseClipboard();
		return str;
	}
	str = lpStr;
	GlobalUnlock(hMem);
	CloseClipboard();
	return str;
}

int GetImageNo(int nCat)
{
	return g_nCatImage[nCat];
}

void WritePrivateProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, int Value, LPCWSTR lpFileName)
{
	wchar_t   ValBuf[16];

	swprintf_s(ValBuf, sizeof(ValBuf), L"%i", Value);

	WritePrivateProfileStringW(lpAppName, lpKeyName, ValBuf, lpFileName);
}

void WritePrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, int Value, LPCSTR lpFileName)
{
	char   ValBuf[16];

	sprintf_s(ValBuf, sizeof(ValBuf), "%i", Value);

	WritePrivateProfileStringA(lpAppName, lpKeyName, ValBuf, lpFileName);
}

string GetStage(char* s, int& t)
{
	string s1 = s;

	if (s1 == "s1" || s1 == "1S" || s1 == "1  Set" || s1 == "1  set" || s1 == "1Set")
		return "1S";
	if (s1 == "s2" || s1 == "2S" || s1 == "2  Set" || s1 == "2  set" || s1 == "2Set")
		return "2S";
	if (s1 == "s3" || s1 == "3S" || s1 == "3  Set" || s1 == "3  set" || s1 == "3Set")
		return "3S";
	if (s1 == "s4" || s1 == "4S" || s1 == "4  Set" || s1 == "4  set" || s1 == "4Set")
		return "4S";
	if (s1 == "s5" || s1 == "5S" || s1 == "5  Set" || s1 == "5  set" || s1 == "5Set")
		return "5S";
	if (s1 == "s6" || s1 == "6S" || s1 == "6  Set" || s1 == "6  set" || s1 == "6Set")
		return "6S";
	if (s1 == "s7" || s1 == "7S" || s1 == "7  Set" || s1 == "7  set" || s1 == "7Set")
		return "7S";
	if (s1 == "s8" || s1 == "8S" || s1 == "8  Set" || s1 == "8  set" || s1 == "8Set")
		return "8S";
	if (s1 == "Q1" || s1 == "1Q" || s1 == "1  Quarter")
		return "1Q";
	if (s1 == "Q2" || s1 == "2Q" || s1 == "2  Quarter")
		return "2Q";
	if (s1 == "Q3" || s1 == "3Q" || s1 == "3  Quarter")
		return "3Q";
	if (s1 == "Q4" || s1 == "4Q" || s1 == "4  Quarter")
		return "4Q";
	if (s1 == "1H" || s1 == "1  Half")
		return "1H";
	if (s1 == "2H" || s1 == "2  Half")
		return "2H";
	if (s1 == "H.T" || s1 == "HT" || s1 == "H.Time" || s1 == "H-Time")
		return "HT";
	if (s1 == "")
		return "";
	if (s1 == "1P" || s1 == "1  Period")
		return "1P";
	if (s1 == "2P" || s1 == "2  Period")
		return "2P";
	if (s1 == "3P" || s1 == "3  Period")
		return "3P";
	if (s1 == "4P" || s1 == "4  Period")
		return "4P";
	if (s1 == "i1" || s1 == "1I" || s1 == "1  innings" || s1 == "1  Inning")
		return "1I";
	if (s1 == "i2" || s1 == "2I" || s1 == "2  innings" || s1 == "2  Inning")
		return "2I";
	if (s1 == "i3" || s1 == "3I" || s1 == "3  innings" || s1 == "3  Inning")
		return "3I";
	if (s1 == "i4" || s1 == "4I" || s1 == "4  innings" || s1 == "4  Inning")
		return "4I";
	if (s1 == "i5" || s1 == "5I" || s1 == "5  innings" || s1 == "5  Inning")
		return "5I";
	if (s1 == "i6" || s1 == "6I" || s1 == "6  innings" || s1 == "6  Inning")
		return "6I";
	if (s1 == "i7" || s1 == "7I" || s1 == "7  innings" || s1 == "7  Inning")
		return "7I";
	if (s1 == "i8" || s1 == "8I" || s1 == "8  innings" || s1 == "8  Inning")
		return "8I";
	if (s1 == "i9" || s1 == "9I" || s1 == "9  innings" || s1 == "9  Inning")
		return "9I";
	if (s1 == "1  frame")
		return "1F";
	if (s1 == "2  frame")
		return "2F";
	if (s1 == "3  frame")
		return "3F";
	if (s1 == "4  frame")
		return "4F";
	if (s1 == "5  frame")
		return "5F";
	if (s1 == "6  frame")
		return "6F";
	if (s1 == "Live")
		return "Live";
	if (s1 == "ET")
		return "ET";
	if (s1 == "Pens")
		return "Pens";
	else
	{
		replace(s1, " ", "");
		replace(s1, "First", "1");
		replace(s1, "1st", "1");
		replace(s1, "Second", "2");
		replace(s1, "2nd", "2");
		replace(s1, "Third", "3");
		replace(s1, "3rd", "3");
		replace(s1, "Fourth", "4");
		replace(s1, "4th", "4");
		replace(s1, "Fifth", "5");
		replace(s1, "5th", "5");
		replace(s1, "6th", "6");
		replace(s1, "7th", "7");
		replace(s1, "8th", "8");
		replace(s1, "9th", "9");
		replace(s1, "10th", "10");
		replace(s1, "Half", "H");
		replace(s1, "-", "");
		replace(s1, "Time", "T");
		replace(s1, "Quarter", "Q");//Basketball, Water Polo
		replace(s1, "Set", "S");//Voleyball, Table Tennis, Tennis
		replace(s1, "Innings", "I");//Baseball
		replace(s1, "Inning", "I");//Baseball
		replace(s1, "Leg", "L");//Darts

		if (t == 45 * 60 && s1 == "2H")
			s1 = "HT";
	}
	return s1;
}

void SetListItemText(CListCtrl& listCtrl, int col, int row, LPCTSTR szText)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = row;
	lvItem.pszText = (TCHAR*)szText;
	lvItem.iSubItem = col;

	if (row < listCtrl.GetItemCount())
		listCtrl.SetItemText(row, col, szText);
	else
	{
		if (col == 0)
			ListView_InsertItem(listCtrl.GetSafeHwnd(), &lvItem);
		else
		{
			lvItem.pszText = _T("");
			lvItem.iSubItem = 0;
			ListView_InsertItem(listCtrl.GetSafeHwnd(), &lvItem);
			listCtrl.SetItemText(row, col, szText);
		}
	}
}

void PeekMessageLoop()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void WaitForThreadToTerminate(HANDLE hThread)
{
	DWORD dwRet;
	do
	{
		dwRet = ::MsgWaitForMultipleObjects(1, &hThread, FALSE, INFINITE, QS_ALLINPUT);
		if (dwRet != WAIT_OBJECT_0)
		{
			PeekMessageLoop();
		}
	} while ((dwRet != WAIT_OBJECT_0) && (dwRet != WAIT_FAILED));
}

void AddToArray(CArrayGameInfo& aryGameInfo, CGameInfo* pGameInfo)
{
	for (int i = 0; i < aryGameInfo.GetSize(); i++)
	{
		if (aryGameInfo[i] == NULL)
		{
			aryGameInfo[i] = pGameInfo;
			return;
		}
	}
	aryGameInfo.Add(pGameInfo);
}

void GetEncPswd(char* szPswd, char* szPswd1)
{
	if (strcmp(szPswd, "") == 0)
	{
		*szPswd1 = 0;
		return;
	}
	char sz[3];
	sz[2] = 0;
	*szPswd1 = 0;
	for (int i = 0; i < (int)strlen(szPswd); i++)
	{
		sprintf_s(sz, "%2X", 0xFF - szPswd[i]);
		strcat_s(szPswd1, MAX_ENC_PSWD, sz);
	}
}

void GetDecPswd(char* szPswd, char* szPswd1)
{
	if (strcmp(szPswd, "") == 0)
	{
		*szPswd1 = 0;
		return;
	}
	char sz[3];
	sz[2] = 0;
	*szPswd1 = 0;
	int c;
	for (int i = 0; i < (int)strlen(szPswd); i+=2)
	{
		memcpy(sz, szPswd + i, 2);
		sscanf_s(sz, "%2X", &c);
		sprintf_s(sz, "%c", 0xFF - c);
		strcat_s(szPswd1, MAX_PSWD, sz);
	}
}

void WriteResData(TCHAR* szFile, char* szText)
{
	FILE* fp;
	_wfopen_s(&fp, szFile, _T("wb"));
	if (fp)
	{
		fwrite(szText, 1, strlen(szText), fp);
		fclose(fp);
	}
}

BOOL IsMasterName(char* name)
{
	CStringA name1;
	if (!g_mapName.Lookup(name, name1))
	{
		if (name1 == name)
			return TRUE;
	}
	return FALSE;
}

BOOL IsSlaveName(char* name)
{
	CStringA name1;
	if (!g_mapName.Lookup(name, name1))
	{
		if (name1 != name)
			return TRUE;
	}
	return FALSE;
}

void SetMasterName(char* name)
{
	g_mapName.SetAt(name, name);
}

void SetSlaveName(char* slave, char* master)
{
	g_mapName.SetAt(slave, master);
}

void CopyGameInfo(CArrayGameInfo& aryGameInfo1, CArrayGameInfo& aryGameInfo2)
{
	aryGameInfo2.RemoveAll();
	for (int i = 0; i < aryGameInfo1.GetSize(); i++)
	{
		CGameInfo* pGameInfo = new CGameInfo(aryGameInfo1[i]);
		aryGameInfo2.Add(pGameInfo);
	}
}

DWORD GetTimeStamp()
{
	return (DWORD)time(NULL);
}

CStringA GetTimeStampA()
{
	CStringA s;
	s.Format("%llu", (UINT64)time(NULL));
	return s;
}

CString GetTimeStampW()
{
	CString s;
	s.Format(_T("%llu"), (UINT64)time(NULL));
	return s;
}

CStringA GetTimeStampLongA()
{
	CStringA s;
	srand((DWORD)time(NULL));

	SYSTEMTIME time1;
	GetSystemTime(&time1);

	s.Format("%llu%.3d", (UINT64)time(NULL), time1.wMilliseconds);
	return s;
}

CString GetTimeStampLongW()
{
	CString s;
	srand((DWORD)time(NULL));

	SYSTEMTIME time1;
	GetSystemTime(&time1);

	s.Format(_T("%llu%.3d"), (UINT64)time(NULL), time1.wMilliseconds);
	return s;
}

CGameData* GetGameID(CArrayGameInfo& aryGameInfo, int nCategory, int nBookMaker, char* szHTeam, char* szATeam)
{
	for (int i = 0; i < aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pGameInfo = aryGameInfo[i];
		if (pGameInfo == NULL || pGameInfo->m_MainInfo.m_nGameCategory != nCategory)
			continue;
		CGameData* pGameData = pGameInfo->GetGameData(nBookMaker);
		if (pGameData == NULL)
			continue;
		if (!strstr(pGameInfo->m_MainInfo.m_szHTName, szHTeam) || !strstr(pGameInfo->m_MainInfo.m_szATName, szATeam))
			continue;
		return pGameData;
	}
	return NULL;
}

CGameData* GetGameData(BetInfo* pBetInfo, int nBetObjNo)
{
	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pGameInfo = g_aryGameInfo[i];
		if (pGameInfo == NULL || pGameInfo->m_MainInfo.m_nGameCategory != pBetInfo->nCategory)
			continue;
		CGameData* pGameData = pGameInfo->GetGameData(pBetInfo->BetVal[nBetObjNo].nBookMaker);
		if (pGameData == NULL)
			continue;
		if (pGameData->m_nGameID == pBetInfo->BetVal[nBetObjNo].dwGameID)
			return pGameData;
	}
	return NULL;
}

CGameInfo* GetGameInfoFromBetInfo(BetInfo* pBetInfo, int nBetObjNo)
{
	CGameData* pGameData = NULL;
	if (!g_site[pBetInfo->BetVal[nBetObjNo].nBookMaker]->m_mapGameID.Lookup(pBetInfo->BetVal[nBetObjNo].dwGameID, pGameData))
	{
		pGameData = GetGameData(pBetInfo, nBetObjNo);
	}
	if (pGameData == NULL)
		return NULL;
	return pGameData->m_pGameInfo;
}

BOOL GetSessionValue(TCHAR* buffer, TCHAR* szSession, TCHAR* szVal)
{
	TCHAR* szPos1 = _tcsstr(buffer, szSession);
	if (szPos1 == NULL)
		return FALSE;
	TCHAR* szPos2 = _tcschr(szPos1, _T(';'));
	if (szPos2 == NULL)
		return FALSE;
	int nLen1 = _tcslen(szSession);
	int nLen = szPos2 - szPos1 - nLen1;
	memcpy(szVal, szPos1 + nLen1, nLen * 2);
	szVal[nLen] = 0;
	return TRUE;
}

BOOL GetSessionValue(TCHAR* buffer, TCHAR* szSession1, TCHAR* szSession, TCHAR* szVal)
{
	TCHAR* szPos1 = _tcsstr(buffer, szSession1);
	if (szPos1 == NULL)
		return FALSE;
	TCHAR* szPos3 = _tcschr(szPos1, _T('='));
	if (szPos3 == NULL)
		return FALSE;
	TCHAR* szPos2 = _tcschr(szPos3, _T(';'));
	if (szPos2 == NULL)
		return FALSE;
	int nLen1 = szPos3 - szPos1 + 1;
	int nLen = szPos2 - szPos1 - nLen1;
	memcpy(szSession, szPos1, (nLen1 - 1) * 2);
	szSession[nLen1 - 1] = 0;
	memcpy(szVal, szPos1 + nLen1, nLen * 2);
	szVal[nLen] = 0;
	return TRUE;
}

BOOL IsExceptionGame(char* ht, char* at, char* lg, char* stage)
{
	if (strlen(ht) >= MAX_TEAM_NAME || strlen(at) >= MAX_TEAM_NAME)
		return TRUE;
	if ((strstr(ht, " Corner") != 0 || strstr(at, " Corner") != 0) ||
		(strstr(ht, "Handicap") != 0 || strstr(at, "Handicap") != 0) ||
		(strstr(ht, "Quarter") != 0 || strstr(at, "Quarter") != 0) ||
		(strstr(ht, "(ET)") != 0 || strstr(at, "(ET)") != 0) ||
		(strstr(ht, "(PEN)") != 0 || strstr(at, "(PEN)") != 0) ||
		(strstr(ht, ":") != 0 || strstr(at, ":") != 0) ||
		(strstr(lg, "Corner") != 0))
		return TRUE;
	if (strcmp(stage, "ET") == 0 || strcmp(stage, "Pens") == 0)
		return TRUE;
	return FALSE;
}

DWORD GetRelativeTime(SYSTEMTIME time1, SYSTEMTIME time2)
{
	FILETIME ft;
	ULARGE_INTEGER ularge;
	__int64 t1, t2;

	SystemTimeToFileTime(&time1, &ft);
	ularge.LowPart = ft.dwLowDateTime;
	ularge.HighPart = ft.dwHighDateTime;
	t1 = ularge.QuadPart;

	SystemTimeToFileTime(&time2, &ft);
	ularge.LowPart = ft.dwLowDateTime;
	ularge.HighPart = ft.dwHighDateTime;
	t2 = ularge.QuadPart;

	return DWORD((t1 - t2) / 10000000);
}

string PlusProc(char* s)
{
	char ss[0x100];
	for (UINT i = 0; i < strlen(s) + 1; i++)
	{
		if (s[i] == ' ')
			ss[i] = '+';
		else
			ss[i] = s[i];
	}
	return ss;
}

string GenUUID()
{
	char i;
	int index;
	char s[37], dec2hex[17];
	s[36] = 0;
	dec2hex[16] = 0;

	srand((unsigned int)time(NULL));

	for (i = 0; i < 16; i++)
	{
		if (i < 10)
			dec2hex[i] = 0x30 + i;
		else
			dec2hex[i] = 'a' + i - 10;
	}

	for (i = 0; i < 36; i++)
	{
		if (i == 8 || i == 13 || i == 18 || i == 23)
			s[i] = '-';
		else if (i == 14)
			s[i] = '4';
		else if (i == 20)
		{
			index = (rand() % 4) | 8;
			s[i] = dec2hex[index];
		}
		else
		{
			index = (rand() % 15);
			s[i] = dec2hex[index];
		}
	}
	return s;
}

BOOL GetOddVal(char* str, string& sHval, float& fOval)
{//"<i class=\"spread\">0-0.5</i><i class=\"odd\">2.09</i>"
 //"<i class=\"spread\"></i><i class=\"odd\">1.70</i>"
	char szHval[30];
	char szOval[30];
	memset(szHval, 0, 30);
	memset(szOval, 0, 30);
	sHval = "";
	fOval = -1;

	if (strcmp(str, "") == 0)
		return FALSE;

	if (strstr(str, "no_spread"))
	{
		char* szSpan = "\">";
		char* szSpan1 = "</i>";
		char* sz1_s = strstr(str, szSpan);
		char* sz1_e = strstr(str, szSpan1);
		if (sz1_s == NULL || sz1_e == NULL)
			ASSERT(0);
		memcpy(szOval, sz1_s + 2, sz1_e - sz1_s - 2);
		sscanf_s(szOval, "%f", &fOval);
	}
	else
	{
		char* szSpan = "\">";
		char* szSpan1 = "</i>";
		char* sz1_s = strstr(str, szSpan);
		char* sz1_e = strstr(str, szSpan1);
		if (sz1_s == NULL || sz1_e == NULL)
			ASSERT(0);
		memcpy(szHval, sz1_s + 2, sz1_e - sz1_s - 2);
		sHval = szHval;

		char* sz2_s = strstr(sz1_e + 4, szSpan);
		char* sz2_e = strstr(sz1_e + 4, szSpan1);
		if (sz2_s == NULL || sz2_e == NULL)
			ASSERT(0);
		memcpy(szOval, sz2_s + 2, sz2_e - sz2_s - 2);

		sscanf_s(szOval, "%f", &fOval);
	}
	return TRUE;
}

string GetStageMin(char* str, int& nMin)
{//"<span class=\"live_game_period\">2H</span> <span class=\"live_game_minute\">2</span>'"
 //"<span class=\"live_game_period\">2S</span>",
	char szStage[30];
	char szMin[5];
	memset(szStage, 0, 30);
	memset(szMin, 0, 5);
	nMin = 0;

	char* szSpan = "\">";
	char* szSpan1 = "</span>";
	char* sz1_s = strstr(str, szSpan);
	char* sz1_e = strstr(str, szSpan1);
	if (sz1_s == NULL || sz1_e == NULL)
		return "";
	memcpy(szStage, sz1_s + 2, sz1_e - sz1_s - 2);

	char* sz2_s = strstr(sz1_e + 7, szSpan);
	char* sz2_e = strstr(sz1_e + 7, szSpan1);
	if (sz2_s == NULL || sz2_e == NULL)
		return szStage;
	memcpy(szMin, sz2_s + 2, sz2_e - sz2_s - 2);
	sscanf_s(szMin, "%d", &nMin);
	nMin *= 60;

	return string(szStage);
}

string GetTeamName(char* szName)
{
	int nStrLen = strlen(szName);
	char p[0x200];
	memcpy(p, szName, nStrLen + 1);

	if (strstr(p, "<i class=") == p)
	{
		char* p1 = strstr(p, ">");
		if (p1 == NULL)
			ASSERT(0);
		strcpy_s(p, nStrLen, p1 + 1);
		p1 = strstr(p, "</i>");
		if (p1 == NULL)
			ASSERT(0);
		*p1 = 0;
		return string(p);
	}

	char* pos = strstr(p, "<span");
	if (pos != NULL)
	{
		*pos = 0;
		return string(p);
	}
	return "";
}

void GetScore(char* str, int& hs, int& as)
{//"<span class=\"score row \"> <span>2</span><span>:</span><span>0</span> </span>"
	char szScore[5];
	memset(szScore, 0, 5);
	char* szSpan = "<span>";
	char* szSpan1 = "</span>";
	hs = -1;
	as = -1;
	char* sz1_s = strstr(str, szSpan);
	char* sz1_e = strstr(str, szSpan1);
	if (sz1_s == NULL || sz1_e == NULL)
		ASSERT(0);
	memcpy(szScore, sz1_s + 6, sz1_e - sz1_s - 6);
	sscanf_s(szScore, "%d", &hs);

	char* sz2_s = strstr(sz1_e + 7, szSpan);
	if (sz2_s[6] != ':')
		ASSERT(0);
	char* sz2_e = strstr(sz1_e + 7, szSpan1);

	char* sz3_s = strstr(sz2_e + 7, szSpan);
	char* sz3_e = strstr(sz2_e + 7, szSpan1);
	if (sz3_s == NULL || sz3_e == NULL)
		ASSERT(0);
	memcpy(szScore, sz3_s + 6, sz3_e - sz3_s - 6);
	sscanf_s(szScore, "%d", &as);
}

