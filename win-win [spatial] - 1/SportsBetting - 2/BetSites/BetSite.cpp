#include "..\stdafx.h"
#include "..\SportsBetting.h"
#include "BetSite.h"
#include "BetSite_188bet.h"
#include "..\global.h"
#include "..\MainFrm.h"
#include "..\GameData.h"
#include <zlib.h>
#include <gzip.h>
using namespace zlib;

CBetSite::CBetSite()
{
	InitializeCriticalSection(&m_csThread);
	InitializeCriticalSection(&m_csBetObjCount);
	m_HttpFile = NULL;
	m_HttpFileBet = NULL;

	m_nGZipSize = 0;
	m_nTextSize = 0;
	m_nGZipSizeBet = 0;
	m_nTextSizeBet = 0;

	m_nReqKindID = 0;
	m_nReqNo = 0;

	m_bufRead = new BYTE[READ_SIZE];
	m_bufGZip = new BYTE[GZIP_SIZE];
	m_szText = new char[TEXT_SIZE];

	m_bufReadBet = new BYTE[READ_SIZE];
	m_bufGZipBet = new BYTE[GZIP_SIZE];
	m_szTextBet = new char[TEXT_SIZE];

	m_nBookMaker = 0;

	m_bDoLogOut = FALSE;
	m_bExitThreadFlg = FALSE;
	m_hThread = NULL;

	m_nTimePeriod = 3000;
	m_nBettingTimePeriod = 1000;

	strcpy_s(m_szID, "");
	strcpy_s(m_szPswd, "");
	strcpy_s(m_szCaptcha, "");

	_tcscpy_s(m_szSession, _T(""));

	m_nLogInStatus = LS_INIT;
	
	m_mapGameID.RemoveAll();

	_tcscpy_s(m_szSession, _T(""));

	m_uuLastBettingTime = 0;
	m_nTimeOut = 2000;

	CString strSession;
	strSession.Format(_T("Main Session %d"), m_nBookMaker);
	m_pSession = new CInternetSession(strSession);
	m_pSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, m_nTimeOut);
	strSession.Format(_T("Bet Session %d"), m_nBookMaker);
	m_pSessionBet = new CInternetSession(strSession);
	m_pSessionBet->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, m_nTimeOut);
}

CBetSite::~CBetSite()
{
	m_pSession->Close();
	delete m_pSession;
	m_pSession = NULL;

	m_pSessionBet->Close();
	delete m_pSessionBet;
	m_pSessionBet = NULL;

	delete m_bufRead;
	delete m_bufGZip;
	delete m_szText;
	delete m_bufReadBet;
	delete m_bufGZipBet;
	delete m_szTextBet;

	m_mapGameID.RemoveAll();
	ExitThread();
	DeleteCriticalSection(&m_csThread);
	DeleteCriticalSection(&m_csBetObjCount);
}

void CBetSite::ReadResponse()
{
	m_nTextSize = ReadResponse(m_HttpFile, m_bufGZip, m_bufRead, m_szText);
}

int CBetSite::ReadResponse(CHttpFile* HttpFile, BYTE* bufGZip, BYTE* bufRead, char* szText)
{
	int nBytesRead;
	int nGZipSize = 0;

	while ((nBytesRead = HttpFile->Read(bufRead, READ_SIZE)) > 0)
	{
		memcpy_s(bufGZip + nGZipSize, GZIP_SIZE - nGZipSize, bufRead, nBytesRead);
		nGZipSize += nBytesRead;
	}

	int nTextSize = 0;
	if (*(DWORD*)bufGZip == 0x00088B1F)
	{
		int size = UnzipStream((char*)bufGZip, nGZipSize, szText, TEXT_SIZE);
		szText[size] = 0;

		nTextSize = size;
	}
	else
	{
		memcpy(szText, bufGZip, nGZipSize);
		nTextSize = nGZipSize;
		szText[nTextSize] = 0;
	}
	return nTextSize;
}

void CBetSite::AnalyzeResText()
{
}

void CBetSite::StartThread(char* szID, char* szPswd)
{
	strcpy_s(m_szID, szID);
	strcpy_s(m_szPswd, szPswd);
	m_bExitThreadFlg = FALSE;
	m_bDoLogOut = FALSE;
	m_nLogInStatus = LS_INIT;
	m_hThread = ::CreateThread(NULL, 0, ::ThreadProc, (LPVOID)m_nBookMaker, 0, NULL);
}

void CBetSite::ExitThread()
{
	m_bDoLogOut = TRUE;

	WaitForThreadToTerminate(m_hThread);

	m_hThread = NULL;
}

void CBetSite::CreateSubThread()
{
}

DWORD CBetSite::ThreadProc()
{
	CreateSubThread();

	FILETIME filetime1, filetime2;
	ULONGLONG time1, time2, time_prev = 0;

	while (m_bExitThreadFlg == FALSE)
	{
		GetSystemTimeAsFileTime(&filetime1);

		EnterCriticalSection(&m_csThread);
		MainProc();
		LeaveCriticalSection(&m_csThread);

		GetSystemTimeAsFileTime(&filetime2);

		time1 = (((ULONGLONG)filetime1.dwHighDateTime) << 32) + filetime1.dwLowDateTime;
		time2 = (((ULONGLONG)filetime2.dwHighDateTime) << 32) + filetime2.dwLowDateTime;

		int nElapseTime = (int)((time2 - time1) / 10000);
		if (nElapseTime < m_nTimePeriod)
			Sleep(m_nTimePeriod - nElapseTime);

		//OutDbgStrAA("\nELAPSED TIME IN MS:%d, %d", (int)((time2 - time1) / 10000), (int)((time1 - time_prev) / 10000));
		time_prev = time1;
	}

	theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)0);
	theFrame->m_wndSetting.PostMessageW(MSG_CUR_BALANCE, (WPARAM)m_nBookMaker, (LPARAM)-1);
	return 0;
}

DWORD WINAPI ThreadProc(PVOID pParam)
{
	int nSite = (int)pParam;
	CBetSite* pSite = NULL;

	pSite = g_site[nSite];
	pSite->ThreadProc();

	return 0;
}

