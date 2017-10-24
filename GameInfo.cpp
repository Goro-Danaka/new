#include "stdafx.h"
#include "GameInfo.h"
#include "GameData.h"
#include "global.h"

CGameInfo::CGameInfo()
{
	m_hTreeItem = NULL;
	m_hTreeParentItem = NULL;
//	m_bValid = TRUE;

	m_aryGameData.RemoveAll();
}

CGameInfo::CGameInfo(CGameInfo* pGameInfo)
{
	if (pGameInfo == NULL)
		return;

	m_hTreeItem = pGameInfo->m_hTreeItem;
	m_hTreeParentItem = pGameInfo->m_hTreeParentItem;
//	m_bValid = pGameInfo->m_bValid;

	m_MainInfo = pGameInfo->m_MainInfo;
	m_BetState = pGameInfo->m_BetState;

	for (int i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
	{
		CGameData* pNewGameData = new CGameData(pGameInfo->m_aryGameData[i]);
		pNewGameData->m_pGameInfo = this;
		m_aryGameData.Add(pNewGameData);
	}
}

CGameInfo::CGameInfo(CMainInfo* pMainInfo)
{
	m_MainInfo = *pMainInfo;
//	m_bValid = TRUE;
	CStringA master;
	if (!g_mapName.Lookup(pMainInfo->m_szhtName, master))
		SetMasterName(pMainInfo->m_szhtName);
	else
		strcpy_s(m_MainInfo.m_szhtName, MAX_TEAM_NAME, master.GetBuffer());

	if (!g_mapName.Lookup(pMainInfo->m_szatName, master))
		SetMasterName(pMainInfo->m_szatName);
	else
		strcpy_s(m_MainInfo.m_szatName, MAX_TEAM_NAME, master.GetBuffer());

	m_aryGameData.RemoveAll();
}

CGameInfo::~CGameInfo()
{
	//for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	//{
	//	CGameInfo* pGameInfo3 = g_aryGameInfo[i];
	//	if (this == pGameInfo3)
	//		_asm int 3
	//}

	for (int i = 0; i < m_aryGameData.GetSize(); i++)
	{
		delete m_aryGameData[i];
		m_aryGameData[i] = NULL;
	}
	m_aryGameData.RemoveAll();
}

BOOL CGameInfo::operator!=(CGameInfo& GameInfo)
{
	BOOL bSameOrSimilar = FALSE;
	if (m_MainInfo.m_nGameCategory != GameInfo.m_MainInfo.m_nGameCategory ||
		::IsEqualGameName(m_MainInfo, GameInfo.m_MainInfo, 1, 1) == FALSE)
		return TRUE;

	if (m_aryGameData.GetSize() != GameInfo.m_aryGameData.GetSize())
		return TRUE;
	for (int i = 0; i < m_aryGameData.GetSize(); i++)
	{
		if (*m_aryGameData[i] != *GameInfo.m_aryGameData[i])
			return TRUE;
	}
	return FALSE;
}

int CGameInfo::GetGameDataNo(int nBookMaker)
{
	for (int i = 0; i < m_aryGameData.GetSize(); i++)
	{
		if (m_aryGameData[i]->m_nBookMaker == nBookMaker)
			return i;
	}
	return -1;
}

CGameData* CGameInfo::GetGameData(int nBookMaker)
{
	for (int i = 0; i < m_aryGameData.GetSize(); i++)
	{
		if (m_aryGameData[i]->m_nBookMaker == nBookMaker)
			return m_aryGameData[i];
	}
	return NULL;
}

void CGameInfo::UpdateHScore()
{
	int nScore = -1, nScore1 = -1, nScore2 = -1;
	for (int i = 0; i < m_aryGameData.GetSize(); i++)
	{
		if (nScore < m_aryGameData[i]->m_MainInfo.m_HTScore.nScore)
			nScore = m_aryGameData[i]->m_MainInfo.m_HTScore.nScore;
		if (nScore1 < m_aryGameData[i]->m_MainInfo.m_HTScore.nScore1)
			nScore1 = m_aryGameData[i]->m_MainInfo.m_HTScore.nScore1;
		if (nScore2 < m_aryGameData[i]->m_MainInfo.m_HTScore.nScore2)
			nScore2 = m_aryGameData[i]->m_MainInfo.m_HTScore.nScore2;
	}
	m_MainInfo.m_HTScore.nScore = nScore;
	m_MainInfo.m_HTScore.nScore1 = nScore1;
	m_MainInfo.m_HTScore.nScore2 = nScore2;
}

void CGameInfo::UpdateAScore()
{
	int nScore = -1, nScore1 = -1, nScore2 = -1;
	for (int i = 0; i < m_aryGameData.GetSize(); i++)
	{
		if (nScore < m_aryGameData[i]->m_MainInfo.m_ATScore.nScore)
			nScore = m_aryGameData[i]->m_MainInfo.m_ATScore.nScore;
		if (nScore1 < m_aryGameData[i]->m_MainInfo.m_ATScore.nScore1)
			nScore1 = m_aryGameData[i]->m_MainInfo.m_ATScore.nScore1;
		if (nScore2 < m_aryGameData[i]->m_MainInfo.m_ATScore.nScore2)
			nScore2 = m_aryGameData[i]->m_MainInfo.m_ATScore.nScore2;
	}
	m_MainInfo.m_ATScore.nScore = nScore;
	m_MainInfo.m_ATScore.nScore1 = nScore1;
	m_MainInfo.m_ATScore.nScore2 = nScore2;
}

void CGameInfo::UpdateTime()
{
	int nTime = 0;
	for (int i = 0; i < m_aryGameData.GetSize(); i++)
	{
		if (nTime < m_aryGameData[i]->m_MainInfo.m_nTime)
			nTime = m_aryGameData[i]->m_MainInfo.m_nTime;
	}
	m_MainInfo.m_nTime = nTime;
}
