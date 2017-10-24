#include "..\stdafx.h"
#include "BetSite_1xbetchina.h"
#include "..\global.h"
#include "..\GameInfo.h"
#include "..\SportsBetting.h"
#include <fstream>
#include "..\MainFrm.h"

using namespace rapidjson;

//https://1xbetchina.com/en/live/

CBetSite_1xbetchina::CBetSite_1xbetchina()
{
	m_nBookMaker = SITE_1xbetchina;
	_tcscpy_s(m_szHash, _T(""));
	_tcscpy_s(m_szUserID, _T(""));
	strcpy_s(m_szBetGUID, "");
	_tcscpy_s(m_che_h, _T(""));
	_tcscpy_s(m_che_i, _T(""));
	_tcscpy_s(m_che_r, _T(""));
}


CBetSite_1xbetchina::~CBetSite_1xbetchina()
{
}

int CBetSite_1xbetchina::MainProc()
{
	if (m_bExitThreadFlg)
		return 0;
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
				OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - REQ_DATA");
				return 0;
			}
			if (!AnalyzeResData())
			{
				OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - AnalyzeResData");
				return 0;
			}
			if (IsNeedGetBalance())
			{
				if (!SendRequestEx(REQ_BALANCE))
				{
					OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - REQ_BALANCE");
					return 0;
				}
				if (!AnalyzeResBalance())
				{
					OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - AnalyzeResBalance");
					return 0;
				}
			}
			OutDbgStrAA("\n[1xbetchina]****************************");
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
			if (m_nLogInStatus == LS_INIT && strcmp(m_szID, "") != 0 && strcmp(m_szPswd, "") != 0)
			{
				if (!SendRequestEx(REQ_LIVE))
				{
					OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - REQ_LIVE");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE3))
				{
					OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - REQ_LIVE3");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE4))
				{
					OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - REQ_LIVE4");
					return 0;
				}
				if (!SendRequestEx(REQ_LOGIN))
				{
					OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - REQ_LOGIN");
					return 0;
				}
				if (!AnalyzeResLogIn())
				{
					OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - AnalyzeResLogIn");
					return 0;
				}
				//if (!SendRequestEx(REQ_LIVE2))
				//{
				//	OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - REQ_LIVE2");
				//	return 0;
				//}
			}
		}
	}
	return 0;
}

void CBetSite_1xbetchina::RefreshData()
{
	m_nTextSize = 0;
	if (m_bExitThreadFlg)
		return;
	if (!SendRequestEx(REQ_DATA))
	{
		OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - REQ_DATA");
		return;
	}
	if (!AnalyzeResData())
	{
		OutDbgStrAA("\n[1xbetchina]:MAIN ERROR - AnalyzeResData");
		return;
	}
}

