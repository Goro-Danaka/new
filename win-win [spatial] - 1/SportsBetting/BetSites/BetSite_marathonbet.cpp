#include "..\stdafx.h"
#include "BetSite_marathonbet.h"
#include "..\global.h"
#include "..\MainInfo.h"
#include "..\GameInfo.h"
#include "..\MainFrm.h"

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
			//SendRequestEx(REQ_LOGOUT);
			m_bExitThreadFlg = TRUE;
			m_nLogInStatus = LS_INIT;
		}
		else
		{
			if (!RefreshData())
				return 0;

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
				m_nLogInStatus = LS_SUCCESS;
				theFrame->m_wndSetting.PostMessageW(MSG_LOGIN_STATUS, (WPARAM)m_nBookMaker, (LPARAM)1);
			}
		}
	}

	if (m_bExitThreadFlg)
		return 0;
	return 0;
}

BOOL CBetSite_marathonbet::SendRequest(int nReqKind)
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

			TCHAR obj[0x40];
			_stprintf_s(obj, _T("/en/live/popular?pageAction=default&_=%s"), GetTimeStampLongW().GetBuffer());
			m_HttpFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, obj, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);
			m_HttpFile->AddRequestHeaders(_T("Accept:application/json, text/javascript, */*; q=0.01"));
			m_HttpFile->AddRequestHeaders(_T("Accept-Encoding:gzip, deflate, br"));
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
	//WriteResData(_T("c:\\marathonbet.AnalyzeResLive.xml"), m_szText);
	return TRUE;
L_FALSE:
	WriteResData(_T("c:\\marathonbet.AnalyzeResLive.xml"), m_szText);
	return FALSE;
}

BOOL CBetSite_marathonbet::CheckCatLabel(int index, char* szValue)
{
	if (!CheckLabel(index, "sport-category-label", szValue))
		return FALSE;
	//OutDbgStrAA("\n=================================");
	//OutDbgStrAA("\n%s", szValue);
	return TRUE;
}

BOOL CBetSite_marathonbet::CheckGameID(int index, char* szValue)
{
	char* szPos = m_szText + index;
	if (strstr(szPos, "data-available-event-treeId=\\\"") != szPos)
		return FALSE;
	char* p1 = strstr(szPos, "\\\"") + 2;
	char* p2 = strstr(p1 + 1, "\\\"");
	ASSERT(p2 != 0);
	int len = p2 - p1;
	memcpy(szValue, p1, len);
	szValue[len] = 0;
	return TRUE;
}

BOOL CBetSite_marathonbet::CheckLeagueLabel(int index, char* szValue)
{
	if (!CheckLabel(index, "category-label-link category-label", szValue, "</a>"))
		return FALSE;
	string s = szValue;
	replace(s, "<span class=\\\"nowrap\\\">", "");
	replace(s, "</span>", "");
	strcpy_s(szValue, 0x200, s.c_str());
	//OutDbgStrAA("\n%s", szValue);
	return TRUE;
}

BOOL CBetSite_marathonbet::CheckEventLabel(int index, char* szValue)
{
	if (!CheckLabel(index, "AVAIL_ET", szValue))
		return FALSE;
	//OutDbgStrAA("\n%s", szValue);
	return TRUE;
}

BOOL CBetSite_marathonbet::CheckScoreLabel(int index, char* szValue, int nGameCategory)
{
	if (nGameCategory != GC_TENNIS)
	{
		if (!CheckLabel(index, "AVAIL_SC", szValue))
			return FALSE;
	}
	else
	{
		if (!CheckLabel1(index, "AVAIL_SC", szValue))
			return FALSE;
	}
	string s = szValue;
	replace(s, "<span class=\\\"italic\\\">", "");
	replace(s, "<span class=\\\"result-description-part\\\">", "");
	strcpy_s(szValue, 0x200, s.c_str());
	char* p1 = strstr(szValue, "<img class=\\\"sport-icon-image\\\"");
	if (p1 != 0)
	{
		char* p2 = strstr(p1, "/>") + 2;
		int len = szValue + strlen(szValue) - p2;
		memcpy(p1, p2, len);
		p1[len] = 0;
	}
	s = szValue;
	replace(s, "</span>", "");
	strcpy_s(szValue, 0x200, s.c_str());
	//OutDbgStrAA("\n%s", szValue);
	return TRUE;
}

BOOL CBetSite_marathonbet::CheckTimeLabel(int index, char* szValue)
{
	if (!CheckLabel(index, "AVAIL_TD", szValue))
		return FALSE;
	//OutDbgStrAA("\n%s", szValue);
	return TRUE;
}

