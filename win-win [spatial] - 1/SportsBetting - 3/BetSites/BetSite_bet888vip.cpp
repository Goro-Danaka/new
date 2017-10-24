#include "..\stdafx.h"
#include "BetSite_bet888vip.h"
#include "..\global.h"
#include "..\MainFrm.h"
#include "..\GameInfo.h"
#include "..\GameData.h"

using namespace rapidjson;

//http://www.bet888vip.com/

CBetSite_bet888vip::CBetSite_bet888vip()
{
	m_nBookMaker = SITE_bet888vip;

	_tcscpy_s(m_szIOIPL, _T(""));
	_tcscpy_s(m_szIBCACHE, _T(""));
	_tcscpy_s(m_szBBSESSID, _T(""));

	strcpy_s(m_SS, "");
	strcpy_s(m_SR, "");
	strcpy_s(m_TS, "");
	strcpy_s(m_SID, "");

	strcpy_s(m_unique, "");
	strcpy_s(m_data_pool, "");
	strcpy_s(m_verify, "");
	strcpy_s(m_secret, "");
	strcpy_s(m_ratio, "");
	strcpy_s(m_concede, "");

	strcpy_s(m_order_auth, "");
	strcpy_s(m_ioratio, "");
	strcpy_s(m_strong, "");
}

CBetSite_bet888vip::~CBetSite_bet888vip()
{
}

int CBetSite_bet888vip::MainProc()
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
			if (!RefreshData())
				return 0;
			if (IsNeedGetBalance())
			{
				if (!RefreshBalance())
					return 0;
			}
			OutDbgStrAA("\n[bet888vip]****************************");
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
						OutDbgStrAA("\n[bet888vip]:MAIN ERROR - REQ_LIVE");
						return 0;
					}
					if (!SendRequestEx(REQ_LIVE3))
					{
						OutDbgStrAA("\n[bet888vip]:MAIN ERROR - REQ_LIVE3");
						return 0;
					}
					//if (!SendRequestEx(REQ_LIVE4))
					//{
					//	OutDbgStrAA("\n[bet888vip]:MAIN ERROR - REQ_LIVE4");
					//	return 0;
					//}
					//if (!AnalyzeResLive())
					//{
					//	OutDbgStrAA("\n[bet888vip]:MAIN ERROR - AnalyzeResLive");
					//	return 0;
					//}
					if (!SendRequestEx(REQ_LIVE1))
					{
						OutDbgStrAA("\n[bet888vip]:MAIN ERROR - REQ_LIVE1");
						return 0;
					}
					if (!AnalyzeResLive1())
					{
						OutDbgStrAA("\n[bet888vip]:MAIN ERROR - AnalyzeResLive1");
						return 0;
					}
					m_nLogInStatus = LS_LIVE;
				}
				if (!SendRequestEx(REQ_LIVE2))
				{
					OutDbgStrAA("\n[bet888vip]:MAIN ERROR - REQ_LIVE2");
					return 0;
				}
				if (!AnalyzeResLive2())
				{
					OutDbgStrAA("\n[bet888vip]:MAIN ERROR - AnalyzeResLive2");
					return 0;
				}

				if (strcmp(m_szCaptcha, "") != 0)
				{
					if (!SendRequestEx(REQ_LOGIN))
					{
						OutDbgStrAA("\n[bet888vip]:MAIN ERROR - REQ_LOGIN");
						return 0;
					}
					if (!AnalyzeResLogIn())
					{
						OutDbgStrAA("\n[bet888vip]:MAIN ERROR - AnalyzeResLogIn");
						return 0;
					}
					if (!SendRequestEx(REQ_LIVE5))
					{
						OutDbgStrAA("\n[bet888vip]:MAIN ERROR - REQ_LIVE5");
						return 0;
					}
					if (m_nLogInStatus == LS_SUCCESS)
					{
						if (!SendRequestEx(REQ_BALANCE))
						{
							OutDbgStrAA("\n[bet888vip]:MAIN ERROR - REQ_BALANCE");
							return 0;
						}
					}
				}
			}
		}
	}
	return 0;
}

