// BetEngine.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "BetEngine.h"
#include "Bet.h"
#include "..\SportsBetting\GameInfo.h"
#include "..\SportsBetting\GameData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CBetEngineApp

BEGIN_MESSAGE_MAP(CBetEngineApp, CWinApp)
END_MESSAGE_MAP()


// CBetEngineApp construction

CBetEngineApp::CBetEngineApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBetEngineApp object

CBetEngineApp theApp;


// CBetEngineApp initialization

BOOL CBetEngineApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

HANDLE g_hEngineCompleteEvent = INVALID_HANDLE_VALUE;

void SetEngineCompleteEvent(HANDLE hEngineCompleteEvent)
{
	g_hEngineCompleteEvent = hEngineCompleteEvent;
}

struct ODDS
{
	float odd1;
	float odd2;
};

struct STAKES
{
	int stake1;
	int stake2;
};

struct MAXODD
{
	float odd;
	int no;
};

struct MAXODDS
{
	MAXODD data[2];
};

BOOL WasBookmakerUsed(CGameInfo* pOneGame, CGameData* pOneSite)
{
	BOOL bCondition = FALSE;

	if (pOneGame->m_BetState.nWasPartialBetting[0] == PB_SUCCESS &&
		pOneGame->m_BetState.nBookMakers[0] == pOneSite->m_nBookMaker) return TRUE;
	if (pOneGame->m_BetState.nWasPartialBetting[1] == PB_SUCCESS &&
		pOneGame->m_BetState.nBookMakers[1] == pOneSite->m_nBookMaker) return TRUE;
	if (pOneGame->m_BetState.nWasPartialBetting[0] == PB_DONT_BET &&
		pOneGame->m_BetState.nBookMakers[0] == pOneSite->m_nBookMaker) return TRUE;
	if (pOneGame->m_BetState.nWasPartialBetting[1] == PB_DONT_BET &&
		pOneGame->m_BetState.nBookMakers[1] == pOneSite->m_nBookMaker) return TRUE;
	return bCondition;
}

BOOL CheckDifferentOddsTypeOrNot(CGameInfo* pOneGame, COddsInfo* pOneOdds)
{
	if (pOneGame->m_BetState.nOddsKind != pOneOdds->m_nOddsInfoKind)
		return TRUE;
	if (pOneGame->m_BetState.nOddsKind == OI_HANDICAP && pOneGame->m_BetState.m_hVal != pOneOdds->m_hVal1)
		return TRUE;
	if ((pOneGame->m_BetState.nOddsKind == OI_GOAL_OU || pOneGame->m_BetState.nOddsKind == OI_TEAM1_GOAL_OU ||
		pOneGame->m_BetState.nOddsKind == OI_TEAM2_GOAL_OU) && pOneGame->m_BetState.m_hVal != pOneOdds->m_hVal1)
		return TRUE;
	return FALSE;
}

MAXODD GetMaxComplementaryOdds(float* pO, int* pNo, int count)
{
	MAXODD ret;
	ret.odd = 0;
	for (int i = 0; i < count; i++)
		if (ret.odd < pO[i])
		{
			ret.odd = pO[i];
			ret.no = pNo[i];
		}
	return ret;
}

int GetComplementaryTwoStakes(ODDS Odds, int num, int stake)
{
	int ret;
	if (num == 0)
		ret = (int)(Odds.odd1 / Odds.odd2 * stake);
	else if (num == 1)
		ret = (int)(Odds.odd2 / Odds.odd1 * stake);
	return ret;
}

void GetComplementaryBetInfo(BetInfo* pBetInfo, CGameInfo* pGame, MAXODD MaxOdd, int nBetKind, int nMoney)
{
	for (int i = 0; i < 3; i++)
		pBetInfo->BetVal[i].bValid = FALSE;

	pBetInfo->nCategory = pGame->m_MainInfo.m_nGameCategory;
	strcpy_s(pBetInfo->szHTeam, MAX_TEAM_NAME, pGame->m_MainInfo.m_szHTName);
	strcpy_s(pBetInfo->szATeam, MAX_TEAM_NAME, pGame->m_MainInfo.m_szATName);
	strcpy_s(pBetInfo->szLeague, MAX_LEAGUE_NAME, pGame->m_MainInfo.m_szLeague);
	pBetInfo->HTScore = pGame->m_MainInfo.m_HTScore;
	pBetInfo->ATScore = pGame->m_MainInfo.m_ATScore;
	pBetInfo->nOddsKind = pGame->m_BetState.nOddsKind;
	pBetInfo->hVal1 = pGame->m_BetState.m_hVal;
	pBetInfo->BetVal[nBetKind].bValid = TRUE;
	pBetInfo->BetVal[nBetKind].dwGameID = pGame->m_aryGameData[MaxOdd.no]->m_nGameID;
	pBetInfo->BetVal[nBetKind].nBookMaker = pGame->m_aryGameData[MaxOdd.no]->m_nBookMaker;
	pBetInfo->BetVal[nBetKind].oVal = MaxOdd.odd;
	pBetInfo->BetVal[nBetKind].nBettingMoney = nMoney;
}