int CBetSite::MainProc()
{
	return 0;
}

int CBetSite::GetGameCategory(char* szGameCategory)
{
	return ::GetGameCategory(szGameCategory);
}

CString CBetSite::GetGameCategory(int nGameCategory)
{
	return ::GetGameCategory(nGameCategory);
}

BOOL CBetSite::UpdateTree(int nIsExitThread)
{
	CString str;
	HTREEITEM hItemCategory;
	for (int nCate = 0; nCate < GC_COUNT; nCate++)
	{
		int nChildCount = 0;
		hItemCategory = theFrame->m_hItemCategory[nCate];
		if (!theFrame->m_bSetCategory[nCate])
		{
			str.Format(_T("%s(0)"), ::GetGameCategory(nCate));
			CString str1 = theFrame->m_treeGameInfo->GetItemText(hItemCategory);
			if (str != str1)
				theFrame->m_treeGameInfo->SetItemText(hItemCategory, str);
			continue;
		}

		if (theFrame->m_treeGameInfo->ItemHasChildren(hItemCategory))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = theFrame->m_treeGameInfo->GetChildItem(hItemCategory);

			while (hChildItem != NULL)
			{
				hNextItem = theFrame->m_treeGameInfo->GetNextItem(hChildItem, TVGN_NEXT);
				nChildCount++;
				hChildItem = hNextItem;
			}
			str.Format(_T("%s(%d)"), ::GetGameCategory(nCate), nChildCount);
		}
		else
			str.Format(_T("%s(0)"), ::GetGameCategory(nCate));

		CString str1 = theFrame->m_treeGameInfo->GetItemText(hItemCategory);
		if (str != str1)
			theFrame->m_treeGameInfo->SetItemText(hItemCategory, str);
	}
	return TRUE;
}

BOOL CBetSite::UpdateTree_AllInfoDownAtOnce(int nIsExitThread)
{
	int nCate;
	BOOL bRet = FALSE;
	HTREEITEM hItemCategory;
	CGameInfo* pGameInfo = NULL;
	CGameData* pGameData = NULL;

	for (nCate = 0; nCate < GC_COUNT; nCate++)
	{
		hItemCategory = theFrame->m_hItemCategory[nCate];
		if (!theFrame->m_treeGameInfo->ItemHasChildren(hItemCategory))
			continue;

		HTREEITEM hNextItem;
		HTREEITEM hChildItem = theFrame->m_treeGameInfo->GetChildItem(hItemCategory);

		while (hChildItem != NULL)
		{
			hNextItem = theFrame->m_treeGameInfo->GetNextItem(hChildItem, TVGN_NEXT);

			pGameInfo = (CGameInfo*)theFrame->m_treeGameInfo->GetItemData(hChildItem);
			if (pGameInfo == NULL)
				ASSERT(0);

			if (!theFrame->m_bSetCategory[nCate] || theFrame->m_wndTeamName.IsExistItem(pGameInfo->m_MainInfo))
			{
				pGameInfo->m_hTreeItem = NULL;
				if (hChildItem == theFrame->m_hCurrentSelGame)
					bRet = TRUE;
				theFrame->m_treeGameInfo->DeleteItem(hChildItem);
			}
			else
			{
				pGameData = pGameInfo->GetGameData(m_nBookMaker);
				if (pGameData != NULL)
				{
					SYSTEMTIME nCurTime;
					GetLocalTime(&nCurTime);
					if (GetRelativeTime(nCurTime, pGameData->m_nLastTime) > REFRESH_TIME_SECOND)
						pGameData->m_bDownInfo = FALSE;
					if (!pGameData->m_bDownInfo || nIsExitThread == THREAD_STOP)
					{
						//OutDbgStrAA("\nDELETE TREE --------- %s : %s", pGameData->m_MainInfo.m_szHTName, pGameData->m_MainInfo.m_szATName);
						if (pGameInfo->m_aryGameData.GetSize() == 1)
						{
							pGameInfo->m_hTreeItem = NULL;
							if (hChildItem == theFrame->m_hCurrentSelGame)
								bRet = TRUE;
							theFrame->m_treeGameInfo->DeleteItem(hChildItem);
						}
					}
				}
			}
			hChildItem = hNextItem;
		}
	}
	TCHAR szHTName[MAX_TEAM_NAME], szATName[MAX_TEAM_NAME], szStage[MAX_STAGE_NAME];
	CString strGameName;

	//OutDbgStrAA("\n===========================");
	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		::EnterCriticalSection(&g_csGameInfoArray);
		CGameInfo* pGameInfo = g_aryGameInfo[i];
		if (pGameInfo == NULL)
		{
			::LeaveCriticalSection(&g_csGameInfoArray);
			continue;
		}
		//OutDbgStrAA("\n%s/%s", pGameInfo->m_MainInfo.m_szHTName, pGameInfo->m_MainInfo.m_szATName);
		if (!theFrame->m_bSetCategory[pGameInfo->m_MainInfo.m_nGameCategory] ||
			(pGameInfo->m_hTreeItem == NULL && pGameInfo->m_hTreeParentItem != NULL) || // removed item in tree
			theFrame->m_wndTeamName.IsExistItem(pGameInfo->m_MainInfo))
		{
			//OutDbgStrAA("\nDELETE pGameInfo --------- %s : %s", pGameInfo->m_MainInfo.m_szHTName, pGameInfo->m_MainInfo.m_szATName);
			delete pGameInfo;
			g_aryGameInfo[i] = NULL;
			::LeaveCriticalSection(&g_csGameInfoArray);
			continue;
		}

		int n = pGameInfo->GetGameDataNo(m_nBookMaker);
		if (n == -1)
		{
			::LeaveCriticalSection(&g_csGameInfoArray);
			continue;
		}
		CGameData* pGameData = pGameInfo->m_aryGameData[n];
		if (!pGameData->m_bDownInfo || nIsExitThread == THREAD_STOP)
		{
			//OutDbgStrAA("\nDELETE pGameData --------- %s : %s", pGameData->m_MainInfo.m_szHTName, pGameData->m_MainInfo.m_szATName);
			pGameInfo->m_aryGameData.RemoveAt(n);
			delete pGameData;
		}
		else if (!nIsExitThread)
			pGameData->m_bDownInfo = FALSE;
		::LeaveCriticalSection(&g_csGameInfoArray);

		::ToUnicode(pGameInfo->m_MainInfo.m_szHTName, szHTName, CP_ACP);
		::ToUnicode(pGameInfo->m_MainInfo.m_szATName, szATName, CP_ACP);
		::ToUnicode(pGameInfo->m_MainInfo.m_szStage, szStage, CP_ACP);
//		strGameName.Format(_T("%s:%s(%d:%d, %.2d:%.2d(%s))"), szHTName, szATName,
//			pGameInfo->m_HTScore.nScore, pGameInfo->m_ATScore.nScore, pGameInfo->m_nTime / 60, pGameInfo->m_nTime % 60, szStage);
		int score1 = !pGameData->m_bHAChanged ? pGameInfo->m_MainInfo.m_HTScore.nScore : pGameInfo->m_MainInfo.m_ATScore.nScore;
		int score2 = !pGameData->m_bHAChanged ? pGameInfo->m_MainInfo.m_ATScore.nScore : pGameInfo->m_MainInfo.m_HTScore.nScore;
		strGameName.Format(_T("%s:%s(%d:%d)"), szHTName, szATName, pGameInfo->m_MainInfo.m_HTScore.nScore, pGameInfo->m_MainInfo.m_ATScore.nScore);

		for (int i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
		{
			if (pGameInfo->m_aryGameData[i]->m_bHAChanged)
				strGameName += _T(" *****");
		}

		pGameInfo->m_hTreeParentItem = theFrame->m_hItemCategory[pGameInfo->m_MainInfo.m_nGameCategory];
		if (pGameInfo->m_hTreeItem)
		{
			CString str1 = theFrame->m_treeGameInfo->GetItemText(pGameInfo->m_hTreeItem);
			if (strGameName != str1)
				theFrame->m_treeGameInfo->SetItemText(pGameInfo->m_hTreeItem, strGameName);
		}
		else
		{
			::EnterCriticalSection(&g_csGameInfoArray);
			HTREEITEM hPrev = theFrame->m_treeGameInfo->GetInsertPos(strGameName, pGameInfo->m_hTreeParentItem);
			int nImageNo = GetImageNo(pGameInfo->m_MainInfo.m_nGameCategory);
			pGameInfo->m_hTreeItem = theFrame->m_treeGameInfo->InsertItem(strGameName, nImageNo, nImageNo, pGameInfo->m_hTreeParentItem, hPrev);
			theFrame->m_treeGameInfo->SetItemData(pGameInfo->m_hTreeItem, (DWORD_PTR)pGameInfo);
			::LeaveCriticalSection(&g_csGameInfoArray);
		}
	}

	return bRet;
}