BOOL CBetSite_bet888vip::SendRequest(int nReqKind)
{
	CHttpConnection* pServer = NULL;
	m_nTextSize = 0;

	DWORD dwRet;
	CString strServerName = _T("www.bet888vip.com");
	char strFormData[1000];
	strcpy_s(strFormData, "");

	try
	{
		if (nReqKind == REQ_LIVE)
		{
			strServerName = _T("mkt.bet888vip.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/"));

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LIVE3)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/cl/index.php?module=System&method=first"));

			m_HttpFile->AddRequestHeaders(_T("*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("gb"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("N"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("T0_IPL_AVRbbbbbbbbbbbbbbbb"), m_szIOIPL);
		}
		else if (nReqKind == REQ_LIVE4)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/infe/rest/fig/advertise/common.json?mobile_open=1"));
			m_HttpFile->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Referer:http://www.bet888vip.com/cl/index.php?module=System&method=First"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("N"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("T0_IPL_AVRbbbbbbbbbbbbbbbb"), m_szIOIPL);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("page_site"), _T("First"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("selected_page"), _T("First"));
		}
		else if (nReqKind == REQ_LIVE1)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/app/member/verify/mkCode.php?_="));

			m_HttpFile->AddRequestHeaders(_T("*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LIVE2)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			TCHAR s[0x400];
			TCHAR sr[0x400];
			::ToUnicode(m_SR, sr, CP_ACP);
			_stprintf_s(s, _T("/tpl/commonFile/images/gdpic/macpic.php?SR=%s"), sr);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, s);

			m_HttpFile->AddRequestHeaders(_T("image/png, image/svg+xml, image/*;q=0.8, */*;q=0.5"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("N"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("T0_IPL_AVRbbbbbbbbbbbbbbbb"), m_szIOIPL);
		}
		else if (nReqKind == REQ_LOGIN)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/infe/login/login"));
			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded"));
			m_HttpFile->AddRequestHeaders(_T("Referer:http://www.bet888vip.com/cl/index.php?module=System&method=First"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("N"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("T0_IPL_AVRbbbbbbbbbbbbbbbb"), m_szIOIPL);
			//m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("IBCACHE"), m_szIBCACHE);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("GTYPE"), _T("FT"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("PTYPE"), _T("S"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("L_GameType"), _T("Intro"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("page_site"), _T("First"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("selected_page"), _T("First"));

			sprintf_s(strFormData, "uid2=guest&SS=%s&SR=%s&TS=%s&username=%s&passwd=%s&rmNum=%s", m_SS, m_SR, m_TS, m_szID, m_szPswd, m_szCaptcha);
		}
		else if (nReqKind == REQ_LIVE5)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/app/member/chk_rule.php"));

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("N"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("T0_IPL_AVRbbbbbbbbbbbbbbbb"), m_szIOIPL);
			//m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("IBCACHE"), m_szIBCACHE);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("GTYPE"), _T("FT"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("PTYPE"), _T("S"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("L_GameType"), _T("Intro"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("page_site"), _T("First"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("selected_page"), _T("First"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("BBSESSID"), m_szBBSESSID);

			sprintf_s(strFormData, "uid=%s", m_SID);
		}
		else if (nReqKind == REQ_BALANCE)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/cl/index.php?module=System"));
			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("N"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("T0_IPL_AVRbbbbbbbbbbbbbbbb"), m_szIOIPL);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("GTYPE"), _T("FT"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("PTYPE"), _T("S"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("L_GameType"), _T("Intro"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("page_site"), _T("First"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("selected_page"), _T("First"));

			sprintf_s(strFormData, "page_site=&uid=%s&gtype=FT&submit=I+agree", m_SID);
		}
		else if (nReqKind == REQ_BALANCE_BB)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			CString strObj;
			strObj.Format(_T("/infe/rest/balance/BB.json?_=%s"), GetTimeStampLongW());
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObj);

			m_HttpFile->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Referer:http://www.bet888vip.com/sport/?page=odds_live"));
			TCHAR SID[0x100];
			::ToUnicode(m_SID, SID, CP_ACP);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), SID);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("Y"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("T0_IPL_AVRbbbbbbbbbbbbbbbb"), m_szIOIPL);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("GTYPE"), _T("FT"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("PTYPE"), _T("S"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("L_GameType"), _T("Intro"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("page_site"), _T("Ball"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("selected_page"), _T("ball"));
			//m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("IBCACHE"), m_szIBCACHE);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("BBSESSID"), m_szBBSESSID);
		}
		else if (nReqKind == REQ_LOGOUT)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/app/member/logout.php"));
			m_HttpFile->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));

			TCHAR SID[0x100];
			::ToUnicode(m_SID, SID, CP_ACP);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), SID);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));
		}
		else if (nReqKind == REQ_DATA)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);
			
			CString strObj;
			strObj.Format(_T("/sport/rest/odds/getOddsListLive.json?odds_type=3&cb=N&gid_list=[]&modify_ts=0&_=%s"), GetTimeStampLongW());
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObj);

			m_HttpFile->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFile->AddRequestHeaders(_T("Referer:http://www.bet888vip.com/sport/?page=odds_live"));
			TCHAR SID[0x100];
			::ToUnicode(m_SID, SID, CP_ACP);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), SID);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("Y"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("OddsType_SPONUUS01001"), _T("2"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("GTYPE"), _T("FT"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("PTYPE"), _T("S"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("L_GameType"), _T("Intro"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("page_site"), _T("Ball"));
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("selected_page"), _T("ball"));
			//m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("IBCACHE"), m_szIBCACHE);
			m_pSession->SetCookie(_T("http://www.bet888vip.com"), _T("BBSESSID"), m_szBBSESSID);
		}
		m_HttpFile->SendRequest(NULL, 0, strFormData, strlen(strFormData));

		m_HttpFile->QueryInfoStatusCode(dwRet);
		if (dwRet != HTTP_STATUS_OK)
			goto L_FALSE;

		BYTE buffer[0x1000];
		DWORD len = 0x1000;
		if (nReqKind == REQ_LIVE && _tcscmp(m_szIOIPL, _T("")) != 0)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetSessionValue((TCHAR*)buffer, _T("T0_IPL_AVRbbbbbbbbbbbbbbbb="), m_szIOIPL))
				goto L_FALSE;
		}
		else if (nReqKind == REQ_LIVE4)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetSessionValue((TCHAR*)buffer, _T("IBCACHE="), m_szIBCACHE))
				goto L_FALSE;
		}
		else if (nReqKind == REQ_LOGIN)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetBBSESSID((TCHAR*)buffer))
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