void GetBetInfoTwo(BetInfo* pBetInfo, CGameInfo* pGame, COddsInfo* pOdds, MAXODDS odds, STAKES stakes)
{
	int i;
	for (i = 0; i < 3; i++)
		pBetInfo->BetVal[i].bValid = FALSE;
	pBetInfo->nCategory = pGame->m_MainInfo.m_nGameCategory;
	strcpy_s(pBetInfo->szHTeam, MAX_TEAM_NAME, pGame->m_MainInfo.m_szHTName);
	strcpy_s(pBetInfo->szATeam, MAX_TEAM_NAME, pGame->m_MainInfo.m_szATName);
	strcpy_s(pBetInfo->szLeague, MAX_LEAGUE_NAME, pGame->m_MainInfo.m_szLeague);
	pBetInfo->HTScore = pGame->m_MainInfo.m_HTScore;
	pBetInfo->ATScore = pGame->m_MainInfo.m_ATScore;
	pBetInfo->nOddsKind = pOdds->m_nOddsInfoKind;
	pBetInfo->hVal1 = pOdds->m_hVal1;
	for (i = 0; i < 2; i++)
	{
		pBetInfo->BetVal[i].bValid = TRUE;
		pBetInfo->BetVal[i].dwGameID = pGame->m_aryGameData[odds.data[i].no]->m_nGameID;
		pBetInfo->BetVal[i].nBookMaker = pGame->m_aryGameData[odds.data[i].no]->m_nBookMaker;
		pBetInfo->BetVal[i].oVal = odds.data[i].odd;
	}
	pBetInfo->BetVal[0].nBettingMoney = stakes.stake1;
	pBetInfo->BetVal[1].nBettingMoney = stakes.stake2;
}

BOOL SpatialBettingCondition_2Odds(MAXODDS odds)
{
	BOOL bReturn = FALSE;
	if ((1 / odds.data[0].odd + 1 / odds.data[1].odd) <= 0.95)
		bReturn = TRUE;
	return bReturn;
}

BOOL SpatialSecondBettingCondition_2Odds(float odd1, float odd2)
{
	BOOL bReturn = FALSE;
	if ((1 / odd1 + 1 / odd2) <= 0.99)
		bReturn = TRUE;
	return bReturn;
}

