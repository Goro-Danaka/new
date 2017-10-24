#pragma once

#include "MainInfo.h"
class CGameData;

typedef CArray<CGameData*, CGameData*> CArrayGameData;

enum
{
	PB_SUCCESS = 0,
	PB_DO_BET = 1,
	PB_DONT_BET,
};

struct BetStatus
{
	BOOL	bIsTotalBettingComplete;

	int		nOddsKind;
	float	m_hVal;

	int		nWasPartialBetting[3];
	int		nBookMakers[3];
	int		nStakes[3];
	float	fOdds[3];

	BetStatus()
	{
		init();
	}
	~BetStatus()
	{
	}

	void init()
	{
		bIsTotalBettingComplete = FALSE;
		nOddsKind = -1;
		m_hVal = -1;
		for (int i = 0; i < 3; i++)
		{
			nWasPartialBetting[i] = PB_DO_BET;
			nBookMakers[i] = -1;
			nStakes[i] = -1;
			fOdds[i] = -1;
		}
	}

	BetStatus& operator=(BetStatus& betStatus)
	{
		memcpy(this, &betStatus, sizeof(BetStatus));
		return *this;
	}

	BOOL operator!=(BetStatus& betStatus)
	{
		return (memcmp(this, &betStatus, sizeof(BetStatus)) != 0);
	}
};

class CGameInfo // contain some site's game data of one game. Corresponds for one node of tree
{
public:
	CGameInfo();
	CGameInfo(CGameInfo* pGameInfo);
	CGameInfo(CMainInfo* pMainInfo);
	~CGameInfo();

	HTREEITEM m_hTreeItem;
	HTREEITEM m_hTreeParentItem;

	CMainInfo m_MainInfo;

	int m_nSiteID;
	BetStatus m_BetState;

	CArrayGameData m_aryGameData;// count of sites that shows current game.

	BOOL operator!=(CGameInfo& GameInfo);
	int GetGameDataNo(int nBookMaker);
	CGameData* GetGameData(int nBookMaker);

	void UpdateHScore();
	void UpdateAScore();
	void UpdateTime();
};

