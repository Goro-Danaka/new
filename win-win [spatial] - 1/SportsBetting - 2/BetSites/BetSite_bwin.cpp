#include "..\stdafx.h"
#include "BetSite_bwin.h"
#include "..\global.h"
#include "..\GameData.h"
#include "..\MainFrm.h"

using namespace rapidjson;
#pragma warning(disable:4819)

//https://www.bwin3818.com/en-gb/live

CBetSite_bwin::CBetSite_bwin()
{
	m_nBookMaker = SITE_bwin;
	m_nTimePeriod = 5000;

#ifdef SUB_THREAD_MODE
	m_hCompletionPort = INVALID_HANDLE_VALUE;
#endif

	m_mapOddInf.RemoveAll();

	strcpy_s(m_szAuthToken, "");
	_tcscpy_s(m_szUserInfo, _T(""));
}

CBetSite_bwin::~CBetSite_bwin()
{
#ifdef SUB_THREAD_MODE
	if (m_hCompletionPort != INVALID_HANDLE_VALUE)
		CloseHandle(m_hCompletionPort);
#endif
	UINT64 iKey;
	ODD_INF* pOddInf;	
	POSITION pos = m_mapOddInf.GetStartPosition();
	while (pos != NULL)
	{
		m_mapOddInf.GetNextAssoc(pos, iKey, pOddInf);
		delete pOddInf;
		m_mapOddInf.RemoveKey(iKey);
	}

	m_mapOddInf.RemoveAll();
}

int CBetSite_bwin::MainProc()
{
	if (m_bExitThreadFlg)
		return 0;

	BOOL bResult = FALSE;
	if (m_nLogInStatus == LS_SUCCESS)
	{
		if (m_bDoLogOut)
		{
			SendRequestEx(REQ_LOGOUT);
			m_bExitThreadFlg = TRUE;
			m_nLogInStatus = LS_INIT;
		}
		else
		{
			if (!SendRequestEx(REQ_DATA))
			{
				OutDbgStrAA("\n[bwin]:MAIN ERROR - REQ_DATA");
				return 0;
			}
			if (!AnalyzeResData())
			{
				OutDbgStrAA("\n[bwin]:MAIN ERROR - AnalyzeResData");
				return 0;
			}
			if (IsNeedGetBalance())
			{
				if (!SendRequestEx(REQ_BALANCE))
				{
					OutDbgStrAA("\n[bwin]:MAIN ERROR - REQ_BALANCE");
					return 0;
				}
				if (!AnalyzeResBalance())
				{
					OutDbgStrAA("\n[bwin]:MAIN ERROR - AnalyzeResBalance");
					return 0;
				}
			}
			OutDbgStrAA("\n[bwin]****************************");
		}
	}
	else
	{
		if (m_bDoLogOut)
		{
			m_bExitThreadFlg = TRUE;
		}
		else
		{
			if ((m_nLogInStatus == LS_INIT || m_nLogInStatus == LS_LIVE) && strcmp(m_szID, "") != 0 && strcmp(m_szPswd, "") != 0)
			{
				if (m_nLogInStatus == LS_INIT)
				{
					if (!SendRequestEx(REQ_LIVE))
					{
						OutDbgStrAA("\n[bwin]:MAIN ERROR - REQ_LIVE");
						return 0;
					}
					if (!AnalyzeResLive())
					{
						OutDbgStrAA("\n[bwin]:MAIN ERROR - AnalyzeResLive");
						return 0;
					}
					m_nLogInStatus = LS_LIVE;
				}

				if (!SendRequestEx(REQ_LOGIN))
				{
					OutDbgStrAA("\n[bwin]:MAIN ERROR - REQ_LOGIN");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE1))
				{
					OutDbgStrAA("\n[bwin]:MAIN ERROR - REQ_LIVE1");
					return 0;
				}
			}
		}
	}
	return 0;
}