BOOL CBetSite_bet888vip::GetBBSESSID(TCHAR* buffer)
{
	if (_tcscmp(m_szBBSESSID, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("BBSESSID="), m_szBBSESSID))
			return FALSE;
	}
	return TRUE;
}

int CBetSite_bet888vip::SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind)
{
	m_nTextSizeBet = 0;
	CHttpConnection* pServer = NULL;
	DWORD dwRet;
	CString strServerName = _T("www.bet888vip.com");
	char strFormData[4000];
	strcpy_s(strFormData, "");

	try
	{
		pServer = m_pSessionBet->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

		if (nReqKind == REQ_ORDER_SELECT)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/sport/rest/order/orderSelect.json"));

			m_HttpFileBet->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			TCHAR SID[0x100];
			::ToUnicode(m_SID, SID, CP_ACP);
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), SID);
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("Y"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("LangKey"), _T("en"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));

			string cat = MyGetGameCategory(pBetInfo->nCategory);
			sprintf_s(strFormData, "odds_type=3&order[%d][game_id]=%d&order[%d][game_type]=%s&order[%d][play_type]=RB&order[%d][order_type]=RB&order[%d][real_type]=%s",
				pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].dwGameID, cat.c_str(), 
				pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].dwGameID, m_szOddID);
		}
		else if (nReqKind == REQ_ORDER_BET)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/sport/rest/order/orderBet.json"));

			m_HttpFileBet->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			TCHAR SID[0x100];
			::ToUnicode(m_SID, SID, CP_ACP);
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), SID);
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("Y"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("LangKey"), _T("en"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));

			string cat = MyGetGameCategory(pBetInfo->nCategory);
			sprintf_s(strFormData, "odds_type=3&order[%d][game_id]=%d&order[%d][game_type]=%s&order[%d][play_type]=RB&order[%d][order_type]=RB&order[%d][real_type]=%s&\
order[%d][hidden][unique]=%s&\
order[%d][hidden][data_pool]=%s&\
order[%d][hidden][verify]=%s&\
order[%d][hidden][secret]=%s&\
order[%d][hidden][ratio]=%s&\
order[%d][hidden][concede]=%s&\
order[%d][order_auth]=%s&\
order[%d][ioratio]=%s&\
order[%d][strong]=%s&\
order[%d][gold]=%d&\
order[%d][better_rate]=N&odds_type=3",
				pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].dwGameID, cat.c_str(), 
				pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].dwGameID, m_szOddID,
				pBetInfo->BetVal[nBetObjNo].dwGameID, m_unique,
				pBetInfo->BetVal[nBetObjNo].dwGameID, m_data_pool,
				pBetInfo->BetVal[nBetObjNo].dwGameID, m_verify,
				pBetInfo->BetVal[nBetObjNo].dwGameID, m_secret,
				pBetInfo->BetVal[nBetObjNo].dwGameID, m_ratio,
				pBetInfo->BetVal[nBetObjNo].dwGameID, m_concede,
				pBetInfo->BetVal[nBetObjNo].dwGameID, m_order_auth,
				pBetInfo->BetVal[nBetObjNo].dwGameID, m_ioratio,
				pBetInfo->BetVal[nBetObjNo].dwGameID, m_strong,
				pBetInfo->BetVal[nBetObjNo].dwGameID, pBetInfo->BetVal[nBetObjNo].nBettingMoney,
				pBetInfo->BetVal[nBetObjNo].dwGameID);
		}
		else if (nReqKind == REQ_BET_BALANCE_BB)
		{
			pServer = m_pSessionBet->GetHttpConnection(_T("www.bet888vip.com"), (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);

			CString strObj;
			strObj.Format(_T("/infe/rest/balance/BB.json?_=%s"), GetTimeStampLongW());
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObj);

			m_HttpFileBet->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFileBet->AddRequestHeaders(_T("Referer:http://www.bet888vip.com/sport/?page=odds_live"));
			TCHAR SID[0x100];
			::ToUnicode(m_SID, SID, CP_ACP);
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), SID);
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("lang"), _T("en"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("langx"), _T("en-us"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("SESSION_ID"), _T("guest"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("LOGINCHK"), _T("Y"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("T0_IPL_AVRbbbbbbbbbbbbbbbb"), m_szIOIPL);
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("GTYPE"), _T("FT"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("PTYPE"), _T("S"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("L_GameType"), _T("Intro"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("page_site"), _T("Ball"));
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("selected_page"), _T("ball"));
			//m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("IBCACHE"), m_szIBCACHE);
			m_pSessionBet->SetCookie(_T("http://www.bet888vip.com"), _T("BBSESSID"), m_szBBSESSID);
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

int CBetSite_bet888vip::DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo)
{
	GetOddID(nBetObjNo, pOddsInfo, m_szOddID);

	int nResult;
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_ORDER_SELECT)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[bet888vip]:BET ERROR - REQ_ORDER_SELECT");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if ((nResult = AnalyzeOrderSelect()) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[bet888vip]:BET ERROR - AnalyzeOrderSelect");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_ORDER_BET)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[bet888vip]:BET ERROR - REQ_ORDER_BET");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if ((nResult = AnalyzeOrderBet()) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[bet888vip]:BET ERROR - AnalyzeOrderBet");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_BET_BALANCE_BB)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[bet888vip]:BET ERROR - REQ_BET_BALANCE_BB");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
		return 0;
	}
	if (!AnalyzeBetBalanceBB())
	{
		OutDbgStrAA("\n[bet888vip]:BET ERROR - AnalyzeBetBalanceBB");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
		return 0;
	}

	pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