BOOL CBetSite_marathonbet::CheckLabel(int index, char* szLabel, char* szValue, char* szEndMark)
{
	char* szPos = m_szText + index;
	if (index + (int)strlen(szLabel) >= m_nTextSize)
		return FALSE;
	if (strstr(szPos, szLabel) != szPos)
		return FALSE;
	char* p1 = strstr(szPos, ">") + 1;
	char* p2 = strstr(szPos + 1, szEndMark);
	ASSERT(p2 != 0);
	int len = p2 - p1;
	memcpy(szValue, p1, len);
	szValue[len] = 0;
	return TRUE;
}

BOOL CBetSite_marathonbet::CheckLabel1(int index, char* szLabel, char* szValue, char* szEndMark)
{
	char* szPos = m_szText + index;
	if (index + (int)strlen(szLabel) >= m_nTextSize)
		return FALSE;
	if (strstr(szPos, szLabel) != szPos)
		return FALSE;
	char* p1 = strstr(szPos, ">") + 1;
	char* p2 = strstr(szPos, szEndMark);
	p2 = strstr(p2 + 1, szEndMark);
	ASSERT(p2 != 0);
	int len = p2 - p1;
	memcpy(szValue, p1, len);
	szValue[len] = 0;
	return TRUE;
}

void CBetSite_marathonbet::GetScore1(char* szValue, int nGameCategory, int& hs, int& hs1, int& hs2, int& as, int& as1, int& as2, string& stage)
{
	hs = 0; as = 0; hs1 = -1; as1 = -1; hs2 = -1; as2 = -1;
	stage = "";
	//OutDbgStrAA("\n%s", szValue);
	if (szValue[strlen(szValue) - 1] == ')')
		szValue[strlen(szValue) - 1] = 0;
	char* p1 = strchr(szValue, '(');
	if (p1)
	{
		char* p2 = strchr(p1 + 1, '(');
		if (p2)
		{
			//1:0 (6:0, 1:0)  (15:0
			if (nGameCategory == GC_TENNIS)
			{
				char* p3 = strchr(szValue, ' ');
				int len = p3 - szValue + 1;
				char* p4 = new char[len];
				memcpy(p4, szValue, len - 1);
				p4[len - 1] = 0;

				float f1 = 0, f2 = 0;
				Split(p4, ':', f1, f2);
				hs = (int)f1;
				as = (int)f2;

				char* p5 = strrchr(szValue, '(');

				int len1 = strlen(szValue) - (p5 - szValue + 1) + 1;

				char* p6 = new char[len1];
				memcpy(p6, p5 + 1, len1 - 1);
				p6[len1 - 1] = 0;

				string s1, s2;
				SplitS(p6, ":", s1, s2); //(30:A)
				if (s1 == "A" || s2 == "A")
				{
					//OutDbgStrAA("\n[Tennis] - %s", p6);
				}

				f1 = 0, f2 = 0;
				Split(p6, ':', f1, f2);
				hs2 = (int)f1;
				as2 = (int)f2;

				char* p7 = strrchr(szValue, ')');
				*p7 = 0;

				char* p8 = strrchr(szValue, ' ');
				if (p8[1] == '(')
					p8++;

				len1 = strlen(szValue) - (p8 - szValue + 1) + 1;

				char* p9 = new char[len1];
				memcpy(p9, p8 + 1, len1 - 1);
				p9[len1 - 1] = 0;

				f1 = 0, f2 = 0;
				Split(p9, ':', f1, f2);
				hs1 = (int)f1;
				as1 = (int)f2;

				delete p9;
				delete p6;
				delete p4;
			}
			else if (nGameCategory == GC_ICE_HOCKEY)
			{
				if (strstr(szValue, " OT "))
				{

				}
				else
				{
					//OutDbgStrAA("\n[%s] - %s", g_szGameCategory[nGameCategory], szValue);
					ASSERT(0);
				}
			}
			else
			{
				//OutDbgStrAA("\n[%s] - %s", g_szGameCategory[nGameCategory], szValue);
				ASSERT(0);
			}
		}
		else
		{
			if (nGameCategory == GC_TENNIS || nGameCategory == GC_TABLE_TENNIS || nGameCategory == GC_VOLLEYBALL ||
				nGameCategory == GC_E_SPORTS || nGameCategory == GC_SNOOKER || nGameCategory == GC_BADMINTON ||
				nGameCategory == GC_BOWLS)
			{
				//1:2 (1:2)
				//1:1 (11:21,  21:10,  14:9
				char* p3 = strchr(szValue, ' ');
				int len = p3 - szValue + 1;
				char* p4 = new char[len];
				memcpy(p4, szValue, len - 1);
				p4[len - 1] = 0;

				float f1 = 0, f2 = 0;
				Split(p4, ':', f1, f2);
				if (nGameCategory == GC_TENNIS)
				{
					hs = 0; as = 0;
					hs1 = (int)f1;
					as1 = (int)f2;
				}
				else
				{
					hs = (int)f1;
					as = (int)f2;
				}
				char* p5 = strrchr(szValue, ' ');
				if (p5[1] == '(')
					p5++;

				int len1 = strlen(szValue) - (p5 - szValue + 1) + 1;

				char* p6 = new char[len1];
				memcpy(p6, p5 + 1, len1 - 1);
				p6[len1 - 1] = 0;

				f1 = 0, f2 = 0;
				Split(p6, ':', f1, f2);
				if (nGameCategory == GC_TENNIS)
				{
					hs2 = (int)f1;
					as2 = (int)f2;
				}
				else
				{
					hs1 = (int)f1;
					as1 = (int)f2;
				}

				delete p6;
				delete p4;
			}
			else if (nGameCategory == GC_FOOTBALL || nGameCategory == GC_BASKETBALL || nGameCategory == GC_HANDBALL || 
				nGameCategory == GC_BASEBALL || nGameCategory == GC_FLOORBALL || nGameCategory == GC_ICE_HOCKEY || nGameCategory == GC_FIELD_HOCKEY)
			{
				//28:43 (4:25, 24:18)
				char* p3 = strchr(szValue, ' ');
				*p3 = 0;
				float f1 = 0, f2 = 0;
				Split(szValue, ':', f1, f2);
				hs = (int)f1;
				as = (int)f2;
			}
			else
			{
				//OutDbgStrAA("\n[%s] - %s", g_szGameCategory[nGameCategory], szValue);
				ASSERT(0);
			}
		}
	}
	else
	{
		//3:2
		float f1 = 0, f2 = 0;
		Split(szValue, ':', f1, f2);
		hs = (int)f1;
		as = (int)f2;
	}
}