BOOL CBetSite_bwin::SendRequest(int nReqKind)
{
	CHttpConnection* pServer = NULL;

	DWORD dwRet;
	char strFormData[0x1000];
	strcpy_s(strFormData, "");

	CString strServerName;
	strServerName = _T("www.bwin3818.com");
	pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

	try
	{
		if (nReqKind == REQ_LIVE)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en-gb/live"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LOGIN)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en-gb/live/login"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.bwin3818.com/en-gb/live"));
			TCHAR szTasty[0x200];
			SYSTEMTIME time;
			GetSystemTime(&time);
			_stprintf_s(szTasty, _T("uid=%.2d%.2d%.2d%.2d%.2d%.2d10819&fst=&pst=&cst=&ns=3&pvt=6&pvis=2&th="), time.wYear - 2000, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("ABTasty"), szTasty);
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("ABTastySession"), _T("referrer=__landingPage=https://www.bwin3818.com/en-gb/live"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("zaynar_locale"), _T("en-gb"));

			char s[0x1000];
			//sprintf_s(s, "0400bpNfiPCR/AUNf94lis1ztrjQCvk297SBqKo6M4HAwREo6q6gOaXQisx1TUs97Br6z0NCKV80r0uxH1ZPRIwM6njw/ujAyYdbGKZt5JLThTvosS1xgSAgNfLEMokGoGJx1x7Vc9CP0wrlxayGiVf3WGA73rYZtIsVyoA80pXCaXYSNAvGGne6ANC1UJgA1BQZQ4JudHOlf1LBNuSqWYFHk2kwoAkfbNob3n0cReYwfEvAG/NV/+Sq0t3ktNwHHJdI6WpaFMKy/EqC4HHueyhipE+R2iB5Ar7M9PE4fRZTRwwK7ZWd2T2HOiRrNg2EXv5TmdhzOU/bu1v/5hVWhInYTCVSVdxOBiQRfpJjOU/wtEEjt6vv3Wo1XSU3IXvE3T+bcoxqWILbmExhQBlZbAyHTEvDPudwqQeSDJ0qCc4gMpergecSo06izQjA5cE2T8ZaTfbncaCE024s/J0glNig9rgA3rY8UwayMUJcYh+SCmEK/XRNo3H5dI6H2W0dl9Ai6rmfRxJsdOMWAxs7FBMuOzWpm4tSpNCCSt/3Q53nCj8tZfj5n4+lWqjdu10X7DVptueqwTG9EQZoTl8xMwdAwbA6otbqxU8gWn3kh8RnPiVCSOP8F/eyAGlZmakw+7cndFvPrWIExT58wjfvV29cCUJ5Wd9sf0lnKULPxBs5mS/qEyMnHm5m5Uf9oOYLTF82/5VinObkTrr+mvvWgPnrWuhz8jWiZkw2QedvHaWiI4UUpNllBb/KsYQonmwROuiCxPIJdauqKdWH4vExtHevOmJHLUeNp3onLVkhiPnaZ/xBf1d974CiNsgH1DGeSyhahMFAUyCBgQ8JdmNWP4mnL3nKW6gN3x2hyAfUMZ5LKFobediPJZrmRU1w7kldV48ZKdi7YSofmT2A/nXrRazxiyzC4D/XcR7hfNcbAfk5/KPd6YYNJ3sUenxfYV3WeA0U/nvOSj0fRbezoEkNBc35XGtVsYmdWtM91YYITyzRwv8PXXpOa/Fh/Q7T6v45cktLXi30KpDmpMz6oP8G/KuTzg6TMVfop73TMr8O3MhREjLn9gmf7Z4VojfcCJH6I4vS0aaFjiiCEHzrIWh2D/DquiJ0sNRD5FoMgJnjRx+vsOpal7z340DdNEFkInNlyhumlCBhyDV1je4ZZL9nLOf9i23qj/cLtpHiiiH75lMYxjpcXLrt9SQ8aENqhXcY/jsVcJjlgOTDxRNauODchk2WrH2LOmVQKLUdYeTcLtcejyDnpaal40M78ZJfgTpOLM1mMmLLVZ1rVcid7tlganS3aDHkcz5TCb3+B1hhIX7typaORBWLCielBu+lh88nqx7OpSZzg7ITib/sZCdUAw3S+g==");
			sprintf_s(s, "");

			sprintf_s(strFormData, "utf8=\xE2""\x9C""\x93""&authenticity_token=%s&username=%s&password=%s&login_io_b_b=%s&button=", m_szAuthToken, m_szID, m_szPswd, s);
		}
		else if (nReqKind == REQ_LIVE1)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en-gb/live"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("login"), _T("Success"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("zaynar_locale"), _T("en-gb"));
		}
		else if (nReqKind == REQ_BALANCE)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/api/wallets.json"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:Accept:application/json, text/plain, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("login"), _T("Success"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("currency_prefix"), _T("&yen;"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("zaynar_locale"), _T("en-gb"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("_vc_a"), m_szUserInfo);
		}
		else if (nReqKind == REQ_LOGOUT)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en-gb/live/logout"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("zaynar_locale"), _T("en-gb"));
		}
		else if (nReqKind == REQ_DATA)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en-gb/live/live/refresh"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/json; charset=utf-8"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("zaynar_locale"), _T("en-gb"));
		}
		else if (nReqKind == REQ_DATA1)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en-gb/live/live/855011900/refresh?sport_id=100&version="), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/json, text/javascript, */*"));
			//m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_pSession->SetCookie(_T("https://www.bwin3818.com"), _T("zaynar_locale"), _T("en-gb"));
		}

		m_HttpFile->SendRequest(NULL, 0, strFormData, strlen(strFormData));

		m_HttpFile->QueryInfoStatusCode(dwRet);
		if (dwRet != HTTP_STATUS_OK)
			goto L_FALSE;

		BYTE buffer[0x1000];
		DWORD len = 0x1000;
		if (nReqKind == REQ_LOGIN)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			//_vc_a=8217259|CN|CNY|HUJIABAO1828|linghong2017@163.com
			if (CheckUserName((TCHAR*)buffer))
			{
				m_nLogInStatus = LS_SUCCESS;
				theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)1);
			}
			else
			{
				m_nLogInStatus = LS_FAIL;
				theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)2);
				goto L_FALSE;
			}
		}
		ReadResponse();

		m_HttpFile->Close();
		delete m_HttpFile;
		m_HttpFile = NULL;

		pServer->Close();
		delete pServer;
		pServer = NULL;
	}
	catch (CInternetException* e) {
		e->Delete();
		goto L_FALSE;
	}
	return TRUE;
L_FALSE:
	if (m_HttpFile)
	{
		m_HttpFile->Close();
		delete m_HttpFile;
		m_HttpFile = NULL;
	}
	if (pServer)
	{
		pServer->Close();
		delete pServer;
		pServer = NULL;
	}
	return FALSE;
}

BOOL CBetSite_bwin::CheckUserName(TCHAR* buffer)
{
	if (!GetSessionValue(buffer, _T("_vc_a="), m_szUserInfo))
		return FALSE;
	return TRUE;
}

struct COMPLETION_KEY
{
	int nCategory;
	DWORD dwGameID;
};

