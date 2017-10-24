#include "..\stdafx.h"
#include "BetSite_bet365.h"
#include "..\global.h"
#include "..\GameInfo.h"
#include "..\SportsBetting.h"
#include "..\MainFrm.h"

//https://www.bet365.com/#/IP/

CBetSite_bet365::CBetSite_bet365() : CBetSite()
{
	m_nBookMaker = SITE_bet365;

	_tcscpy_s(m_wszPstk, _T(""));
	_tcscpy_s(m_wszAps, _T(""));
	_tcscpy_s(m_szSession, _T(""));
	_tcscpy_s(m_wszRmbs, _T(""));
	_tcscpy_s(m_wszUrlPremlp, _T(""));
	_tcscpy_s(m_wszUrlPshudlp, _T(""));
	_tcscpy_s(m_wszPremlpID, _T(""));
	_tcscpy_s(m_wszPshudlpID, _T(""));

	m_nPacketNoPremlp = 0;
	m_nPacketNoPshudlp = 0;

}

CBetSite_bet365::~CBetSite_bet365()
{
}

int CBetSite_bet365::MainProc()
{
	m_nTextSize = 0;
	if (m_bExitThreadFlg)
		return 0;

	if (m_nLogInStatus == LS_SUCCESS)
	{
		if (m_bDoLogOut)
		{
			if (!SendRequestEx(REQ_LOGOUT))
			{
				OutDbgStrAA("\n[bet365]:MAIN ERROR - REQ_LOGOUT");
				return 0;
			}
			if (!AnalyzeResLogOut())
			{
				OutDbgStrAA("\n[bet365]:MAIN ERROR - AnalyzeResLogOut");
				return 0;
			}
		}
		else
		{
			if (!SendRequestEx(REQ_DATA))
			{
				OutDbgStrAA("\n[bet365]:MAIN ERROR - REQ_DATA");
				return 0;
			}
			if (!AnalyzeResData())
			{
				OutDbgStrAA("\n[bet365]:MAIN ERROR - AnalyzeResData");
				return 0;
			}
			OutDbgStrAA("\n[bet365]****************************");
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
			if (m_nLogInStatus == LS_INIT/* && strcmp(m_szID, "") != 0 && strcmp(m_szPswd, "") != 0*/)
			{
				if (!SendRequestEx(REQ_LIVE))
				{
					OutDbgStrAA("\n[bet365]:MAIN ERROR - REQ_LIVE");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE1))
				{
					OutDbgStrAA("\n[bet365]:MAIN ERROR - REQ_LIVE1");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE2))
				{
					OutDbgStrAA("\n[bet365]:MAIN ERROR - REQ_LIVE2");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE3))
				{
					OutDbgStrAA("\n[bet365]:MAIN ERROR - REQ_LIVE3");
					return 0;
				}
				if (!AnalyzeResLive())
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - AnalyzeResLive");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE4))
				{
					OutDbgStrAA("\n[bet365]:MAIN ERROR - REQ_LIVE4");
					return 0;
				}
				if (!AnalyzeResLive4())
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - AnalyzeResLive4");
					return 0;
				}
				if (!SendRequestEx(REQ_LIVE5))
				{
					OutDbgStrAA("\n[bet365]:MAIN ERROR - REQ_LIVE5");
					return 0;
				}
				if (!AnalyzeResLive5())
				{
					OutDbgStrAA("\n[188bet]:MAIN ERROR - AnalyzeResLive5");
					return 0;
				}
				//if (!SendRequestEx(REQ_LOGIN))
				//{
				//	OutDbgStrAA("\n[bet365]:MAIN ERROR - REQ_LOGIN");
				//	return 0;
				//}
				//if (!AnalyzeResLogIn())
				//{
				//	OutDbgStrAA("\n[188bet]:MAIN ERROR - AnalyzeResLogIn");
				//	return 0;
				//}
				m_nLogInStatus = LS_SUCCESS;
			}
		}
	}

	if (m_bExitThreadFlg)
		return 0;
	return 0;
}