L_END:
	return 0;
}

int CBetSite_bet888vip::AnalyzeOrderSelect()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\bet888vip.AnalyzeOrderSelect.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		if (!root.IsObject())
			goto L_FALSE;
		if (!root.HasMember("data"))
			goto L_FALSE;
		Value& data = root["data"];
		if (data.IsArray() && data.Size() == 0)
			goto L_FALSE;
		for (Value::MemberIterator itr2 = data.MemberBegin(); itr2 != data.MemberEnd(); ++itr2)
		{
			const char* szGameID = itr2->name.GetString();
			if (strcmp(szGameID, "message") == 0)
				goto L_FALSE;
			Value& GameID = data[szGameID];
			int min_gold = 0, max_gold = 0;
			if (GameID["min_gold"].IsInt())
				min_gold = GameID["min_gold"].GetInt();
			else if (GameID["min_gold"].IsString())
				sscanf_s(GameID["min_gold"].GetString(), "%d", &min_gold);
			else
				_asm int 3
			if (GameID["max_gold"].IsInt())
				max_gold = GameID["max_gold"].GetInt();
			else if (GameID["max_gold"].IsString())
				sscanf_s(GameID["max_gold"].GetString(), "%d", &max_gold);
			else
				_asm int 3
			if (max_gold == 0 || min_gold > max_gold) // "Sorry,you have exceeded your maximum wger limit 0 !!Please key your wager again!!(600020)",
				return BET_BLOCKED;
			Value& hidden = GameID["hidden"];
			strcpy_s(m_unique, 0x100, hidden["unique"].GetString());
			strcpy_s(m_data_pool, 0x100, hidden["data_pool"].GetString());
			strcpy_s(m_verify, 0x100, hidden["verify"].GetString());
			strcpy_s(m_secret, 0x100, hidden["secret"].GetString());
			strcpy_s(m_ratio, 0x100, hidden["ratio"].GetString());
			strcpy_s(m_concede, 0x100, hidden["concede"].GetString());

			strcpy_s(m_order_auth, 0x100, GameID["order_auth"].GetString());
			strcpy_s(m_ioratio, 0x100, GameID["ioratio"].GetString());
			strcpy_s(m_strong, 0x100, GameID["strong"].GetString());
			break;
		}
	}
	return BET_SUCCESS;
L_FALSE:
	//WriteResData(_T("d:\\bet888vip.AnalyzeOrderSelect.json"), m_szTextBet);
	return BET_FAIL;
}

int CBetSite_bet888vip::AnalyzeOrderBet()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\bet888vip.AnalyzeOrderBet.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		if (!root.IsObject())
			goto L_FALSE;
		if (!root.HasMember("data"))
			goto L_FALSE;
		Value& data = root["data"];
		for (Value::MemberIterator itr2 = data.MemberBegin(); itr2 != data.MemberEnd(); ++itr2)
		{
			const char* szGameID = itr2->name.GetString();
			int nGameID = 0;
			sscanf_s(szGameID, "%d", &nGameID);
			if (nGameID == 0)
				goto L_FALSE;
			Value& GameID = data[szGameID];
			if (!GameID.HasMember("bet_status") || !GameID["bet_status"].IsString())
				goto L_FALSE;
			char* bet_status = (char*)GameID["bet_status"].GetString();
			if (strcmp(bet_status, "success") == 0)
				break;
			int bet_code = GameID["bet_code"].GetInt();
			if (bet_code == 600020) // "Sorry,you have exceeded your maximum wger limit 0 !!Please key your wager again!!(600020)",
				return BET_BLOCKED;
			if (bet_code == 600005 || // Receive disabled, your wager is currently unacceptable.(600005)
				bet_code == 610013) // "Receive disabled, your wager is currently unacceptable.(610013)",
				goto L_FALSE;
			goto L_FALSE;
		}
	}
	return BET_SUCCESS;
L_FALSE:
	//WriteResData(_T("d:\\bet888vip.AnalyzeOrderBet.json"), m_szTextBet);
	return BET_FAIL;
}