BOOL CBetSite_bwin::AnalyzeResData()
{
	if (m_szText == NULL || strlen(m_szText) < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\bwin.AnalyzeResData.json"), m_szText);
	{
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;
		if (!root.IsObject())
			goto L_FALSE;
		if (!root.HasMember("sports"))
			goto L_FALSE;
		Value& sports = root["sports"];

		//OutDbgStrAA("\n----------------------------------------------");
		for (int i = 0; i < (int)sports.Size(); i++)
		{
			Value& cat = sports[i];
			char* szCat = (char*)cat["title"].GetString();
			int nGameCategory = GetGameCategory((char*)szCat);
			if (nGameCategory == -1 || !theFrame->m_bSetCategory[nGameCategory])
				continue;

			Value& events = cat["events"];
			for (int j = 0; j < (int)events.Size(); j++)
			{
				Value& game = events[j];
				Value& outcomes = game["outcomes"];
				if (outcomes.Size() == 0)
					continue;
				DWORD dwID = game["id"].GetUint();

				char* ht = (char*)game["home_team"].GetString();
				char* at = (char*)game["away_team"].GetString();
				char* lg = (char*)game["meeting"].GetString();

				int hs = -1, as = -1, hs1 = -1, as1 = -1, hs2 = -1, as2 = -1;
				if (nGameCategory == GC_TENNIS)
				{
					if (game["status"].IsString())
					{
						char* szscore = (char*)game["status"].GetString();
						GetScore(szscore, hs, as);
					}

					if (game["home_score"].IsString())
					{
						const char* szhs = game["home_score"].GetString();
						sscanf_s(szhs, "%d", &hs2);
					}
					else
						hs2 = game["home_score"].GetInt();

					if (game["away_score"].IsString())
					{
						const char* szas = game["away_score"].GetString();
						sscanf_s(szas, "%d", &as2);
					}
					else
						as2 = game["away_score"].GetInt();
				}
				else
				{
					if (game["home_score"].IsString())
					{
						const char* szhs = game["home_score"].GetString();
						sscanf_s(szhs, "%d", &hs);
					}
					else
						hs = game["home_score"].GetInt();

					if (game["away_score"].IsString())
					{
						const char* szas = game["away_score"].GetString();
						sscanf_s(szas, "%d", &as);
					}
					else
						as = game["away_score"].GetInt();
				}

				char* stage = (char*)game["status"].GetString();
				int ipt = 0;

				if (IsExceptionGame(ht, at, lg, stage))
					continue;
				CMainInfo MainInfo(nGameCategory, ht, at, SCORE(hs, hs1, hs2), SCORE(as, as1, as2), stage, ipt, lg);
				::EnterCriticalSection(&g_csGameInfoArray);
				CGameInfo* pGameInfo2 = NULL;
				if (!GetGameInfoProc(&MainInfo, &pGameInfo2, dwID))
				{
					::LeaveCriticalSection(&g_csGameInfoArray);
					continue;
				}
#ifndef SUB_THREAD_MODE
				CGameInfo* pGameInfo = new CGameInfo(pGameInfo2);
				CGameData* pGameData = pGameInfo->GetGameData(m_nBookMaker);
#endif
				::LeaveCriticalSection(&g_csGameInfoArray);
#ifdef SUB_THREAD_MODE
				COMPLETION_KEY* pKey = new COMPLETION_KEY;
				pKey->nCategory = nGameCategory;
				pKey->dwGameID = dwID;

				if (!m_bExitThreadFlg)
					PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD)pKey, NULL);
#else
				pGameData->RemoveAllOddsInfo();

				UINT64 dwOddID1 = 0, dwOddID2 = 0, dwOddID3 = 0;
				UINT64 dwPriceID1 = 0, dwPriceID2 = 0, dwPriceID3 = 0;
				UINT64 dwMarketID1 = 0, dwMarketID2 = 0, dwMarketID3 = 0;
				char szTitle[MAX_TEAM_NAME * 3], szMarket[0x40], szPriceFmt[0x20];

				strcpy_s(szTitle, game["title"].GetString());

				for (int k = 0; k < (int)outcomes.Size(); k++)
				{
					BOOL bExistDraw = FALSE;
					char* sz2 = (char*)outcomes[k + 1]["description"].GetString();
					if (_stricmp(sz2, "Draw") == 0)
						bExistDraw = TRUE;

					float fVal1 = 0, fVal2 = 0, fVal3 = 0;
					fVal1 = outcomes[k]["price_decimal"].GetFloat();
					dwOddID1 = outcomes[k]["id"].GetInt64();
					dwPriceID1 = outcomes[k]["price_id"].GetInt64();
					dwMarketID1 = outcomes[k]["market_id"].GetInt64();
					strcpy_s(szMarket, outcomes[k]["market"].GetString());
					strcpy_s(szPriceFmt, outcomes[k]["price"].GetString());
					if (bExistDraw)
					{
						fVal3 = outcomes[k + 1]["price_decimal"].GetFloat();
						dwOddID3 = outcomes[k + 1]["id"].GetInt64();
						dwPriceID3 = outcomes[k + 1]["price_id"].GetInt64();
						dwMarketID3 = outcomes[k + 1]["market_id"].GetInt64();

						fVal2 = outcomes[k + 2]["price_decimal"].GetFloat();
						dwOddID2 = outcomes[k + 2]["id"].GetInt64();
						dwPriceID2 = outcomes[k + 2]["price_id"].GetInt64();
						dwMarketID2 = outcomes[k + 2]["market_id"].GetInt64();
					}
					else
					{
						fVal2 = outcomes[k + 1]["price_decimal"].GetFloat();
						dwPriceID2 = outcomes[k + 1]["price_id"].GetInt64();
						dwOddID2 = outcomes[k + 1]["id"].GetInt64();
						dwMarketID2 = outcomes[k + 1]["market_id"].GetInt64();
					}

					MyAddOddInfo(pGameData, OI_MAIN, 0, fVal1, fVal2, fVal3, dwOddID1, dwOddID2, dwOddID3,
						dwPriceID1, dwPriceID2, dwPriceID3, dwMarketID1, dwMarketID2, dwMarketID3, (char*)PlusProc(szTitle).c_str(), (char*)PlusProc(szMarket).c_str(), szPriceFmt);

					k += 3;
				}
				::EnterCriticalSection(&g_csGameInfoArray);
				CopyGameDataProc(pGameData);
				::LeaveCriticalSection(&g_csGameInfoArray);
#endif
			}
		}

#ifdef SUB_THREAD_MODE
		theFrame->PostMessageW(MSG_UPDATE_EVENT_TREE, (WPARAM)m_nBookMaker);
#else
		theFrame->PostMessageW(MSG_UPDATE, (WPARAM)m_nBookMaker);
#endif

	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bwin.AnalyzeResData.json"), m_szText);
	return FALSE;
}