void CBetSite::CopyGameDataProc(CGameData* pGameData)
{
	memcpy(&pGameData->m_nBeforeTime, &pGameData->m_nLastTime, sizeof(SYSTEMTIME));
	GetLocalTime(&pGameData->m_nLastTime);

	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pGameInfo = g_aryGameInfo[i];
		if (pGameInfo == NULL)
			continue;
		if (strcmp(pGameInfo->m_MainInfo.m_szHTName, pGameData->m_pGameInfo->m_MainInfo.m_szHTName) == 0 && 
			strcmp(pGameInfo->m_MainInfo.m_szATName, pGameData->m_pGameInfo->m_MainInfo.m_szATName) == 0)
		{
			//if (strstr(pGameData->m_MainInfo.m_szHTName, "Reykjavik") && strstr(pGameData->m_MainInfo.m_szATName, "Reykjavik"))
			//	_asm int 3
			CGameData* pGameData1 = pGameInfo->GetGameData(pGameData->m_nBookMaker);
			if (pGameData1 == NULL)
			{
				pGameData1 = new CGameData(pGameData);
				pGameInfo->m_aryGameData.Add(pGameData1);
				pGameInfo->m_MainInfo = pGameData->m_pGameInfo->m_MainInfo;
				break;
			}
			if (pGameData1->m_nGameID != pGameData->m_nGameID)
				continue;
			pGameInfo->m_MainInfo = pGameData->m_pGameInfo->m_MainInfo;
			*pGameData1 = *pGameData;
			break;
		}
	}

	delete pGameData->m_pGameInfo;
	pGameData->m_pGameInfo = NULL;
}

CGameInfo* CBetSite::CheckGameInfo(int nBookMaker, CMainInfo* pMainInfo, BOOL& bSameOrSimilar, BOOL& bHAChanged, int nEventID)
{
	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pGameInfo = g_aryGameInfo[i];
		if (pGameInfo == NULL)
			continue;
		if (pGameInfo->m_MainInfo.m_nGameCategory != pMainInfo->m_nGameCategory)
			continue;
		CGameData* pGameData = pGameInfo->GetGameData(nBookMaker);
		if (strcmp(pGameInfo->m_MainInfo.m_szHTName, pMainInfo->m_szHTName) == 0 &&
			strcmp(pGameInfo->m_MainInfo.m_szATName, pMainInfo->m_szATName) == 0)
		{
			if (pGameData != NULL)
			{
				if (strcmp(pGameData->m_MainInfo.m_szHTName, pMainInfo->m_szHTName) == 0 &&
					strcmp(pGameData->m_MainInfo.m_szATName, pMainInfo->m_szATName) == 0)
				{
					_asm int 3
					g_site[nBookMaker]->m_mapGameID.SetAt(nEventID, pGameData);
					bSameOrSimilar = TRUE;
					bHAChanged = FALSE;
					return pGameData->m_pGameInfo;
				}
				if (strcmp(pGameData->m_MainInfo.m_szHTName, pMainInfo->m_szATName) == 0 &&
					strcmp(pGameData->m_MainInfo.m_szATName, pMainInfo->m_szHTName) == 0)
				{
					_asm int 3
					g_site[nBookMaker]->m_mapGameID.SetAt(nEventID, pGameData);
					bSameOrSimilar = TRUE;
					bHAChanged = TRUE;
					return pGameData->m_pGameInfo;
				}
				return NULL;
			}
			return NULL;
		}
	}
	return NULL;
}