void CBetSite_marathonbet::GetTimeStage(char* szValue, int nGameCategory, int& ipt, string& stage)
{
	stage = "";
	if (strchr(szValue, ':') != NULL && strchr(szValue, '<') == NULL)
	{
		float f1 = 0, f2 = 0;
		Split(szValue, ':', f1, f2);
		int min = (int)f1;
		int sec = (int)f2;
		ipt = min * 60 + sec;
	}
	else if (strstr(szValue, "HT") != NULL)
	{
		if (nGameCategory == GC_FOOTBALL)
			ipt = 45 * 60;
		//else
		//	OutDbgStrAA("\n[%s] - %s", g_szGameCategory[nGameCategory], szValue);
	}
	else if (strchr(szValue, '<') != NULL)
	{
		if (strstr(szValue, "additional-time"))
		{
			char* p1 = strchr(szValue, '<');
			*p1 = 0;
			char* p2 = new char[strlen(szValue)];
			int j = 0;
			for (int i = 0; i < (int)strlen(szValue) + 1; i++)
			{
				if (szValue[i] != ' ')
				{
					p2[j] = szValue[i];
					j++;
				}
			}
			int n;
			sscanf_s(p2, "%d", &n);
			ipt = n * 60;
		}
		else
			OutDbgStrAA("\n[%s] - %s", g_szGameCategory[nGameCategory], szValue);
	}
	//else
	//	OutDbgStrAA("\n[%s] - %s", g_szGameCategory[nGameCategory], szValue);
}