ODD_INF* CBetSite_bwin::GetOddInf(int nBetObjNo, COddsInfo* pOddsInfo, UINT64& dwOddID)
{
	if (nBetObjNo == 0)
		dwOddID = pOddsInfo->m_dwOddID1;
	else if (nBetObjNo == 1)
		dwOddID = pOddsInfo->m_dwOddID2;
	else if (nBetObjNo == 2)
		dwOddID = pOddsInfo->m_dwOddID3;
	else
		ASSERT(0);

	ODD_INF* pOddInf = NULL;
	m_mapOddInf.Lookup(dwOddID, pOddInf);
	return pOddInf;
}

int CBetSite_bwin::SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind)
{
	CHttpConnection* pServer = NULL;
	CGameData* pGameData = NULL;
	if (!g_site[pBetInfo->BetVal[nBetObjNo].nBookMaker]->m_mapGameID.Lookup(pBetInfo->BetVal[nBetObjNo].dwGameID, pGameData))
		return BET_GAME_REMOVED;
	COddsInfo* pOddsInfo = NULL;
	int nCheckResult = CheckBetInfo(pBetInfo, nBetObjNo, pGameData, &pOddsInfo);
	if (nCheckResult != BET_CHECK_OK)
		return nCheckResult;

	UINT64 dwOddID;
	ODD_INF* pOddInf = GetOddInf(nBetObjNo, pOddsInfo, dwOddID);

	char szTeamName[MAX_TEAM_NAME];
	if (nBetObjNo == 0)
		strcpy_s(szTeamName, pGameData->m_MainInfo.m_szHTName);
	else if (nBetObjNo == 1)
		strcpy_s(szTeamName, pGameData->m_MainInfo.m_szATName);
	else if (nBetObjNo == 2)
		strcpy_s(szTeamName, "Draw");

	char szGameName[0x100];
	sprintf_s(szGameName, "%s v %s", pGameData->m_MainInfo.m_szHTName, pGameData->m_MainInfo.m_szATName);
	string s1 = szGameName;
	replace(s1, " ", "+");
	strcpy_s(szGameName, s1.c_str());
	s1 = pOddInf->m_szMarketDescription;
	replace(s1, " ", "+");
	strcpy_s(pOddInf->m_szMarketDescription, 0x40, s1.c_str());
	
	string sUUID = GenUUID();
	DWORD dwRet;
	CString strServerName = _T("www.bwin3818.com");
	char strFormData[4000];
	strcpy_s(strFormData, "");

	try
	{
		pServer = m_pSessionBet->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

		if (nReqKind == REQ_PlaceBetNewBwin)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en-gb/live/betslip/new"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSessionBet->SetCookie(_T("https://www.bwin3818.com"), _T("login"), _T("Success"));
			m_pSessionBet->SetCookie(_T("https://www.bwin3818.com"), _T("zaynar-locale"), _T("en-gb"));
			m_pSessionBet->SetCookie(_T("https://www.bwin3818.com"), _T("_vc_a"), m_szUserInfo);

			sprintf_s(strFormData,
				"betslip={\
\"openstate\":\"open\",\
\"use_promo_cash\":false,\
\"singles\":[{\
\"id\":%llu,\
\"outcome_id\":%llu,\
\"market_id\":\"%llu\",\
\"price\":\"%.2f\",\
\"price_id\":\"%llu\",\
\"price_formatted\":\"%s\",\
\"event_description\":\"%s\",\
\"outcome_description\":\"%s\",\
\"market_description\":\"%s\",\
\"each_way_price\":\"false\",\
\"has_sp\":false,\
\"stake\":\"0.00\",\
\"ew_selected\":false,\
\"error\":\"\"}],\
\"multiples\":[],\
\"logged_in\":\"1\",\
\"price_format\":3,\
\"uuid\":\"%s\"}", dwOddID, dwOddID, pOddInf->m_dwMarketID, pBetInfo->BetVal[nBetObjNo].oVal, pOddInf->m_dwPriceID, pOddInf->m_szPriceFormatted,
szGameName, szTeamName, pOddInf->m_szMarketDescription, sUUID.c_str());
		}		
		else if (nReqKind == REQ_BetSlipSave)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en-gb/live/betslip/save"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSessionBet->SetCookie(_T("https://www.bwin3818.com"), _T("login"), _T("Success"));
			m_pSessionBet->SetCookie(_T("https://www.bwin3818.com"), _T("zaynar-locale"), _T("en-gb"));
			m_pSessionBet->SetCookie(_T("https://www.bwin3818.com"), _T("_vc_a"), m_szUserInfo);

			sprintf_s(strFormData, 
"place_bets=Place+Bets&\
betslip={\
\"openstate\":\"open\",\
\"use_promo_cash\":false,\
\"singles\":[{\
\"id\":%llu,\
\"outcome_id\":%llu,\
\"market_id\":\"%llu\",\
\"price\":\"%.2f\",\
\"price_id\":\"%llu\",\
\"price_formatted\":\"%s\",\
\"event_description\":\"%s\",\
\"outcome_description\":\"%s\",\
\"market_description\":\"%s\",\
\"each_way_price\":\"false\",\
\"has_sp\":false,\
\"stake\":\"%d\",\
\"ew_selected\":false,\
\"error\":\"\"}],\
\"multiples\":[],\
\"logged_in\":\"1\",\
\"price_format\":3,\
\"uuid\":\"%s\"}&\
uuid=%s&\
page_type=tab", dwOddID, dwOddID, pOddInf->m_dwMarketID, pBetInfo->BetVal[nBetObjNo].oVal, pOddInf->m_dwPriceID, pOddInf->m_szPriceFormatted,
			szGameName, szTeamName, pOddInf->m_szMarketDescription, pBetInfo->BetVal[nBetObjNo].nBettingMoney, sUUID.c_str(), sUUID.c_str());
		}

		m_HttpFileBet->SendRequest(NULL, 0, strFormData, strlen(strFormData));
		m_HttpFileBet->QueryInfoStatusCode(dwRet);
		if (dwRet == HTTP_STATUS_OK)
			m_nTextSizeBet = ReadResponse(m_HttpFileBet, m_bufGZipBet, m_bufReadBet, m_szTextBet);
		else
		{
			m_HttpFileBet->Close();
			delete m_HttpFileBet;
			m_HttpFileBet = NULL;

			pServer->Close();
			delete pServer;
			pServer = NULL;
			return BET_FAIL;
		}
		m_HttpFileBet->Close();
		delete m_HttpFileBet;
		m_HttpFileBet = NULL;

		pServer->Close();
		delete pServer;
		pServer = NULL;

	}
	catch (CInternetException* e) {
		if (m_HttpFileBet)
		{
			m_HttpFileBet->Close();
			delete m_HttpFileBet;
			m_HttpFileBet = NULL;
		}
		if (pServer)
		{
			pServer->Close();
			delete pServer;
			pServer = NULL;
		}
		e->Delete();
		return BET_TIMEOUT;
	}
	return BET_SUCCESS;
}

