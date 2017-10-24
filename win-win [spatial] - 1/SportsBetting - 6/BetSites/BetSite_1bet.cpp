#include "..\stdafx.h"
#include "BetSite_1bet.h"
#include "..\global.h"
#include "..\GameInfo.h"
#include "..\SportsBetting.h"
#include "..\MainFrm.h"

using namespace rapidjson;

CBetSite_1bet::CBetSite_1bet()
{
	m_nBookMaker = SITE_1bet;
}

CBetSite_1bet::~CBetSite_1bet()
{
}

int CBetSite_1bet::MainProc()
{
	SendRequestEx();
	AnalyzeResText();
	return 0;
}
void CBetSite_1bet::SendRequestEx()
{
	CHttpConnection* pServer = NULL;

	DWORD dwRet;
	CString strServerName = _T("mkt.1bet.com");
	char strFormData[1000];

//https://mkt.1bet.com/UnderOver_data.aspx?Market=l&Sport=1&DT=&RT=U&CT=07%2F18%2F2017+12%3A01%3A40&Game=0&OrderBy=0&OddsType=4&MainLeague=0&k467775097=v467775545&_=1500393712108
	try
	{
		if (m_bExitThreadFlg)
		{
			m_pSession->Close();
			return;
		}
		pServer = m_pSession->GetHttpConnection(strServerName, (INTERNET_PORT)INTERNET_DEFAULT_HTTPS_PORT);

//		m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,
//			_T("/Live_data.aspx?Market=l&Sport=0&RT=W&CT=&Game=0&OddsType=4&MainLeague=0&k1556832610=v1556833058"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);
		m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,
			_T("/UnderOver_data.aspx?Market=t&Sport=1&DT=&RT=U&CT=&Game=0&OrderBy=0&OddsType=4&MainLeague=0&k467775097=v467775545"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);
		m_pSession->SetCookie(_T("https://mkt.1bet.com"), _T("ASP.NET_SessionId"), _T("gh11q4efj2twp4tyudbhof0s"));
		m_HttpFile->SendRequest(NULL, 0, strFormData, strlen(strFormData));

		m_HttpFile->QueryInfoStatusCode(dwRet);

		if (!m_bExitThreadFlg)
			ReadResponse();

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
	}

	if (m_bExitThreadFlg)
		return;

	return;
}

void CBetSite_1bet::MakeArray(char* s, CStrArray& arr)
{
	char *strToken = NULL;
	char *next_token = NULL;
	strToken = strtok_s(s, ",", &next_token);
	strToken[strlen(strToken) - 1] = 0;
	string strVal = strToken + 1;
	arr.Add(strVal);
	while (strToken != NULL)
	{
		strToken = strtok_s(NULL, ",", &next_token);
		if (strToken == NULL)
			return;
		strToken[strlen(strToken) - 1] = 0;
		string strVal = strToken + 1;
		arr.Add(strVal);
	}
}

void CBetSite_1bet::Proc(char* s)
{
	CStrArray arrVal;
	MakeArray(s, arrVal);
	Parse(arrVal);
}

void CBetSite_1bet::Parse(CStrArray& arrVal)
{
	if (arrVal[0] != "")
	{
		OutDbgStrAA("%s/%s", arrVal[6].c_str(), arrVal[7].c_str());
	}
	else
	{
	}
}

void CBetSite_1bet::AnalyzeResText()
{
	char* p = m_szText;
	char* q = NULL;
	while (*p != 0)
	{
		if (strstr(p, "=['") == p)
		{
			q = NULL;
			for (int j = 0; j < (int)strlen(p); j++)
			{
				q = p + j;
				if (strstr(q, "];") == q)
				{
					char* s = new char[q - p - 1];
					memcpy(s, p + 2, q - p - 2);
					s[q - p - 2] = 0;
					Proc(s);
					delete s;
					break;
				}
			}
			if (q == NULL)
				ASSERT(0);
			p = q + 2;
		}
		else
			p++;
	}
}