BOOL DoComplementaryBetting(CArrayGameInfo& aryGameInfo, BetInfo* pBetInfo, float* fBalance)  
{
	BOOL bReturn = FALSE;
	MAXODD MaxOdd;
	ODDS odds;
	int resStake;
	int order;
	int nCount;

	for (int i = 0; i < aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pOneGame = (CGameInfo*)aryGameInfo[i];
		if (pOneGame == NULL) continue;
		order = 0;
		if ((pOneGame->m_BetState.nWasPartialBetting[0] == PB_DO_BET || pOneGame->m_BetState.nWasPartialBetting[0] == PB_DONT_BET) 
			&& pOneGame->m_BetState.nWasPartialBetting[1] == PB_SUCCESS)
			order = 1;
		if ((pOneGame->m_BetState.nWasPartialBetting[1] == PB_DO_BET || pOneGame->m_BetState.nWasPartialBetting[1] == PB_DONT_BET)
			&& pOneGame->m_BetState.nWasPartialBetting[0] == PB_SUCCESS)
			order = 2;
		if (order == 0) continue;
		nCount = 0;
		float* pOdd = new float[SITE_COUNT];
		int* pSiteNo = new int[SITE_COUNT];
		for (int j = 0; j < pOneGame->m_aryGameData.GetSize(); j++)
		{
			CGameData* pOneSite = (CGameData*)pOneGame->m_aryGameData[j];
			if (fBalance[pOneSite->m_nBookMaker] < 200) continue;
			if (WasBookmakerUsed(pOneGame, pOneSite)) continue;
			for (int k = 0; k < pOneSite->m_aryOddsInfo.GetSize(); k++)
			{
				COddsInfo* pOneOdds = (COddsInfo*)pOneSite->m_aryOddsInfo[k];
				if (CheckDifferentOddsTypeOrNot(pOneGame, pOneOdds)) continue;
				if(order == 1)
					pOdd[nCount] = pOneOdds->m_oVal1;
				else if(order == 2)
					pOdd[nCount] = pOneOdds->m_oVal2;
				pSiteNo[nCount] = j;
				nCount++;
				break;
			}
		}
		MaxOdd = GetMaxComplementaryOdds(pOdd, pSiteNo, nCount);
		delete pOdd;
		delete pSiteNo;
		if (nCount == 0)
			continue;
		if (order == 1)						
		{
			if (SpatialSecondBettingCondition_2Odds(MaxOdd.odd, pOneGame->m_BetState.fOdds[1]))
			{
				odds.odd1 = MaxOdd.odd;
				odds.odd2 = pOneGame->m_BetState.fOdds[1];
				resStake = GetComplementaryTwoStakes(odds, 1, pOneGame->m_BetState.nStakes[1]);
				GetComplementaryBetInfo(pBetInfo, pOneGame, MaxOdd, 0, resStake);
				return TRUE;
			}
/*			else if(MaxOdd.odd < pOneGame->m_BetState.fOdds[1])
			{
				odds.odd1 = MaxOdd.odd;
				odds.odd2 = pOneGame->m_BetState.fOdds[1];
				resStake = GetComplementaryTwoStakes(odds, 1, pOneGame->m_BetState.nStakes[1]);
				GetComplementaryBetInfo(pBetInfo, pOneGame, MaxOdd, 0, resStake);
				return TRUE;
			}*/
		}
		else if (order == 2)                  
		{
			if (SpatialSecondBettingCondition_2Odds(pOneGame->m_BetState.fOdds[0], MaxOdd.odd))
			{
				odds.odd1 = pOneGame->m_BetState.fOdds[0];
				odds.odd2 = MaxOdd.odd;
				resStake = GetComplementaryTwoStakes(odds, 0, pOneGame->m_BetState.nStakes[0]);
				GetComplementaryBetInfo(pBetInfo, pOneGame, MaxOdd, 1, resStake);
				return TRUE;
			}
/*			else if (pOneGame->m_BetState.fOdds[0] > MaxOdd.odd)
			{
				odds.odd1 = pOneGame->m_BetState.fOdds[0];
				odds.odd2 = MaxOdd.odd;
				resStake = GetComplementaryTwoStakes(odds, 0, pOneGame->m_BetState.nStakes[0]);
				GetComplementaryBetInfo(pBetInfo, pOneGame, MaxOdd, 1, resStake);
				return TRUE;
			}*/
		}
	}
	return bReturn;
}

BOOL IsRawGame(CGameInfo* pOneGame)
{
	BOOL bCondition = TRUE;
	if (pOneGame == NULL) return FALSE;
	if (pOneGame->m_aryGameData.GetSize() < 3) return FALSE;
	if (pOneGame->m_BetState.bIsTotalBettingComplete == TRUE) return FALSE;
	if (pOneGame->m_BetState.nWasPartialBetting[0] == PB_SUCCESS || pOneGame->m_BetState.nWasPartialBetting[1] == PB_SUCCESS)
		return FALSE;
	return bCondition;
}

MAXODDS GetMaxOdds(float* pO1, float* pO2, int* pNo, int count)
{
	MAXODDS ret;
	ret.data[0].odd = -1;
	if (count == 1)
		return ret;
	if (pO1[0] <= pO2[0])  
	{
		ret.data[0].odd = 0;
		for (int i = 0; i < count; i++)
			if (ret.data[0].odd < pO1[i])
			{
				ret.data[0].odd = pO1[i];
				ret.data[0].no = pNo[i];
			}
		ret.data[1].odd = 0;
		for (int i = 0; i < count; i++)
			if (ret.data[1].odd < pO2[i] && ret.data[0].no != pNo[i])
			{
				ret.data[1].odd = pO2[i];
				ret.data[1].no = pNo[i];
			}
	}
	else if(pO1[0] > pO2[0])
	{
		ret.data[1].odd = 0;
		for (int i = 0; i < count; i++)
			if (ret.data[1].odd < pO2[i])
			{
				ret.data[1].odd = pO2[i];
				ret.data[1].no = pNo[i];
			}
		ret.data[0].odd = 0;
		for (int i = 0; i < count; i++)
			if (ret.data[0].odd < pO1[i] && ret.data[1].no != pNo[i])
			{
				ret.data[0].odd = pO1[i];
				ret.data[0].no = pNo[i]; 
			}
	}
	return ret;
}

