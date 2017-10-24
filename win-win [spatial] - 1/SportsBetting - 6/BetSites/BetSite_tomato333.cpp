#include "..\stdafx.h"
#include "BetSite_tomato333.h"
#include "..\global.h"

//#include <winhttp.h>
//#pragma comment(lib, "winhttp.lib")

//http://www.tomato333.com/euro/live-betting
//http://159p2sdr8810.asia.tomato333.com/web-root/restricted/default.aspx?loginname=caae91d8933095412f485a57012349a2&redirect=true

CBetSite_tomato333::CBetSite_tomato333()
{
	m_nBookMaker = SITE_tomato333;
	_tcscpy_s(m_szSession, _T(""));
}

CBetSite_tomato333::~CBetSite_tomato333()
{
}

int CBetSite_tomato333::MainProc()
{
	if (m_bExitThreadFlg)
		return 0;

	BOOL bResult = FALSE;
	if (m_nLogInStatus == LS_SUCCESS)
	{
		if (m_bDoLogOut)
		{
			bResult = SendRequestEx(REQ_LOGOUT);
			AnalyzeResLogOut();
		}
		else
		{
			bResult = SendRequestEx(REQ_DATA);
			AnalyzeResData();
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
			if (m_nLogInStatus == LS_INIT)
			{
				if (_tcscmp(m_szSession, _T("")) == 0)
					bResult = SendRequestEx(REQ_LIVE);
				bResult = SendRequestEx(REQ_LOGIN);
				AnalyzeResLogIn();
				if (m_nLogInStatus == LS_SUCCESS)
				{
					bResult = SendRequestEx(REQ_BALANCE);
					AnalyzeResBalance();
				}
			}
		}
	}
	return 0;
}

