#include "..\stdafx.h"
#include "BetSite_marathonbet.h"
#include "..\global.h"

//https://www.marathonbet.com/en/

CBetSite_marathonbet::CBetSite_marathonbet()
{
	m_nBookMaker = SITE_marathonbet;
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
			bResult = SendRequestEx(REQ_LOGOUT);
			AnalyzeResLogOut();
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

	if (m_bExitThreadFlg)
		return 0;
	bResult = SendRequestEx(REQ_DATA);
	AnalyzeResData();
	return 0;
}

BOOL CBetSite_marathonbet::SendRequestEx(int nReqKind)
{
	CHttpConnection* pServer = NULL;

	DWORD dwRet;
	CString strServerName = _T("lu.marathonbet.com");
	//char strFormData[1000];

	try
	{
		pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

		if (nReqKind == REQ_LOGIN)
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
				_T("/en/liveupdate/homepage/?callback=jQuery11020801700738982357_1498398202100&markets=&available=5609960%2C5608788%2C5608570%2C5610109%2C5588534%2C5608847%2C5609812%2C5605096%2C5608848%2C5608329%2C5610159%2C5608870%2C5608598%2C5609063%2C5609028%2C5609025%2C5609027%2C5609919%2C5609029%2C5609031%2C5609018%2C5609032%2C5609888%2C5609627%2C5609625%2C5609490%2C5608972%2C5608971%2C5608973%2C5608968%2C5609012%2C5609056%2C5609058%2C5606055%2C5606054%2C5606059%2C5606056%2C5606050%2C5609065%2C5609067%2C5609075%2C5564447%2C5609077%2C5609078%2C5609079%2C5609096%2C5609105%2C5609106%2C5609107%2C5609108%2C5609113%2C5609112%2C5609110%2C5609890%2C5609116%2C5609645%2C5564344%2C5609125%2C5609126%2C5609129%2C5609136%2C5609137%2C5609143%2C5605115%2C5605116%2C5609146%2C5609145%2C5609165%2C5609166%2C5609167%2C5609180%2C5609181%2C5609182%2C5609183%2C5609184%2C5605363%2C5609185%2C5609188%2C5609194%2C5608508%2C5609225%2C5609511%2C5609512%2C5610106%2C5609513%2C5607810%2C5607808%2C5609892%2C5610108%2C5610120%2C5609993%2C5603631%2C5608330%2C5607964%2C5607917%2C5607918%2C5607823%2C5608105%2C5608107%2C5608100%2C5609932%2C5610141%2C5609958%2C5609239%2C5609236%2C5607399%2C5608088%2C5608089%2C5608116%2C5608114%2C5608578%2C5608577%2C5607195%2C5609980%2C5607329%2C5608153%2C5608151%2C5608068%2C5608077%2C5608081%2C5605543%2C5564066%2C5604622%2C5604620%2C5608531&updated=1498398086568&oddsType=Decimal&siteStyle=SIMPLE&region=CN&_=1498398202101"),
				NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);
			m_HttpFile->SendRequest(NULL, 0, NULL, 0);
		}
		else if (nReqKind == REQ_BET)
		{
		}
		else if (nReqKind == REQ_BALANCE)
		{
		}

		m_HttpFile->QueryInfoStatusCode(dwRet);
		if (dwRet == HTTP_STATUS_OK)
			ReadResponse();
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

void CBetSite_marathonbet::AnalyzeResData()
{
	int len = strlen(m_szText);
}

void CBetSite_marathonbet::AnalyzeResLogIn()
{

}

void CBetSite_marathonbet::AnalyzeResLogOut()
{

}

void CBetSite_marathonbet::AnalyzeResBalance()
{

}
