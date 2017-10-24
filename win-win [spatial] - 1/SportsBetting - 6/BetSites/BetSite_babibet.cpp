#include "..\stdafx.h"
#include "BetSite_babibet.h"
#include "..\global.h"
#include "..\GameInfo.h"
#include "..\GameData.h"
#include "..\SportsBetting.h"
#include <fstream>
#include "..\MainFrm.h"

using namespace rapidjson;

//https://www.babibet.com/en/sport/live

CBetSite_babibet::CBetSite_babibet()
{
	m_nBookMaker = SITE_babibet;
	m_nMinBetMoney = 8;

	_tcscpy_s(m_szSV, _T(""));
}


CBetSite_babibet::~CBetSite_babibet()
{
}

int CBetSite_babibet::MainProc()
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
			//if (!SendRequestEx(REQ_HB))
			//{
			//	OutDbgStrAA("\n[babibet]:MAIN ERROR - REQ_HB");
			//	return 0;
			//}
			if (!RefreshData())
				return 0;
			if (IsNeedGetBalance())
			{
				if (!RefreshBalance())
					return 0;
			}
			OutDbgStrAA("\n[babibet]****************************");
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
						OutDbgStrAA("\n[babibet]:MAIN ERROR - REQ_LIVE");
						return 0;
					}
					m_nLogInStatus = LS_LIVE;
				}
				if (!SendRequestEx(REQ_LOGIN))
				{
					OutDbgStrAA("\n[babibet]:MAIN ERROR - REQ_LOGIN");
					return 0;
				}
				if (!AnalyzeResLogIn())
				{
					OutDbgStrAA("\n[babibet]:MAIN ERROR - AnalyzeResLogIn");
					return 0;
				}
				if (m_nLogInStatus == LS_SUCCESS)
				{
					if (!SendRequestEx(REQ_LIVE3))
					{
						OutDbgStrAA("\n[babibet]:MAIN ERROR - REQ_LIVE3");
						return 0;
					}
					if (!SendRequestEx(REQ_LIVE2))
					{
						OutDbgStrAA("\n[babibet]:MAIN ERROR - REQ_LIVE2");
						return 0;
					}
					if (!SendRequestEx(REQ_LIVE5))
					{
						OutDbgStrAA("\n[babibet]:MAIN ERROR - REQ_LIVE5");
						return 0;
					}
					if (!SendRequestEx(REQ_LIVE1))
					{
						OutDbgStrAA("\n[babibet]:MAIN ERROR - REQ_LIVE1");
						return 0;
					}
					if (!SendRequestEx(REQ_LIVE4))
					{
						OutDbgStrAA("\n[babibet]:MAIN ERROR - REQ_LIVE4");
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

BOOL CBetSite_babibet::SendRequest(int nReqKind)
{
	CHttpConnection* pServer = NULL;
	m_nTextSize = 0;

	DWORD dwRet;
	CString strServerName = _T("www.babibet.com");
	char strFormData[1000];
	strcpy_s(strFormData, "");

	try
	{
		pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

		if (nReqKind == REQ_LIVE)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en/sport/live"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LOGIN)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/euro/operation"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_pSession->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);

			//strcpy_s(strFormData, "sport_id=1&time_range=today&method=getEuroSport");
			sprintf_s(strFormData, "login_form[username]=%s&login_form[password]=%s&method=login&system_data[current_url]=https://www.babibet.com/en/sport/live", m_szID, m_szPswd);
		}
		else if (nReqKind == REQ_LIVE3)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/zh/euro/live"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
			m_HttpFile->AddRequestHeaders(_T("referer:https://www.babibet.com/en/sport/live"));
			m_pSession->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);
		}
		else if (nReqKind == REQ_LIVE2)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/zh/euro/operation"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.babibet.com/zh/euro/live"));
			m_HttpFile->AddRequestHeaders(_T("x-requested-with:XMLHttpRequest"));
			m_pSession->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);

			strcpy_s(strFormData, "method=serviceOneTwo&system_data[current_url]=https://www.babibet.com/zh/euro/live");
		}
		else if (nReqKind == REQ_LIVE5)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/zh/euro/operation?method=getEuroLive"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.babibet.com/zh/euro/live"));
			m_HttpFile->AddRequestHeaders(_T("x-requested-with:XMLHttpRequest"));
			m_pSession->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);

			strcpy_s(strFormData, "containers={}&method=getEuroLive");
		}
		else if (nReqKind == REQ_LIVE1)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en/euro/live"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.babibet.com/zh/euro/live"));
			m_pSession->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);
		}
		else if (nReqKind == REQ_LIVE4)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en/sport/live"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.babibet.com/en/euro/live"));
			m_pSession->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);
		}
		else if (nReqKind == REQ_HB)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/sport/operation?hb"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFile->AddRequestHeaders(_T("referer:https://www.babibet.com/en/sport/live"));
			m_pSession->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);

			//strcpy_s(strFormData, "sport_id=1&time_range=today&method=getEuroSport");
			strcpy_s(strFormData, "method=hb");
		}
		else if (nReqKind == REQ_DATA)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/sport/operation?method=updateOdds&asian_selection=live"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));

			strcpy_s(strFormData, "viewParams[market_types_sports_main]=1&viewParams[sports_game_periods_main]=1&viewParams[option_index_lte]=3&viewParams[asian_selection]=live&containers={}&method=updateOdds");
		}
		else if (nReqKind == REQ_BALANCE)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/euro/operation"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFile->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_pSession->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);

			strcpy_s(strFormData, "method=getMemberBalance");
		}
		else if (nReqKind == REQ_LOGOUT)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en/index/logout"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.babibet.com/en/euro/live"));
			m_pSession->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);
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
			if (!GetDELASESS((TCHAR*)buffer))
				goto L_FALSE;
		}
		else if (nReqKind == REQ_LIVE1)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetSV((TCHAR*)buffer))
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