BOOL CBetSite_1xbetchina::SendRequest(int nReqKind)
{
	CHttpConnection* pServer = NULL;
	m_nTextSize = 0;

	DWORD dwRet;
	CString strServerName = _T("1xbetchina.com");
	char strFormData[1000];
	strcpy_s(strFormData, "");

	try
	{
		pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

		if (nReqKind == REQ_LIVE)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en/live/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LIVE3)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/getuserdata"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("*/*"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://1xbetchina.com/en/live/"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
		}
		else if (nReqKind == REQ_LIVE4)
		{
			TCHAR obj[0x40];
			_stprintf_s(obj, _T("/en/redirect/stat/run/?_=%s"), GetTimeStampLongW().GetBuffer());
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, obj, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://1xbetchina.com/en/live/"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			//m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			//m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("right_side"), _T("biggerRight"));
			//m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("is_rtl"), _T("1"));
			//TCHAR ts[0x20];
			//_stprintf_s(ts, _T("%s"), GetTimeStampW().GetBuffer());
			//m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("start_megafight"), ts);
			//m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("typeBetNames"), _T("full"));
			//m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("tzo"), _T("8"));
			//m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("blocks"), _T("1,1,1,1,1,1,1,1"));
			//m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("completed_user_settings"), _T("true"));
		}
		else if (nReqKind == REQ_LOGIN)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/user/auth/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:gzip, application/json, text/javascript, */*; q=0.01"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://1xbetchina.com/en/live/"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);

			sprintf_s(strFormData, "uLogin=%s&uPassword=%s", m_szID, m_szPswd);
		}
		else if (nReqKind == REQ_LIVE2)
		{
			TCHAR obj[0x40];
			_stprintf_s(obj, _T("/getuserdata"));
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, obj, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://1xbetchina.com/en/live/"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("right_side"), _T("biggerRight"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("is_rtl"), _T("1"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("typeBetNames"), _T("full"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("tzo"), _T("8"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("blocks"), _T("1,1,1,1,1,1,1,1"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("completed_user_settings"), _T("true"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("dnb"), _T("1"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("uhash"), m_szHash);
			TCHAR szID[0x100];
			ToUnicode(m_szID, szID, CP_ACP);
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("ua"), szID);
		}
		else if (nReqKind == REQ_DATA)
		{
			//m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/LiveFeed/Get1x2_Zip?count=500&lng=en&cfview=0&mode=4&country=90"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/LiveFeed/Get1x2_Zip?count=500&lng=en&mode=4&country=90"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip"));
		}
		else if (nReqKind == REQ_BALANCE)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/user/balance/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:gzip, application/json, text/javascript, */*; q=0.01"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://1xbetchina.com/en/live/"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			TCHAR szID[0x100];
			ToUnicode(m_szID, szID, CP_ACP);
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("ua"), szID);
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("uhash"), m_szHash);
		}
		else if (nReqKind == REQ_LOGOUT)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/user/disconnect"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://1xbetchina.com/en/live/"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			TCHAR szID[0x100];
			ToUnicode(m_szID, szID, CP_ACP);
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("ua"), szID);
			m_pSession->SetCookie(_T("https://1xbetchina.com"), _T("uhash"), m_szHash);
		}
		m_HttpFile->SendRequest(NULL, 0, strFormData, strlen(strFormData));

		m_HttpFile->QueryInfoStatusCode(dwRet);
		if (dwRet != HTTP_STATUS_OK)
			goto L_FALSE;

		BYTE buffer[0x1000];
		DWORD len = 0x1000;
		if (nReqKind == REQ_LIVE)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetSessionVal((TCHAR*)buffer))
				goto L_FALSE;
		}
		else if (nReqKind == REQ_LOGIN)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetHash((TCHAR*)buffer))
				goto L_FALSE;
		}
		//else if (nReqKind == REQ_LIVE4)
		//{
		//	m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
		//	if (!GetCheH((TCHAR*)buffer))
		//		goto L_FALSE;
		//}
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

UINT64 CBetSite_1xbetchina::GetOddID(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo)
{
	if (nBetObjNo == 0)
		return pOddsInfo->m_dwOddID1;
	else if (nBetObjNo == 1)
		return pOddsInfo->m_dwOddID2;
	else if (nBetObjNo == 2)
		return pOddsInfo->m_dwOddID3;
	ASSERT(0);
	return 0;
}

//Including Overtime
//rdata=[{"idGame":139783237,"sportNameText":"27351.+ABA+League.+Supercup.+Cedevita-Budu%C4%87nost+Podgorica+(Including+Overtime)",
//"opp":"Cedevita+-+Budu%C4%87nost+Podgorica","sport":"ABA+League.+Supercup","nameGroup":"Team+Wins","idGroup":101,"type":401,"sobId":"1397832370401",
//"nameBet":"Team+Cedevita++Wins","koef":1.42,"coefText":1.42,"dopV":0,"block":0,"PlayerId":0,"Direction":1,"InstrumentId":0,"Seconds":0,
//"Price":0,"Expired":0,"betBlock":1,"evPlId":0,"IsRelation":0}]&alle=

void CBetSite_1xbetchina::GetNameInfo(BetInfo* pBetInfo, int nBetObjNo, CStringA& strNameGroup, CStringA& strNameBet, int& idGroup, int& type)
{
	strNameGroup = "";
	strNameBet = "";
	if (pBetInfo->nOddsKind == OI_MAIN)
	{
		if (strstr(pBetInfo->szLeague, "Including Overtime") == 0)
		{
			char sz[5];
			sz[0] = '1';
			sz[1] = (char)0xD0;
			sz[2] = (char)0xA5;
			sz[3] = '2';
			sz[4] = 0;
			strNameGroup = sz;
			idGroup = 1;
			if (nBetObjNo == 0)
			{
				type = 1;
				strNameBet = "W+^1^";
			}
			else if (nBetObjNo == 1)
			{
				type = 3;
				strNameBet = "W+^2^";
			}
			else if (nBetObjNo == 2)
			{
				type = 2;
				strNameBet = "Draw";
			}
		}
		else
		{
			strNameGroup = "Team+Wins";
			idGroup = 101;
			if (nBetObjNo == 0)
			{
				type = 401;
				strNameBet.Format("Team+%s++Wins", pBetInfo->szHTeam);
			}
			else if (nBetObjNo == 1)
			{
				type = 402;
				strNameBet.Format("Team+%s++Wins", pBetInfo->szATeam);
			}
		}
	}
	else if (pBetInfo->nOddsKind == OI_GOAL_OU)
	{
		idGroup = 17;
		strNameGroup = "Total";
		if (nBetObjNo == 0)
		{
			type = 9;
			strNameBet.Format("Total+Over+%s", F2SA(pBetInfo->hVal1));
		}
		else if (nBetObjNo == 1)
		{
			type = 10;
			strNameBet.Format("Total+Under+%s", F2SA(pBetInfo->hVal1));
		}
	}
	else if (pBetInfo->nOddsKind == OI_HANDICAP)
	{
		idGroup = 2;
		strNameGroup = "Handicap";
		if (nBetObjNo == 0)
		{
			type = 7;
			if (pBetInfo->hVal1 > 0)
				strNameBet.Format("Handi""\xD1""\x81""ap+^1^+(+%s)", F2SA(pBetInfo->hVal1));
			else
				strNameBet.Format("Handi""\xD1""\x81""ap+^1^+(-%s)", F2SA(-pBetInfo->hVal1));
		}
		else if (nBetObjNo == 1)
		{
			type = 8;
			if (pBetInfo->hVal1 > 0)
				strNameBet.Format("Handi""\xD1""\x81""ap+^2^+(+%s)", F2SA(pBetInfo->hVal1));
			else
				strNameBet.Format("Handi""\xD1""\x81""ap+^2^+(-%s)", F2SA(-pBetInfo->hVal1));
		}
	}
	else if (pBetInfo->nOddsKind == OI_TEAM1_GOAL_OU)
	{
		idGroup = 15;
		strNameGroup = "Team/player+total+1";
		if (nBetObjNo == 0)
		{
			type = 11;
			strNameBet.Format("Individual+Total+^1^+Over+%s", F2SA(pBetInfo->hVal1));
		}
		else if (nBetObjNo == 1)
		{
			type = 12;
			strNameBet.Format("Individual+Total+^1^+Under+%s", F2SA(pBetInfo->hVal1));
		}
	}
	else if (pBetInfo->nOddsKind == OI_TEAM2_GOAL_OU)
	{
		idGroup = 62;
		strNameGroup = "Team/player+total+2";
		if (nBetObjNo == 0)
		{
			type = 13;
			strNameBet.Format("Individual+Total+^2^+Over+%s", F2SA(pBetInfo->hVal1));
		}
		else if (nBetObjNo == 1)
		{
			type = 14;
			strNameBet.Format("Individual+Total+^2^+Under+%s", F2SA(pBetInfo->hVal1));
		}
	}
	else if (pBetInfo->nOddsKind == OI_GOAL_OE)
	{
		idGroup = 14;
		strNameGroup = "Even/Odd";
		if (nBetObjNo == 0)
		{
			type = 182;
			strNameBet.Format("Total+Even+-+Yes");
		}
		else if (nBetObjNo == 1)
		{
			type = 183;
			strNameBet.Format("Total+Even+-+No");
		}
	}
	return;
}

void CBetSite_1xbetchina::GetSobID(BetInfo* pBetInfo, int type, int nGameID, CStringA& strSobID, CStringA& dopV)
{
	if (pBetInfo->nOddsKind == OI_MAIN || pBetInfo->nOddsKind == OI_GOAL_OE)
	{
		strSobID.Format("%d0%d", nGameID, type);
		dopV = "0";
	}
	else
	{
		strSobID.Format("%d%s%d", nGameID, F2SA(pBetInfo->hVal1), type);
		dopV = F2SA(pBetInfo->hVal1);
	}
}

int CBetSite_1xbetchina::SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind)
{
	m_nTextSizeBet = 0;
	CHttpConnection* pServer = NULL;

	DWORD dwRet;
	CString strServerName = _T("1xbetchina.com");
	char strFormData[4000];
	strcpy_s(strFormData, "");

	try
	{
		pServer = m_pSessionBet->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

		if (nReqKind == REQ_SET_RDATA || nReqKind == REQ_SET_RDATA1 || nReqKind == REQ_SET_RDATA2 || nReqKind == REQ_SET_RDATA3)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/datalinelive/setrdata"), _T("https://1xbetchina.com/en/live/"), 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			//m_HttpFileBet->AddRequestHeaders(_T("referer:https://1xbetchina.com/en/live/"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("right_side"), _T("biggerRight"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("is_rtl"), _T("1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("typeBetNames"), _T("full"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("completed_user_settings"), _T("true"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			//m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("che_h"), m_che_h);
			//m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("che_i"), m_che_i);
			//m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("che_r"), m_che_r);
			TCHAR szID[0x100];
			ToUnicode(m_szID, szID, CP_ACP);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("ua"), szID);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("uhash"), m_szHash);

			sprintf_s(strFormData,
					"rdata=[{\
\"idGame\":%d,\
\"sportNameText\":\"%s+\",\
\"opp\":\"%s+-+%s\",\
\"sport\":\"%s+\",\
\"nameGroup\":\"%s\",\
\"idGroup\":%d,\
\"type\":%d,\
\"sobId\":\"%s\",\
\"nameBet\":\"%s\",\
\"koef\":%.2f,\
\"coefText\":\"%.2f\",\
\"dopV\":%s,\
\"block\":0,\"PlayerId\":0,\"Direction\":1,\"InstrumentId\":0,\"Seconds\":0,\"Price\":0,\"Expired\":0,\"evPlId\":0,\"betBlock\":1",
pBetInfo->BetVal[nBetObjNo].dwGameID,
PlusProc(pBetInfo->szLeague).c_str(),
PlusProc(pBetInfo->szHTeam).c_str(), PlusProc(pBetInfo->szATeam).c_str(),
PlusProc(pBetInfo->szLeague).c_str(),
m_strNameGroup.GetBuffer(),
m_idGroup,
m_type,
m_strSobID.GetBuffer(),
m_strNameBet.GetBuffer(),
pBetInfo->BetVal[nBetObjNo].oVal,
pBetInfo->BetVal[nBetObjNo].oVal,
m_dopV.GetBuffer());

			if (nReqKind == REQ_SET_RDATA)
			{
				strcat_s(strFormData, "}]&alle=0");
			}
			else if (nReqKind == REQ_SET_RDATA1)
			{
				strcat_s(strFormData, ",\"IsRelation\":0}]&alle=0");
			}
			else if (nReqKind == REQ_SET_RDATA2)
			{
				strcat_s(strFormData, ",\"IsRelation\":0,\"ex\":0}]&alle=0");
			}
			else if (nReqKind == REQ_SET_RDATA3)
			{
				strcpy_s(strFormData, 4000, "rdata=[]&alle=0");
			}
		}
		else if (nReqKind == REQ_UPDATE_COUPON)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/LiveUtil/UpdateCoupon"), _T("https://1xbetchina.com/en/live/"), 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			//m_HttpFileBet->AddRequestHeaders(_T("referer:https://1xbetchina.com/en/live/"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/json"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("right_side"), _T("biggerRight"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("is_rtl"), _T("1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("typeBetNames"), _T("full"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("tzo"), _T("8"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("blocks"), _T("1,1,1,1,1,1,1,1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("completed_user_settings"), _T("true"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			//m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("che_h"), m_che_h);
			//m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("che_i"), m_che_i);
			//m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("che_r"), m_che_r);
			TCHAR szID[0x100];
			ToUnicode(m_szID, szID, CP_ACP);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("ua"), szID);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("uhash"), m_szHash);

			sprintf_s(strFormData,
"{\"Events\":[{\"GameId\":%d,\"Type\":%d,\"Coef\":%.2f,\"Param\":%s,\"PlayerId\":0,\"Kind\":1,\"InstrumentId\":0,\"Seconds\":0,\"Price\":0,\"Expired\":0}],\
\"NeedUpdateLine\":false,\"Lng\":\"en\",\"UserId\":\"%s\",\"CfView\":0}", 
pBetInfo->BetVal[nBetObjNo].dwGameID, m_type, pBetInfo->BetVal[nBetObjNo].oVal, m_dopV.GetBuffer(), m_szID);
		}
		else if (nReqKind == REQ_EXPRESS)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/datalinelive/generateexpress?page=0&cyber=0&kind=1"), _T("https://1xbetchina.com/en/live/"), 1, NULL, NULL, INTERNET_FLAG_SECURE);
			m_HttpFileBet->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			//m_HttpFileBet->AddRequestHeaders(_T("referer:https://1xbetchina.com/en/live/Football/"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("right_side"), _T("biggerRight"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("dnb"), _T("1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("is_rtl"), _T("1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("typeBetNames"), _T("full"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("tzo"), _T("8"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("blocks"), _T("1,1,1,1,1,1,1,1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("completed_user_settings"), _T("true"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			//m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("che_h"), m_che_h);
			//m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("che_i"), m_che_i);
			//m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("che_r"), m_che_r);
			TCHAR szID[0x100];
			ToUnicode(m_szID, szID, CP_ACP);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("ua"), szID);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("uhash"), m_szHash);
		}
		else if (nReqKind == REQ_BET_GETDATA)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/LiveFeed/GetSportsShortZip?lng=en&country=1"), _T("https://1xbetchina.com/en/live/"), 1, NULL, NULL, INTERNET_FLAG_SECURE);
			m_HttpFileBet->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFileBet->AddRequestHeaders(_T("If-Modified-Since:Sat, 1 Jan 2000 00:00:00 GMT"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("dnb"), _T("1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("is_rtl"), _T("1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("typeBetNames"), _T("full"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("tzo"), _T("8"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("blocks"), _T("1,1,1,1,1,1,1,1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("completed_user_settings"), _T("true"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("right_side"), _T("biggerRight"));
			TCHAR szID[0x100];
			ToUnicode(m_szID, szID, CP_ACP);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("ua"), szID);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("uhash"), m_szHash);
		}
		else if (nReqKind == REQ_PUTS_BETCOMMON || nReqKind == REQ_PUTS_BETCOMMON1)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/datalinelive/putbetscommon"), _T("https://1xbetchina.com/en/live/"), 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("dnb"), _T("1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("is_rtl"), _T("1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("typeBetNames"), _T("full"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("tzo"), _T("8"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("blocks"), _T("1,1,1,1,1,1,1,1"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("completed_user_settings"), _T("true"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("right_side"), _T("biggerRight"));
			TCHAR szID[0x100];
			ToUnicode(m_szID, szID, CP_ACP);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("ua"), szID);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("uhash"), m_szHash);

			//UINT64 nType = GetOddID(pBetInfo, nBetObjNo, pOddsInfo);
			char szHash[0x100];
			ToAscii(m_szHash, szHash, CP_ACP);
			if (nReqKind == REQ_PUTS_BETCOMMON)
			{
				sprintf_s(strFormData,
"HasSpecialCoeffs=0&Live=true&\
Events[0][GameId]=%d&Events[0][Type]=%d&Events[0][Coef]=%.2f&Events[0][Param]=%s&Events[0][PlayerId]=0&Events[0][Kind]=1&\
Events[0][InstrumentId]=0&Events[0][Seconds]=0&Events[0][Price]=0&Events[0][Expired]=0&\
Summ=%d&Lng=en&UserId=%s&Vid=0&hash=%s&CfView=0&notWait=true&Source=50&CheckCf=2", 
pBetInfo->BetVal[nBetObjNo].dwGameID, m_type, pBetInfo->BetVal[nBetObjNo].oVal, m_dopV.GetBuffer(), pBetInfo->BetVal[nBetObjNo].nBettingMoney, m_szID, szHash);
			}
			else if (nReqKind == REQ_PUTS_BETCOMMON1)
			{
				//ASSERT(strcmp(m_szBetGUID, "") != 0);
				sprintf_s(strFormData,
"HasSpecialCoeffs=0&Live=false&\
Events[0][GameId]=%d&Events[0][Type]=%d&Events[0][Coef]=%.2f&Events[0][Param]=%s&Events[0][PlayerId]=0&Events[0][Kind]=1&\
Events[0][InstrumentId]=0&Events[0][Seconds]=0&Events[0][Price]=0&Events[0][Expired]=0&\
Summ=%d&Lng=en&UserId=%s&Vid=0&hash=%s&CfView=0&notWait=true&Source=50&CheckCf=2&betGUID=%s", 
pBetInfo->BetVal[nBetObjNo].dwGameID, m_type, pBetInfo->BetVal[nBetObjNo].oVal, m_dopV.GetBuffer(), pBetInfo->BetVal[nBetObjNo].nBettingMoney, m_szID, szHash, m_szBetGUID);
			}
		}
		else if (nReqKind == REQ_BET_BALANCE1)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/user/balance/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Accept:gzip, application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Referer:https://1xbetchina.com/en/live/"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("lng"), _T("en"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("cur"), _T("CNY"));
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("SESSION"), m_szSession);
			TCHAR szID[0x100];
			ToUnicode(m_szID, szID, CP_ACP);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("ua"), szID);
			m_pSessionBet->SetCookie(_T("https://1xbetchina.com"), _T("uhash"), m_szHash);
		}
		m_HttpFileBet->SendRequest(NULL, 0, strFormData, strlen(strFormData));

		m_HttpFileBet->QueryInfoStatusCode(dwRet);
		if (dwRet == HTTP_STATUS_OK)
		{
			m_nTextSizeBet = ReadResponse(m_HttpFileBet, m_bufGZipBet, m_bufReadBet, m_szTextBet);
		}
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

int CBetSite_1xbetchina::DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo)
{
	GetNameInfo(pBetInfo, nBetObjNo, m_strNameGroup, m_strNameBet, m_idGroup, m_type);
	GetSobID(pBetInfo, m_type, pBetInfo->BetVal[nBetObjNo].dwGameID, m_strSobID, m_dopV);

	int nResult = BET_FAIL;
	OutDbgStrAA("\n ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_SET_RDATA)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_SET_RDATA");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_UPDATE_COUPON)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_UPDATE_COUPON");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	float coeff = 0;
	if (!AnalyzeBetUpdateCoupon(pBetInfo, nBetObjNo, coeff))
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - AnalyzeBetUpdateCoupon");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_ANALYZE_ERROR2;
		goto L_END;
	}
	
	{
		pBetInfo->BetVal[nBetObjNo].oVal_orig = pBetInfo->BetVal[nBetObjNo].oVal;
		pBetInfo->BetVal[nBetObjNo].oVal_cur = coeff;
		if (coeff >= pBetInfo->BetVal[nBetObjNo].oVal)
			pBetInfo->BetVal[nBetObjNo].oVal = coeff;
		else
		{
			OutDbgStrAA("\n[1xbetchina]:BET ERROR - Coeff Decreased");
			pBetInfo->BetVal[nBetObjNo].nResult = BET_VALUE_DECREASED;
			goto L_END;
		}
	}

	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_SET_RDATA1)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_SET_RDATA1");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_UPDATE_COUPON)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_UPDATE_COUPON");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	coeff = 0;
	if (!AnalyzeBetUpdateCoupon(pBetInfo, nBetObjNo, coeff))
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - AnalyzeBetUpdateCoupon");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_ANALYZE_ERROR2;
		goto L_END;
	}
	
	{
		pBetInfo->BetVal[nBetObjNo].oVal_orig = pBetInfo->BetVal[nBetObjNo].oVal;
		pBetInfo->BetVal[nBetObjNo].oVal_cur = coeff;
		if (coeff >= pBetInfo->BetVal[nBetObjNo].oVal)
			pBetInfo->BetVal[nBetObjNo].oVal = coeff;
		else
		{
			OutDbgStrAA("\n[1xbetchina]:BET ERROR - Coeff Decreased");
			pBetInfo->BetVal[nBetObjNo].nResult = BET_VALUE_DECREASED;
			goto L_END;
		}
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_BET_BALANCE1)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_BALANCE");
		return 0;
	}
	float f1, f2;
	if (!AnalyzeBetResBalance(1, f1))
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - AnalyzeResBalance");
		return 0;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_PUTS_BETCOMMON)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_PUTS_BETCOMMON");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	int waitTime = 0;
	nResult = AnalyzeBetPutBet(waitTime);
	if (nResult == BET_SUCCESS1)
	{
		pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
		return 0;
	}
	if (nResult != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - AnalyzeBetPutBet");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	Sleep(waitTime);
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_BET_BALANCE1)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_BALANCE");
		return 0;
	}
	if (!AnalyzeBetResBalance(2, f2))
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - AnalyzeResBalance");
		return 0;
	}
	if (f1 == f2)
	{
		if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_PUTS_BETCOMMON1)) != BET_SUCCESS)
		{
			OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_PUTS_BETCOMMON1");
			pBetInfo->BetVal[nBetObjNo].nResult = nResult;
			goto L_END;
		}
		nResult = AnalyzeBetPutBet1();
		if (nResult != BET_SUCCESS)
		{
			OutDbgStrAA("\n[1xbetchina]:BET ERROR - AnalyzeBetPutBet1");
			pBetInfo->BetVal[nBetObjNo].nResult = nResult;
			goto L_END;
		}
		if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_BET_BALANCE1)) != BET_SUCCESS)
		{
			OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_BALANCE");
			return 0;
		}
		if (!AnalyzeBetResBalance(3, f2))
		{
			OutDbgStrAA("\n[1xbetchina]:BET ERROR - AnalyzeResBalance");
			return 0;
		}
	}
	else
	{
		OutDbgStrAA("\n WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW");
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_SET_RDATA3)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_SET_RDATA3");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;// nResult;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_EXPRESS)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[1xbetchina]:BET ERROR - REQ_EXPRESS");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;// nResult;
		goto L_END;
	}

	pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
L_END:
	return 0;
}

BOOL CBetSite_1xbetchina::AnalyzeBetSetRData()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	//WriteResData(_T("c:\\1xbetchina.rdata.json"), m_szTextBet);
	return TRUE;

L_FALSE:
	WriteResData(_T("c:\\1xbetchina.AnalyzeSetRData.json"), m_szTextBet);
	return FALSE;
}

BOOL CBetSite_1xbetchina::AnalyzeBetUpdateCoupon(BetInfo* pBetInfo, int nBetObjNo, float& coeff)
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("c:\\1xbetchina.rdata.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		if (!root.HasMember("Value") || !root["Value"].HasMember("Coef"))
			goto L_FALSE;
		coeff = root["Value"]["Coef"].GetFloat();
	}
	return TRUE;

L_FALSE:
	WriteResData(_T("c:\\1xbetchina.AnalyzeUpdateCoupon.json"), m_szTextBet);
	return FALSE;
}

int CBetSite_1xbetchina::AnalyzeBetPutBet(int& waitTime)
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("c:\\1xbetchina.AnalyzePutBet.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		int nErr = root["ErrorCode"].GetInt();
		if (nErr != 0)
		{
			if (nErr == 104) // "Unable to place bet. Failed to check events",
				return BET_CHECK_EVENT_FAIL;
			if (nErr == 129) // "Unable to place a bet on the outcome of 'Sam Barry - Alexander Knight'"
				return BET_UNABLE;
			if (nErr == 130) // "The odds have changed for event 'China U19 - Tajikistan U19'"
				return BET_VALUE_CHANGED;
			if (nErr == 131) //"Selected odds for event 'Moreirense - Sporting' are temporarily blocked for betting!",
				return BET_BLOCKED;
			if (nErr == 134) // "You have already placed a bet on 'Torpedo Vladimir - Pskov-747'"
				return BET_ALREADY_PLACED;
			if (nErr == 135)
				return BET_BLOCKED;
			_asm int 3
			goto L_FALSE;
		}
		if (!root.HasMember("Value"))
			goto L_FALSE;

		strcpy_s(m_szBetGUID, "");
		if (!root["Value"]["betGUID"].IsNull())
		{
			strcpy_s(m_szBetGUID, 0x100, root["Value"]["betGUID"].GetString());
			waitTime = root["Value"]["waitTime"].GetInt();
		}
		else
		{
			float fBal = root["Value"]["Balance"].GetFloat();
			if (fBal == 0)
				return BET_SUCCESS1;
		}
	}
	return BET_SUCCESS;
L_FALSE:
	WriteResData(_T("d:\\1xbetchina.AnalyzePutBet.json"), m_szTextBet);
	return BET_PARSE_FAIL;
}

int CBetSite_1xbetchina::AnalyzeBetPutBet1()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\1xbetchina.AnalyzePutBet1.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		int nErr = root["ErrorCode"].GetInt();
		if (nErr != 0)
		{
			if (nErr == 104) // "Unable to place bet. Failed to check events",
				return BET_CHECK_EVENT_FAIL;
			if (nErr == 129) // "Unable to place a bet on the outcome of 'Sam Barry - Alexander Knight'"
				return BET_UNABLE;
			if (nErr == 130) // "The odds have changed for event 'China U19 - Tajikistan U19'"
				return BET_VALUE_CHANGED;
			if (nErr == 131) //"Selected odds for event 'Moreirense - Sporting' are temporarily blocked for betting!",
				return BET_BLOCKED;
			if (nErr == 135)
				return BET_BLOCKED;
			_asm int 3
			goto L_FALSE;
		}
		float fBal = root["Value"]["Balance"].GetFloat();
		if (fBal == 0)
			return BET_ANALYZE_ERROR6;
		g_fBalance[m_nBookMaker] = fBal;
	}

	return BET_SUCCESS;
L_FALSE:
	WriteResData(_T("d:\\1xbetchina.AnalyzePutBet1.json"), m_szTextBet);
	return BET_PARSE_FAIL;
}

BOOL CBetSite_1xbetchina::AnalyzeResRedirect()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\1xbetchina.AnalyzeResRedirect.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_1xbetchina::AnalyzeResLogIn()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;

	//WriteResData(_T("d:\\1xbetchina.login.json"), m_szText);
	{
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;

		if (root.HasMember("success") == FALSE)
			goto L_FALSE;

		bool val = root["success"].GetBool();
		if (val)
		{
			m_nLogInStatus = LS_SUCCESS;
			theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)1);
		}
		else
		{
			m_nLogInStatus = LS_FAIL;
			theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)2);
		}
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\1xbetchina.AnalyzeResLogIn.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_1xbetchina::AnalyzeResLogOut()
{
	m_bExitThreadFlg = TRUE;
	m_nLogInStatus = LS_INIT;
	return TRUE;
}

BOOL CBetSite_1xbetchina::AnalyzeResBalance()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	//WriteResData(_T("d:\\1xbetchina.AnalyzeResBalance.json"), m_szText);
	{
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;

		if (root.HasMember("balance") == FALSE)
			goto L_FALSE;

		Value& balance = root["balance"];

		Value& balance_i = balance[0];

		char* kode = (char*)balance_i["kode"].GetString();
		g_fBalance[m_nBookMaker] = balance_i["money"].GetFloat();

		theFrame->m_wndSetting.PostMessageW(MSG_CUR_BALANCE, (WPARAM)m_nBookMaker, (LPARAM)(g_fBalance[m_nBookMaker] * 100));
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\1xbetchina.AnalyzeResBalance.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_1xbetchina::AnalyzeBetResBalance(int nStep, float& fMoney)
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	//WriteResData(_T("d:\\1xbetchina.AnalyzeBetResBalance.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;

		if (root.HasMember("balance") == FALSE)
			goto L_FALSE;

		Value& balance = root["balance"];

		Value& balance_i = balance[0];

		char* kode = (char*)balance_i["kode"].GetString();
		g_fBalance[m_nBookMaker] = balance_i["money"].GetFloat();

		char msg[0x40];
		sprintf_s(msg, "\n[%d]==================== %f =========================", nStep, g_fBalance[m_nBookMaker]);
		OutDbgStrAA(msg);
		fMoney = g_fBalance[m_nBookMaker];

		theFrame->m_wndSetting.PostMessageW(MSG_CUR_BALANCE, (WPARAM)m_nBookMaker, (LPARAM)(g_fBalance[m_nBookMaker] * 100));
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\1xbetchina.AnalyzeBetResBalance.json"), m_szTextBet);
	return FALSE;
}

BOOL CBetSite_1xbetchina::AnalyzeResData()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\1xbetchina.AnalyzeResData.json"), m_szText);
	{
		UINT i, j;
		DWORD dwOddID1 = 0, dwOddID2 = 0, dwOddID3 = 0;
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;

		if (root.HasMember("Value") == FALSE)
			goto L_FALSE;

		Value& val = root["Value"];
		//OutDbgStrAA_1xbetchina("\n====================================");
		int nCount = val.Size();
		for (i = 0; i < val.Size(); i++)
		{
			//if (strcmp(Val_i["SN"].GetString(), "Field Hockey") == 0)
			//	_asm int 3
			Value& Val_i = val[i];
			int nGameCategory = GetGameCategory((char*)Val_i["SN"].GetString());
			if (nGameCategory == -1 || !theFrame->m_bSetCategory[nGameCategory])
				continue;

			if (!Val_i.HasMember("O1") || !Val_i.HasMember("O2"))
				continue;
			//OutDbgStrAA_1xbetchina("\n%s(%s, %s)", Val_i["SN"].GetString(), Val_i["O1"].GetString(), Val_i["O2"].GetString());

			int nID = Val_i["I"].GetInt();
			const char* ht = rtrim((char*)Val_i["O1"].GetString(), "\t ");
			const char* at = rtrim((char*)Val_i["O2"].GetString(), "\t ");

			if (!Val_i.HasMember("SC"))
				continue;
			Value& val_sc = Val_i["SC"];
			if (!val_sc.HasMember("FS"))
				continue;
			Value& val_fs = val_sc["FS"];
			int hs = 0, as = 0, hs1 = -1, as1 = -1, hs2 = -1, as2 = -1;
			if (val_fs.HasMember("S1"))
				hs = val_fs["S1"].GetInt();
			if (val_fs.HasMember("S2"))
				as = val_fs["S2"].GetInt();
			int ipt = 0;
			if (val_sc.HasMember("TS"))
				ipt = val_sc["TS"].GetInt();
			string stage = "";
			if (val_sc.HasMember("CPS"))
				stage = (char*)val_sc["CPS"].GetString();

			if (nGameCategory == GC_TENNIS || nGameCategory == GC_TABLE_TENNIS || nGameCategory == GC_BASEBALL || nGameCategory == GC_VOLLEYBALL)
			{
				int nCur = 0;
				if (val_sc.HasMember("CP"))
				{
					nCur = val_sc["CP"].GetInt();
					if (val_sc.HasMember("PS"))
					{
						Value& val_ps = val_sc["PS"];
						for (UINT kk = 0; kk < val_ps.Size(); kk++)
						{
							if (val_ps[kk]["Key"].GetInt() == nCur)
							{
								hs1 = 0; as1 = 0;
								if (val_ps[kk]["Value"].HasMember("S1"))
									hs1 = val_ps[kk]["Value"]["S1"].GetInt();
								if (val_ps[kk]["Value"].HasMember("S2"))
									as1 = val_ps[kk]["Value"]["S2"].GetInt();
							}
						}
						if (nGameCategory == GC_TENNIS)
						{
							hs2 = 0; as2 = 0;
							if (val_sc.HasMember("SS"))
							{
								Value& val_ss = val_sc["SS"];
								char *szhs2, *szas2;
								if (val_ss.HasMember("S1"))
								{
									szhs2 = (char*)val_ss["S1"].GetString();
									sscanf_s(szhs2, "%d", &hs2);
								}
								if (val_ss.HasMember("S2"))
								{
									szas2 = (char*)val_ss["S2"].GetString();
									sscanf_s(szas2, "%d", &as2);
								}
							}
						}
					}
				}
			}
			char szLeague[MAX_LEAGUE_NAME], szLeagueInfo[MAX_LEAGUE_NAME];
			strcpy_s(szLeague, "");
			if (Val_i.HasMember("L") && strlen((char*)Val_i["L"].GetString()) < MAX_LEAGUE_NAME)
				strcpy_s(szLeague, (char*)Val_i["L"].GetString());
			if (Val_i.HasMember("V"))
			{
				strcpy_s(szLeagueInfo, (char*)Val_i["V"].GetString());
				if (strcmp(szLeagueInfo, "") != 0)
				{
					strcat_s(szLeague, " (");
					strcat_s(szLeague, szLeagueInfo);
					strcat_s(szLeague, ")");
				}
			}
			if (IsExceptionGame((char*)ht, (char*)at, szLeague, (char*)stage.c_str()))
				continue;
			//if (strstr(ht, "Toluca") || strstr(ht, "Cruz"))
			//	_asm int 3
			CMainInfo MainInfo(nGameCategory, (char*)ht, (char*)at, SCORE(hs, hs1, hs2), SCORE(as, as1, as2), (char*)stage.c_str(), ipt, szLeague);
			::EnterCriticalSection(&g_csGameInfoArray);
			CGameInfo* pGameInfo2 = NULL;
			if (!GetGameInfoProc(&MainInfo, &pGameInfo2, nID))
			{
				::LeaveCriticalSection(&g_csGameInfoArray);
				continue;
			}
			CGameInfo* pGameInfo = new CGameInfo(pGameInfo2);
			CGameData* pGameData = pGameInfo->GetGameData(m_nBookMaker);
			::LeaveCriticalSection(&g_csGameInfoArray);

			if (pGameInfo->m_nSiteID == m_nBookMaker && pGameData->m_bHAChanged)
				_asm int 3
			COddsInfo* pOddsInfo = NULL;
			float n1, n2;
			char* s1 = NULL;
			char* s2 = NULL;
			char* s3 = NULL;
			float f1, f2;

			if (!Val_i.HasMember("E"))
				continue;
			Value& E = Val_i["E"];
			UINT nSize_E = E.Size();

			pGameData->RemoveAllOddsInfo();
			//		if (strstr(pGameData->m_szHTName, "Afgh") != 0)
			//			_asm int 3;

			//GameId: res[i].I,
			//Num : res[i].N,
			//SportId : res[i].SI,
			//EventGroup : res[i].G,
			//Cf : res[i].C,
			//Prm : res[i].P || 0,
			//Type : res[i].T,
			//MarketName : res[i].MN || '',
			//Player : res[i].PL || '',
			//PeriodName : res[i].PN || '',
			//ExpressId : res[i].EI,
			//CfNewView : res[i].CV,
			//SportName : res[i].SN || '',
			//SportNameRus : res[i].SR || '',
			//Opp1 : res[i].O1 || '',
			//Opp2 : res[i].O2 || '',
			//Champ : res[i].L || '',
			//ChampRus : res[i].LR || '',
			//GameVid : res[i].V || '',
			//GameType : res[i].TG || '',
			//CurrPeriodStr : res[i].PS || '',
			//TimeDirection : res[i].TD,
			//Start : createDate(res[i].S),
			//S1 : res[i].S1,
			//S2 : res[i].S2,
			//CurrentPeriod : res[i].CP || '',
			//live : live || 0,
			//ChampEng : res[i].LE || '',
			//SportNameEng : res[i].SE || '',
			//Opp1Eng : res[i].O1E || '',
			//Opp2Eng : res[i].O2E || '',
			//LigaId : res[i].LI || 0,
			//Kind : res[i].KND || 0,
			//GameDescript : res[i].TN

			j = 0;
			while (j < nSize_E)
			{
				float hVal1 = 0, hVal2 = 0, oVal1 = 0, oVal2 = 0;
				dwOddID1 = 0; dwOddID2 = 0; dwOddID3 = 0;

				Value& e_j = E[j];
				if (!e_j.HasMember("C") || !e_j.HasMember("G") || !e_j.HasMember("T"))
					continue;
				Value& e_j_c = e_j["C"];
				Value& e_j_g = e_j["G"];
				Value& e_j_t = e_j["T"];

				if (e_j_g.GetInt() == 1 || e_j_g.GetInt() == 101)//Main
				{
					int n = 0;
					MainOddsProc(pGameData, E, j, n);
					j += n;
				}
				else if (e_j_g.GetInt() == 2)//Handicap
				{
					n1 = 0;
					if (e_j.HasMember("P"))
						n1 = e_j["P"].GetFloat();

					f1 = e_j_c.GetFloat();
					DWORD dwOddID = e_j["T"].GetInt();
					if (j + 1 >= nSize_E)
						break;
					if (E[j + 1]["G"].GetInt() != 2) // not continued pair
					{
						pOddsInfo = GetOdds(pGameData, OI_HANDICAP, n1);
						if (!pOddsInfo)
						{
							pOddsInfo = MyAddOddInfo(pGameData, OI_HANDICAP, n1, 0, 0, 0, dwOddID1, dwOddID2, dwOddID3);///////?????????
						}
						if (e_j_t.GetInt() % 2 == 1)
						{
							UpdateOddInfo(pGameData, pOddsInfo, OI_HANDICAP, f1, 0, 0, dwOddID);
						}
						else
						{
							UpdateOddInfo(pGameData, pOddsInfo, OI_HANDICAP, 0, f1, 0, dwOddID);
						}

						j++;
						continue;
					}
					n2 = 0;
					if (E[j + 1].HasMember("P"))
						n2 = E[j + 1]["P"].GetFloat();
					f2 = E[j + 1]["C"].GetFloat();
					dwOddID1 = E[j]["T"].GetInt();
					dwOddID2 = E[j + 1]["T"].GetInt();

					ASSERT(n1 + n2 == 0);

					MyAddOddInfo(pGameData, OI_HANDICAP, n1, f1, f2, 0, dwOddID1, dwOddID2, 0);

					j += 2;
				}
				else if (e_j_g.GetInt() == 17)//Goal OU
				{
					n1 = 0;
					if (e_j.HasMember("P"))
						n1 = e_j["P"].GetFloat();
					f1 = e_j_c.GetFloat();
					DWORD dwOddID = e_j["T"].GetInt();
					if (j + 1 >= nSize_E)
						break;
					if (E[j + 1]["G"].GetInt() != 17)
					{
						pOddsInfo = GetOdds(pGameData, OI_GOAL_OU, n1);
						if (!pOddsInfo)
						{
							pOddsInfo = MyAddOddInfo(pGameData, OI_GOAL_OU, n1, 0, 0, 0, dwOddID1, dwOddID2, dwOddID3);
						}
						if (e_j_t.GetInt() % 2 == 1)
							UpdateOddInfo(pGameData, pOddsInfo, OI_GOAL_OU, f1, 0, 0, dwOddID);
						else
							UpdateOddInfo(pGameData, pOddsInfo, OI_GOAL_OU, 0, f1, 0, dwOddID);

						j++;
						continue;
					}
					n2 = 0;
					if (E[j + 1].HasMember("P"))
						n2 = E[j + 1]["P"].GetFloat();
					f2 = E[j + 1]["C"].GetFloat();
					dwOddID1 = E[j]["T"].GetInt();
					dwOddID2 = E[j + 1]["T"].GetInt();

					ASSERT(n1 == n2);
					MyAddOddInfo(pGameData, OI_GOAL_OU, n1, f1, f2, 0, dwOddID1, dwOddID2, 0);
					j += 2;
				}
				else if (e_j_g.GetInt() == 15)//Team1 OU
				{
					n1 = 0;
					if (e_j.HasMember("P"))
						n1 = e_j["P"].GetFloat();
					f1 = e_j_c.GetFloat();
					DWORD dwOddID = e_j["T"].GetInt();
					if (j + 1 >= nSize_E)
						break;
					if (E[j + 1]["G"].GetInt() != 15)
					{
						pOddsInfo = GetOdds(pGameData, OI_TEAM1_GOAL_OU, n1);
						if (!pOddsInfo)
						{
							pOddsInfo = MyAddOddInfo(pGameData, OI_TEAM1_GOAL_OU, n1, 0, 0, 0, dwOddID1, dwOddID2, dwOddID3);
						}
						if (e_j_t.GetInt() % 2 == 1)
							UpdateOddInfo(pGameData, pOddsInfo, OI_TEAM1_GOAL_OU, f1, 0, 0, dwOddID);
						else
							UpdateOddInfo(pGameData, pOddsInfo, OI_TEAM1_GOAL_OU, 0, f1, 0, dwOddID);
						j++;
						continue;
					}
					n2 = 0;
					if (E[j + 1].HasMember("P"))
						n2 = E[j + 1]["P"].GetFloat();
					f2 = E[j + 1]["C"].GetFloat();
					dwOddID1 = E[j]["T"].GetInt();
					dwOddID2 = E[j + 1]["T"].GetInt();

					ASSERT(n1 == n2);
					MyAddOddInfo(pGameData, OI_TEAM1_GOAL_OU, n1, f1, f2, 0, dwOddID1, dwOddID2, 0);
					j += 2;
				}
				else if (e_j_g.GetInt() == 62)//Team2 OU
				{
					n1 = 0;
					if (e_j.HasMember("P"))
						n1 = e_j["P"].GetFloat();
					f1 = e_j_c.GetFloat();
					DWORD dwOddID = e_j["T"].GetInt();
					if (j + 1 >= nSize_E)
						break;
					if (E[j + 1]["G"].GetInt() != 62)
					{
						pOddsInfo = GetOdds(pGameData, OI_TEAM2_GOAL_OU, n1);
						if (!pOddsInfo)
						{
							pOddsInfo = MyAddOddInfo(pGameData, OI_TEAM2_GOAL_OU, n1, 0, 0, 0, dwOddID1, dwOddID2, dwOddID3);
						}
						if (e_j_t.GetInt() % 2 == 1)
							UpdateOddInfo(pGameData, pOddsInfo, OI_TEAM2_GOAL_OU, f1, 0, 0, dwOddID);
						else
							UpdateOddInfo(pGameData, pOddsInfo, OI_TEAM2_GOAL_OU, 0, f1, 0, dwOddID);
						j++;
						continue;
					}
					n2 = 0;
					if (E[j + 1].HasMember("P"))
						n2 = E[j + 1]["P"].GetFloat();
					f2 = E[j + 1]["C"].GetFloat();
					dwOddID1 = E[j]["T"].GetInt();
					dwOddID2 = E[j + 1]["T"].GetInt();

					ASSERT(n1 == n2);
					MyAddOddInfo(pGameData, OI_TEAM2_GOAL_OU, n1, f1, f2, 0, dwOddID1, dwOddID2, 0);
					j += 2;
				}
				else if (e_j_g.GetInt() == 8 || //double chance[3]
					e_j_g.GetInt() == 19 || //Both Team To Score(Yes/No)[2]
					e_j_g.GetInt() == 2766 || //1X2 In Regular Time[3]
					e_j_g.GetInt() == 2250)//???[3]
					j++;
				else
				{
					int nn = e_j_g.GetInt();
					//OutDbgStrAA("%d (%s:%s), %f, %d", nGameCategory, ht, at, e_j_c.GetFloat(), nn);
					j++;
				}
				//OutDbgStrAA_1xbetchina("\n%d, %f, %f, %f", pOddsInfo->m_nOddsInfoKind, pOddsInfo->m_hVal1, pOddsInfo->m_oVal1, pOddsInfo->m_oVal2);
			}
			::EnterCriticalSection(&g_csGameInfoArray);
			CopyGameDataProc(pGameData);
			::LeaveCriticalSection(&g_csGameInfoArray);

		}
		theFrame->PostMessageW(MSG_UPDATE, (WPARAM)m_nBookMaker);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\1xbetchina.AnalyzeResLogIn.json"), m_szText);
	return FALSE;
}

COddsInfo* CBetSite_1xbetchina::MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3)
{
	COddsInfo* pOddsInfo = new COddsInfo;
	AddOddInfo(pOddsInfo, pGameData, nOddsKind, hVal1, fOval1, fOval2, fOval3, dwOddID1, dwOddID2, dwOddID3);
	return pOddsInfo;
}

BOOL CBetSite_1xbetchina::UpdateTree(int nIsExitThread)
{
	int nRet = UpdateTree_AllInfoDownAtOnce(nIsExitThread);
	CBetSite::UpdateTree(nIsExitThread);
	return nRet;
}

int CBetSite_1xbetchina::GetCorrespondOdd(CGameData* pGameData, int nOddsKind, float fP)
{
	if (nOddsKind != OI_TEAM1_GOAL_OU && nOddsKind != OI_TEAM2_GOAL_OU)
		return -1;
	int nOddsKind1 = nOddsKind;
	if (pGameData->m_bHAChanged)
	{
		if (nOddsKind == OI_TEAM1_GOAL_OU)
			nOddsKind1 = OI_TEAM2_GOAL_OU;
		else if (nOddsKind == OI_TEAM2_GOAL_OU)
			nOddsKind1 = OI_TEAM1_GOAL_OU;
	}
	for (int i = 0; i < pGameData->m_aryOddsInfo.GetSize(); i++)
	{
		if (pGameData->m_aryOddsInfo[i]->m_nOddsInfoKind != nOddsKind1)
			continue;
		if (pGameData->m_aryOddsInfo[i]->m_hVal1 != fP)
			continue;
		return i;
	}
	return -1;
}

COddsInfo* CBetSite_1xbetchina::GetOdds(CGameData* pGameData, int nOddsKind, float fP)
{
	for (int i = 0; i < pGameData->m_aryOddsInfo.GetSize(); i++)
	{
		if (pGameData->m_aryOddsInfo[i]->m_nOddsInfoKind != nOddsKind)
			continue;
		if (nOddsKind == OI_GOAL_OU)
		{
			if (pGameData->m_aryOddsInfo[i]->m_hVal1 != fP)
				continue;
			return pGameData->m_aryOddsInfo[i];
		}
		else if (nOddsKind == OI_TEAM1_GOAL_OU || nOddsKind == OI_TEAM2_GOAL_OU)
		{
			int k = GetCorrespondOdd(pGameData, nOddsKind, fP);
			if (k == -1)
				ASSERT(0);
			return pGameData->m_aryOddsInfo[k];
		}
		else if (nOddsKind == OI_HANDICAP)
		{
			if (pGameData->m_aryOddsInfo[i]->m_hVal1 != fP && pGameData->m_aryOddsInfo[i]->m_hVal1 + fP != 0)
				continue;
			return pGameData->m_aryOddsInfo[i];
		}
	}
	return NULL;
}

void CBetSite_1xbetchina::MainOddsProc(CGameData* pGameData, Value& E, int index, int& nCount)
{
	float oVal1 = 0, oVal2 = 0, fDraw = 0;
	DWORD dwOddID1 = 0, dwOddID2 = 0, dwOddID3 = 0;
	int j = index;
	while (j < (int)E.Size())
	{
		Value& e_j = E[j];
		Value& e_j_c = e_j["C"];//Coef
		Value& e_j_g = e_j["G"];//Event Group
		Value& e_j_t = e_j["T"];//Type

		if (e_j_g.GetInt() != 1 && e_j_g.GetInt() != 101)
			break;
		if (e_j_t.GetInt() == 1 || e_j_t.GetInt() == 401)
		{
			oVal1 = e_j_c.GetFloat();
			dwOddID1 = e_j_t.GetInt();
		}
		else if (e_j_t.GetInt() == 2 || e_j_t.GetInt() == 403)
		{
			fDraw = e_j_c.GetFloat();
			dwOddID3 = e_j_t.GetInt();
		}
		else if (e_j_t.GetInt() == 3 || e_j_t.GetInt() == 402)
		{
			oVal2 = e_j_c.GetFloat();
			dwOddID2 = e_j_t.GetInt();
		}
		else
			ASSERT(0);
		j++;
		nCount++;
	}
	MyAddOddInfo(pGameData, OI_MAIN, 0, oVal1, oVal2, fDraw, dwOddID1, dwOddID2, dwOddID3);
}

BOOL CBetSite_1xbetchina::GetSessionVal(TCHAR* buffer)
{
	if (_tcscmp(m_szSession, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("SESSION="), m_szSession))
			return FALSE;
	}
	return TRUE;
}

BOOL CBetSite_1xbetchina::GetCheH(TCHAR* buffer)
{
	if (_tcscmp(m_che_h, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("che_h="), m_che_h))
			return FALSE;
	}
	return TRUE;
}

BOOL CBetSite_1xbetchina::GetHash(TCHAR* buffer)
{
	if (_tcscmp(m_szHash, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("uhash="), m_szHash))
			return FALSE;
	}
	return TRUE;
}

void CBetSite_1xbetchina::UpdateOddInfo(CGameData* pGameData, COddsInfo* pOddsInfo, int nOddsKind, float fOval1, float fOval2, float fOval3, DWORD dwOddID)
{
	if (fOval1 != 0)
	{
		pOddsInfo->m_oVal1 = fOval1;
		pOddsInfo->m_dwOddID1 = dwOddID;
	}
	if (fOval2 != 0)
	{
		pOddsInfo->m_oVal2 = fOval2;
		pOddsInfo->m_dwOddID2 = dwOddID;
	}
	if (nOddsKind == OI_MAIN && fOval3 != 0)
	{
		pOddsInfo->fDraw = fOval3;
		pOddsInfo->m_dwOddID3 = dwOddID;
	}

	if (pGameData->m_bHAChanged && (nOddsKind == OI_MAIN || nOddsKind == OI_HANDICAP))
	{
		if (fOval1 != 0)
		{
			pOddsInfo->m_oVal2 = fOval1;
			pOddsInfo->m_dwOddID2 = dwOddID;
		}
		if (fOval2 != 0)
		{
			pOddsInfo->m_oVal1 = fOval2;
			pOddsInfo->m_dwOddID1 = dwOddID;
		}
	}
}