BOOL CBetSite_bet888vip::AnalyzeBalanceBB()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	{
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;
		if (!root.IsObject())
			goto L_FALSE;
		if (!root.HasMember("data"))
			goto L_FALSE;
		if (!root["data"].IsString())
			goto L_FALSE;
		char* szBalance = (char*)root["data"].GetString();
		sscanf_s(szBalance, "%f", &g_fBalance[m_nBookMaker]);

		theFrame->m_wndSetting.PostMessageW(MSG_CUR_BALANCE, (WPARAM)m_nBookMaker, (LPARAM)(g_fBalance[m_nBookMaker] * 100));
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bet888vip.AnalyzeBalanceBB.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_bet888vip::AnalyzeBetBalanceBB()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		if (!root.IsObject())
			goto L_FALSE;
		if (!root.HasMember("data"))
			goto L_FALSE;
		if (!root["data"].IsString())
			goto L_FALSE;
		char* szBalance = (char*)root["data"].GetString();
		sscanf_s(szBalance, "%f", &g_fBalance[m_nBookMaker]);

		theFrame->m_wndSetting.PostMessageW(MSG_CUR_BALANCE, (WPARAM)m_nBookMaker, (LPARAM)(g_fBalance[m_nBookMaker] * 100));
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bet888vip.AnalyzeBetBalanceBB.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_bet888vip::AnalyzeResLive()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bet888vip.AnalyzeResLive.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_bet888vip::AnalyzeResLive1()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	{
		char *next_token = NULL;
		char *SS, *SR, *TS;
		SS = strtok_s(m_szText, ";", &next_token);
		SR = strtok_s(NULL, ";", &next_token);
		TS = strtok_s(NULL, ";", &next_token);
		if (!SS || !SR || !TS)
			goto L_FALSE;
		strcpy_s(m_SS, SS);
		strcpy_s(m_SR, SR);
		strcpy_s(m_TS, TS);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bet888vip.AnalyzeResLive1.json"), m_szText);
	return FALSE;
}

void CBetSite_bet888vip::DisplayCaptcha(BYTE* buf, int len, HANDLE hEvent)
{
	FILE* fp;
	TCHAR szFile[MAX_PATH];
	_stprintf_s(szFile, _T("./%s_captcha.png"), g_wszSiteName[m_nBookMaker]);
	_wfopen_s(&fp, szFile, _T("wb"));
	if (fp)
	{
		fwrite(buf, 1, len, fp);
		fclose(fp);
	}

	theFrame->PostMessageW(MSG_CAPTCHA, (WPARAM)m_nBookMaker, (LPARAM)hEvent);
}

BOOL CBetSite_bet888vip::AnalyzeResLive2()
{
	if (m_szText == NULL)
		goto L_FALSE;
	{
		HANDLE theHandle = CreateEvent(nullptr, true, true, nullptr);
		ResetEvent(theHandle);
		DisplayCaptcha((BYTE*)m_szText, m_nTextSize, theHandle);
		WaitForSingleObject(theHandle, INFINITE);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bet888vip.AnalyzeResLive2.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_bet888vip::AnalyzeResLogIn()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	{
		if (strstr(m_szText, "CheckCode error") != 0)
		{
			m_nLogInStatus = LS_FAIL;
			theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)2);
			goto L_FALSE;
		}
		else if (strstr(m_szText, "setSid") != 0)
		{
			char* p = strstr(m_szText, "setSid");
			memcpy(m_SID, p + 9, 40);
			m_SID[40] = 0;
			m_nLogInStatus = LS_SUCCESS;
			theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)1);
		}
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bet888vip.AnalyzeResLogIn.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_bet888vip::AnalyzeResLogOut()
{
	m_bExitThreadFlg = TRUE;
	m_nLogInStatus = LS_INIT;
	strcpy_s(m_szCaptcha, "");
	return TRUE;
}

BOOL CBetSite_bet888vip::AnalyzeResBalance()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	{
		char* p = strstr(m_szText, "_bbsportBalance");
		if (p == NULL)
			goto L_FALSE;
		char* p1 = strstr(p, "<strong>");
		char* p2 = strstr(p, "</strong>");

		char szBalance[40];
		memcpy(szBalance, p1 + 8, p2 - p1 - 8);
		szBalance[p2 - p1 - 8] = 0;
		sscanf_s(szBalance, "%f", &g_fBalance[m_nBookMaker]);

		theFrame->m_wndSetting.PostMessageW(MSG_CUR_BALANCE, (WPARAM)m_nBookMaker, (LPARAM)(g_fBalance[m_nBookMaker] * 100));
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bet888vip.AnalyzeResBalance.json"), m_szText);
	return FALSE;
}

string Odds[GC_COUNT][0x40];
//RB: 'Full',
//RV : '1st Half',
//RU : '2nd Half',