UINT64 CBetSite_babibet::GetOddID(int nBetObjNo, COddsInfo* pOddsInfo)
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

int CBetSite_babibet::SendBetRequest(BetInfo* pBetInfo, int nBetObjNo, int nReqKind)
{
	m_nTextSizeBet = 0;
	CHttpConnection* pServer = NULL;
	DWORD dwRet;
	CString strServerName = _T("www.babibet.com");
	char strFormData[4000];
	strcpy_s(strFormData, "");

	try
	{
		pServer = m_pSessionBet->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

		if (nReqKind == REQ_GET_ODD_BABI)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/sport/operation?method=getOdd"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFileBet->AddRequestHeaders(_T("referer:https://www.babibet.com/en/sport/live"));
			m_pSessionBet->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);

			sprintf_s(strFormData, "action=add&oddID=e_%llu&oddValue=%.2f&type=sport_single&method=getOdd", m_dwOddID, pBetInfo->BetVal[nBetObjNo].oVal);
		}
		else if (nReqKind == REQ_UPDATE_BETSLIP_BABI)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/sport/operation?method=updateBetSlip"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFileBet->AddRequestHeaders(_T("referer:https://www.babibet.com/en/sport/live"));
			m_pSessionBet->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);
			//m_pSessionBet->SetCookie(_T("https://www.babibet.com"), _T("sv"), m_szSV);

			sprintf_s(strFormData, "odds[]=e_%llu&type=sport_single&method=updateBetSlip", m_dwOddID);
			//sprintf_s(strFormData, "type=sport_single&method=updateBetSlip");
		}
		else if (nReqKind == REQ_PLACE_BET_BABI)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/sport/operation?method=placeBet"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
			m_HttpFileBet->AddRequestHeaders(_T("referer:https://www.babibet.com/en/sport/live"));
			m_pSessionBet->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);
			//m_pSessionBet->SetCookie(_T("https://www.babibet.com"), _T("sv"), m_szSV);

			sprintf_s(strFormData, "odds[e_%llu][stake]=%d&odds[e_%llu][odd]=%.2f&bet_slip[type]=single&bet_slip[options][accept_any_odds]=0&bet_slip[options][container_type]=sport_single&method=placeBet",
				m_dwOddID, pBetInfo->BetVal[nBetObjNo].nBettingMoney, m_dwOddID, pBetInfo->BetVal[nBetObjNo].oVal);
		}
		else if (nReqKind == REQ_REMOVE_ODD_BABI)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/sport/operation?method=removeOdd"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));
			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFileBet->AddRequestHeaders(_T("referer:https://www.babibet.com/en/sport/live"));
			m_pSessionBet->SetCookie(_T("https://www.babibet.com"), _T("DELASESS"), m_szSession);
			//m_pSessionBet->SetCookie(_T("https://www.babibet.com"), _T("sv"), m_szSV);

			sprintf_s(strFormData, "action=remove&oddID=e_%llu&type=sport_single&method=removeOdd", m_dwOddID);
		}
		else if (nReqKind == REQ_BET_BALANCE_BABI)
		{
			m_HttpFileBet = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/en/euro/operation"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFileBet->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
			m_HttpFileBet->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded; charset=UTF-8"));
			m_HttpFileBet->AddRequestHeaders(_T("X-Requested-With:XMLHttpRequest"));

			strcpy_s(strFormData, "method=getMemberBalance");
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

int CBetSite_babibet::DoBettingEx(BetInfo* pBetInfo, int nBetObjNo, COddsInfo* pOddsInfo)
{
	m_dwOddID = GetOddID(nBetObjNo, pOddsInfo);

	int nResult;
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_GET_ODD_BABI)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - BET_GET_ODD_FAIL (%.2f)", pBetInfo->hVal1);
		pBetInfo->BetVal[nBetObjNo].nResult = BET_GET_ODD_FAIL;
		goto L_END;
	}
	if ((nResult = AnalyzeBetREQ_GET_ODD()) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - AnalyzeBetREQ_GET_ODD");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_UPDATE_BETSLIP_BABI)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - REQ_UPDATE_BETSLIP");
		pBetInfo->BetVal[nBetObjNo].nResult = REQ_UPDATE_BETSLIP_FAIL;
		goto L_END;
	}
	float coeff = 0;
	nResult = AnalyzeBetREQ_UPDATE_BETSLIP(pBetInfo, nBetObjNo, coeff);
	if (nResult != BET_SUCCESS)
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - AnalyzeBetREQ_UPDATE_BETSLIP");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if (coeff == 0)
		_asm int 3
	pBetInfo->BetVal[nBetObjNo].oVal_orig = pBetInfo->BetVal[nBetObjNo].oVal;
	pBetInfo->BetVal[nBetObjNo].oVal_cur = coeff;
	if (coeff >= pBetInfo->BetVal[nBetObjNo].oVal)
		pBetInfo->BetVal[nBetObjNo].oVal = coeff;
	else
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - Coeff Decreased");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_VALUE_DECREASED1;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_PLACE_BET_BABI)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - REQ_PLACE_BET");
		pBetInfo->BetVal[nBetObjNo].nResult = REQ_PLACE_BET_FAIL;
		goto L_END;
	}
	if ((nResult = AnalyzeBetREQ_PLACE_BET()) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - AnalyzeBetREQ_PLACE_BET");
		pBetInfo->BetVal[nBetObjNo].nResult = nResult;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_REMOVE_ODD_BABI)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - REQ_REMOVE_ODD_FAIL");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;// nResult;
		goto L_END;
	}
	if (!AnalyzeBetREQ_REMOVE_ODD())
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - AnalyzeBetREQ_REMOVE_ODD");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;// BET_ANALYZE_ERROR4;
		goto L_END;
	}
	if ((nResult = SendBetRequestEx(pBetInfo, nBetObjNo, REQ_BET_BALANCE_BABI)) != BET_SUCCESS)
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - REQ_BET_BALANCE");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
		return 0;
	}
	if (!AnalyzeBetResBalance())
	{
		OutDbgStrAA("\n[babibet]:BET ERROR - AnalyzeBetResBalance");
		pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
		return 0;
	}

	pBetInfo->BetVal[nBetObjNo].nResult = BET_SUCCESS;