BOOL CBetSite_bet365::GetUrls(char* szText)
{
	char szPremlp[0x40];
	char szPshudlp[0x40];

	char* szUrl1 = strstr(szText, "https://premlp-");
	if (szUrl1 == NULL)
		return FALSE;
	char* szUrl2 = strstr(szUrl1, ",");
	int len = szUrl2 - szUrl1 - 8;
	memcpy(szPremlp, szUrl1 + 8, len);
	szPremlp[len] = 0;

	szUrl1 = strstr(szText, "https://pshudlp");
	if (szUrl1 == NULL)
		return FALSE;
	szUrl2 = strstr(szUrl1, ",");
	len = szUrl2 - szUrl1 - 8;
	memcpy(szPshudlp, szUrl1 + 8, len);
	szPshudlp[len] = 0;

	ToUnicode(szPremlp, m_wszUrlPremlp, CP_ACP);
	ToUnicode(szPshudlp, m_wszUrlPshudlp, CP_ACP);
	return TRUE;
}

BOOL CBetSite_bet365::AnalyzeResLive()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	//WriteResData(_T("d:\\bet365.AnalyzeResLive.xml"), m_szText);
	if (!GetUrls(m_szText))
		goto L_FALSE;
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\bet365.AnalyzeResLive.xml"), m_szText);
	return FALSE;
}

void CBetSite_bet365::GetPremlpDataPacket(int nPacketNoPremlp, char* strFormData)
{
	//if (nPacketNoPremlp == 1)
	//	sprintf_s(strFormData, 4000, "method=22&topic=CONFIG_1_3,HL_L1_Z3_C1_W5,HM_L1_Z3_C1_W5,HR_L1_Z3_C1_W5&s=%d&clientid=%s", m_nPacketNoPremlp, m_szPremlpID);
	//else
	//	sprintf_s(strFormData, 4000, "method=1&s=%d&topic=&clientid=%s", m_nPacketNoPremlp, m_szPremlpID);
	sprintf_s(strFormData, 4000, "method=1&s=%d&topic=&clientid=%s", m_nPacketNoPremlp, m_szPremlpID);
}

