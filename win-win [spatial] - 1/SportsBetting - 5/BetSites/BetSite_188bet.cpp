#include "..\stdafx.h"
#include "BetSite_188bet.h"
#include "..\global.h"
#include "..\GameInfo.h"
#include "..\GameData.h"
#include "..\SportsBetting.h"
#include <fstream>
#include "..\MainFrm.h"
//#include <vld.h>

using namespace rapidjson;

//https://www.188188188188b.com/en-gb/sports/all/in-play

CBetSite_188bet::CBetSite_188bet() : CBetSite()
{
	m_nBookMaker = SITE_188bet;
	_tcscpy_s(m_szCache_sb188, _T(""));
	_tcscpy_s(m_szCache_a188, _T(""));
	_tcscpy_s(m_szSession_sb188, _T(""));
	_tcscpy_s(m_szSession_fptoken, _T(""));
	_tcscpy_s(m_szSession_fptoken_val, _T(""));
	_tcscpy_s(m_szReferer, _T(""));

	_tcscpy_s(m_szRedirectUrl, _T(""));
	_tcscpy_s(m_szPassport, _T(""));

	m_cid = 0;
	strcpy_s(m_did, "");

	m_nMinBetMoney = 10;
	m_nBettingTimePeriod = 6000;
	m_nTimeOut = 3000;

	m_pSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, m_nTimeOut);
	m_pSessionBet->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, m_nTimeOut);
}

CBetSite_188bet::~CBetSite_188bet()
{
}

int CBetSite_188bet::MainProc()
{
	m_nTextSize = 0;
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
			if (!RefreshData())
				return 0;
			if (IsNeedGetBalance())
			{
				if (!RefreshBalance())
					return 0;
			}
			OutDbgStrAA("\n[188bet]****************************");
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
						OutDbgStrAA("\n[188bet]:MAIN ERROR - REQ_LIVE");
						return 0;
					}
					if (!SendRequestEx(REQ_LIVE3))//dyn_wdp.js
					{
						OutDbgStrAA("\n[188bet]:MAIN ERROR - REQ_LIVE3");
						return 0;
					}
					if (!SendRequestEx(REQ_LIVE1))
					{
						OutDbgStrAA("\n[188bet]:MAIN ERROR - REQ_LIVE1");
						return 0;
					}
					m_nLogInStatus = LS_LIVE;
				}
				if (!SendRequestEx(REQ_LOGIN))
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - REQ_LOGIN");
					return 0;
				}
				if (!AnalyzeResLogIn())
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - AnalyzeResLogIn");
					return 0;
				}
				if (!SendRequestEx(REQ_LOGIN2))//postlogin
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - REQ_LOGIN2");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE2))///zh-cn/sports/all/in-play
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - REQ_LIVE2");
					return 0;
				}
				if (!AnalyzeResLive2())
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - AnalyzeResLive2");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE5))///zh-cn/sports/all/in-play?q...
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - REQ_LIVE5");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE4))///en-gb/sports/all/in-play
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - REQ_LIVE4");
					return 0;
				}
				if (!AnalyzeResLive4())
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - AnalyzeResLive4");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE6))///en-gb/sports/all/in-play?q...
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - REQ_LIVE6");
					return 0;
				}
			}
		}
	}
	return 0;
}