CGameInfo* CBetSite::CreateGameInfo(CMainInfo* pMainInfo, int nEventID)
{
	CGameInfo* pGameInfo = new CGameInfo(pMainInfo);
	CGameData* pGameData = new CGameData(pGameInfo, m_nBookMaker, TRUE, FALSE);
	pGameData->m_nGameID = nEventID;
	pGameData->m_MainInfo = *pMainInfo;
	pGameInfo->m_aryGameData.Add(pGameData);
	pGameInfo->m_nSiteID = m_nBookMaker;

	::AddToArray(g_aryGameInfo, pGameInfo);
	if (nEventID != pGameData->m_nGameID)
		_asm int 3
	m_mapGameID.SetAt(nEventID, pGameData);

	//OutDbgStrAA("\n%s/%s", pMainInfo->m_szHTName, pMainInfo->m_szATName);
	if (pGameInfo->m_nSiteID == m_nBookMaker && pGameData->m_bHAChanged)
		_asm int 3
	return pGameInfo;
}

CGameInfo* CBetSite::GetGameInfo(int nBookMaker, CMainInfo& info, BOOL& bSameOrSimilar, BOOL& bHAChanged, int nEventID)
{
	bHAChanged = FALSE;
	bSameOrSimilar = FALSE;
	//	if (strstr(info.m_szHTName, "Besikt"))
	//		_asm int 3
	CGameData* pGameData0 = NULL;
	//if (nEventID == 1764083)
	//	_asm int 3
	BOOL bFound = g_site[nBookMaker]->m_mapGameID.Lookup(nEventID, pGameData0);
	if (bFound)
	{
		if (nEventID != pGameData0->m_nGameID)
			_asm int 3
		bSameOrSimilar = TRUE;
		bHAChanged = pGameData0->m_bHAChanged;
		return pGameData0->m_pGameInfo;
	}

	CGameInfo* pGameInfo = NULL;
	CGameInfo* pGameInfo1 = NULL;
	CGameData* pGameData2 = NULL;
	CGameInfo* pGameInfo3 = NULL;
	CGameInfo* pGameInfo4 = NULL;
	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		pGameInfo = g_aryGameInfo[i];
		if (pGameInfo == NULL || pGameInfo->m_MainInfo.m_nGameCategory != info.m_nGameCategory)
			continue;
		//if (strstr(pGameInfo->m_MainInfo.m_szHTName, "Team TVIS") != 0)
		//	_asm int 3
		pGameData2 = pGameInfo->GetGameData(nBookMaker);
		if (pGameData2 == NULL)
			continue;

		if (strcmp(pGameData2->m_MainInfo.m_szHTName, info.m_szHTName) == 0 &&
			strcmp(pGameData2->m_MainInfo.m_szATName, info.m_szATName) == 0 &&
			strcmp(pGameData2->m_MainInfo.m_szLeague, info.m_szLeague) == 0)
		{
			int iKey;
			CGameData* pGameData1;
			POSITION pos = g_site[nBookMaker]->m_mapGameID.GetStartPosition();
			while (pos != NULL)
			{
				g_site[nBookMaker]->m_mapGameID.GetNextAssoc(pos, iKey, pGameData1);
				if (pGameData2 == pGameData1)
				{
					pGameData1->m_nGameID = nEventID;
					g_site[nBookMaker]->m_mapGameID.RemoveKey(iKey);
					g_site[nBookMaker]->m_mapGameID.SetAt(nEventID, pGameData2);
					bSameOrSimilar = TRUE;
					bHAChanged = FALSE;
					return pGameData2->m_pGameInfo;
				}
			}
			if (pGameData2->m_nGameID == nEventID)
			{
				g_site[nBookMaker]->m_mapGameID.SetAt(nEventID, pGameData2);
				bSameOrSimilar = TRUE;
				bHAChanged = FALSE;
				return pGameData2->m_pGameInfo;
			}
			else
				_asm int 3
		}
		if (strcmp(pGameData2->m_MainInfo.m_szHTName, info.m_szATName) == 0 &&
			strcmp(pGameData2->m_MainInfo.m_szATName, info.m_szHTName) == 0 &&
			strcmp(pGameData2->m_MainInfo.m_szLeague, info.m_szLeague) == 0)
		{
			int iKey;
			CGameData* pGameData1;
			POSITION pos = g_site[nBookMaker]->m_mapGameID.GetStartPosition();
			while (pos != NULL)
			{
				g_site[nBookMaker]->m_mapGameID.GetNextAssoc(pos, iKey, pGameData1);
				if (pGameData2 == pGameData1)
				{
					pGameData1->m_nGameID = nEventID;
					g_site[nBookMaker]->m_mapGameID.RemoveKey(iKey);
					g_site[nBookMaker]->m_mapGameID.SetAt(nEventID, pGameData2);
					bSameOrSimilar = TRUE;
					bHAChanged = TRUE;
					return pGameData2->m_pGameInfo;
				}
			}
			if (pGameData2->m_nGameID == nEventID)
			{
				g_site[nBookMaker]->m_mapGameID.SetAt(nEventID, pGameData2);
				bSameOrSimilar = TRUE;
				bHAChanged = TRUE;
				return pGameData2->m_pGameInfo;
			}
			else
				_asm int 3
		}
	}

	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		pGameInfo = g_aryGameInfo[i];
		if (pGameInfo == NULL || pGameInfo->m_MainInfo.m_nGameCategory != info.m_nGameCategory)
			continue;

		//if (strstr(pGameInfo->m_MainInfo.m_szHTName, "Team TVIS") != 0)
		//	_asm int 3
		if (::IsEqualGameName(pGameInfo->m_MainInfo, info, 1, 1))
		{
			bSameOrSimilar = TRUE;
			bHAChanged = FALSE;
			return pGameInfo;
		}
		else if (::IsEqualGameName(pGameInfo->m_MainInfo, info, 1, -1))
		{
			bSameOrSimilar = TRUE;
			bHAChanged = TRUE;
			pGameInfo3 = pGameInfo;
		}
		else if (::IsEqualGameName(pGameInfo->m_MainInfo, info, -1, 1))
		{
			bSameOrSimilar = FALSE;
			bHAChanged = FALSE;
			pGameInfo4 = pGameInfo;
		}
		else if (::IsEqualGameName(pGameInfo->m_MainInfo, info, -1, -1))
		{
			bSameOrSimilar = FALSE;
			bHAChanged = TRUE;
			pGameInfo1 = pGameInfo;
		}
		else
		{
			if (strcmp(pGameInfo->m_MainInfo.m_szhtName, info.m_szhtName) == 0 || strcmp(pGameInfo->m_MainInfo.m_szatName, info.m_szatName) == 0)
			{
				bSameOrSimilar = FALSE;
				bHAChanged = FALSE;
				pGameInfo4 = pGameInfo;
			}
			else if (strcmp(pGameInfo->m_MainInfo.m_szhtName, info.m_szatName) == 0 || strcmp(pGameInfo->m_MainInfo.m_szatName, info.m_szhtName) == 0)
			{
				bSameOrSimilar = FALSE;
				bHAChanged = TRUE;
				pGameInfo1 = pGameInfo;
			}
		}

	}
	if (pGameInfo3)
	{
		if (!(pGameInfo3->m_nSiteID == nBookMaker && bHAChanged))
			return pGameInfo3;
	}
	if (pGameInfo4 && pGameInfo4->GetGameData(nBookMaker) == NULL)
	{
		if (!(pGameInfo4->m_nSiteID == nBookMaker && bHAChanged))
			return pGameInfo4;
	}
	if (pGameInfo1 && pGameInfo1->GetGameData(nBookMaker) == NULL)
	{
		if (!(pGameInfo1->m_nSiteID == nBookMaker && bHAChanged))
			return pGameInfo1;
	}

	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pGameInfo4 = g_aryGameInfo[i];
		if (pGameInfo4 == NULL)
			continue;
		if (pGameInfo4->m_MainInfo.m_nGameCategory != info.m_nGameCategory)
			continue;
		for (int j = 0; j < pGameInfo4->m_aryGameData.GetSize(); j++)
		{
			CGameData* pGameData = pGameInfo4->m_aryGameData[j];
			if (pGameData->m_nGameID == nEventID)
			{
				g_site[nBookMaker]->m_mapGameID.SetAt(nEventID, pGameData);
				bSameOrSimilar = TRUE;
				bHAChanged = pGameData->m_bHAChanged;
				pGameInfo = pGameData->m_pGameInfo;
				return pGameInfo;
			}
		}
	}

	return NULL;
}