BOOL CBetSite_bet888vip::AnalyzeResData()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\bet888vip.AnalyzeResData.json"), m_szText);
	{
		char szOddID1[8], szOddID2[8], szOddID3[8];
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;
		if (!root.IsObject())
			goto L_FALSE;
		if (!root.HasMember("data"))
			goto L_FALSE;
		Value& data = root["data"];
		if (!data.HasMember("league"))
			goto L_FALSE;
		Value& leagues = data["league"];
		if (leagues.IsArray() && leagues.Size() == 0)
			return TRUE;
		CMap<int, int, CStringA, CStringA&> mapLeague;
		for (Value::MemberIterator itr2 = leagues.MemberBegin(); itr2 != leagues.MemberEnd(); ++itr2)
		{
			const char* szNum = itr2->name.GetString();
			int league_no;
			sscanf_s(szNum, "%d", &league_no);
			char* league_name = (char*)leagues[szNum].GetString();
			mapLeague.SetAt(league_no, CStringA(league_name));
		}

		if (!data.HasMember("game") || !data.HasMember("odds_map"))
			goto L_FALSE;

		Value& odds_map = data["odds_map"];
		if (odds_map.IsArray() && odds_map.Size() == 0)
			return TRUE;///????????????????????
		for (Value::MemberIterator itr2 = odds_map.MemberBegin(); itr2 != odds_map.MemberEnd(); ++itr2)
		{
			const char* szCat = itr2->name.GetString();
			Value RB;
			if (odds_map[szCat].HasMember("RB"))
				RB = odds_map[szCat]["RB"];
			else if (odds_map[szCat].HasMember("RU"))
				RB = odds_map[szCat]["RU"];
			else if (odds_map[szCat].HasMember("RV"))
				RB = odds_map[szCat]["RV"];
			int nSize = RB.Size();
			int nCat = GetGameCategory((char*)szCat);
			for (int i = 0; i < nSize; i++)
			{
				Odds[nCat][i] = RB[i].GetString();
			}
		}

		Value& game = data["game"];
		if (game.IsArray() && game.Size() == 0)
			return TRUE;///????????????????????

		for (Value::MemberIterator itr1 = game.MemberBegin(); itr1 != game.MemberEnd(); ++itr1)
		{
			const char* szNumber = itr1->name.GetString();
			Value& num = game[szNumber];

			if (!num.HasMember("tid_h") || !num.HasMember("tid_a") || !num.HasMember("lid") || !num.HasMember("game_no"))
				continue;
			string ht = (char*)num["tid_h"].GetString();
			string at = (char*)num["tid_a"].GetString();
			string league = (char*)num["lid"].GetString();
			char* szID = (char*)num["game_no"].GetString();
			int nID;
			sscanf_s(szNumber, "%d", &nID);

			if (!num.HasMember("game_type"))
				continue;
			char* szCategory = (char*)num["game_type"].GetString();
			int nGameCategory = GetGameCategory(szCategory);
			if (nGameCategory == -1 || !theFrame->m_bSetCategory[nGameCategory])
				continue;
			//int ipt = ::GetTime((char*)num["game_time"].GetString(), nGameCategory);
			int ipt = 0;
			string stage = "";
			if (!num.HasMember("game_start_time"))
				continue;
			if (num["game_start_time"].IsInt())
				ipt = num["game_start_time"].GetInt() * 60;
			else if (num["game_start_time"].IsString())
			{
				const char* sss = num["game_start_time"].GetString();
				if (strcmp(sss, "mid") == 0)
				{
					ipt = 45 * 60;
					stage = "HT";
				}
			}
			int hs = -1, as = -1, hs1 = -1, as1 = -1, hs2 = -1, as2 = -1;
			if (!num.HasMember("score_h") || !num.HasMember("score_a"))
				continue;
			if (num["score_h"].IsNumber())
				hs = num["score_h"].GetInt();
			else
			{
				char* szhs = (char*)num["score_h"].GetString();
				sscanf_s(szhs, "%d", &hs);
			}
			if (num["score_a"].IsNumber())
				as = num["score_a"].GetInt();
			else
			{
				char* szas = (char*)num["score_a"].GetString();
				sscanf_s(szas, "%d", &as);
			}
			if (!num.HasMember("lid"))
				continue;
			char* szlid = (char*)num["lid"].GetString();
			int nlid;
			sscanf_s(szlid, "%d", &nlid);
			CStringA strLeague;
			mapLeague.Lookup(nlid, strLeague);

			//if (nGameCategory == GC_TENNIS || nGameCategory == GC_TABLE_TENNIS || nGameCategory == GC_BASEBALL || nGameCategory == GC_VOLLEYBALL)
			//	_asm int 3
			if (IsExceptionGame((char*)ht.c_str(), (char*)at.c_str(), strLeague.GetBuffer(), (char*)stage.c_str()))
				continue;
			CMainInfo MainInfo(nGameCategory, (char*)ht.c_str(), (char*)at.c_str(), SCORE(hs, hs1, hs2), SCORE(as, as1, as2), (char*)stage.c_str(), ipt, strLeague.GetBuffer());
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

			if (pGameData == NULL)
				ASSERT(0);

			pGameData->RemoveAllOddsInfo();

			if (!data.HasMember("insert") || !data["insert"].HasMember(league.c_str()) || !data["insert"][league.c_str()].HasMember(szNumber))
				continue;
			Value& Number = data["insert"][league.c_str()][szNumber];
			Value odds;
			if (Number.HasMember("RB"))
				odds = Number["RB"];
			else if (Number.HasMember("RU"))
				odds = Number["RU"];
			else if (Number.HasMember("RV"))
				odds = Number["RV"];
			int i = 0, count = odds.Size();
			float hVal1 = 0, hVal2 = 0, oVal1, oVal2;
			BOOL bAsian = FALSE;
			char* sPos = NULL;
			while (i < count)
			{
				strcpy_s(szOddID1, "");
				strcpy_s(szOddID2, "");
				strcpy_s(szOddID3, "");
				hVal1 = 0; hVal2 = 0; oVal1 = 0; oVal2 = 0;
				bAsian = FALSE;
				sPos = NULL;
				if (Odds[nGameCategory][i] == "RH_R" || Odds[nGameCategory][i] == "R2H_R")
				{
					ASSERT(i + 4 <= count);
					BOOL bOK = TRUE;
					if (!odds[i].IsString())
					{
						hVal1 = odds[i].GetFloat();
						hVal1 = -hVal1;
					}
					else
					{
						char* s1 = (char*)odds[i].GetString();
						if (strcmp(s1, "") != 0)
						{
							if ((sPos = strstr(s1, "/")) != 0)
							{
								char* a = new char[sPos - s1 + 1];
								memcpy(a, s1, sPos - s1);
								a[sPos - s1] = 0;
								sscanf_s(a, "%f", &hVal1);
								sscanf_s(sPos + 1, "%f", &hVal2);
								delete a;
								bAsian = TRUE;
							}
							else
							{
								sscanf_s(s1, "%f", &hVal1);
								hVal1 = -hVal1;
							}
						}
						else
						{
							if (!odds[i + 1].IsString())
								hVal1 = odds[i + 1].GetFloat();
							else
							{
								char* s2 = (char*)odds[i + 1].GetString();
								if (strcmp(s2, "") != 0)
								{
									if ((sPos = strstr(s2, "/")) != 0)
									{
										char* a = new char[sPos - s2 + 1];
										memcpy(a, s2, sPos - s2);
										a[sPos - s2] = 0;
										sscanf_s(a, "%f", &hVal1);
										sscanf_s(sPos + 1, "%f", &hVal2);
										delete a;
										bAsian = TRUE;
									}
									else
									{
										sscanf_s(s2, "%f", &hVal1);
									}
								}
								else
									bOK = FALSE;
							}
						}
					}
					if (bOK)
					{
						char* s3 = (char*)odds[i + 2].GetString();
						sscanf_s(s3, "%f", &oVal1);
						char* s4 = (char*)odds[i + 3].GetString();
						sscanf_s(s4, "%f", &oVal2);

						if (!bAsian)
							MyAddOddInfo(pGameData, OI_HANDICAP, hVal1, oVal1, oVal2, 0, (char*)Odds[nGameCategory][i + 2].c_str(), (char*)Odds[nGameCategory][i + 3].c_str(), "");
					}
					i += 4;
				}
				else if (Odds[nGameCategory][i] == "OUH_R" || Odds[nGameCategory][i] == "OU2H_R")
				{
					ASSERT(i + 4 <= count);
					BOOL bOK = TRUE;
					if (!odds[i].IsString())
					{
						hVal1 = odds[i].GetFloat();
					}
					else
					{
						char* s1 = (char*)odds[i].GetString();
						if (strcmp(s1, "") != 0)
						{
							if ((sPos = strstr(s1, "/")) != 0)
							{
								char* a = new char[sPos - s1 + 1];
								memcpy(a, s1, sPos - s1);
								a[sPos - s1] = 0;
								sscanf_s(a, "%f", &hVal1);
								sscanf_s(sPos + 1, "%f", &hVal2);
								delete a;
								bAsian = TRUE;
							}
							else
							{
								sscanf_s(s1, "%f", &hVal1);
							}
						}
						else
						{
							if (!odds[i + 1].IsString())
								hVal1 = odds[i + 1].GetFloat();
							else
							{
								char* s2 = (char*)odds[i + 1].GetString();
								if (strcmp(s2, "") != 0)
								{
									if ((sPos = strstr(s2, "/")) != 0)
									{
										char* a = new char[sPos - s2 + 1];
										memcpy(a, s2, sPos - s2);
										a[sPos - s2] = 0;
										sscanf_s(a, "%f", &hVal1);
										sscanf_s(sPos + 1, "%f", &hVal2);
										delete a;
										bAsian = TRUE;
									}
									else
									{
										sscanf_s(s2, "%f", &hVal1);
									}
								}
								else
									bOK = FALSE;
							}
						}
					}
					if (bOK)
					{
						char* s3 = (char*)odds[i + 2].GetString();
						sscanf_s(s3, "%f", &oVal1);
						char* s4 = (char*)odds[i + 3].GetString();
						sscanf_s(s4, "%f", &oVal2);

						if (!bAsian)
							MyAddOddInfo(pGameData, OI_GOAL_OU, hVal1, oVal1, oVal2, 0, (char*)Odds[nGameCategory][i + 2].c_str(), (char*)Odds[nGameCategory][i + 3].c_str(), "");
					}
					i += 4;
				}
				else if (Odds[nGameCategory][i] == "MH")
				{
					ASSERT(i + 2 <= count);
					float h = 0, a = 0, d = 0, oVal3 = 0;
					char* s1 = (char*)odds[i].GetString();
					if (strcmp(s1, "") != 0)
					{
						sscanf_s(s1, "%f", &oVal1);
						char* s2 = (char*)odds[i + 1].GetString();
						sscanf_s(s2, "%f", &oVal2);
						if (i + 2 < count && Odds[nGameCategory][i + 2] == "MN")
						{
							ASSERT(i + 3 <= count);
							char* s3 = (char*)odds[i + 2].GetString();
							sscanf_s(s3, "%f", &oVal3);
							strcpy_s(szOddID3, "MN");
							i++;
						}
						if (!bAsian)
							MyAddOddInfo(pGameData, OI_MAIN, 0, oVal1, oVal2, oVal3, "MH", "MC", szOddID3);
					}
					i += 2;
					if (i < count && Odds[nGameCategory][i] == "MN")
						i++;
				}
				else if (Odds[nGameCategory][i] == "ODD")
				{
					ASSERT(i + 2 <= count);
					char* s1 = (char*)odds[i].GetString();
					if (strcmp(s1, "") != 0)
					{
						sscanf_s(s1, "%f", &oVal1);
						char* s2 = (char*)odds[i + 1].GetString();
						sscanf_s(s2, "%f", &oVal2);
						if (!bAsian)
							MyAddOddInfo(pGameData, OI_GOAL_OE, 0, oVal1, oVal2, 0, "ODD", "EVEN", "");
					}
					i += 2;
				}
				else if (Odds[nGameCategory][i] == "R0H" || Odds[nGameCategory][i] == "R12H_R" || Odds[nGameCategory][i] == "R12H")
				{
					i += 2;
				}
				else
				{
					string k = Odds[nGameCategory][i];
					ASSERT(0);
				}
			}

			::EnterCriticalSection(&g_csGameInfoArray);
			CopyGameDataProc(pGameData);
			::LeaveCriticalSection(&g_csGameInfoArray);
		}
		theFrame->PostMessageW(MSG_UPDATE, (WPARAM)m_nBookMaker);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bet888vip.AnalyzeResData.json"), m_szText);
	return FALSE;
}