BOOL CBetSite_marathonbet::AnalyzeResData()
{
	if (m_szText == NULL || m_nTextSize < 20)
		goto L_FALSE;
	WriteResData(_T("c:\\marathonbet.AnalyzeResData.xml"), m_szText);
	{
		char szCat[0x200];
		char szValue[0x200];
		int a = 0;
		int nGameCategory = GC_FOOTBALL;
		char szLeague[MAX_LEAGUE_NAME];
		string ht, at;
		DWORD dwID = 0;
		int ipt = 0;
		string stage = "1H";
		int hs = 0, as = 0, hs1 = -1, as1 = -1, hs2 = -1, as2 = -1;

		int nGameCount = 0;
	
		for (int i = 0; i < m_nTextSize; i++)
		{
			if (CheckCatLabel(i, szCat))
			{
				dwID = 0;
				ipt = 0;
				stage = "1H";
				hs = 0; as = 0; hs1 = -1; as1 = -1; hs2 = -1; as2 = -1;

				nGameCategory = GetGameCategory(szCat);
			}
			else if (CheckLeagueLabel(i, szValue))
				strcpy_s(szLeague, szValue);
			else if (CheckGameID(i, szValue))
			{
				sscanf_s(szValue, "%d", &dwID);
			}
			else if (CheckEventLabel(i, szValue))
			{
				SplitS(szValue, " - ", ht, at);
			}
			else if (CheckScoreLabel(i, szValue, nGameCategory))
			{
				//if (nGameCategory == GC_VOLLEYBALL)
				//	_asm int 3
				GetScore1(szValue, nGameCategory, hs, hs1, hs2, as, as1, as2, stage);
				ipt = 0;
				if (nGameCategory == GC_TENNIS || nGameCategory == GC_TABLE_TENNIS || nGameCategory == GC_VOLLEYBALL ||
					nGameCategory == GC_E_SPORTS || nGameCategory == GC_SNOOKER || nGameCategory == GC_BADMINTON ||
					nGameCategory == GC_BASEBALL || nGameCategory == GC_BOWLS)
				{
					if (IsExceptionGame((char*)ht.c_str(), (char*)at.c_str(), szLeague, (char*)stage.c_str()))
						continue;
					nGameCount++;
					CMainInfo MainInfo(nGameCategory, (char*)ht.c_str(), (char*)at.c_str(), SCORE(hs, hs1, hs2), SCORE(as, as1, as2), (char*)stage.c_str(), ipt, szLeague);
					::EnterCriticalSection(&g_csGameInfoArray);
					CGameInfo* pGameInfo2 = NULL;
					if (!GetGameInfoProc(&MainInfo, &pGameInfo2, dwID))
					{
						::LeaveCriticalSection(&g_csGameInfoArray);
						continue;
					}
					CGameInfo* pGameInfo = new CGameInfo(pGameInfo2);
					CGameData* pGameData = pGameInfo->GetGameData(m_nBookMaker);

					if (pGameData == NULL)
						ASSERT(0);

					CopyGameDataProc(pGameData);
					::LeaveCriticalSection(&g_csGameInfoArray);
				}
			}
			else if (CheckTimeLabel(i, szValue))
			{
				//if (nGameCategory == GC_FOOTBALL || nGameCategory == GC_BASKETBALL || nGameCategory == GC_HANDBALL ||
				//		nGameCategory == GC_FLOORBALL || nGameCategory == GC_ICE_HOCKEY || nGameCategory == GC_FIELD_HOCKEY)
				GetTimeStage(szValue, nGameCategory, ipt, stage);
				if (IsExceptionGame((char*)ht.c_str(), (char*)at.c_str(), szLeague, (char*)stage.c_str()))
					continue;
				nGameCount++;
				CMainInfo MainInfo(nGameCategory, (char*)ht.c_str(), (char*)at.c_str(), SCORE(hs, hs1, hs2), SCORE(as, as1, as2), (char*)stage.c_str(), ipt, szLeague);
				::EnterCriticalSection(&g_csGameInfoArray);
				CGameInfo* pGameInfo2 = NULL;
				if (!GetGameInfoProc(&MainInfo, &pGameInfo2, dwID))
				{
					::LeaveCriticalSection(&g_csGameInfoArray);
					continue;
				}
				CGameInfo* pGameInfo = new CGameInfo(pGameInfo2);
				CGameData* pGameData = pGameInfo->GetGameData(m_nBookMaker);

				if (pGameData == NULL)
					ASSERT(0);

				CopyGameDataProc(pGameData);
				::LeaveCriticalSection(&g_csGameInfoArray);
			}
		}
		//OutDbgStrAA("\n [COUNT] **************** %d", nGameCount);
		theFrame->PostMessageW(MSG_UPDATE, (WPARAM)m_nBookMaker);
	}
	return TRUE;
L_FALSE:
	WriteResData(_T("c:\\marathonbet.AnalyzeResData.xml"), m_szText);
	return FALSE;
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

BOOL CBetSite_marathonbet::UpdateTree(int nIsExitThread)
{
	//OutDbgStrAA("\n [GAME1] ============================= %d", g_aryGameInfo.GetSize());
	int nRet = UpdateTree_AllInfoDownAtOnce(nIsExitThread);
	CBetSite::UpdateTree(nIsExitThread);
	return nRet;
}