L_END:
	return 0;
}

int CBetSite_babibet::AnalyzeBetREQ_GET_ODD()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("C:\\babibet.AnalyzeBetREQ_GET_ODD.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		if (!root.HasMember("status"))
			goto L_FALSE;
		char* szStatus = (char*)root["status"].GetString();
		if (strcmp(szStatus, "success") != 0)
		{
			if (!root.HasMember("errors"))
				goto L_FALSE;
			Value& root_data = root["errors"];
			if (root_data.IsArray() && root_data.Size() == 0)
				goto L_FALSE;
			if (!root_data.IsObject())
				goto L_FALSE;
			for (Value::MemberIterator itr1 = root_data.MemberBegin(); itr1 != root_data.MemberEnd(); ++itr1)
			{
				const char* szNumber = itr1->name.GetString();
				Value& num = root_data[szNumber];
				if (!num.HasMember("message"))
					goto L_FALSE;
				const char* msg = num["message"].GetString();
				if (strstr(msg, "Odds are updating") != 0)
					return BET_VALUE_CHANGED;
				if (strstr(msg, "Odds changed") != 0)
					return BET_VALUE_CHANGED;
				if (strstr(msg, "Odds are not available") != 0)
					return BET_ODDS_NOT_AVAILABLE;
			}
			goto L_FALSE;
		}
	}
	return BET_SUCCESS;