int CBetSite_bwin::DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo)
{
	int nResult = BET_FAIL;
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_PlaceBetNewBwin)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[bwin]:BET ERROR - REQ_PlaceBetNewBwin");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if (!AnalyzePlaceBetNew())
	{
		OutDbgStrAA("\n[bwin]:BET ERROR - AnalyzePlaceBetNew");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_ANALYZE_ERROR1;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_BetSlipSave)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[bwin]:BET ERROR - REQ_BetSlipSave");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if (!AnalyzeBetSlipSave())
	{
		OutDbgStrAA("\n[bwin]:BET ERROR - AnalyzeBetSlipSave");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_ANALYZE_ERROR2;
		goto L_END;
	}

	pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
L_END:
	return 0;
}

BOOL CBetSite_bwin::AnalyzePlaceBetNew()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bwin.AnalyzePlaceBetNew.json"), m_szTextBet);
	return FALSE;
}

BOOL CBetSite_bwin::AnalyzeBetSlipSave()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bwin.AnalyzeBetSlipSave.json"), m_szTextBet);
	return FALSE;
}

int GetCategory(int nID);
#ifdef SUB_THREAD_MODE
BOOL CBetSite_bwin::AnalyzeResData1(char* szText)
{
	if (szText == NULL || strlen(szText) < 20)
		goto L_FALSE;
	{
		UINT64 dwOddID1 = 0, dwOddID2 = 0, dwOddID3 = 0;
		UINT64 dwPriceID1 = 0, dwPriceID2 = 0, dwPriceID3 = 0;
		UINT64 dwMarketID1 = 0, dwMarketID2 = 0, dwMarketID3 = 0;
		char szMarket[0x100], szPriceFmt[0x10];
		Document root;
		ParseResult ok = root.Parse(szText);
		if (!ok)
			goto L_FALSE;
		if (!root.IsObject())
			goto L_FALSE;

		int nID = root["event_id"].GetInt();

		TCHAR szFile[MAX_PATH];
		_stprintf_s(szFile, _T("d:\\bwin.com.thread_%d.json"), nID);
		//WriteResData(szFile, szText);

		char* szTeam = (char*)root["description"].GetString();
		CGameData* pGameData = NULL;
		CGameData* pGameData1 = NULL;

		//OutDbgStrAA("\nLOOKUP : dwID=%d, (%s)", nID, szTeam);

		::EnterCriticalSection(&g_csGameInfoArray);
		BOOL bFound = m_mapGameID.Lookup(nID, pGameData1);
		if (!bFound)
		{
			//ASSERT(0);
			::LeaveCriticalSection(&g_csGameInfoArray);
			goto L_FALSE;
		}

		UINT i, j;

		CGameInfo* pGameInfo = new CGameInfo(pGameData1->m_pGameInfo);
		::LeaveCriticalSection(&g_csGameInfoArray);
		pGameData = pGameInfo->GetGameData(m_nBookMaker);
		pGameData->RemoveAllOddsInfo();

		Value& markets = root["markets"];
		for (i = 0; i < markets.Size(); i++)
		{
			dwOddID1 = 0; dwOddID2 = 0; dwOddID3 = 0;
			dwPriceID1 = 0; dwPriceID2 = 0; dwPriceID3 = 0;
			dwMarketID1 = 0; dwMarketID2 = 0; dwMarketID3 = 0;

			Value& markets_i = markets[i];
			char* szOddsKind = (char*)markets_i["title"].GetString();
			float fVal1 = 0, fVal2 = 0, fVal3 = 0;
			if (strcmp(szOddsKind, "Match Betting") == 0 || strcmp(szOddsKind, "To Win ") == 0 || strcmp(szOddsKind, "Win/Draw/Win") == 0)
			{//Main
				if (strstr((char*)markets_i["period"].GetString(), "Half") != 0 || strstr((char*)markets_i["period"].GetString(), " Period") != 0)
					continue;
				Value& outcomes = markets_i["outcomes"];
				for (j = 0; j < outcomes.Size(); j++)
				{
					Value& outcomes_i = outcomes[j];
					char* szTeamName = (char*)outcomes_i["description"].GetString();
					if (strcmp(szTeamName, "Draw") == 0)
					{
						dwOddID3 = outcomes_i["id"].GetInt64();
						dwPriceID3 = outcomes_i["price_id"].GetInt64();
						dwMarketID3 = outcomes_i["market_id"].GetInt64();
						fVal3 = outcomes_i["price_decimal"].GetFloat();
					}
					//else if (strcmp(szTeamName, pGameData->m_MainInfo.m_szHTName) == 0)
					else if (j == 0)
					{
						dwOddID1 = outcomes_i["id"].GetInt64();
						dwPriceID1 = outcomes_i["price_id"].GetInt64();
						dwMarketID1 = outcomes_i["market_id"].GetInt64();
						strcpy_s(szMarket, outcomes_i["market"].GetString());
						strcpy_s(szPriceFmt, outcomes_i["price"].GetString());
						fVal1 = outcomes_i["price_decimal"].GetFloat();
					}
					//else if (strcmp(szTeamName, pGameData->m_MainInfo.m_szATName) == 0)
					else
					{
						dwOddID2 = outcomes_i["id"].GetInt64();
						dwPriceID2 = outcomes_i["price_id"].GetInt64();
						dwMarketID2 = outcomes_i["market_id"].GetInt64();
						fVal2 = outcomes_i["price_decimal"].GetFloat();
					}
				}
				MyAddOddInfo(pGameData, OI_MAIN, 0, fVal1, fVal2, fVal3, dwOddID1, dwOddID2, dwOddID3,
					dwPriceID1, dwPriceID2, dwPriceID3, dwMarketID1, dwMarketID2, dwMarketID3, szMarket, szPriceFmt);
			}
			else if (strstr(szOddsKind, "Asian Handicap (") != 0/* || strstr(szOddsKind, "Handicap") != 0*/)
			{
				char* sz1 = strstr(szOddsKind, "Asian Handicap (") + 16;
				char* sz2 = strstr(sz1 + 1, ")");
				char* sz = new char[sz2 - sz1 + 1];
				memcpy(sz, sz1, sz2 - sz1);
				sz[sz2 - sz1] = 0;
				float f1 = 0, f2 = 0;
				BOOL bAsian = FALSE;
				if (strstr(sz, ",") == 0)
					sscanf_s(sz, "%f", &f1);
				else
				{
					_asm int 3
					Split(sz, ',', f1, f2);
					bAsian = TRUE;
				}

				Value& outcomes = markets_i["outcomes"];
				for (j = 0; j < outcomes.Size(); j++)
				{
					Value& outcomes_i = outcomes[j];
					char* szTeamName = (char*)outcomes_i["description"].GetString();
					//if (strstr(szTeamName, pGameData->m_MainInfo.m_szHTName) != 0)
					if (j == 0)
					{
						dwOddID1 = outcomes_i["id"].GetInt64();
						dwPriceID1 = outcomes_i["price_id"].GetInt64();
						dwMarketID1 = outcomes_i["market_id"].GetInt64();
						strcpy_s(szMarket, outcomes_i["market"].GetString());
						strcpy_s(szPriceFmt, outcomes_i["price"].GetString());
						fVal1 = outcomes_i["price_decimal"].GetFloat();
					}
					//else if (strstr(szTeamName, pGameData->m_MainInfo.m_szATName) != 0)
					else
					{
						dwOddID2 = outcomes_i["id"].GetInt64();
						dwPriceID2 = outcomes_i["price_id"].GetInt64();
						dwMarketID2 = outcomes_i["market_id"].GetInt64();
						fVal2 = outcomes_i["price_decimal"].GetFloat();
					}
				}
				if (!bAsian)
					MyAddOddInfo(pGameData, OI_HANDICAP, f1, fVal1, fVal2, 0, dwOddID1, dwOddID2, dwOddID3,
						dwPriceID1, dwPriceID2, dwPriceID3, dwMarketID1, dwMarketID2, dwMarketID3, szMarket, szPriceFmt);
			}
			else if ((strstr(szOddsKind, "Total Goals - Over/Under ") != 0 || strstr(szOddsKind, "Total Points Over/Under") != 0 || strstr(szOddsKind, "Total Points U/O") != 0 || strstr(szOddsKind, "Total Goals Under / Over") != 0) &&
				strstr(szOddsKind, "Half") == 0)
			{
				float f = 0;
				Value& outcomes = markets_i["outcomes"];
				for (j = 0; j < outcomes.Size(); j++)
				{
					Value& outcomes_i = outcomes[j];
					char* szTeamName = (char*)outcomes_i["description"].GetString();
					if (strstr(szTeamName, "Over") != 0)
					{
						dwOddID1 = outcomes_i["id"].GetInt64();
						dwPriceID1 = outcomes_i["price_id"].GetInt64();
						dwMarketID1 = outcomes_i["market_id"].GetInt64();
						strcpy_s(szMarket, outcomes_i["market"].GetString());
						strcpy_s(szPriceFmt, outcomes_i["price"].GetString());
						sscanf_s(szTeamName, "Over %f", &f);
						fVal1 = outcomes_i["price_decimal"].GetFloat();
					}
					else if (strstr(szTeamName, "Under") != 0)
					{
						dwOddID2 = outcomes_i["id"].GetInt64();
						dwPriceID2 = outcomes_i["price_id"].GetInt64();
						dwMarketID2 = outcomes_i["market_id"].GetInt64();
						sscanf_s(szTeamName, "Under %f", &f);
						fVal2 = outcomes_i["price_decimal"].GetFloat();
					}
					else
						ASSERT(0);
				}
				MyAddOddInfo(pGameData, OI_GOAL_OU, f, fVal1, fVal2, 0, dwOddID1, dwOddID2, dwOddID3,
					dwPriceID1, dwPriceID2, dwPriceID3, dwMarketID1, dwMarketID2, dwMarketID3, szMarket, szPriceFmt);
			}
			else if ((strstr(szOddsKind, "Odd/Even Total Goals") != 0 || strstr(szOddsKind, "Total Points Odd/Even") != 0 || strstr(szOddsKind, "Total Goals Odd/Even") != 0) && strstr(szOddsKind, "Half") == 0)
			{
				Value& outcomes = markets_i["outcomes"];
				for (j = 0; j < outcomes.Size(); j++)
				{
					Value& outcomes_i = outcomes[j];
					char* szTeamName = (char*)outcomes_i["description"].GetString();
					if (strstr(szTeamName, "Odd") != 0)
					{
						dwOddID1 = outcomes_i["id"].GetInt64();
						dwPriceID1 = outcomes_i["price_id"].GetInt64();
						dwMarketID1 = outcomes_i["market_id"].GetInt64();
						strcpy_s(szMarket, outcomes_i["market"].GetString());
						strcpy_s(szPriceFmt, outcomes_i["price"].GetString());
						fVal1 = outcomes_i["price_decimal"].GetFloat();
					}
					else if (strstr(szTeamName, "Even") != 0)
					{
						dwOddID2 = outcomes_i["id"].GetInt64();
						dwPriceID2 = outcomes_i["price_id"].GetInt64();
						dwMarketID2 = outcomes_i["market_id"].GetInt64();
						fVal2 = outcomes_i["price_decimal"].GetFloat();
					}
					else
						ASSERT(0);
				}
				MyAddOddInfo(pGameData, OI_GOAL_OE, 0, fVal1, fVal2, 0, dwOddID1, dwOddID2, dwOddID3,
					dwPriceID1, dwPriceID2, dwPriceID3, dwMarketID1, dwMarketID2, dwMarketID3, szMarket, szPriceFmt);
			}
		}
		::EnterCriticalSection(&g_csGameInfoArray);
		CopyGameDataProc(pGameData);
		::LeaveCriticalSection(&g_csGameInfoArray);
		theFrame->PostMessageW(MSG_UPDATE_EVENT_LIST, (WPARAM)m_nBookMaker, (LPARAM)nID);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bwin.AnalyzeResData1.json"), m_szTextBet);
	return FALSE;
}
#endif

COddsInfo* CBetSite_bwin::MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, 
	UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3, UINT64 dwPriceID1, UINT64 dwPriceID2, UINT64 dwPriceID3, UINT64 dwMarketID1, UINT64 dwMarketID2, UINT64 dwMarketID3,
	char* szTitle, char* szMarket, char* price_fmt)
{
	COddsInfo* pOddsInfo = new COddsInfo;
	AddOddInfo(pOddsInfo, pGameData, nOddsKind, hVal1, fOval1, fOval2, fOval3, dwOddID1, dwOddID2, dwOddID3);

	ODD_INF* pOddInf = new ODD_INF;
	pOddInf->m_dwPriceID = dwPriceID1;
	pOddInf->m_dwMarketID = dwMarketID1;
	strcpy_s(pOddInf->m_szTitle, MAX_TEAM_NAME * 3, szTitle);
	strcpy_s(pOddInf->m_szPriceFormatted, 0x20, price_fmt);
	strcpy_s(pOddInf->m_szMarketDescription, 0x40, szMarket);
	m_mapOddInf.SetAt(dwOddID1, pOddInf);

	pOddInf = new ODD_INF;
	pOddInf->m_dwPriceID = dwPriceID2;
	pOddInf->m_dwMarketID = dwMarketID2;
	strcpy_s(pOddInf->m_szTitle, MAX_TEAM_NAME * 3, szTitle);
	strcpy_s(pOddInf->m_szPriceFormatted, 0x20, price_fmt);
	strcpy_s(pOddInf->m_szMarketDescription, 0x40, szMarket);
	m_mapOddInf.SetAt(dwOddID2, pOddInf);

	pOddInf = new ODD_INF;
	pOddInf->m_dwPriceID = dwPriceID3;
	pOddInf->m_dwMarketID = dwMarketID3;
	strcpy_s(pOddInf->m_szTitle, MAX_TEAM_NAME * 3, szTitle);
	strcpy_s(pOddInf->m_szPriceFormatted, 0x20, price_fmt);
	strcpy_s(pOddInf->m_szMarketDescription, 0x40, szMarket);
	m_mapOddInf.SetAt(dwOddID3, pOddInf);

	return pOddsInfo;
}

