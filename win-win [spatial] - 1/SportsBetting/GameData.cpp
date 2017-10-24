#include "stdafx.h"
#include "GameData.h"

CGameData::CGameData()
{
	m_nBookMaker = 0;
	m_nGameID = 0;

	GetLocalTime(&m_nLastTime);
	GetLocalTime(&m_nBeforeTime);

	m_bDownInfo = TRUE;
	m_bHAChanged = FALSE;
	m_bBettingDone = FALSE;

	m_pGameInfo = NULL;
	m_aryOddsInfo.RemoveAll();
}

CGameData::CGameData(CGameData* pGameData)
{
	if (pGameData == NULL)
		return;

	m_nBookMaker = pGameData->m_nBookMaker;
	m_nGameID = pGameData->m_nGameID;
	m_nLastTime = pGameData->m_nLastTime;
	m_nBeforeTime = pGameData->m_nBeforeTime;

	m_MainInfo = pGameData->m_MainInfo;

	m_bDownInfo = pGameData->m_bDownInfo;
	m_bHAChanged = pGameData->m_bHAChanged;

	m_pGameInfo = NULL;
	if (m_aryOddsInfo.GetSize() != 0)
		ASSERT(0);
	m_aryOddsInfo.RemoveAll();
	for (int i = 0; i < pGameData->m_aryOddsInfo.GetSize(); i++)
	{
		COddsInfo* pNewOddsInfo = new COddsInfo();
		COddsInfo* pOdds1 = pGameData->m_aryOddsInfo[i];

		memcpy(pNewOddsInfo, pOdds1, sizeof(COddsInfo));
		m_aryOddsInfo.Add(pNewOddsInfo);
	}
	return;
}

CGameData::CGameData(CGameInfo* pGameInfo, int nBookMaker, int bDownInfo, BOOL bHAChanged)
{
	m_bDownInfo = bDownInfo;
	m_nBookMaker = nBookMaker;
	m_nGameID = 0;
	m_pGameInfo = pGameInfo;
	m_bHAChanged = bHAChanged;
	GetLocalTime(&m_nLastTime);
	GetLocalTime(&m_nBeforeTime);

	for (int i = 0; i < m_aryOddsInfo.GetSize(); i++)
	{
		if (m_aryOddsInfo[i])
		{
			delete m_aryOddsInfo[i];
			m_aryOddsInfo[i] = NULL;
		}
	}
	m_aryOddsInfo.RemoveAll();
}

CGameData::~CGameData()
{
	int iKey;
	CGameData* pGameData1;
	::EnterCriticalSection(&g_csGameInfoArray);
	if (g_site[m_nBookMaker])
	{
		POSITION pos = g_site[m_nBookMaker]->m_mapGameID.GetStartPosition();
		while (pos != NULL)
		{
			g_site[m_nBookMaker]->m_mapGameID.GetNextAssoc(pos, iKey, pGameData1);
			if (this == pGameData1)
			{
				//_asm int 3
				g_site[m_nBookMaker]->m_mapGameID.RemoveKey(iKey);
				//OutDbgStrAA("\nMAP KEY DELETE %d", iKey);
				break;
			}
		}
	}
	RemoveAllOddsInfo();
	::LeaveCriticalSection(&g_csGameInfoArray);
}

void CGameData::Update(CMainInfo* info, int bDownInfo)
{
	m_MainInfo.m_HTScore = info->m_HTScore;
	m_MainInfo.m_ATScore = info->m_ATScore;

	m_pGameInfo->UpdateHScore();
	m_pGameInfo->UpdateAScore();

	m_bDownInfo = bDownInfo;
	
	if (strcmp(info->m_szStage, "") != 0)
		strcpy_s(m_pGameInfo->m_MainInfo.m_szStage, 0x20, info->m_szStage);

	m_MainInfo.m_nTime = info->m_nTime;
	m_pGameInfo->UpdateTime();
	if (m_pGameInfo->m_MainInfo.m_nTime < info->m_nTime)
		m_pGameInfo->m_MainInfo.m_nTime = info->m_nTime;

	if (strcmp(info->m_szStage, "") != 0)
		strcpy_s(m_MainInfo.m_szStage, 0x20, info->m_szStage);
}

void CGameData::RemoveAllOddsInfo()
{
	for (int i = 0; i < m_aryOddsInfo.GetSize(); i++)
	{
		if (m_aryOddsInfo[i])
		{
			delete m_aryOddsInfo[i];
			m_aryOddsInfo[i] = NULL;
		}
	}
	m_aryOddsInfo.RemoveAll();
}

COddsInfo* CGameData::GetExistOdds(int nOddsInfoKind, float fOddsVal1)
{
	for (int i = 0; i < m_aryOddsInfo.GetSize(); i++)
	{
		COddsInfo* pOI = (COddsInfo*)m_aryOddsInfo[i];
		if (pOI->m_nOddsInfoKind != nOddsInfoKind)
			continue;
		if (nOddsInfoKind == OI_MAIN || nOddsInfoKind == OI_GOAL_OE)
			return pOI;

		if (pOI->m_hVal1 == fOddsVal1)
			return pOI;
	}
	return NULL;
}

BOOL CGameData::operator!=(CGameData& GameData)
{
	if (m_aryOddsInfo.GetSize() != GameData.m_aryOddsInfo.GetSize())
		return TRUE;
	for (int i = 0; i < m_aryOddsInfo.GetSize(); i++)
	{
		if (*m_aryOddsInfo[i] != *GameData.m_aryOddsInfo[i])
			return TRUE;
	}
	return FALSE;
}

void CGameData::operator=(CGameData& GameData)
{
	m_MainInfo = GameData.m_MainInfo;
	m_nBookMaker = GameData.m_nBookMaker;
	m_nGameID = GameData.m_nGameID;
	m_bDownInfo = GameData.m_bDownInfo;
	m_bHAChanged = GameData.m_bHAChanged;
	m_bBettingDone = GameData.m_bBettingDone;
	m_nLastTime = GameData.m_nLastTime;
	m_nBeforeTime = GameData.m_nBeforeTime;

	int n = m_aryOddsInfo.GetSize();
	//if (n == 0)
	{
		for (int i = 0; i < m_aryOddsInfo.GetSize(); i++)
		{
			COddsInfo* pOdds1 = m_aryOddsInfo[i];
			delete m_aryOddsInfo[i];
			m_aryOddsInfo[i] = NULL;
		}
		m_aryOddsInfo.RemoveAll();

//		for (int i = 0; i < GameData.m_aryOddsInfo.GetSize(); i++)
//			m_aryOddsInfo.Add(GameData.m_aryOddsInfo[i]);

		for (int i = 0; i < GameData.m_aryOddsInfo.GetSize(); i++)
		{
			COddsInfo* pNewOddsInfo = new COddsInfo();
			COddsInfo* pOdds1 = GameData.m_aryOddsInfo[i];

			memcpy(pNewOddsInfo, GameData.m_aryOddsInfo[i], sizeof(COddsInfo));
			m_aryOddsInfo.Add(pNewOddsInfo);
		}

	}
}