L_FALSE:
	WriteResData(_T("c:\\babibet.AnalyzeBetREQ_GET_ODD.json"), m_szTextBet);
	return BET_PARSE_FAIL;
}

int CBetSite_babibet::AnalyzeBetREQ_UPDATE_BETSLIP(BetInfo* pBetInfo, int nBetObjNo, float& coeff)
{
	coeff = 0;
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("c:\\babibet.AnalyzeBetREQ_UPDATE_BETSLIP.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		if (!root.HasMember("status"))
			goto L_FALSE;
		char* szStatus = (char*)root["status"].GetString();
		if (strcmp(szStatus, "success") != 0)
			goto L_FALSE;
		Value& odds = root["odds"];
		if (!odds.IsObject())
			goto L_FALSE;
		for (Value::MemberIterator itr1 = odds.MemberBegin(); itr1 != odds.MemberEnd(); ++itr1)
		{
			const char* szNumber = itr1->name.GetString();
			Value& num = odds[szNumber];
			if (!num.HasMember("single_odd_decimal"))
				goto L_FALSE;
			char* szOVal = (char*)num["single_odd_decimal"].GetString();
			float fOval;
			sscanf_s(szOVal, "%f", &fOval);
			//if (fOval < pBetInfo->BetVal[nBetObjNo].oVal)
			//	return BET_VALUE_DECREASED;
			//else
			{
				coeff = fOval;
				break;
			}
		}
	}
	return BET_SUCCESS;
L_FALSE:
	WriteResData(_T("c:\\babibet.AnalyzeBetREQ_UPDATE_BETSLIP.json"), m_szTextBet);
	return BET_PARSE_FAIL;
}

int CBetSite_babibet::AnalyzeBetREQ_PLACE_BET()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	WriteResData(_T("c:\\babibet.AnalyzeBetREQ_PLACE_BET.json"), m_szTextBet);
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		if (!root.HasMember("status"))
			goto L_FALSE;
		char* szStatus = (char*)root["status"].GetString();
		if (strcmp(szStatus, "success") != 0)
		{
			if (!root.HasMember("errors"))
				goto L_FALSE;
			Value& root_data = root["errors"];
			if (!root_data.IsObject())
				goto L_FALSE;
			for (Value::MemberIterator itr1 = root_data.MemberBegin(); itr1 != root_data.MemberEnd(); ++itr1)
			{
				const char* szNumber = itr1->name.GetString();
				Value& num = root_data[szNumber];
				if (!num.HasMember("message"))
					goto L_FALSE;
				const char* msg = num["message"].GetString();
				if (strstr(msg, "Odds are updating") != 0)
					return BET_VALUE_CHANGED;
				if (strstr(msg, "Odds changed") != 0)
					return BET_VALUE_CHANGED;
				if (strstr(msg, "Odds are not valid") != 0)
					return BET_ODDS_NOT_VALID;
			}
			goto L_FALSE;
		}
	}
	return BET_SUCCESS;
L_FALSE:
	WriteResData(_T("c:\\babibet.AnalyzeBetREQ_PLACE_BET.json"), m_szTextBet);
	return BET_PARSE_FAIL;
}

BOOL CBetSite_babibet::AnalyzeBetREQ_REMOVE_ODD()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;
		if (!root.HasMember("status"))
			goto L_FALSE;
		char* szStatus = (char*)root["status"].GetString();
		if (strcmp(szStatus, "success") != 0)
			goto L_FALSE;
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("c:\\babibet.AnalyzeBetREQ_REMOVE_ODD.json"), m_szTextBet);
	return FALSE;
}