#ifdef SUB_THREAD_MODE
static DWORD WINAPI SubThreadProc(LPVOID lpParameter)
{
	CBetSite_bwin* pSite = (CBetSite_bwin*)lpParameter;
	return pSite->SubThread(NULL);
}

void CBetSite_bwin::CreateSubThread()
{
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!m_hCompletionPort)
		return;

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int nCPUCount = SUBTHREAD_COUNT;// systemInfo.dwNumberOfProcessors;
	for (int i = 0; i < nCPUCount; ++i)
	{
		HANDLE hThread = CreateThread(NULL, 0, ::SubThreadProc, this, 0, NULL);
		CloseHandle(hThread);
	}
}
#endif

int GetCategory(int nID)
{
	switch (nID)
	{
	case 433100: return GC_BASEBALL;
	case 601600: return GC_BASKETBALL;
	case 1250: return GC_BEACH_VOLLEYBALL;
	case 17500: return GC_CRICKET;
	case 100: return GC_FOOTBALL;
	case 1100: return GC_HANDBALL;
	case 2100: return GC_ICE_HOCKEY;
	case 364700: return GC_SNOOKER;
	case 650: return GC_TABLE_TENNIS;
	case 600: return GC_TENNIS;
	case 1200: return GC_VOLLEYBALL;
	case 31400: return GC_WATER_POLO;
	default:
		break;
	}
	return -1;
}