BOOL CBetSite_bet365::SendRequest(int nReqKind)
{
	CHttpConnection* pServer = NULL;
	m_nTextSize = 0;

	DWORD dwRet;
	char strFormData[4000];
	strcpy_s(strFormData, "");

	CString strServerName;
	
	try
	{

		if (nReqKind == REQ_LIVE)
		{
			strServerName = _T("www.bet365.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LIVE1)
		{
			strServerName = _T("www.bet365.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LIVE2)
		{
			strServerName = _T("www.bet365.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/home/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("aps03"), m_wszAps);
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("pstk"), m_wszPstk);
		}
		else if (nReqKind == REQ_LIVE3)
		{
			strServerName = _T("www.bet365.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("aps03"), m_wszAps);
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("pstk"), m_wszPstk);
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("session"), m_szSession);
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("rmbs"), m_wszRmbs);
		}
		else if (nReqKind == REQ_LIVE4)
		{
			strServerName = m_wszUrlPremlp;
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/pow2/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:text/plain"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.bet365.com/?"));
			m_HttpFile->AddRequestHeaders(_T("Origin:https://www.bet365.com"));

			ToAscii(m_wszPstk, m_szPstk, CP_ACP);
			sprintf_s(strFormData, "method=0&transporttimeout=20&type=F&topic=__time,S_%s", m_szPstk);
		}
		else if (nReqKind == REQ_LIVE5)
		{
			strServerName = m_wszUrlPshudlp;
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/pow2/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:text/plain"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.bet365.com/?"));
			m_HttpFile->AddRequestHeaders(_T("Origin:https://www.bet365.com"));

			ToAscii(m_wszPstk, m_szPstk, CP_ACP);
			sprintf_s(strFormData, "method=0&transporttimeout=20&type=F&topic=__time,S_%s", m_szPstk);
		}
		else if (nReqKind == REQ_DATA)
		{
			strServerName = m_wszUrlPremlp;
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			CString strObj;
			strObj.Format(_T("/pow2/?id=%s"), m_wszPremlpID);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:text/plain"));
			m_HttpFile->AddRequestHeaders(_T("Referer:https://www.bet365.com/?"));
			m_HttpFile->AddRequestHeaders(_T("Origin:https://www.bet365.com"));

			GetPremlpDataPacket(m_nPacketNoPremlp, strFormData);
			m_nPacketNoPremlp++;
		}
		else if (nReqKind == REQ_LOGIN)
		{
			strServerName = _T("member.bet365.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/Members/lp/default.aspx"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("aps03"), m_wszAps);
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("pstk"), m_wszPstk);

			ToAscii(m_wszPstk, m_szPstk, CP_ACP);
			sprintf_s(strFormData, "txtUserName=%s&txtPassword=%s&txtType=85&txtTKN=%s&txtHPFV=NOTSET NOTSET&txtScreenSize=1920 x 1080&txtFlashVersion=NOTSET&IS=1", m_szID, m_szPswd, m_szPstk);
		}
		else if (nReqKind == REQ_LOGOUT)
		{
			strServerName = _T("member.bet365.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/Members/lp/logout/default.aspx"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("aps03"), m_wszAps);
			m_pSession->SetCookie(_T("https://www.bet365.com"), _T("pstk"), m_wszPstk);
		}

		m_HttpFile->SendRequest(NULL, 0, strFormData, strlen(strFormData));

		m_HttpFile->QueryInfoStatusCode(dwRet);
		if (dwRet != HTTP_STATUS_OK)
			goto L_FALSE;

		BYTE buffer[0x1000];
		DWORD len = 0x1000;
		if (nReqKind == REQ_LIVE1)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetPstk((TCHAR*)buffer))
				goto L_FALSE;
		}
		else if (nReqKind == REQ_LIVE2)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetRmbs((TCHAR*)buffer))
				goto L_FALSE;
		}
		else if (nReqKind == REQ_LIVE3)
		{
			m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
			if (!GetPstk((TCHAR*)buffer))
				goto L_FALSE;
			if (!GetRmbs((TCHAR*)buffer))
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

BOOL CBetSite_bet365::AnalyzeResData()
{
	if (m_szText == NULL || m_nTextSize < 20)
		return FALSE;
	TCHAR szFile[MAX_PATH];
	_stprintf_s(szFile, _T("d:\\bet365.AnalyzeResData_%.4d.txt"), m_nPacketNoPremlp);
	WriteResData(szFile, m_szText);
	return TRUE;
}

BOOL CBetSite_bet365::AnalyzeResLogIn()
{
	if (m_szText == NULL || m_nTextSize < 20)
		return FALSE;
	return TRUE;
}

BOOL CBetSite_bet365::AnalyzeResLogOut()
{
	if (m_szText == NULL || m_nTextSize < 20)
		return FALSE;
	return TRUE;
}

BOOL CBetSite_bet365::AnalyzeResBalance()
{
	if (m_szText == NULL || m_nTextSize < 20)
		return FALSE;
	return TRUE;
}

BOOL CBetSite_bet365::AnalyzeResLive1()
{
	if (m_szText == NULL || m_nTextSize < 20)
		return FALSE;
	return TRUE;
}

BOOL CBetSite_bet365::AnalyzeResLive2()
{
	if (m_szText == NULL || m_nTextSize < 20)
		return FALSE;
	return TRUE;
}

BOOL CBetSite_bet365::AnalyzeResLive4()
{
	if (m_szText == NULL || m_nTextSize < 20)
		return FALSE;

	strcpy_s(m_szPremlpID, m_szText + 4);
	ToUnicode(m_szPremlpID, m_wszPremlpID, CP_ACP);
	return TRUE;
}

BOOL CBetSite_bet365::AnalyzeResLive5()
{
	if (m_szText == NULL || m_nTextSize < 20)
		return FALSE;
	strcpy_s(m_szPshudlpID, m_szText + 4);
	ToUnicode(m_szPshudlpID, m_wszPshudlpID, CP_ACP);
	return TRUE;
}

BOOL CBetSite_bet365::GetPstk(TCHAR* buffer)
{
	if (_tcscmp(m_wszPstk, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("pstk="), m_wszPstk))
			return FALSE;
	}
	if (_tcscmp(m_wszAps, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("aps03="), m_wszAps))
			return FALSE;
	}
	return TRUE;
}

BOOL CBetSite_bet365::GetRmbs(TCHAR* buffer)
{
	if (_tcscmp(m_szSession, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("session="), m_szSession))
			return FALSE;
	}
	if (_tcscmp(m_wszRmbs, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("rmbs="), m_wszRmbs))
			return FALSE;
	}
	return TRUE;
}