int SelectStake(float fOdd)
{
	if (fOdd <= 1.0) return 0;
	if (fOdd > 10.0) return 10;
	if (fOdd > 1.0 && fOdd <= 3.65)
	{
		if (fOdd <= 1.15) return 90;
		if (fOdd <= 1.25) return 85;
		if (fOdd <= 1.35) return 75;
		if (fOdd <= 1.45) return 70;
		if (fOdd <= 1.55) return 65;
		if (fOdd <= 1.75) return 60;
		if (fOdd <= 1.95) return 55;
		if (fOdd <= 2.15) return 50;
		if (fOdd <= 2.35) return 45;
		if (fOdd <= 2.65) return 40;
		if (fOdd <= 3.05) return 35;
		if (fOdd <= 3.05) return 35;
		if (fOdd <= 3.65) return 30;
	}
	else
	{
		if (fOdd <= 4.45) return 25;
		if (fOdd <= 5.75) return 20;
		if (fOdd <= 7.55) return 15;
		if (fOdd <= 10.0) return 10;
	}
}

STAKES GetTwoStakes(MAXODDS odds)
{
	STAKES stakes;
	if (odds.data[0].odd >= odds.data[1].odd)
	{
		stakes.stake1 = SelectStake(odds.data[0].odd);
		stakes.stake2 = (int)(odds.data[0].odd / odds.data[1].odd * stakes.stake1);
	}
	else
	{
		stakes.stake2 = SelectStake(odds.data[1].odd);
		stakes.stake1 = (int)(odds.data[1].odd / odds.data[0].odd * stakes.stake2);
	}
	return stakes;
}

int GetIncompleteBettingCount(CArrayGameInfo& aryGameInfo)
{
	int nIncompleteCnt = 0;
	for (int m = 0; m < aryGameInfo.GetSize(); m++)
	{
		CGameInfo* pOneGame = (CGameInfo*)aryGameInfo[m];
		if(pOneGame->m_BetState.nWasPartialBetting[0] == PB_SUCCESS && (pOneGame->m_BetState.nWasPartialBetting[1] == PB_DO_BET
			|| pOneGame->m_BetState.nWasPartialBetting[1] == PB_DONT_BET) || (pOneGame->m_BetState.nWasPartialBetting[0] == PB_DO_BET 
			|| pOneGame->m_BetState.nWasPartialBetting[0] == PB_DONT_BET) && pOneGame->m_BetState.nWasPartialBetting[1] == PB_SUCCESS)
			nIncompleteCnt++;
	}
	return nIncompleteCnt;
}

BOOL IsEffectiveTime(CGameInfo* pOneGame)
{
	BOOL bFlag = FALSE;

	switch (pOneGame->m_MainInfo.m_nGameCategory)
	{
	case GC_FOOTBALL:           
		if (pOneGame->m_MainInfo.m_nTime <= 4800) 
			bFlag = TRUE;
		break;
	case GC_BEACH_SOCCER:      
		if (pOneGame->m_MainInfo.m_nTime <= 1560 && pOneGame->m_MainInfo.m_nTime > 0)
			bFlag = TRUE;
		break;
	case GC_BASKETBALL:     
		if (pOneGame->m_MainInfo.m_nTime <= 1800 && pOneGame->m_MainInfo.m_nTime > 0)
			bFlag = TRUE;
		break;
	case GC_FIELD_HOCKEY:		
	case GC_ICE_HOCKEY:			
	case GC_FLOORBALL:			
	case GC_HANDBALL:			
		if (pOneGame->m_MainInfo.m_nTime <= 3000 && pOneGame->m_MainInfo.m_nTime > 0)
			bFlag = TRUE;
		break;
	case GC_WATER_POLO:			
		if (pOneGame->m_MainInfo.m_nTime <= 1320 && pOneGame->m_MainInfo.m_nTime > 0)
			bFlag = TRUE;
		break;
	case GC_TENNIS:			
	case GC_VOLLEYBALL:				
	case GC_BEACH_VOLLEYBALL:		
	case GC_TABLE_TENNIS:			
	case GC_BADMINTON:				   
		if (pOneGame->m_MainInfo.m_HTScore.nScore != -1 && pOneGame->m_MainInfo.m_ATScore.nScore != -1)
			if (pOneGame->m_MainInfo.m_HTScore.nScore == 0 && pOneGame->m_MainInfo.m_ATScore.nScore == 0 ||
				pOneGame->m_MainInfo.m_HTScore.nScore == 0 && pOneGame->m_MainInfo.m_ATScore.nScore == 1 ||
				pOneGame->m_MainInfo.m_HTScore.nScore == 1 && pOneGame->m_MainInfo.m_ATScore.nScore == 0 ||
				pOneGame->m_MainInfo.m_HTScore.nScore == 1 && pOneGame->m_MainInfo.m_ATScore.nScore == 1)
				bFlag = TRUE;
		break;
	case GC_BASEBALL:			
	case GC_BOWLS:                      
	case GC_CRICKET:			
	case GC_DARTS:
	case GC_E_SPORTS:			
	case GC_RUGBY_LEAGUE:
	case GC_RUGBY_SEVENS:
	case GC_SNOOKER:			
		break;
	default:
		break;
	}
	return bFlag;
}