int GetID(int nCat)
{
	switch (nCat)
	{
	case GC_BASEBALL: return 433100;
	case GC_BASKETBALL: return 601600;
	case GC_BEACH_VOLLEYBALL: return 1250;
	case GC_CRICKET: return 17500;
	case GC_FOOTBALL: return 100;
	case GC_HANDBALL: return 1100;
	case GC_ICE_HOCKEY: return 2100;
	case GC_SNOOKER: return 364700;
	case GC_TABLE_TENNIS: return 650;
	case GC_TENNIS: return 600;
	case GC_VOLLEYBALL: return 1200;
	case GC_WATER_POLO: return 31400;
	default:
		break;
	}
	return 0;
}

#ifdef SUB_THREAD_MODE
DWORD CBetSite_bwin::SubThread(LPVOID lpParameter)
{
	ULONG CompletionKey;

	CHttpConnection* pServer = NULL;
	BYTE* bufRead = new BYTE[READ_SIZE];
	BYTE* bufGZip = new BYTE[GZIP_SIZE];
	char* szText = new char[TEXT_SIZE];
	int nGZipSize = 0;

	DWORD dwRet;
	CString strServerName;
	strServerName = _T("www.bwin3818.com");

	DWORD NumBytesSent = 0;
	LPOVERLAPPED ol;
	while (!m_bExitThreadFlg)
	{
		if (!GetQueuedCompletionStatus(m_hCompletionPort, &NumBytesSent, &CompletionKey, &ol, 200))
			continue;
		//main_refresh_url:  "https://www.bwin3818.com/en-gb/live/live/refresh",
		//event_load_url : "https://www.bwin3818.com/en-gb/live/live/:id?sport_id=current_sport_id",
		//event_refresh_url : "https://www.bwin3818.com/en-gb/live/live/:id/refresh?sport_id=current_sport_id",
		COMPLETION_KEY* pKey = (COMPLETION_KEY*)CompletionKey;
		if (pKey == NULL)
			break;
		CString strObj, strMsg;
		strObj.Format(_T("/en-gb/live/live/%ld?sport_id=%d/refresh&version="), pKey->dwGameID, GetID(pKey->nCategory));
		//strMsg.Format(_T("\n/en-gb/live/live/%ld?sport_id=%d/refresh&version="), pKey->dwGameID, GetID(pKey->nCategory));
		//OutDbgStrWW(strMsg);
		delete pKey;
		pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
		CHttpFile* HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObj, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

		HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		HttpFile->AddRequestHeaders(_T("Content-Type:application/json, text/javascript, */*"));

		try
		{
			if (HttpFile->SendRequest(NULL, 0, NULL, 0))
			{
				int nTextSize = 0;
				HttpFile->QueryInfoStatusCode(dwRet);
				if (dwRet == HTTP_STATUS_OK)
					nTextSize = ReadResponse(HttpFile, bufGZip, bufRead, szText);

				AnalyzeResData1(szText);
			}
		}
		catch (CException* e)
		{
			int a = (int)e;
			TCHAR szERR[0x100];
			e->GetErrorMessage(szERR, 0x100);
			//OutDbgStrWW(_T("\n ERR:%s"), szERR);
		}

		HttpFile->Close();
		delete HttpFile;
		HttpFile = NULL;

		pServer->Close();
		delete pServer;
	}
	pServer = NULL;

	delete bufRead;
	delete bufGZip;
	delete szText;
	return 0;
}
#endif