BOOL CBetSite_188bet::SendRequest(int nReqKind)
{
	CHttpConnection* pServer = NULL;
	m_nTextSize = 0;

	DWORD dwRet;
	CString strServerName;
	char strFormData[4000];
	strcpy_s(strFormData, "");

	try
	{
		if (nReqKind == REQ_LIVE)
		{
			strServerName = _T("www.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en-gb/sports/all/in-play"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LIVE3)
		{
			strServerName = _T("www.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/iojs/latest/dyn_wdp.js"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:application/javascript, */*;q=0.8"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("a-188cash2"), m_szCache_a188);
		}
		else if (nReqKind == REQ_LIVE1)
		{
			strServerName = _T("sb.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en-gb/sports/all/in-play?q=&country=JP&currency=USD&tzoff=-240&allowRacing=false&reg=Japan&rc=JP"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.188188188188b.com/en-gb/sports/all/in-play"));
			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LIVE2)
		{
			strServerName = _T("www.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/zh-cn/sports/all/in-play"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.188188188188b.com/en-gb/sports/all/in-play"));
			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("a-188cash2"), m_szCache_a188);
		}
		else if (nReqKind == REQ_LIVE5)
		{
			strServerName = _T("sb.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, m_szReferer, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.188188188188b.com/zh-cn/sports/all/in-play"));
			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession_sb188);
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("timeZone"), _T("480"));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("mc"), _T(""));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("HighlightedSport"), _T(""));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("sb188cash"), m_szCache_sb188);
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("settingProfile"), _T("OddsType=1&NoOfLinePerEvent=1&SortBy=1&AutoRefreshBetslip=True"));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("fav3"), _T(""));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("CCEnlargeStatus"), _T("true"));
		}
		else if (nReqKind == REQ_LIVE6)
		{
			strServerName = _T("sb.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, m_szReferer, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.188188188188b.com/en-gb/sports/all/in-play"));
			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession_sb188);
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("timeZone"), _T("480"));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("mc"), _T(""));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("HighlightedSport"), _T(""));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("sb188cash"), m_szCache_sb188);
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("settingProfile"), _T("OddsType=1&NoOfLinePerEvent=1&SortBy=1&AutoRefreshBetslip=True"));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("fav3"), _T(""));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("CCEnlargeStatus"), _T("true"));
		}
		else if (nReqKind == REQ_LIVE4)
		{
			strServerName = _T("www.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en-gb/sports/all/in-play"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.188188188188b.com/zh-cn/sports/all/in-play"));
			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("a-188cash2"), m_szCache_a188);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), m_szSession_fptoken, m_szSession_fptoken_val);

			TCHAR szCookie[0x100];
			TCHAR did[0x400];
			ToUnicode(m_did, did, CP_ACP);
			_stprintf_s(szCookie, _T("{\"sb\":true,\"c\":\"CN\",\"cid\":%d,\"r\":\"China\",\"l\":\"zh-cn,en-gb\",\"favs\":\"\",\"sv\":1,\"did\":\"%s\",\"spcreg\":\"\",\"spcdomain\":\"\",\"ratelist\":{}}"), m_cid, did);

			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("prefer"), szCookie);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("check"), _T("{\"inboxReminder\":true,\"lowBalance\":true}"));
		}
		else if (nReqKind == REQ_BALANCE)
		{
			strServerName = _T("www.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/service/userapi/getBalance"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:application/json, text/plain, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession);
		}
		else if (nReqKind == REQ_LOGIN)
		{
			strServerName = _T("www.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/service/userapi/login"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/json;charset=utf-8"));
			m_HttpFile->AddRequestHeaders(_T("Accept:application/json, text/plain, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("a-188cash2"), m_szCache_a188);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), m_szSession_fptoken, m_szSession_fptoken_val);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("prefer"), _T("{\"sb\":true,\"c\":\"JP\",\"cid\":105,\"r\":\"Japan\",\"l\":\"ja-jp,en-gb\",\"favs\":\"\",\"sv\":1,\"did\":\"\",\"spcreg\":\"\",\"spcdomain\":\"\",\"ratelist\":{}}"));

			sprintf_s(strFormData, "{\"BlackBox\":\"\",\"Ud\":\"%s\",\"Pd\":\"%s\"}", m_szID, m_szPswd);
		}
		else if (nReqKind == REQ_LOGIN2)
		{
			strServerName = _T("www.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/postlogin"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.188188188188b.com/en-gb/sports/all/in-play"));
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("a-188cash2"), m_szCache_a188);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), m_szSession_fptoken, m_szSession_fptoken_val);
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("prefer"), _T("{\"sb\":true,\"c\":\"JP\",\"cid\":105,\"r\":\"Japan\",\"l\":\"ja-jp,en-gb\",\"favs\":\"\",\"sv\":1,\"did\":\"\",\"spcreg\":\"\",\"spcdomain\":\"\",\"ratelist\":{}}"));

			char szPassport[0x100];
			ToAscii(m_szPassport, szPassport, CP_ACP);
			sprintf_s(strFormData, "passport=%s&postpage=/en-gb/sports/all/in-play&timezone=+08:00", szPassport);
		}
		else if (nReqKind == REQ_LOGIN_REDIRECT)
		{
			strServerName = _T("www.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, m_szRedirectUrl, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LOGOUT)
		{
			strServerName = _T("www.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/service/userapi/logout"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/json"));
			m_HttpFile->AddRequestHeaders(_T("Accept:application/json, text/plain, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession);

			sprintf_s(strFormData, "{}");
		}
		else if (nReqKind == REQ_DATA)
		{
			strServerName = _T("sb.188188188188b.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en-gb/Service/CentralService?GetData"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip"));
			m_pSession->SetCookie(_T("https://sb.188188188188b.com"), _T("settingProfile"), _T("OddsType=1"));

			strcpy_s(strFormData, "reqUrl=/en-gb/sports/all/in-play?q=");
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
			if (!GetSessionVal_a188((TCHAR*)buffer))
				goto L_FALSE;
		}
		else if (nReqKind == REQ_LIVE3)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetSessionVal_fptoken((TCHAR*)buffer))
				goto L_FALSE;
		}
		else if (nReqKind == REQ_LIVE1)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetSessionVal_sb188((TCHAR*)buffer))
				goto L_FALSE;
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

int CBetSite_188bet::SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind)
{
	m_nTextSizeBet = 0;
	CHttpConnection* pServer = NULL;
	DWORD dwRet;
	CString strServerName = _T("sb.188188188188b.com");
	char strFormData[4000];
	strcpy_s(strFormData, "");
	TCHAR wzBuffer[0x400];

	TCHAR szRef[0x400];
	_stprintf_s(szRef, _T("https://sb.188188188188b.com%s"), m_szReferer);

	try
	{
		pServer = m_pSessionBet->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

		if (nReqKind == REQ_GetBySelectionId)
		{
			TCHAR strObj[1000];
			_stprintf_s(strObj, _T("/en-gb/Service/BetSlipService?GetBySelectionId&ts=%s&SelList=%llu&EventList=%d&OddsList=%.2f&HdpList=%s&ScoreList=%d:%d&SStakeList=0&InplayList=true&ParentEventList=%d&_=%s"),
				GetTimeStampLongW().GetBuffer(), m_dwOddID, pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].oVal, m_szHVal,
				m_hs, m_as, pBetInfo->BetVal[nBetObjNo].dwGameID, GetTimeStampLongW().GetBuffer());
			//_stprintf_s(strObj, _T("/en-gb/Service/MyBetService?GetMyBet&ts=1503594044876&_=1503594044302"));
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObj, szRef, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			//_stprintf_s(szRef, _T("Referer:%s"), m_szReferer);
			//m_HttpFileBet->AddRequestHeaders(szRef);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("settingProfile"), _T("OddsType=1&NoOfLinePerEvent=1&SortBy=1&AutoRefreshBetslip=True"));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession_sb188);
			ToUnicode(m_szID, wzBuffer, CP_ACP);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("mc"), wzBuffer);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("timeZone"), _T("480"));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("HighlightedSport"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("fav3"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("BS@Cookies"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("favByBetType"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("fav-com"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("CCEnlargeStatus"), _T("true"));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("sb188cash"), m_szCache_sb188);
		}
		else if (nReqKind == REQ_GetRemainSelectionId)
		{
			TCHAR strObj[1000];
			_stprintf_s(strObj, _T("/en-gb/Service/BetSlipService?GetRemainSelectionId&ts=%s&SelList=%llu&EventList=%d&OddsList=%.2f&HdpList=%s&ScoreList=%d:%d&SStakeList=%d&CBStakeList=&InplayList=true&IsUpdate=true&ParentEventList=%d&_=%s"),
				GetTimeStampLongW().GetBuffer(), m_dwOddID, pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].oVal, m_szHVal, pBetInfo->BetVal[nBetObjNo].nBettingMoney,
				m_hs, m_as, pBetInfo->BetVal[nBetObjNo].dwGameID, GetTimeStampLongW().GetBuffer());
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObj, szRef, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			//_stprintf_s(szRef, _T("Referer:%s"), m_szReferer);
			//m_HttpFileBet->AddRequestHeaders(szRef);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("settingProfile"), _T("OddsType=1&NoOfLinePerEvent=1&SortBy=1&AutoRefreshBetslip=True"));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession_sb188);
			ToUnicode(m_szID, wzBuffer, CP_ACP);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("mc"), wzBuffer);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("timeZone"), _T("480"));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("CCEnlargeStatus"), _T("true"));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("sb188cash"), m_szCache_sb188);
		}
		else if (nReqKind == REQ_PlaceBetNew)
		{
			TCHAR strObj[1000];
			_stprintf_s(strObj, _T("/en-gb/Service/BetSlipService?PlaceBetNew?ts=%s"), GetTimeStampLongW().GetBuffer());
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj, szRef, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			//_stprintf_s(szRef, _T("Referer:https://sb.188188188188b.com%s"), m_szReferer);
			//m_HttpFileBet->AddRequestHeaders(szRef);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("ASP.NET_SessionId"), m_szSession_sb188);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("timeZone"), _T("480"));
			ToUnicode(m_szID, wzBuffer, CP_ACP);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("mc"), wzBuffer);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("HighlightedSport"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("fav3"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("BS@Cookies"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("favByBetType"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("fav-com"), _T(""));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("CCEnlargeStatus"), _T("true"));
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("sb188cash"), m_szCache_sb188);
			m_pSessionBet->SetCookie(_T("https://sb.188188188188b.com"), _T("settingProfile"), _T("OddsType=1&NoOfLinePerEvent=1&SortBy=1&AutoRefreshBetslip=True"));

			char szHVal[0x10];
			ToAscii(m_szHVal, szHVal, CP_ACP);
			sprintf_s(strFormData, "SingleList=%llu@%.2f@%s@%d:%d@%d@true@%.2f@%d@false@%d&ComboList=&NoOfCombine=1&source=1",
				m_dwOddID, pBetInfo->BetVal[nBetObjNo].oVal, szHVal, m_hs, m_as,
				pBetInfo->BetVal[nBetObjNo].nBettingMoney, pBetInfo->BetVal[nBetObjNo].oVal, pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].dwGameID);
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

CString MakeHandicapLabel1(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo)
{
	CString str = _T("");
	if (pOddsInfo->m_nOddsInfoKind != OI_HANDICAP)
		return str;
	if (pOddsInfo->m_hVal1 >= 0)
	{
		if (nBetObjNo == 0)
			str.Format(_T("+%s"), F2SW(abs(pOddsInfo->m_hVal1)));
		else if (nBetObjNo == 1)
			str.Format(_T("-%s"), F2SW(abs(pOddsInfo->m_hVal1)));
		else
			ASSERT(0);
	}
	else if (pOddsInfo->m_hVal1 < 0)
	{
		if (nBetObjNo == 0)
			str.Format(_T("-%s"), F2SW(abs(pOddsInfo->m_hVal1)));
		else if (nBetObjNo == 1)
			str.Format(_T("+%s"), F2SW(abs(pOddsInfo->m_hVal1)));
		else
			ASSERT(0);
	}
	return str;
}

UINT64 CBetSite_188bet::GetOddID(int nBetObjNo, COddsInfo* pOddsInfo)
{
	if (nBetObjNo == 0)
		return pOddsInfo->m_dwOddID1;
	else if (nBetObjNo == 1)
		return pOddsInfo->m_dwOddID2;
	else if (nBetObjNo == 2)
		return pOddsInfo->m_dwOddID3;
	ASSERT(0);
	return -1;
}

string CBetSite_188bet::GetHVal(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo)
{//+0.5/1
	char s[0x100];
	if (pOddsInfo->m_nOddsInfoKind == OI_MAIN || pOddsInfo->m_nOddsInfoKind == OI_GOAL_OE)
		return "null";
	if (pOddsInfo->m_nOddsInfoKind == OI_HANDICAP)
	{
		CString str = MakeHandicapLabel1(pBetInfo, nBetObjNo, pOddsInfo);
		::ToAscii((LPTSTR)(LPCTSTR)str, s, CP_ACP);
		return s;
	}
	if (pOddsInfo->m_nOddsInfoKind == OI_GOAL_OU || pOddsInfo->m_nOddsInfoKind == OI_TEAM1_GOAL_OU || pOddsInfo->m_nOddsInfoKind == OI_TEAM2_GOAL_OU)
	{
		CString str = MakeGoalLabel(pOddsInfo);
		::ToAscii((LPTSTR)(LPCTSTR)str, s, CP_ACP);
		return s;
	}
	return "";
}

int CBetSite_188bet::DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo)
{
	string s = GetHVal(pBetInfo, nBetObjNo, pOddsInfo);
	ToUnicode(s.c_str(), m_szHVal, CP_ACP);

	m_hs = 0;
	m_as = 0;
	if (pBetInfo->nCategory != GC_TENNIS && pBetInfo->nCategory != GC_BASEBALL && pBetInfo->nCategory != GC_VOLLEYBALL)
	{
		m_hs = pBetInfo->HTScore.nScore;
		m_as = pBetInfo->ATScore.nScore;
	}
	m_dwOddID = GetOddID(nBetObjNo, pOddsInfo);

	int nResult = BET_FAIL;
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_GetBySelectionId)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[188bet]:BET ERROR - REQ_GetBySelectionId");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if (!AnalyzeBetGetBySelectionId())
	{
		OutDbgStrAA("\n[188bet]:BET ERROR - AnalyzeBetGetBySelectionId");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_ANALYZE_ERROR1;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_GetRemainSelectionId)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[188bet]:BET ERROR - REQ_GetRemainSelectionId");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if (!AnalyzeBetGetRemainSelectionId())
	{
		OutDbgStrAA("\n[188bet]:BET ERROR - AnalyzeBetGetRemainSelectionId");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_ANALYZE_ERROR2;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_PlaceBetNew)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[188bet]:BET ERROR - REQ_PlaceBetNew");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if ((nResult = AnalyzeBetPlaceBetNew()) != BET_SUCCESS)
	{
		if (nResult == BET_PARSE_FAIL)
			OutDbgStrAA("\n[188bet]:BET ERROR - AnalyzeBetPlaceBetNew");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}

	pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
L_END:
	return 0;
}

BOOL CBetSite_188bet::AnalyzeBetGetBySelectionId()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\188bet.AnalyzeBetGetBySelectionId.json"), m_szTextBet);
	{
		Document root;

		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;

		if (root.HasMember("bal") == FALSE)
			goto L_FALSE;

		if (root["bal"].GetFloat() == 0)
			goto L_FALSE;
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\188bet.AnalyzeBetGetBySelectionId.json"), m_szTextBet);
	return FALSE;
}

BOOL CBetSite_188bet::AnalyzeBetGetRemainSelectionId()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\188bet.AnalyzeBetGetRemainSelectionId.json"), m_szTextBet);
	return FALSE;
}
int nnnn = 0;
int CBetSite_188bet::AnalyzeBetPlaceBetNew()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\188bet.AnalyzeBetPlaceBetNew.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;

		if (root.HasMember("berr") == FALSE)
			goto L_FALSE;

		if (root["berr"].GetInt() != 9999)
		{
			if (!root.HasMember("s"))
				goto L_FALSE;
			if (!root["s"].IsArray() || root["s"].Size() == 0)
				goto L_FALSE;
			if (!root["s"][0].HasMember("serr"))
				goto L_FALSE;
			const char* err = root["s"][0]["serr"].GetString();
			if (strcmp(err, "Odds_Changed") == 0 || strcmp(err, "BS_OddsUpdating") == 0)
				return BET_VALUE_CHANGED;
			goto L_FALSE;
		}
		else
		{
			g_fBalance[m_nBookMaker] = root["bal"].GetFloat();
		}
	}
	return BET_SUCCESS;
L_FALSE:
	WriteResData(_T("d:\\188bet.AnalyzeBetPlaceBetNew.json"), m_szTextBet);
	return BET_PARSE_FAIL;
}

BOOL CBetSite_188bet::AnalyzeResLogIn()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	{
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;

		if (!root.HasMember("passport"))
		{
			m_nLogInStatus = LS_FAIL;
			theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)2);
			goto L_FALSE;
		}

		Value& val = root["passport"];
		char szPassport[0x100];
		if (!root.HasMember("passport"))
			goto L_FALSE;
		strcpy_s(szPassport, root["passport"].GetString());
		::ToUnicode(szPassport, m_szPassport, CP_ACP);

		if (root.HasMember("returnCode") == FALSE)
			goto L_FALSE;

		if (strcmp(root["returnCode"].GetString(), "0000") == 0)
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
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\188bet.AnalyzeResLogIn.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_188bet::AnalyzeResLogOut()
{
	m_bExitThreadFlg = TRUE;
	m_nLogInStatus = LS_INIT;
	return TRUE;
}

BOOL CBetSite_188bet::AnalyzeResLive4()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	{
		char* sz = strstr(m_szText, "pd: JSON.parse('");
		if (sz == NULL)
			goto L_FALSE;
		char* szComma = strstr(sz + 1, "')");
		int len = szComma - sz - 16;

		char szReferer[0x400];
		memcpy(szReferer, sz + 16, len);
		szReferer[len] = 0;

		Document root;
		ParseResult ok = root.Parse(szReferer);
		if (!ok)
			goto L_FALSE;

		m_cid = root["cid"].GetInt();
		strcpy_s(m_did, root["did"].GetString());

		char* szBalance = strstr(m_szText, "allowtransparency=\"true\" src=\"") + 30;
		if (szBalance == NULL)
			goto L_FALSE;
		szComma = strstr(szBalance + 1, "\"");
		len = szComma - szBalance;

		memcpy(szReferer, szBalance, len);
		szReferer[len] = 0;
		string s = szReferer;
		replace(s, "&amp;", "&");
		replace(s, "https://sb.188188188188b.com", "");

		ToUnicode(s.c_str(), m_szReferer, CP_ACP);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\188bet.AnalyzeResLive4.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_188bet::AnalyzeResLive2()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	//WriteResData(_T("d:\\188bet.AnalyzeResLive2.json"), m_szText);
	{
		char* sz = strstr(m_szText, "pd: JSON.parse('");
		if (sz == NULL)
			goto L_FALSE;
		char* szComma = strstr(sz + 1, "')");
		int len = szComma - sz - 16;

		char szReferer[0x400];
		memcpy(szReferer, sz + 16, len);
		szReferer[len] = 0;

		Document root;
		ParseResult ok = root.Parse(szReferer);
		if (!ok)
			goto L_FALSE;

		m_cid = root["cid"].GetInt();
		strcpy_s(m_did, root["did"].GetString());

		char* szBalance = strstr(m_szText, "allowtransparency=\"true\" src=\"") + 30;
		if (szBalance == NULL)
			goto L_FALSE;
		szComma = strstr(szBalance + 1, "\"");
		len = szComma - szBalance;

		memcpy(szReferer, szBalance, len);
		szReferer[len] = 0;
		string s = szReferer;
		replace(s, "&amp;", "&");
		replace(s, "https://sb.188188188188b.com", "");

		ToUnicode(s.c_str(), m_szReferer, CP_ACP);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\188bet.AnalyzeResLive2.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_188bet::AnalyzeResBalance()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\188bet.AnalyzeResBalance.json"), m_szText);
	{
		char* szBalance = strstr(m_szText, "\"balance\":");
		if (szBalance == NULL)
			goto L_FALSE;
		szBalance += 10;
		char* szComma = strstr(szBalance, ",");
		int len = szComma - szBalance;

		char* szMoney = new char[len + 1];
		memcpy(szMoney, szBalance, len);
		szMoney[len] = 0;

		float fBalance;
		sscanf_s(szMoney, "%f", &fBalance);
		delete szMoney;

		g_fBalance[m_nBookMaker] = fBalance;

		theFrame->m_wndSetting.PostMessageW(MSG_CUR_BALANCE, (WPARAM)m_nBookMaker, (LPARAM)(g_fBalance[m_nBookMaker] * 100));
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\188bet.AnalyzeResBalance.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_188bet::AnalyzeResData()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\188bet.AnalyzeResData.json"), m_szText);
	{
		UINT i, j, k;
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;

		if (!root.HasMember("uvd") || !root.HasMember("lpd"))
			goto L_FALSE;

		Value& lpd = root["lpd"];
		if (!lpd.HasMember("ips"))
			goto L_FALSE;
		Value& ips = lpd["ips"];
		if (!ips.HasMember("ismd"))
			goto L_FALSE;
		Value& ismd = ips["ismd"];

		if (!root.HasMember("mod"))
			goto L_FALSE;
		Value& mod = root["mod"];
		if (!mod.HasMember("d"))
			goto L_FALSE;
		Value& d = mod["d"];

		int gid, n = 0;
		int kkk = 0;
		for (i = 0; i < ismd.Size(); i++)
		{
			int nGameCategory = GetGameCategory((char*)ismd[i]["sn"].GetString());
			if (nGameCategory == -1 || !theFrame->m_bSetCategory[nGameCategory])
				continue;

			if (!ismd[i].HasMember("puc"))
				continue;
			Value& puc = ismd[i]["puc"];
			for (j = 0; j < puc.Size(); j++)
			{
				if (!puc[j].HasMember("ces") || !puc[j].HasMember("cn"))
					continue;
				Value& ces = puc[j]["ces"];
				char* szLeague = (char*)puc[j]["cn"].GetString();
				for (k = 0; k < ces.Size(); k++)
				{
					Value& ces_k = ces[k];
					if (!ces_k.HasMember("ht") || !ces_k.HasMember("at") || !ces_k.HasMember("eid") || !ces_k.HasMember("ipt") || !ces_k.HasMember("etts"))
						continue;
					if (!ces_k.HasMember("hs") || !ces_k.HasMember("as") || !ces_k["hs"].HasMember("v") || !ces_k["as"].HasMember("v"))
						continue;
					const char* ht = rtrim((char*)ces_k["ht"].GetString(), "\t ");
					const char* at = rtrim((char*)ces_k["at"].GetString(), "\t ");

					gid = ces_k["eid"].GetInt();
					int ipt = ::GetTime(ces_k["ipt"].GetString(), nGameCategory);
					string stage = (char*)ces_k["etts"].GetString();
					int hs = atoi(ces_k["hs"]["v"].GetString());
					int as = atoi(ces_k["as"]["v"].GetString());
					int hs1 = -1, as1 = -1, hs2 = -1, as2 = -1;

					if (nGameCategory == GC_TENNIS || nGameCategory == GC_TABLE_TENNIS || nGameCategory == GC_VOLLEYBALL)
					{
						if (!ces_k.HasMember("sb") || !ces_k["sb"].HasMember("cp") || !ces_k["sb"].HasMember("ps"))
							continue;
						//if (nGameCategory == GC_TABLE_TENNIS || nGameCategory == GC_VOLLEYBALL)
						//	_asm int 3
						stage = (char*)ces_k["sb"]["cp"].GetString();
						Value& ps = ces_k["sb"]["ps"];
						for (UINT kk = 0; kk < ps.Size(); kk++)
						{
							Value& ps_kk = ps[kk];
							if (ps_kk["p"].GetString() == stage)
							{
								if (!ps_kk.HasMember("h") || !ps_kk.HasMember("a"))
									continue;
								hs1 = ps_kk["h"].GetInt();
								as1 = ps_kk["a"].GetInt();
							}
							else if (ps_kk["p"].GetString() == "p")
							{
								if (!ps_kk.HasMember("h") || !ps_kk.HasMember("a"))
									continue;
								hs2 = ps_kk["h"].GetInt();
								as2 = ps_kk["a"].GetInt();
							}
						}
					}
					if (stage == "HT")
						ipt = 45 * 60;
					if (IsExceptionGame((char*)ht, (char*)at, szLeague, (char*)stage.c_str()))
						continue;
					//if (strstr(ht, "Shabab") && strstr(at, "That"))
					//	_asm int 3
					CMainInfo MainInfo(nGameCategory, (char*)ht, (char*)at, SCORE(hs, hs1, hs2), SCORE(as, as1, as2), (char*)stage.c_str(), ipt, szLeague);
					::EnterCriticalSection(&g_csGameInfoArray);
					CGameInfo* pGameInfo2 = NULL;
					if (!GetGameInfoProc(&MainInfo, &pGameInfo2, gid))
					{
						::LeaveCriticalSection(&g_csGameInfoArray);
						continue;
					}
					::LeaveCriticalSection(&g_csGameInfoArray);
				}
			}
		}
		int nn = d.Size();
		for (i = 0; i < d.Size(); i++)
		{
			if (!d[i].HasMember("data") || !d[i]["data"].HasMember("d") || !d[i]["data"]["d"].HasMember("c"))
				continue;
			Value& c = d[i]["data"]["d"]["c"];
			int mm = c.Size();
			for (j = 0; j < c.Size(); j++)
			{
				if (!c[j].HasMember("e"))
					continue;
				Value& e = c[j]["e"];
				int kk = e.Size();
				for (k = 0; k < e.Size(); k++)
				{
					CGameData* pGameData = NULL;
					if (!e[k].HasMember("k"))
						continue;
					gid = e[k]["k"].GetInt();
					::EnterCriticalSection(&g_csGameInfoArray);
					BOOL bFound = m_mapGameID.Lookup(gid, pGameData);
					if (!bFound)
					{
						::LeaveCriticalSection(&g_csGameInfoArray);
						continue;
					}
					CGameInfo* pGameInfo = new CGameInfo(pGameData->m_pGameInfo);
					//int n = pGameInfo->GetGameDataNo(m_nBookMaker);
					//pGameData = pGameInfo->m_aryGameData[n];
					pGameData = pGameInfo->GetGameData(m_nBookMaker);
					::LeaveCriticalSection(&g_csGameInfoArray);

					//if (strstr(pGameData->m_MainInfo.m_szHTName, "Shabab") && strstr(pGameData->m_MainInfo.m_szATName, "That"))
					//	_asm int 3
					pGameData->RemoveAllOddsInfo();

					if (!e[k].HasMember("o"))
						continue;
					Value& o = e[k]["o"];
					if (o.HasMember("ahpt"))
						ArrayProc(OI_HANDICAP, pGameData, o["ahpt"]);
					else if (o.HasMember("ah"))
						ArrayProc(OI_HANDICAP, pGameData, o["ah"]);

					if (o.HasMember("oupt"))
						ArrayProc(OI_GOAL_OU, pGameData, o["oupt"]);
					else if (o.HasMember("ou"))
						ArrayProc(OI_GOAL_OU, pGameData, o["ou"]);
					else if (o.HasMember("oufts"))
						ArrayProc(OI_GOAL_OU, pGameData, o["oufts"]);

					if (o.HasMember("1x2"))
						ArrayProc(OI_MAIN, pGameData, o["1x2"]);
					else if (o.HasMember("ml"))
						ArrayProc(OI_MAIN, pGameData, o["ml"]);

					if (o.HasMember("oe"))
						ArrayProc(OI_GOAL_OE, pGameData, o["oe"]);

					if (e[k].HasMember("cel"))
					{
						Value& cel = e[k]["cel"];
						for (int m = 0; m < (int)cel.Size(); m++)
						{
							if (!cel[m].HasMember("o") || !cel[m].HasMember("cei") || !cel[m]["cei"].HasMember("n") || !cel[m].HasMember("i"))
								continue;
							Value& o1 = cel[m]["o"];
							Value& n1 = cel[m]["cei"]["n"];
							Value& i1 = cel[m]["i"][0];
							const char* cc = n1.GetString();
							const char* ccc = i1.GetString();
							if (strstr(n1.GetString(), "Team Goals") == NULL && strstr(n1.GetString(), "Team Points") == NULL)
								continue;
							if (o1.HasMember("ou"))
							{
								Value& ou = o1["ou"];
								if (!pGameData->m_bHAChanged)
								{
									if (strstr(i1.GetString(), pGameData->m_pGameInfo->m_MainInfo.m_szHTName) != NULL)
										ArrayProc(OI_TEAM1_GOAL_OU, pGameData, ou);
									else if (strstr(i1.GetString(), pGameData->m_pGameInfo->m_MainInfo.m_szATName) != NULL)
										ArrayProc(OI_TEAM2_GOAL_OU, pGameData, ou);
								}
								else
								{
									if (strstr(i1.GetString(), pGameData->m_pGameInfo->m_MainInfo.m_szHTName) != NULL)
										ArrayProc(OI_TEAM2_GOAL_OU, pGameData, ou);
									else if (strstr(i1.GetString(), pGameData->m_pGameInfo->m_MainInfo.m_szATName) != NULL)
										ArrayProc(OI_TEAM1_GOAL_OU, pGameData, ou);
								}
							}
						}
					}
					::EnterCriticalSection(&g_csGameInfoArray);
					CopyGameDataProc(pGameData);
					::LeaveCriticalSection(&g_csGameInfoArray);
				}
			}
		}
		theFrame->PostMessageW(MSG_UPDATE, (WPARAM)m_nBookMaker);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\188bet.AnalyzeResData.json"), m_szText);
	return FALSE;
}

COddsInfo* CBetSite_188bet::MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3)
{
	//if (strstr(pGameData->m_MainInfo.m_szhtName, "calcu") == 0)
	//	_asm int 3
	COddsInfo* pOddsInfo = new COddsInfo;
	AddOddInfo(pOddsInfo, pGameData, nOddsKind, hVal1, fOval1, fOval2, fOval3, dwOddID1, dwOddID2, dwOddID3);
	return pOddsInfo;
}

void CBetSite_188bet::ArrayProc(int nKind, CGameData* pGameData, Value& val)
{
	float fVal1, fVal2, fVal3 = 0;
	DWORD dwOddID1 = 0, dwOddID2 = 0, dwOddID3 = 0;
	if (nKind == OI_MAIN || nKind == OI_GOAL_OE)
	{
		if (val.Size() != 6 && val.Size() != 4)
			ASSERT(0);
		char* szVal1 = (char*)val[1].GetString();
		sscanf_s((char*)val[0].GetString(), "o%d", &dwOddID1);
		char* szVal2 = (char*)val[3].GetString();
		sscanf_s((char*)val[2].GetString(), "o%d", &dwOddID2);
		char* szVal3 = NULL;
		if (nKind == OI_MAIN && val.Size() == 6)
		{
			szVal3 = (char*)val[5].GetString();
			sscanf_s((char*)val[4].GetString(), "o%d", &dwOddID3);
		}
		
		sscanf_s(szVal1, "%f", &fVal1);
		sscanf_s(szVal2, "%f", &fVal2);
		if (IsEqualF(fVal1, 0) && IsEqualF(fVal2, 0))
			return;
		if (nKind == OI_MAIN && val.Size() == 6)
			sscanf_s(szVal3, "%f", &fVal3);

		MyAddOddInfo(pGameData, nKind, 0, fVal1, fVal2, fVal3, dwOddID1, dwOddID2, dwOddID3);
	}
	else
	{
		if (val.Size() % 8 != 0)
			ASSERT(0);
		char* szVal[8];
		DWORD dwID[4];

		for (int i = 0; i < (int)val.Size(); i += 8)
		{
			int k = 0;
			for (int j = 0; j < 8; j++)
			{
				szVal[j] = (char*)val[i + j].GetString();
				if (j % 2 == 0)
				{
					if ((j / 4 == 0 && szVal[j][0] != 'h') ||
						(j / 4 == 1 && szVal[j][0] != 'o'))
						ASSERT(0);
					sscanf_s(szVal[j] + 1, "%d", &dwID[k]);
					k++;
				}
			}
			if (dwID[0] != dwID[2] || dwID[1] != dwID[3])
				ASSERT(0);
			dwOddID1 = dwID[2];
			dwOddID2 = dwID[3];

			BOOL bAsian = FALSE;
			float hVal1 = 0xFFFFF, hVal2 = 0xFFFFF;
			if (nKind == OI_HANDICAP)
			{
				if (szVal[1][0] != '+' && szVal[1][0] != '-')
				{
					if (strcmp((const char*)szVal[1], "0") != 0 || strcmp((const char*)szVal[3], "0") != 0)
						ASSERT(0);
					bAsian = 0;
					hVal1 = 0;
					hVal2 = 0;
				}
				else
				{
					if ((szVal[1][0] == '+' && szVal[3][0] != '-') ||
						(szVal[1][0] == '-' && szVal[3][0] != '+') ||
						strcmp(szVal[1] + 1, szVal[3] + 1) != 0)
						return;/////////???????????

					bAsian = (strchr(szVal[1], '/') != NULL);
					if (bAsian)
						Split(szVal[1], '/', hVal1, hVal2);
					else
					{
						sscanf_s(szVal[1], "%f", &hVal1);
					}
				}
				sscanf_s(szVal[5], "%f", &fVal1);
				sscanf_s(szVal[7], "%f", &fVal2);
				if (IsEqualF(fVal1, 0) && IsEqualF(fVal2, 0))
					continue;

				if (!bAsian)
					MyAddOddInfo(pGameData, nKind, hVal1, fVal1, fVal2, 0, dwOddID1, dwOddID2, dwOddID3);
			}
			else // OI_GOAL_OU
			{
				if (szVal[1][0] == '+' || szVal[1][0] == '-')
					ASSERT(0);
				//if (strcmp(szVal[1], szVal[3]) != 0)
				//	ASSERT(0);

				bAsian = (strchr(szVal[1], '/') != NULL);
				if (bAsian)
					Split(szVal[1], '/', hVal1, hVal2);
				else
					sscanf_s(szVal[1], "%f", &hVal1);

				sscanf_s(szVal[5], "%f", &fVal1);
				sscanf_s(szVal[7], "%f", &fVal2);
				if (IsEqualF(fVal1, 0) && IsEqualF(fVal2, 0))
					continue;
				if (!bAsian)
					MyAddOddInfo(pGameData, nKind, hVal1, fVal1, fVal2, 0, dwOddID1, dwOddID2, dwOddID3);
			}
		}
	}

}

int CBetSite_188bet::GetGameCategory(char* szGameCategory)
{
	if (_strcmpi(szGameCategory, "Snooker / Pool") == 0)
		return GC_SNOOKER;
	if (_strcmpi(szGameCategory, "eSports") == 0)
		return GC_E_SPORTS;
	return CBetSite::GetGameCategory(szGameCategory);
}

CString CBetSite_188bet::GetGameCategory(int nGameCategory)
{
	if (nGameCategory == GC_SNOOKER)
		return _T("Snooker / Pool");
	if (nGameCategory == GC_E_SPORTS)
		return _T("eSports");
	CBetSite::GetGameCategory(nGameCategory);
	return _T("");
}

BOOL CBetSite_188bet::UpdateTree(int nIsExitThread)
{
	int nRet = UpdateTree_AllInfoDownAtOnce(nIsExitThread);
	CBetSite::UpdateTree(nIsExitThread);
	return nRet;
}

BOOL CBetSite_188bet::GetSessionVal_fptoken(TCHAR* buffer)
{
	if (!GetSessionValue(buffer, _T("fp_token_"), m_szSession_fptoken, m_szSession_fptoken_val))
		return FALSE;
	return TRUE;
}

BOOL CBetSite_188bet::GetSessionVal_sb188(TCHAR* buffer)
{
	if (_tcscmp(m_szSession_sb188, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("ASP.NET_SessionId="), m_szSession_sb188))
			return FALSE;
	}
	if (_tcscmp(m_szCache_sb188, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("sb188cash="), m_szCache_sb188))
			return FALSE;
	}
	return TRUE;
}

BOOL CBetSite_188bet::GetSessionVal_a188(TCHAR* buffer)
{
	if (_tcscmp(m_szSession, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("ASP.NET_SessionId="), m_szSession))
			return FALSE;
	}
	if (_tcscmp(m_szCache_a188, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("a-188cash2="), m_szCache_a188))
			return FALSE;
	}

	return TRUE;
}