void CBetSite_bet888vip::GetOddID(int nBetObjNo, COddsInfo* pOddsInfo, char* szOddID)
{
	if (nBetObjNo == 0)
	{
		GetOddIntStr(pOddsInfo->m_dwOddID1, szOddID);
		return;
	}
	else if (nBetObjNo == 1)
	{
		GetOddIntStr(pOddsInfo->m_dwOddID2, szOddID);
		return;
	}
	else if (nBetObjNo == 2)
	{
		GetOddIntStr(pOddsInfo->m_dwOddID3, szOddID);
		return;
	}
	else
		ASSERT(0);
	return;
}

#define ODD_ID_COUNT 22
char* g_szOdds[ODD_ID_COUNT] =
{"", "RH_R", "RC_R", "RH", "RC", "R2H_R", "R2C_R", "R2H", "R2C", "OUH_R", "OUC_R", "OUH", "OUC", "OU2H_R", "OU2C_R", "OU2H", "OU2C", "MH", "MC", "MN", "ODD", "EVEN" };

UINT64 CBetSite_bet888vip::GetOddIntID(char* szOddID)
{
	for (int i = 0; i < ODD_ID_COUNT; i++)
	{
		if (strcmp(szOddID, g_szOdds[i]) == 0)
			return i;
	}
	ASSERT(0);
	return -1;
}