BOOL CBetSite::GetGameInfoProc(CMainInfo* pMainInfo, CGameInfo** ppGameInfo, int nEventID)
{
	CGameInfo* pGameInfo = NULL;
	CGameData* pGameData = NULL;

	//if (strstr(pMainInfo->m_szHTName, "Lorenskog") && strstr(pMainInfo->m_szATName, "Nordstrand"))
	//	_asm int 3
	if (theFrame->m_wndTeamName.IsExistItem(*pMainInfo))
		return FALSE;

	BOOL bSameOrSimilar = FALSE;
	BOOL bHAChanged = FALSE;
	pGameInfo = GetGameInfo(m_nBookMaker, *pMainInfo, bSameOrSimilar, bHAChanged, nEventID);
	if (pGameInfo && bHAChanged)
	{
		ASSERT(pGameInfo->m_nSiteID != m_nBookMaker);
		//if ((!strstr(pMainInfo->m_szHTName, "Riffa") || !strstr(pMainInfo->m_szATName, "Ahli")) &&
		//	(!strstr(pMainInfo->m_szHTName, "Maccabi") || !strstr(pMainInfo->m_szATName, "Hapoel")))
		//	_asm int 3
		pMainInfo->HAChange();
	}
	
	if (pGameInfo == NULL)
	{
		*ppGameInfo = CreateGameInfo(pMainInfo, nEventID);
		return TRUE;
	}
	else
	{
		pGameData = pGameInfo->GetGameData(m_nBookMaker);
		if (bSameOrSimilar)
		{
			if (pGameData != NULL)
			{
				//ASSERT(pGameData->m_nGameID == nEventID);
				pGameData->Update(pMainInfo, TRUE);
				if (pGameInfo->m_nSiteID == m_nBookMaker && pGameData->m_bHAChanged)
					_asm int 3
				* ppGameInfo = pGameInfo;
				return TRUE;
			}
			else
			{
				pGameData = new CGameData(pGameInfo, m_nBookMaker, TRUE, bHAChanged);
				pGameData->m_nGameID = nEventID;
				pGameData->m_MainInfo = *pMainInfo;
				::EnterCriticalSection(&g_csSameName);
				SetSlaveName(pMainInfo->m_szhtName, pGameInfo->m_MainInfo.m_szhtName);
				SetSlaveName(pMainInfo->m_szatName, pGameInfo->m_MainInfo.m_szatName);
				::LeaveCriticalSection(&g_csSameName);
				pGameInfo->m_aryGameData.Add(pGameData);
				if (nEventID != pGameData->m_nGameID)
					_asm int 3
				m_mapGameID.SetAt(nEventID, pGameData);
				if (pGameInfo->m_nSiteID == m_nBookMaker && pGameData->m_bHAChanged)
					_asm int 3
				* ppGameInfo = pGameInfo;
				return TRUE;
			}
		}
		else
		{
			if (/*pMainInfo->m_HTScore == pGameInfo->m_MainInfo.m_HTScore &&  pMainInfo->m_ATScore == pGameInfo->m_MainInfo.m_ATScore && */pGameInfo->m_nSiteID != m_nBookMaker)
			{
				CMainInfo* pMainInfo1 = new CMainInfo(pGameInfo->m_MainInfo);
				pMainInfo1->m_bMaster = TRUE;
				CMainInfo* pMainInfo2 = new CMainInfo(*pMainInfo);
				pMainInfo2->m_bMaster = FALSE;
				theFrame->PostMessageW(MSG_CHECK_TEAMNAME, (WPARAM)pMainInfo1, (LPARAM)pMainInfo2);
			}
			else
			{
				if (bHAChanged)
					pMainInfo->HAChange();
				
				*ppGameInfo = CreateGameInfo(pMainInfo, nEventID);
				return TRUE;
			}
		}
	}
	*ppGameInfo = NULL;
	return FALSE;
}