BOOL DoSpatialBetting(CArrayGameInfo& aryGameInfo, BetInfo* pBetInfo, float* fBalance)
{
	BOOL bReturn = FALSE;
	STAKES stakes;
	MAXODDS odds;

	if(GetIncompleteBettingCount(aryGameInfo) > 5) return bReturn;
	for (int i = 0; i < aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pOneGame = (CGameInfo*)aryGameInfo[i];
		if (!IsEffectiveTime(pOneGame)) continue;
		if (!IsRawGame(pOneGame)) continue;
		for (int j = 0; j < pOneGame->m_aryGameData.GetSize() - 1; j++)
		{
			CGameData* pOneSite = (CGameData*)pOneGame->m_aryGameData[j];
			if (fBalance[pOneSite->m_nBookMaker] < 200) continue;
			for (int k = 0; k < pOneSite->m_aryOddsInfo.GetSize(); k++)
			{
				COddsInfo* pOneOdds = (COddsInfo*)pOneSite->m_aryOddsInfo[k];
				if (pOneOdds->m_nOddsInfoKind == OI_MAIN) continue;
				int nCount = 0;
				float* pOdd1 = new float[SITE_COUNT];
				float* pOdd2 = new float[SITE_COUNT];
				int* pSiteNo = new int[SITE_COUNT];
				pOdd1[nCount] = pOneOdds->m_oVal1;
				pOdd2[nCount] = pOneOdds->m_oVal2;
				pSiteNo[nCount] = j;
				nCount++;
				for (int l = j + 1; l < pOneGame->m_aryGameData.GetSize(); l++)
				{
					CGameData* pOneSiteNext = (CGameData*)pOneGame->m_aryGameData[l];
					if (fBalance[pOneSiteNext->m_nBookMaker] < 200) continue;
					for (int m = 0; m < pOneSiteNext->m_aryOddsInfo.GetSize(); m++)
					{
						COddsInfo* pOneOddsNext = (COddsInfo*)pOneSiteNext->m_aryOddsInfo[m];
						if (*pOneOdds == *pOneOddsNext)
						{
							pOdd1[nCount] = pOneOddsNext->m_oVal1;
							pOdd2[nCount] = pOneOddsNext->m_oVal2;
							pSiteNo[nCount] = l;
							nCount++;
							break;
						}
					}
				}
				odds = GetMaxOdds(pOdd1, pOdd2, pSiteNo, nCount);
				delete pOdd1;
				delete pOdd2;
				delete pSiteNo;
				if (odds.data[0].odd == -1) continue;
				if (SpatialBettingCondition_2Odds(odds))
				{
					stakes = GetTwoStakes(odds);
					GetBetInfoTwo(pBetInfo, pOneGame, pOneOdds, odds, stakes);
					return TRUE;
				}
			}
		}
	}
	return bReturn;
}

BOOL GetSpatialBetInfo(CArrayGameInfo& aryGameInfo, BetInfo* pBetInfo, float* fBalance, BOOL bExitBetThreadFlg, BOOL& bBettingCompleted)
{
	BOOL bReturn;
	bBettingCompleted = FALSE;
	if (GetIncompleteBettingCount(aryGameInfo) == 0 && bExitBetThreadFlg == TRUE)
	{
		bBettingCompleted = TRUE;
		return FALSE;
	}

	bReturn = DoComplementaryBetting(aryGameInfo, pBetInfo, fBalance);
	if (bReturn)
		return bReturn;
	else if(!bExitBetThreadFlg)
		bReturn = DoSpatialBetting(aryGameInfo, pBetInfo, fBalance);
	return bReturn;
}

BOOL SetGameInfoArray(CArrayGameInfo& aryGameInfo, BetInfo* pBetInfo, int nBetStrategy, float* fBalance, BOOL bExitBetThreadFlg, BOOL& bBettingCompleted)
{
	return GetSpatialBetInfo(aryGameInfo, pBetInfo, fBalance, bExitBetThreadFlg, bBettingCompleted);
}
