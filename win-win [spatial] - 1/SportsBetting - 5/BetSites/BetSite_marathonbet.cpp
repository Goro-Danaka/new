#include "..\stdafx.h"
#include "BetSite_marathonbet.h"
#include "..\global.h"

//https://www.marathonbet.com/en/

CBetSite_marathonbet::CBetSite_marathonbet()
{
	m_nBookMaker = SITE_marathonbet;

	_tcscpy_s(m_puid, _T(""));
}

CBetSite_marathonbet::~CBetSite_marathonbet()
{
}

int CBetSite_marathonbet::MainProc()
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
			OutDbgStrAA("\n[marathonbet]****************************");
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
			if (m_nLogInStatus == LS_INIT /*&& strcmp(m_szID, "") != 0 && strcmp(m_szPswd, "") != 0*/)
			{
				if (!SendRequestEx(REQ_LIVE))
				{
					OutDbgStrAA("\n[marathonbet]:MAIN ERROR - REQ_LIVE");
					return 0;
				}
				if (!AnalyzeResLive())
				{
					OutDbgStrAA("\n[marathonbet]:MAIN ERROR - AnalyzeResLive");
					return 0;
				}
				/*if (!SendRequestEx(REQ_LOGIN))
				{
					OutDbgStrAA("\n[marathonbet]:MAIN ERROR - REQ_LOGIN");
					return 0;
				}
				if (!AnalyzeResLogIn())
				{
					OutDbgStrAA("\n[marathonbet]:MAIN ERROR - AnalyzeResLogIn");
					return 0;
				}*/
			}
		}
	}

	if (m_bExitThreadFlg)
		return 0;
	bResult = SendRequestEx(REQ_DATA);
	AnalyzeResData();
	return 0;
}

BOOL CBetSite_marathonbet::SendRequestEx(int nReqKind)
{
	CHttpConnection* pServer = NULL;
	m_nTextSize = 0;

	DWORD dwRet;
	CString strServerName = _T("www.marathonbet.com");
	char strFormData[1000];
	strcpy_s(strFormData, "");

	try
	{
		pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

		if (nReqKind == REQ_LIVE)
		{
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/en/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			m_HttpFile->AddRequestHeaders(_T("Accept:text/html, application/xhtml+xml, */*"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate"));
		}
		else if (nReqKind == REQ_LOGIN)
		{
		}
		else if (nReqKind == REQ_LOGOUT)
		{
		}
		else if (nReqKind == REQ_DATA)
		{
			m_szText[0] = 0;

			pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
				_T("/en/liveupdate/homepage/?callback=jQuery11020801700738982357_1498398202100&markets=&available=&updated=1498398086568&oddsType=Decimal&siteStyle=SIMPLE&region=CN&_=1498398202101"),
				NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);
		}
		else if (nReqKind == REQ_BET)
		{
		}
		else if (nReqKind == REQ_BALANCE)
		{
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
			//if (!GetPUIDVal((TCHAR*)buffer))
			//	goto L_FALSE;
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

BOOL CBetSite_marathonbet::GetPUIDVal(TCHAR* buffer)
{
	if (_tcscmp(m_puid, _T("")) == 0)
	{
		if (!GetSessionValue(buffer, _T("puid="), m_puid))
			return FALSE;
	}
	return TRUE;
}

BOOL CBetSite_marathonbet::AnalyzeResLive()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	WriteResData(_T("d:\\marathonbet.AnalyzeResLive.xml"), m_szText);
	return TRUE;
L_FALSE:
	WriteResData(_T("d:\\marathonbet.AnalyzeResLive.xml"), m_szText);
	return FALSE;
}

BOOL CBetSite_marathonbet::AnalyzeResData()
{
	int len = strlen(m_szText);
	return TRUE;
}

BOOL CBetSite_marathonbet::AnalyzeResLogIn()
{
	return TRUE;

}

BOOL CBetSite_marathonbet::AnalyzeResLogOut()
{
	return TRUE;

}

BOOL CBetSite_marathonbet::AnalyzeResBalance()
{
	return TRUE;
}