BOOL CBetSite::AddOddInfo(COddsInfo* pOddsInfo, CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3)
{
	pGameData->m_aryOddsInfo.Add(pOddsInfo);
	pOddsInfo->m_nOddsInfoKind = nOddsKind;
	if (pGameData->m_bHAChanged)
	{
		if (nOddsKind == OI_TEAM1_GOAL_OU)
			pOddsInfo->m_nOddsInfoKind = OI_TEAM2_GOAL_OU;
		if (nOddsKind == OI_TEAM2_GOAL_OU)
			pOddsInfo->m_nOddsInfoKind = OI_TEAM1_GOAL_OU;
	}
	pOddsInfo->m_hVal1 = hVal1;
	if (pGameData->m_bHAChanged && nOddsKind == OI_HANDICAP)
		pOddsInfo->m_hVal1 = -hVal1;

	pOddsInfo->m_oVal1 = fOval1;
	pOddsInfo->m_oVal2 = fOval2;
	pOddsInfo->m_dwOddID1 = dwOddID1;
	pOddsInfo->m_dwOddID2 = dwOddID2;
	if (pGameData->m_bHAChanged && (nOddsKind == OI_MAIN || nOddsKind == OI_HANDICAP))
	{
		pOddsInfo->m_oVal1 = fOval2;
		pOddsInfo->m_oVal2 = fOval1;
		pOddsInfo->m_dwOddID1 = dwOddID2;
		pOddsInfo->m_dwOddID2 = dwOddID1;
	}
	if (nOddsKind == OI_MAIN)
	{
		pOddsInfo->fDraw = fOval3;
		pOddsInfo->m_dwOddID3 = dwOddID3;
	}

	return TRUE;
}

int CBetSite::ZipStream(char *a, int sa, char *b, int sb)
{
	z_stream defstream;
	defstream.zalloc = Z_NULL;
	defstream.zfree = Z_NULL;
	defstream.opaque = Z_NULL;
	defstream.avail_in = (uInt)sa; // size of input
	defstream.next_in = (Bytef *)a; // input char array
	defstream.avail_out = (uInt)sb; // size of output
	defstream.next_out = (Bytef *)b; // output char array

	deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
	deflate(&defstream, Z_FINISH);
	deflateEnd(&defstream);
	return defstream.total_out;
}

int CBetSite::UnzipStream(char *input, int nSizeIn, char *output, int nSizeOut)
{
	z_stream infstream;
	infstream.zalloc = Z_NULL;
	infstream.zfree = Z_NULL;
	infstream.opaque = Z_NULL;
	infstream.avail_in = (uInt)nSizeIn;
	infstream.next_in = (Bytef *)input;
	infstream.avail_out = (uInt)nSizeOut;
	infstream.next_out = (Bytef *)output;

	int r = inflateInit2(&infstream, 16 + MAX_WBITS);
	r = inflate(&infstream, Z_NO_FLUSH);
	r = inflateEnd(&infstream);
	return infstream.total_out;
}

#define READ_SIZE (4 * 1024) // 4K

int CBetSite::Unzip(TCHAR* szGZfileName, BYTE* buf)
{
	char szGZfileName_s[MAX_PATH];
	ToAscii(szGZfileName, szGZfileName_s, CP_ACP);
	gzFile gzFile = gzopen(szGZfileName_s, "rb");
	gzrewind(gzFile);

	char *buff = new char[READ_SIZE];
	int nSize = 0;

	while (!gzeof(gzFile))
	{
		int len = gzread(gzFile, buff, READ_SIZE);
		memcpy(buf + nSize, buff, len);
		nSize += len;
	}
	delete buff;
	gzclose(gzFile);

	return nSize;
}

int CBetSite::DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo)
{
	return 0;
}

void CBetSite::RefreshData()
{
}