BOOL CBetSite_bwin::UpdateTree(int nIsExitThread)
{
	int nRet = UpdateTree_AllInfoDownAtOnce(nIsExitThread);
	CBetSite::UpdateTree(nIsExitThread);
	return nRet;
}

void CBetSite_bwin::GetScore(char* score, int& hs, int& as)
{
	int a = 0, b = 0;
	hs = 0;
	as = 0;
	char *strToken = NULL;
	char *next_token = NULL;
	strToken = strtok_s(score, " ", &next_token);
	while (strToken != NULL)
	{
		sscanf_s(strToken, "%d-%d", &a, &b);
		strToken = strtok_s(NULL, " ", &next_token);
		if (strToken == NULL)
			return;
		if (a > b)
			hs++;
		else
			as++;
	}
}

BOOL CBetSite_bwin::AnalyzeResLogOut()
{
	m_bExitThreadFlg = TRUE;
	m_nLogInStatus = LS_INIT;
	return TRUE;
}

BOOL CBetSite_bwin::AnalyzeResBalance()
{
	if (m_szText == NULL || strlen(m_szText) < 20)
		goto L_FALSE;
	//	WriteResData(_T("d:\\bwin.com.json"), m_szText);
	{
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;
		if (!root.IsObject())
			goto L_FALSE;
		if (!root.HasMember("total_balance"))
			goto L_FALSE;
		char* szBalance = (char*)root["total_balance"].GetString();
		if (!GetBalance(szBalance))
			goto L_FALSE;
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bwin.AnalyzeResBalance.json"), m_szTextBet);
	return FALSE;
}

BOOL CBetSite_bwin::AnalyzeResLive()
{
	if (m_szText == NULL || strlen(m_szText) < 20)
		goto L_FALSE;
	{
		//<meta name="csrf-token" content="
		char* sessPos = strstr(m_szText, "<meta name=\"csrf-token\" content=\"");
		if (sessPos == NULL)
			goto L_FALSE;
		char* commaPos = strstr(sessPos + 34, "\"");
		if (commaPos == NULL)
			goto L_FALSE;
		*commaPos = 0;
		strcpy_s(m_szAuthToken, sessPos + 33);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bwin.AnalyzeResLive.json"), m_szTextBet);
	return FALSE;
}

BOOL CBetSite_bwin::GetBalance(char* szAmount)
{//"CNY 900.00"
	if (strstr(szAmount, "") == 0)
		return FALSE;

	char *strToken = NULL;
	char *strToken1 = NULL;
	char *next_token = NULL;
	strToken1 = strtok_s(szAmount, " ", &next_token);
	strToken = strtok_s(NULL, ",", &next_token);
	if (strcmp(strToken1, "CNY") != 0)
		ASSERT(0);

	int n = strlen(strToken);
	strToken1 = new char[n + 1];
	memset(strToken1, 0, n + 1);

	int i = 0, j = 0;
	for (i = 0; i < n; i++)
	{
		if (strToken[i] != ',')
		{
			strToken1[j] = strToken[i];
			j++;
		}
	}

	sscanf_s(strToken1, "%f", &g_fBalance[m_nBookMaker]);
	delete strToken1;

	theFrame->m_wndSetting.PostMessageW(MSG_CUR_BALANCE, (WPARAM)m_nBookMaker, (LPARAM)(g_fBalance[m_nBookMaker] * 100));
	return TRUE;
}

void CBetSite_bwin::GetLoginResult(TCHAR* szStr)
{
	TCHAR* sessPos = _tcsstr(szStr, _T("login="));
	if (sessPos == NULL)
		return;
}