void CBetSite_bet888vip::GetOddIntStr(UINT64 nOddID, char* szOddID)
{
	strcpy_s(szOddID, 8, g_szOdds[nOddID]);
}

COddsInfo* CBetSite_bet888vip::MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, char* szOddID1, char* szOddID2, char* szOddID3)
{
	COddsInfo* pOddsInfo = new COddsInfo;
	if (pGameData->m_MainInfo.m_nGameCategory == GC_FOOTBALL)
	{
		if (fOval1 != 0)
			fOval1 += 0.2f;
		if (fOval2 != 0)
			fOval2 += 0.2f;
		if (fOval3 != 0)
			fOval3 += 0.2f;
	}
	UINT64 dwOddID1 = GetOddIntID(szOddID1);
	UINT64 dwOddID2 = GetOddIntID(szOddID2);
	UINT64 dwOddID3 = GetOddIntID(szOddID3);
	AddOddInfo(pOddsInfo, pGameData, nOddsKind, hVal1, fOval1, fOval2, fOval3, dwOddID1, dwOddID2, dwOddID3);
	return pOddsInfo;
}

int CBetSite_bet888vip::GetGameCategory(char* szGameCategory)
{
	if (_strcmpi(szGameCategory, "FT") == 0)
		return GC_FOOTBALL;
	if (_strcmpi(szGameCategory, "TN") == 0)
		return GC_TENNIS;
	if (_strcmpi(szGameCategory, "BK") == 0)
		return GC_BASKETBALL;
	if (_strcmpi(szGameCategory, "BS") == 0)
		return GC_BASEBALL;
	if (_strcmpi(szGameCategory, "IH") == 0)
		return GC_ICE_HOCKEY;
	return CBetSite::GetGameCategory(szGameCategory);
}

string CBetSite_bet888vip::MyGetGameCategory(int nGameCategory)
{
	switch (nGameCategory)
	{
	case GC_FOOTBALL:
		return "FT";
	case GC_TENNIS:
		return "TN";
	case GC_BASKETBALL:
		return "BK";
	case GC_BASEBALL:
		return "BS";
	case GC_ICE_HOCKEY:
		return "IH";
	}
	return "";
}

BOOL CBetSite_bet888vip::UpdateTree(int nIsExitThread)
{
	int nRet = UpdateTree_AllInfoDownAtOnce(nIsExitThread);
	CBetSite::UpdateTree(nIsExitThread);
	return nRet;
}