int CBetSite::DoBetting(BetInfo* pBetInfo, int nBetObjNo)
{
	int MAX_REPEAT = 2;
	CGameData* pGameData = NULL;
	COddsInfo* pOddsInfo = NULL;
	m_repeat = 0;
	::EnterCriticalSection(&m_csThread);
	while(m_repeat < MAX_REPEAT)
	{
		pGameData = NULL;
		pOddsInfo = NULL;
		if (m_repeat != 0)
		{
			Sleep(500);
			OutDbgStrAA("\n************  Repeat %s %d  *****************", g_szSiteName[m_nBookMaker], m_repeat);
			RefreshData();
		}
		if (!g_site[m_nBookMaker]->m_mapGameID.Lookup(pBetInfo->BetVal[nBetObjNo].dwGameID, pGameData))
		{
			pBetInfo->BetVal[nBetObjNo].nResult = BET_GAME_REMOVED;
			break;
		}
		ASSERT(pBetInfo->BetVal[nBetObjNo].dwGameID == pGameData->m_nGameID);
		int nCheckResult = CheckBetInfo(pBetInfo, nBetObjNo, pGameData, &pOddsInfo);
		if (nCheckResult != BET_CHECK_OK)
		{
			pBetInfo->BetVal[nBetObjNo].nResult = nCheckResult;
			if (nCheckResult == BET_ODD_REMOVED)
				break;
			if (nCheckResult == BET_VALUE_DECREASED)
				break;
		}
		else
		{
			ASSERT(pOddsInfo != NULL);
			DoBettingEx(pBetInfo, nBetObjNo, pOddsInfo);
			if (pBetInfo->BetVal[nBetObjNo].nResult == BET_SUCCESS)
				break;
			if (pBetInfo->BetVal[nBetObjNo].nResult == BET_VALUE_CHANGED || pBetInfo->BetVal[nBetObjNo].nResult == BET_ODDS_NOT_VALID)
				MAX_REPEAT = 4;
		}
		m_repeat++;
	}
	SYSTEMTIME BettingEndTime;
	GetSystemTime(&BettingEndTime);

	if (pBetInfo->BetVal[nBetObjNo].nResult != BET_SUCCESS && pBetInfo->BetVal[nBetObjNo].nResult != BET_SUCCESS1)
		RefreshData();

	::LeaveCriticalSection(&m_csThread);

	if (pBetInfo->BetVal[nBetObjNo].nResult == BET_SUCCESS || pBetInfo->BetVal[nBetObjNo].nResult == BET_SUCCESS1)
		Sleep(m_nBettingTimePeriod);

	BetStatusDisplay* betState = new BetStatusDisplay;
	strcpy_s(betState->szHTeam, MAX_TEAM_NAME, pBetInfo->szHTeam);
	strcpy_s(betState->szATeam, MAX_TEAM_NAME, pBetInfo->szATeam);
	strcpy_s(betState->szLeague, MAX_LEAGUE_NAME, pBetInfo->szLeague);
	betState->nCategory = pBetInfo->nCategory;
	betState->nOddKind = pBetInfo->nOddsKind;
	betState->fHVal = pBetInfo->hVal1;
	betState->fOVal_orig = pBetInfo->BetVal[nBetObjNo].oVal_orig;
	betState->fOVal_cur = pBetInfo->BetVal[nBetObjNo].oVal_cur;
	betState->nObj = nBetObjNo;
	betState->nBookMaker = m_nBookMaker;
	betState->nBettingMoney = pBetInfo->BetVal[nBetObjNo].nBettingMoney;
	betState->fBalance = g_fBalance[m_nBookMaker];
	betState->nResult = pBetInfo->BetVal[nBetObjNo].nResult;
	betState->dwEngineTime = GetRelativeTime(g_BettingStartTime, g_EngineStartTime);
	betState->dwBettingTime = GetRelativeTime(BettingEndTime, g_BettingStartTime);
	betState->nRepeat = m_repeat;
	theFrame->m_wndSetting.PostMessageW(MSG_DISP_BET_STATUS, (WPARAM)betState);

	OutDbgStrAA("\nMSG_DISP_BET_STATUS");
	::EnterCriticalSection(&g_csGameInfoArray);

	CGameInfo* pGameInfo = GetGameInfoFromBetInfo(pBetInfo, nBetObjNo);

	pGameInfo->m_BetState.nOddsKind = pBetInfo->nOddsKind;
	pGameInfo->m_BetState.m_hVal = pBetInfo->hVal1;
	pGameInfo->m_BetState.nBookMakers[nBetObjNo] = m_nBookMaker;
	pGameInfo->m_BetState.nStakes[nBetObjNo] = pBetInfo->BetVal[nBetObjNo].nBettingMoney;
	pGameInfo->m_BetState.fOdds[nBetObjNo] = pBetInfo->BetVal[nBetObjNo].oVal;

	pGameInfo->m_BetState.nWasPartialBetting[nBetObjNo] = PB_DO_BET;

#ifdef NO_ENGINE
	if (pBetInfo->BetVal[nBetObjNo].nResult == BET_SUCCESS || 
		pBetInfo->BetVal[nBetObjNo].nResult == BET_GAME_REMOVED || 
		pBetInfo->BetVal[nBetObjNo].nResult == BET_CHECK_EVENT_FAIL || 
		pBetInfo->BetVal[nBetObjNo].nResult == BET_ODD_REMOVED ||
		pBetInfo->BetVal[nBetObjNo].nResult == BET_ALREADY_PLACED)
#else
	if (pBetInfo->BetVal[nBetObjNo].nResult == BET_SUCCESS || pBetInfo->BetVal[nBetObjNo].nResult == BET_ALREADY_PLACED)
#endif
	{
		pGameInfo->m_BetState.nWasPartialBetting[nBetObjNo] = PB_SUCCESS;
	}
	else if (pBetInfo->BetVal[nBetObjNo].nResult == BET_UNABLE || 
		pBetInfo->BetVal[nBetObjNo].nResult == BET_BLOCKED || 
		pBetInfo->BetVal[nBetObjNo].nResult == BET_PARSE_FAIL)
	{
		pGameInfo->m_BetState.nWasPartialBetting[nBetObjNo] = PB_DONT_BET;
	}

	int nValidCount = 0, nSuccessCount = 0;
	for (int i = 0; i < 3; i++)
	{
		if (pBetInfo->BetVal[i].bValid)
			nValidCount++;
		if (pBetInfo->BetVal[i].bValid && pBetInfo->BetVal[i].nResult == BET_SUCCESS)
			nSuccessCount++;
	}
	if (nValidCount == nSuccessCount)
		pGameInfo->m_BetState.bIsTotalBettingComplete = TRUE;

	::LeaveCriticalSection(&g_csGameInfoArray);
#ifdef BET_SUBTHREAD_MODE
	::EnterCriticalSection(&m_csBetObjCount);
	theFrame->m_nBetObjCount--;
	if (theFrame->m_nBetObjCount == 0)
		SetEvent(theFrame->m_hBettingCompleteEvent);
	::LeaveCriticalSection(&m_csBetObjCount);
#endif
	return 0;
}