BOOL CBetSite_babibet::AnalyzeResLogIn()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	{
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;
		if (!root.HasMember("status"))
			goto L_FALSE;
		char* szStatus = (char*)root["status"].GetString();
		if (strcmp(szStatus, "success") == 0)
		{
			m_nLogInStatus = LS_SUCCESS;
			theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)1);
		}
		else if (strcmp(szStatus, "error") == 0)
		{
			//m_nLogInStatus = LS_FAIL;
			theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)2);
			goto L_FALSE;
		}
		else
			ASSERT(0);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("c:\\babibet.AnalyzeResLogIn.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_babibet::AnalyzeResLogOut()
{
	if (m_szText == NULL || m_nTextSize < 10)
		return FALSE;

	m_bExitThreadFlg = TRUE;
	m_nLogInStatus = LS_INIT;
	return TRUE;
}

BOOL CBetSite_babibet::AnalyzeBetResBalance()
{
	if (m_szTextBet == NULL || m_nTextSizeBet < 20)
		goto L_FALSE;
	{
		Document root;
		ParseResult ok = root.Parse(m_szTextBet);
		if (!ok)
			goto L_FALSE;

		if (!root.HasMember("status"))
			goto L_FALSE;
		char* szStatus = (char*)root["status"].GetString();
		if (strcmp(szStatus, "success") != 0)
			goto L_FALSE;

		if (!root.HasMember("data"))
			goto L_FALSE;
		Value& root_data = root["data"];

		BOOL bFound = FALSE;
		if (!root_data.IsObject())
			goto L_FALSE;
		for (Value::MemberIterator itr1 = root_data.MemberBegin(); itr1 != root_data.MemberEnd(); ++itr1)
		{
			const char* szNumber = itr1->name.GetString();
			Value& num = root_data[szNumber];

			if (!num.HasMember("info"))
				continue;
			Value& num_info = num["info"];

			if (!num_info.HasMember("creditType"))
				continue;
			Value& num_info_creditType = num_info["creditType"];
			char* szCreditType = (char*)num_info_creditType.GetString();
			if (strcmp(szCreditType, "Balance") != 0)
				continue;
			if (!num_info.HasMember("amount"))
				continue;
			Value& num_info_amount = num_info["amount"];
			char* szAmount = (char*)num_info_amount.GetString();
			GetBalance(szAmount);
			bFound = TRUE;
			break;
		}
		if (!bFound)
			goto L_FALSE;
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("c:\\babibet.AnalyzeBetResBalance.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_babibet::AnalyzeResBalance()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	{
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;

		if (!root.HasMember("status"))
			goto L_FALSE;
		char* szStatus = (char*)root["status"].GetString();
		if (strcmp(szStatus, "success") != 0)
			goto L_FALSE;

		if (!root.HasMember("data"))
			goto L_FALSE;
		Value& root_data = root["data"];

		BOOL bFound = FALSE;
		if (!root_data.IsObject())
			goto L_FALSE;
		for (Value::MemberIterator itr1 = root_data.MemberBegin(); itr1 != root_data.MemberEnd(); ++itr1)
		{
			const char* szNumber = itr1->name.GetString();
			Value& num = root_data[szNumber];

			if (!num.HasMember("info"))
				continue;
			Value& num_info = num["info"];

			if (!num_info.HasMember("creditType"))
				continue;
			Value& num_info_creditType = num_info["creditType"];
			char* szCreditType = (char*)num_info_creditType.GetString();
			if (strcmp(szCreditType, "Balance") != 0)
				continue;
			if (!num_info.HasMember("amount"))
				continue;
			Value& num_info_amount = num_info["amount"];
			char* szAmount = (char*)num_info_amount.GetString();
			GetBalance(szAmount);
			bFound = TRUE;
			break;
		}
		if (!bFound)
			goto L_FALSE;
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("c:\\babibet.AnalyzeResBalance.json"), m_szText);
	return FALSE;
}

BOOL CBetSite_babibet::AnalyzeResData()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	WriteResData(_T("c:\\babibet.AnalyzeResData.json"), m_szText);
	try
	{
		Document root;
		ParseResult ok = root.Parse(m_szText);
		if (!ok)
			goto L_FALSE;
		if (!root.HasMember("data"))
			goto L_FALSE;
		Value& root_data = root["data"];
		if (root_data.HasMember("live") == FALSE)
			goto L_FALSE;

		Value& live = root_data["live"];
		Value& live_data = live["data"];

		const char* action = live["action"].GetString();

		if (live_data.IsArray() && live_data.Size() == 0)
			return TRUE;
		if (!live_data.IsObject())
			goto L_FALSE;
		for (Value::MemberIterator itr1 = live_data.MemberBegin(); itr1 != live_data.MemberEnd(); ++itr1)
		{
			const char* szCat = itr1->name.GetString();
			int nGameCategory = GetGameCategory((char*)szCat);
			if (nGameCategory == -1 || !theFrame->m_bSetCategory[nGameCategory])
				continue;
			if (!live_data.HasMember(szCat))
				continue;
			Value& cat = live_data[szCat];
			if (!cat.HasMember("data"))
				continue;
			Value& cat_data = cat["data"];

			if (cat_data.IsArray() && cat_data.Size() == 0)
				continue;
			if (!cat_data.IsObject())
				goto L_FALSE;
			for (Value::MemberIterator itr2 = cat_data.MemberBegin(); itr2 != cat_data.MemberEnd(); ++itr2)
			{
				const char* szNoLive = itr2->name.GetString();
				if (!cat_data.HasMember(szNoLive))
					continue;
				Value& no_league = cat_data[szNoLive];
				if (!no_league.HasMember("data"))
					continue;
				Value& no_league_data = no_league["data"];
				if (!no_league["info"].HasMember("title"))
					continue;
				char* szLeague = (char*)no_league["info"]["title"].GetString();

				if (no_league_data.IsArray() && no_league_data.Size() == 0)
					continue;
				if (!no_league.IsObject())
					goto L_FALSE;
				for (Value::MemberIterator itr3 = no_league_data.MemberBegin(); itr3 != no_league_data.MemberEnd(); ++itr3)
				{
					const char* szGame = itr3->name.GetString();
					Value& game = no_league_data[szGame];
					if (!game.HasMember("info"))
						continue;
					Value& game_info = game["info"];

					//if (!game_info["has_live_incidents"].GetBool())
					//	continue;
					int nID;
					char* szID = (char*)game_info["event_id"].GetString();
					sscanf_s(szID, "%d", &nID);

					if (!game_info.HasMember("team_home") || !game_info.HasMember("team_home") || !game_info.HasMember("event_info_line_one"))
						continue;
					string ht = GetTeamName((char*)game_info["team_home"].GetString());
					string at = GetTeamName((char*)game_info["team_away"].GetString());

					int hs = -1, as = -1, hs1 = -1, as1 = -1, hs2 = -1, as2 = -1;
					if (nGameCategory == GC_TENNIS || nGameCategory == GC_TABLE_TENNIS || nGameCategory == GC_VOLLEYBALL)
						GetScore((char*)game_info["event_info_line_one"].GetString(), hs1, as1);
					else
						GetScore((char*)game_info["event_info_line_one"].GetString(), hs, as);
					int ipt = 0;
					if (!game_info.HasMember("event_info_line_two"))
						continue;
					string stage = GetStageMin((char*)game_info["event_info_line_two"].GetString(), ipt);

					//OutDbgStrAA("\n%s:%s (%d:%d), %s(%d)", ht, at, hs, as, stage, ipt);
					if (IsExceptionGame((char*)ht.c_str(), (char*)at.c_str(), szLeague, (char*)stage.c_str()))
						continue;
					//					if (strstr(ht.c_str(), "Shabab") && strstr(at.c_str(), "That"))
					//					{
					//						ht = "That Ras";
					//						at = "Shabab AI Ordon";
					//						int temp;
					//						temp = hs;
					//						hs = as;
					//						as = temp;
					////						_asm int 3
					//					}
					CMainInfo MainInfo(nGameCategory, (char*)ht.c_str(), (char*)at.c_str(), SCORE(hs, hs1, hs2), SCORE(as, as1, as2), (char*)stage.c_str(), ipt, szLeague);
					::EnterCriticalSection(&g_csGameInfoArray);
					CGameInfo* pGameInfo2 = NULL;
					if (!GetGameInfoProc(&MainInfo, &pGameInfo2, nID))
					{
						::LeaveCriticalSection(&g_csGameInfoArray);
						continue;
					}
					CGameInfo* pGameInfo = new CGameInfo(pGameInfo2);
					//int n = pGameInfo2->GetGameDataNo(m_nBookMaker);
					//CGameData* pGameData = pGameInfo->m_aryGameData[n];
					CGameData* pGameData = pGameInfo->GetGameData(m_nBookMaker);
					::LeaveCriticalSection(&g_csGameInfoArray);

					if (pGameData == NULL)
						ASSERT(0);

					pGameData->RemoveAllOddsInfo();

					if (!game_info.IsObject())
						goto L_FALSE;
					for (Value::MemberIterator itr4 = game_info.MemberBegin(); itr4 != game_info.MemberEnd(); ++itr4)
					{
						const char* szItem = itr4->name.GetString();
						Value& item = game_info[szItem];
						if (item.GetType() != kObjectType || !item.HasMember("odd"))
							continue;
						GetOddVals(nGameCategory, pGameData, game_info, (char*)szItem);
					}

					::EnterCriticalSection(&g_csGameInfoArray);
					CopyGameDataProc(pGameData);
					::LeaveCriticalSection(&g_csGameInfoArray);
				}
			}
		}
		theFrame->PostMessageW(MSG_UPDATE, (WPARAM)m_nBookMaker);
	}
	catch (CException* e)
	{
		goto L_FALSE;
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("c:\\babibet.AnalyzeResData.json"), m_szText);
	return FALSE;
}

int CBetSite_babibet::GetGameCategory(char* szGameCategory)
{
	if (_strcmpi(szGameCategory, "soccer") == 0)
		return GC_FOOTBALL;
	if (_strcmpi(szGameCategory, "icehockey") == 0)
		return GC_ICE_HOCKEY;
	return CBetSite::GetGameCategory(szGameCategory);
}

CString CBetSite_babibet::GetGameCategory(int nGameCategory)
{
	if (nGameCategory == GC_FOOTBALL)
		return _T("soccer");
	if (nGameCategory == GC_ICE_HOCKEY)
		return _T("icehockey");
	CBetSite::GetGameCategory(nGameCategory);
	return _T("");
}

BOOL CBetSite_babibet::UpdateTree(int nIsExitThread)
{
	int nRet = UpdateTree_AllInfoDownAtOnce(nIsExitThread);
	CBetSite::UpdateTree(nIsExitThread);
	return nRet;
}

COddsInfo* CBetSite_babibet::MyAddOddInfo(CGameData* pGameData, int nOddsKind, float hVal1, float fOval1, float fOval2, float fOval3, UINT64 dwOddID1, UINT64 dwOddID2, UINT64 dwOddID3)
{
	COddsInfo* pOddsInfo = new COddsInfo;
	AddOddInfo(pOddsInfo, pGameData, nOddsKind, hVal1, fOval1, fOval2, fOval3, dwOddID1, dwOddID2, dwOddID3);
	return pOddsInfo;
}

void CBetSite_babibet::GetOddVals(int nGameCategory, CGameData* pGameData, Value& info, char* szItem)
{
	char* num = NULL;
	char szItem1[0x20];
	char szItem2[0x20];
	int rowno = 0, catno = 0, bigcol = 0, smallcol = 0;
	DWORD dwOddID1 = 0, dwOddID2 = 0, dwOddID3 = 0;
	if (strstr(szItem, "over_"))
	{
		num = szItem + 5;
		//if (nGameCategory == GC_VOLLEYBALL)
		//	_asm int 3
		sscanf_s(num, "%d_%d_%d_%d", &rowno, &catno, &bigcol, &smallcol);
		if ((nGameCategory == GC_FOOTBALL && (bigcol == 2)) || // smallcol = 2
			(nGameCategory == GC_VOLLEYBALL && (catno == 8 && smallcol == 3)) ||
			(nGameCategory == GC_BASKETBALL && (bigcol == 18)) ||
			(nGameCategory == GC_BASEBALL && (bigcol == 31 && smallcol == 3)) ||
			(nGameCategory == GC_HANDBALL && smallcol == 3) ||
			(nGameCategory == GC_TENNIS && (catno == 19)) ||
			(nGameCategory == GC_ICE_HOCKEY && (bigcol == 41)))
		{
			sprintf_s(szItem1, "under_%s", num);
			if (!info.HasMember(szItem1))
				return;

			string sHval, sHval1;
			float fOval, fOval1;
			if (!GetOddVal((char*)info[szItem]["odd"].GetString(), sHval, fOval))
				return;
			if (!GetOddVal((char*)info[szItem1]["odd"].GetString(), sHval1, fOval1))
				return;
			if (sHval == "" && sHval1 == "")
				return;
			if (sHval != "" && sHval1 != "")
				ASSERT(0);
			sscanf_s((char*)info[szItem]["id"].GetString(), "e_%d", &dwOddID1);
			sscanf_s((char*)info[szItem1]["id"].GetString(), "e_%d", &dwOddID2);
			char szHval[30];
			if (sHval != "")
				strcpy_s(szHval, sHval.c_str());
			else
				strcpy_s(szHval, sHval1.c_str());

			float hVal1 = 0, hVal2 = 0;
			if (strstr(szHval, "-") == NULL || strstr(szHval, "-") == szHval)
			{
				sscanf_s(szHval, "%f", &hVal1);
				MyAddOddInfo(pGameData, OI_GOAL_OU, hVal1, fOval, fOval1, 0, dwOddID1, dwOddID2, dwOddID3);
			}

		}
	}
	else if (strstr(szItem, "home_"))
	{
		num = szItem + 5;
		//if (nGameCategory == GC_VOLLEYBALL)
		//	_asm int 3
		sscanf_s(num, "%d_%d_%d_%d", &rowno, &catno, &bigcol, &smallcol);
		if ((nGameCategory == GC_FOOTBALL && bigcol == 2) || // smallcol = 1,3
			(nGameCategory == GC_VOLLEYBALL && ((catno == 15 && smallcol == 24) || (catno == 8 && smallcol == 2))) ||
			(nGameCategory == GC_BASKETBALL && bigcol == 18) ||
			(nGameCategory == GC_BASEBALL && (bigcol == 31 && (smallcol == 24 || smallcol == 2))) ||
			(nGameCategory == GC_HANDBALL && smallcol == 2) ||
			(nGameCategory == GC_TENNIS && ((catno == 15 && smallcol == 24) || (catno == 19 && smallcol == 2))) ||
			(nGameCategory == GC_ICE_HOCKEY && bigcol == 41))
		{
			sprintf_s(szItem1, "away_%s", num);
			if (!info.HasMember(szItem1))
				return;
			string sHval = "", sHval1 = "", sHval2 = "";
			float fOval = -1, fOval1 = -1, fOval2 = -1;
			if (!GetOddVal((char*)info[szItem]["odd"].GetString(), sHval, fOval))
				return;
			if (!GetOddVal((char*)info[szItem1]["odd"].GetString(), sHval1, fOval1))
				return;
			sscanf_s((char*)info[szItem]["id"].GetString(), "e_%d", &dwOddID1);
			sscanf_s((char*)info[szItem1]["id"].GetString(), "e_%d", &dwOddID2);

			sprintf_s(szItem2, "draw_%s", num);
			if (info.HasMember(szItem2))
			{
				GetOddVal((char*)info[szItem2]["odd"].GetString(), sHval2, fOval2);
				sscanf_s((char*)info[szItem2]["id"].GetString(), "e_%d", &dwOddID3);
			}

			if (sHval == "" && sHval1 == "" && sHval2 == "")
			{
				if (fOval == -1 && fOval1 == -1 && fOval2 == -1)
					return;

				float fDraw = (fOval2 == -1) ? 0 : fOval2;
				//if (strstr(pGameData->m_MainInfo.m_szHTName, "Shabab") && strstr(pGameData->m_MainInfo.m_szATName, "That"))
				//{
				//	float f = fOval;
				//	fOval = fOval1;
				//	fOval1 = f;
				//}
				MyAddOddInfo(pGameData, OI_MAIN, 0, fOval, fOval1, fDraw, dwOddID1, dwOddID2, dwOddID3);
			}
			else
			{
				if (sHval2 != "")
					ASSERT(0);
				char szHval[30];
				if (sHval != "")
					strcpy_s(szHval, sHval.c_str());
				else
					strcpy_s(szHval, sHval1.c_str());

				float hVal1 = 0, hVal2 = 0;
				if (strstr(szHval, "-") == NULL || strstr(szHval, "-") == szHval)
				{
					sscanf_s(szHval, "%f", &hVal1);
					if (sHval != "")
						hVal1 = -hVal1;
					MyAddOddInfo(pGameData, OI_HANDICAP, hVal1, fOval, fOval1, 0, dwOddID1, dwOddID2, dwOddID3);
				}
			}
		}
	}
	else if (strstr(szItem, "under_") || strstr(szItem, "away_") || strstr(szItem, "draw_"))
	{

	}
	else
	{
		ASSERT(0);
	}
}

void CBetSite_babibet::GetBalance(char* szAmount)
{//"1,034.14 CNY"
	if (strstr(szAmount, "") == 0)
		return;

	char *strToken = NULL;
	char *strToken1 = NULL;
	char *next_token = NULL;
	strToken = strtok_s(szAmount, " ", &next_token);
	strToken1 = strtok_s(NULL, ",", &next_token);
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
}

BOOL CBetSite_babibet::GetDELASESS(TCHAR* buffer)
{
	if (_tcscmp(m_szSession, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("DELASESS="), m_szSession))
			return FALSE;
	}
	return TRUE;
}

BOOL CBetSite_babibet::GetSV(TCHAR* buffer)
{
	TCHAR szSV[0x100];
	if (!GetSessionValue(buffer, _T("sv="), szSV))
		return FALSE;

	int n = _tcslen(szSV);
	int j = 0;
	TCHAR temp[3];
	temp[2] = 0;
	for (int i = 0; i <= n; i++)
	{
		if (szSV[i] != _T('%'))
			m_szSV[j] = szSV[i];
		else
		{
			ASSERT(i + 2 < n);
			temp[0] = szSV[i + 1];
			temp[1] = szSV[i + 2];
			DWORD b;
			_stscanf_s(temp, _T("%X"), &b);
			m_szSV[j] = (TCHAR)b;
			i += 2;
		}
		j++;
	}
	return TRUE;
}
