#pragma once

#include "OddsInfo.h"
#include "global.h"
#include "GameInfo.h"
#include "MainInfo.h"

typedef CArray<COddsInfo*, COddsInfo*> CArrayOddsInfo;


class CGameData
{
public:
	CGameData();
	CGameData(CGameData* pGameData);
	CGameData(CGameInfo* pGameInfo, int nBookMaker, int bDownInfo, BOOL bHAChanged);
	~CGameData();

	int m_nBookMaker;
	int m_nGameID;

	CMainInfo m_MainInfo;

	BOOL m_bDownInfo;
	BOOL m_bHAChanged;
	BOOL m_bBettingDone;

	SYSTEMTIME m_nLastTime;
	SYSTEMTIME m_nBeforeTime;

	CGameInfo* m_pGameInfo;
	CArrayOddsInfo m_aryOddsInfo;

	void Update(CMainInfo* info, int bDownInfo);
	void RemoveAllOddsInfo();

	COddsInfo* GetExistOdds(int nOddsInfoKind, float fBetVal1 = 0);

	BOOL operator!=(CGameData& GameData);
	void operator=(CGameData& GameData);

};