int CBetSite::CheckBetInfo(BetInfo* pBetInfo, int nBetObjNo, CGameData* pGameData, COddsInfo** ppOddsInfo)
{
	COddsInfo* pOddsInfo = NULL;
	*ppOddsInfo = NULL;
	for (int i = 0; i < pGameData->m_aryOddsInfo.GetSize(); i++)
	{
		pOddsInfo = pGameData->m_aryOddsInfo[i];
		if (pOddsInfo == NULL)
			continue;
		*ppOddsInfo = pOddsInfo;

		if (pBetInfo->nOddsKind != pOddsInfo->m_nOddsInfoKind)
			continue;
		float oVal = pBetInfo->BetVal[nBetObjNo].oVal;

		pBetInfo->BetVal[nBetObjNo].oVal_orig = pBetInfo->BetVal[nBetObjNo].oVal;
		if (pBetInfo->nOddsKind == OI_MAIN)
		{
			if (nBetObjNo == 0)
			{
				pBetInfo->BetVal[nBetObjNo].oVal_cur = pOddsInfo->m_oVal1;
				if (pBetInfo->BetVal[nBetObjNo].oVal > pOddsInfo->m_oVal1)
					return BET_VALUE_DECREASED;
				else
					pBetInfo->BetVal[nBetObjNo].oVal = pOddsInfo->m_oVal1;
			}
			else if (nBetObjNo == 1)
			{
				pBetInfo->BetVal[nBetObjNo].oVal_cur = pOddsInfo->m_oVal2;
				if (pBetInfo->BetVal[nBetObjNo].oVal > pOddsInfo->m_oVal2)
					return BET_VALUE_DECREASED;
				else
					pBetInfo->BetVal[nBetObjNo].oVal = pOddsInfo->m_oVal2;
			}
			else if (nBetObjNo == 2)
			{
				pBetInfo->BetVal[nBetObjNo].oVal_cur = pOddsInfo->fDraw;
				if (pBetInfo->BetVal[nBetObjNo].oVal > pOddsInfo->fDraw)
					return BET_VALUE_DECREASED;
				else
					pBetInfo->BetVal[nBetObjNo].oVal = pOddsInfo->fDraw;
			}
			return BET_CHECK_OK;
		}
		else if (pBetInfo->nOddsKind == OI_GOAL_OE)
		{
			if (nBetObjNo == 0)
			{
				pBetInfo->BetVal[nBetObjNo].oVal_cur = pOddsInfo->m_oVal1;
				if (pBetInfo->BetVal[nBetObjNo].oVal > pOddsInfo->m_oVal1)
					return BET_VALUE_DECREASED;
				else
					pBetInfo->BetVal[nBetObjNo].oVal = pOddsInfo->m_oVal1;
			}
			else if (nBetObjNo == 1)
			{
				pBetInfo->BetVal[nBetObjNo].oVal_cur = pOddsInfo->m_oVal2;
				if (pBetInfo->BetVal[nBetObjNo].oVal > pOddsInfo->m_oVal2)
					return BET_VALUE_DECREASED;
				else
					pBetInfo->BetVal[nBetObjNo].oVal = pOddsInfo->m_oVal2;
			}
			return BET_CHECK_OK;
		}
		else if (pBetInfo->nOddsKind == OI_HANDICAP)
		{
			if (pBetInfo->hVal1 != pOddsInfo->m_hVal1)
				continue;
			if (nBetObjNo == 0)
			{
				pBetInfo->BetVal[nBetObjNo].oVal_cur = pOddsInfo->m_oVal1;
				if (pBetInfo->BetVal[nBetObjNo].oVal > pOddsInfo->m_oVal1)
					return BET_VALUE_DECREASED;
				else
					pBetInfo->BetVal[nBetObjNo].oVal = pOddsInfo->m_oVal1;
			}
			else if (nBetObjNo == 1)
			{
				pBetInfo->BetVal[nBetObjNo].oVal_cur = pOddsInfo->m_oVal2;
				if (pBetInfo->BetVal[nBetObjNo].oVal > pOddsInfo->m_oVal2)
					return BET_VALUE_DECREASED;
				else
					pBetInfo->BetVal[nBetObjNo].oVal = pOddsInfo->m_oVal2;
			}
			return BET_CHECK_OK;
		}
		else if (pBetInfo->nOddsKind == OI_GOAL_OU || pBetInfo->nOddsKind == OI_TEAM1_GOAL_OU || pBetInfo->nOddsKind == OI_TEAM2_GOAL_OU)
		{
			if (pBetInfo->hVal1 != pOddsInfo->m_hVal1)
				continue;
			if (nBetObjNo == 0)
			{
				pBetInfo->BetVal[nBetObjNo].oVal_cur = pOddsInfo->m_oVal1;
				if (pBetInfo->BetVal[nBetObjNo].oVal > pOddsInfo->m_oVal1)
					return BET_VALUE_DECREASED;
				else
					pBetInfo->BetVal[nBetObjNo].oVal = pOddsInfo->m_oVal1;
			}
			else if (nBetObjNo == 1)
			{
				pBetInfo->BetVal[nBetObjNo].oVal_cur = pOddsInfo->m_oVal2;
				if (pBetInfo->BetVal[nBetObjNo].oVal > pOddsInfo->m_oVal2)
					return BET_VALUE_DECREASED;
				else
					pBetInfo->BetVal[nBetObjNo].oVal = pOddsInfo->m_oVal2;
			}
			return BET_CHECK_OK;
		}
		else
			ASSERT(0);
	}
	return BET_ODD_REMOVED;
}

BOOL CBetSite::SendRequest(int nReqKind)
{
	return TRUE;
}

BOOL CBetSite::SendRequestEx(int nReqKind)
{
	if (!SendRequest(nReqKind))
	{
		if (!SendRequest(nReqKind))
			return FALSE;
	}
	return TRUE;
}

int CBetSite::SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind)
{
	return BET_SUCCESS;
}

#define SLIP_TIME 500
int CBetSite::SendBetRequestEx(BetInfo* pBetInfo, int nBetObjNo, int nReqKind)
{
	int nRet;
	if ((nRet = SendBetRequest(pBetInfo, nBetObjNo, nReqKind)) != BET_SUCCESS)
	{
		Sleep(SLIP_TIME);
		if ((nRet = SendBetRequest(pBetInfo, nBetObjNo, nReqKind)) != BET_SUCCESS)
		{
			return nRet;
		}
	}
	return nRet;
}

BOOL CBetSite::IsNeedGetBalance()
{
	return TRUE;
}