BOOL CBetSite_tomato333::SendRequestEx(int nReqKind)
{
	CHttpConnection* pServer = NULL;

	DWORD dwRet;
	CString strServerName = _T("www.tomato333.com");
	char strFormData[1000];

	try
	{

		if (nReqKind == REQ_LIVE)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/euro/live-betting"));

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));

			m_HttpFile->SendRequest(NULL, 0, NULL, 0);
		}
		else if (nReqKind == REQ_LOGIN)
		{
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/web/public/process-sign-in.aspx"));

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_HttpFile->AddRequestHeaders(_T("Content-Type:application/x-www-form-urlencoded"));
			m_HttpFile->AddRequestHeaders(_T("Referer://www.tomato333.com/en/euro"));
			m_pSession->SetCookie(_T("http://www.tomato333.com"), _T("ASP.NET_SessionId"), m_szSession);
			m_pSession->SetCookie(_T("http://www.tomato333.com"), _T("odnbv9"), m_szOdnb);
			m_pSession->SetCookie(_T("http://www.tomato333.com"), _T("tag-id"), _T("P1r70DJrg/YFvJqOm4wYTg=="));

			SYSTEMTIME time;
			GetSystemTime(&time);
			//sprintf_s(strFormData, "id=%s&password=%s&lang=en&tk=12280,12280,2,0,0,0,0,%.4d%.2d%.2d,1,3,0,4&5=1&type=form&tzDiff=0&HidCK=&tag=&fingerprint=", 
			//	m_szID, m_szPswd, time.wYear, time.wMonth, time.wDay);
			sprintf_s(strFormData, "id=hujiabao333&password=hjbtj177&lang=en&tk=4874,4874,21,1,0,0,0,20170814,1,0,0,4&5=1&type=form&tzDiff=0&HidCK=NAg99uEh&tag=P1r70DJrg/YFvJqOm4wYTg==&fingerprint=5ad90c12cd35381cbfcd3f8bbe6aed1a");
			m_HttpFile->SendRequest(NULL, 0, strFormData, strlen(strFormData));
		}
		else if (nReqKind == REQ_LOGOUT)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/logout.aspx"));

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("http://www.tomato333.com"), _T("ASP.NET_SessionId"), m_szSession);

			m_HttpFile->SendRequest(NULL, 0, NULL, 0);
		}
		else if (nReqKind == REQ_DATA)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/euro/live-betting/badminton"));

			m_HttpFile->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("http://www.tomato333.com"), _T("ASP.NET_SessionId"), m_szSession);

			m_HttpFile->SendRequest(NULL, 0, NULL, 0);
		}
		else if (nReqKind == REQ_BET)
		{
		}
		else if (nReqKind == REQ_BALANCE)
		{
			CString strServerName = _T("159p2sdr8810.asia.tomato333.com");
			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/web-root/restricted/top-module/action-data.aspx?action=update-credit-ticket"));

			m_HttpFile->AddRequestHeaders(_T("Accept:*/*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
			m_pSession->SetCookie(_T("http://159p2sdr8810.asia.tomato333.com"), _T("ASP.NET_SessionId"), m_szSession);

			m_HttpFile->SendRequest(NULL, 0, NULL, 0);
		}

		m_HttpFile->QueryInfoStatusCode(dwRet);
		if (dwRet == HTTP_STATUS_OK)
		{
			BYTE buffer[0x1000];
			DWORD len = 0x1000;
			if (nReqKind == REQ_LIVE)
			{
				m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
				GetSessionVal((TCHAR*)buffer);
				GetOdnbVal((TCHAR*)buffer);
			}
			if (nReqKind == REQ_LOGIN)
			{
				CString str = m_HttpFile->GetObject();
				m_HttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, (void *)buffer, &len);
				GetRedirectUrl((TCHAR*)buffer);
			}
			ReadResponse();
		}
		else
		{
			m_HttpFile->Close();
			delete m_HttpFile;
			m_HttpFile = NULL;

			pServer->Close();
			delete pServer;
			pServer = NULL;
			return FALSE;
		}

		m_HttpFile->Close();
		delete m_HttpFile;
		m_HttpFile = NULL;

		pServer->Close();
		delete pServer;
		pServer = NULL;
	}
	catch (CInternetException* e) {
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
		e->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CBetSite_tomato333::AnalyzeResData()
{
	if (m_szText == NULL || strlen(m_szText) < 20)
		return FALSE;
	//	WriteResData(_T("c:\\bwin.com.json"), m_szText);
	/*
	$P.onUpdate('od',[824,1,[[1,[[5,39101,[2177889,'H. S. Prannoy (IND)','Lin Yu Hsien (TPE)','5.067',10,'08/04/2017 15:00','',0,0,1,,{179:[0,0]}],[1,0,61846782,45,0,0,{179:[1,0,61846782,45,0,0,,,{1:45,2:45,3:15,4:15},0]},,{1:45,2:45,3:15,4:15},0],[[2,2,[]],[36942179,[1,0,1,1,200,0.00,2350807],[1.30,3.08]]],3],[5,39104,[2178181,'Ayako Sakuramoto (JPN) / Yukiko Takahata (JPN)','Joyce Choong Wai Chi (MAS) / Goh Yea Ching (MAS)','5.078',10,'08/04/2017 14:15','',0,0,1,,{179:[0,0]}],,[[2,2,[]],[36942218,[1,0,1,1,200,0.00,2351330],[1.70,2.13]]],3]],[],[]]]])
	*/
	char* s1 = strstr(m_szText, "$P.onUpdate");
	if (s1 == 0)
		return FALSE;
	char* s2 = strstr(s1, ";");
	int n = s2 - s1;
	char* szData = new char[n];
	memcpy(szData, s1, n);
	szData[n] = 0;

	delete szData;
	return TRUE;
}

void CBetSite_tomato333::AnalyzeResLogIn()
{
	if (m_szText == NULL || strlen(m_szText) < 20)
		return;

//	WriteResData(_T("c:\\tomato333.com.json"), m_szText);
	m_nLogInStatus = LS_SUCCESS;
}

void CBetSite_tomato333::AnalyzeResLogOut()
{

}

BOOL CBetSite_tomato333::AnalyzeResBalance()
{
	if (m_szText == NULL || strlen(m_szText) < 20)
		return FALSE;
	return TRUE;
}

void CBetSite_tomato333::GetRedirectUrl(TCHAR* buffer)
{

}

void CBetSite_tomato333::GetSessionVal(TCHAR* buffer)
{
	TCHAR* sessPos = NULL;
	TCHAR* commaPos = NULL;

	sessPos = _tcsstr(buffer, _T("ASP.NET_SessionId="));
	if (sessPos == NULL)
		return;
	commaPos = _tcschr(sessPos, _T(';'));
	if (commaPos == NULL)
		return;
	int len = commaPos - sessPos - 18;
	memcpy(m_szSession, sessPos + 18, len * 2);
	m_szSession[len] = 0;
}

void CBetSite_tomato333::GetOdnbVal(TCHAR* buffer)
{
	TCHAR* sessPos = NULL;
	TCHAR* commaPos = NULL;

	sessPos = _tcsstr(buffer, _T("odnbv9="));
	if (sessPos == NULL)
		return;
	commaPos = _tcschr(sessPos, _T(';'));
	if (commaPos == NULL)
		return;
	*commaPos = 0;
	int len = commaPos - sessPos - 7;
	memcpy(m_szOdnb, sessPos + 7, len * 2);
	m_szOdnb[len] = 0;
}
