
// SportsBettingView.cpp : implementation of the CSportsBettingView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SportsBetting.h"
#endif

#include "SportsBettingDoc.h"
#include "SportsBettingView.h"
#include "GameData.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define COL_WIDTH 60

// CSportsBettingView

IMPLEMENT_DYNCREATE(CSportsBettingView, CView)

BEGIN_MESSAGE_MAP(CSportsBettingView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CSportsBettingView construction/destruction

CSportsBettingView::CSportsBettingView()
{
	// TODO: add construction code here

}

CSportsBettingView::~CSportsBettingView()
{
}

BOOL CSportsBettingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void CSportsBettingView::OnDraw(CDC* pDC)
{
}

void CSportsBettingView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	theFrame->m_pMainView = this;
	theFrame->m_Grid = &m_Grid;
	InitListControl();
}

void CSportsBettingView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
}

// CSportsBettingView diagnostics

#ifdef _DEBUG
void CSportsBettingView::AssertValid() const
{
	CView::AssertValid();
}

void CSportsBettingView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSportsBettingDoc* CSportsBettingView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSportsBettingDoc)));
	return (CSportsBettingDoc*)m_pDocument;
}
#endif //_DEBUG


// CSportsBettingView message handlers


int CSportsBettingView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_Grid.Create(CRect(0, 0, 0, 0), this, IDC_LIST);
	return 0;
}


void CSportsBettingView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (::IsWindow(m_Grid.m_hWnd))
		m_Grid.MoveWindow(0, 0, cx, cy, TRUE);
}

void CSportsBettingView::InitListControl()
{
	m_nFixCols = 2;
	m_nFixRows = 2;

	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_Grid.SetFixedColumnSelection(TRUE);
	m_Grid.SetFixedRowSelection(TRUE);

	m_Grid.SetEditable(FALSE);
	return;
}


void CSportsBettingView::DeleteAllItems()
{
	m_Grid.DeleteAllItems();
}

BOOL CSportsBettingView::IsTableStructureChanged(CGameInfo* pGameInfo)
{
	if (theFrame->m_pCurSelGameInfo == NULL)
		return TRUE;

	if (*pGameInfo != *theFrame->m_pCurSelGameInfo)
		return TRUE;
	return FALSE;
}

int CSportsBettingView::GetSameCaption(CString strLabel, int nStartColNo, int nCount)
{
	for (int i = 0; i < nCount; i++)
	{
		CString str = m_Grid.GetItemText(1, nStartColNo + i);
		if (str == strLabel)
		{
			return nStartColNo + i;
		}
	}
	return -1;
}

TCHAR g_szHTeam[MAX_TEAM_NAME], g_szATeam[MAX_TEAM_NAME], g_szGame[0x200], g_szStage1[0x40], g_szLeague[MAX_LEAGUE_NAME];

void CSportsBettingView::UpdateList(CGameInfo* pGameInfo, BOOL bNewTreeItem)
{
	if (bNewTreeItem || IsTableStructureChanged(pGameInfo))
	{
		m_Grid.DeleteAllText();
		//m_Grid.DeleteAllColor();
		if (theFrame->m_pCurSelGameInfo)
			delete theFrame->m_pCurSelGameInfo;
		theFrame->m_pCurSelGameInfo = new CGameInfo(pGameInfo);
	}

	//if (strstr(pGameInfo->m_szHTName, "Akademiya") != 0)
	//	_asm int 3
	m_nCols = 80;
	m_nRows = 2 + 3 * pGameInfo->m_aryGameData.GetSize();

	m_Grid.SetRowCount(m_nRows);
	m_Grid.SetColumnCount(m_nCols);

	GV_ITEM Item;
	Item.mask = GVIF_TEXT;

	int i, j;
	int nColNo = 0;
	int nStartColNo = 0;

	m_Grid.MergeCells(0, 0, 1, 0);
	CString strTime;
	TCHAR szStage[MAX_STAGE_NAME];
	string sStage = GetStage(pGameInfo->m_MainInfo.m_szStage, pGameInfo->m_MainInfo.m_nTime);
	::ToUnicode(sStage.c_str(), szStage, CP_ACP);
	strTime.Format(_T("%.2d:%.2d(%s)"), pGameInfo->m_MainInfo.m_nTime / 60, pGameInfo->m_MainInfo.m_nTime % 60, szStage);
	m_Grid.SetItemText(0, 0, strTime);
	//BookMaker
	m_Grid.SetColumnWidth(nColNo, 100);
	for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
	{
		CGameData* pGameData = pGameInfo->m_aryGameData[i];
		if (!pGameData)
			ASSERT(0);

		TCHAR szBookMaker[MAX_BOOKMAKER];
		m_Grid.MergeCells(2 + i * 3, nColNo, 4 + i * 3, nColNo);
		::ToUnicode(g_szSiteName[pGameData->m_nBookMaker], szBookMaker, CP_ACP);
		m_Grid.SetItemText(2 + i * 3, nColNo, szBookMaker);
	}
	nColNo++;

	//Comment
	m_Grid.SetColumnWidth(nColNo, 90);
	m_Grid.MergeCells(0, nColNo, 1, nColNo);
	m_Grid.SetItemText(0, nColNo, _T("Comment"));

	for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
	{
		CGameData* pGameData = pGameInfo->m_aryGameData[i];
		if (!pGameData)
			ASSERT(0);

		m_Grid.SetItemText(2 + i * 3, nColNo, _T("ht/over/odd"));
		m_Grid.SetItemText(3 + i * 3, nColNo, _T("at/under/even"));
		m_Grid.SetItemText(4 + i * 3, nColNo, _T("draw"));
	}
	nColNo++;

	//Time
	m_Grid.SetColumnWidth(nColNo, 140);
	m_Grid.MergeCells(0, nColNo, 1, nColNo);
	m_Grid.SetItemText(0, nColNo, _T("Time"));
	for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
	{
		CGameData* pGameData = pGameInfo->m_aryGameData[i];
		if (!pGameData)
			ASSERT(0);

		DWORD dwPeriod = GetRelativeTime(pGameData->m_nLastTime, pGameData->m_nBeforeTime);
		TCHAR szDuration[0x20];
		_stprintf_s(szDuration, _T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d (%ds)"), pGameData->m_nLastTime.wYear, pGameData->m_nLastTime.wMonth, pGameData->m_nLastTime.wDay,
			pGameData->m_nLastTime.wHour, pGameData->m_nLastTime.wMinute, pGameData->m_nLastTime.wSecond, dwPeriod);

		m_Grid.MergeCells(2 + i * 3, nColNo, 4 + i * 3, nColNo);
		m_Grid.SetItemText(2 + i * 3, nColNo, szDuration);
	}
	nColNo++;

	//Main
	m_Grid.SetColumnWidth(nColNo, COL_WIDTH);
	m_Grid.MergeCells(0, nColNo, 1, nColNo);
	m_Grid.SetItemText(0, nColNo, _T("Main"));

	for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
	{
		CGameData* pGameData = pGameInfo->m_aryGameData[i];
		if (!pGameData)
			ASSERT(0);

		//data
		for (j = 0; j < pGameData->m_aryOddsInfo.GetSize(); j++)
		{
			COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[j];
			if (pOddsInfo == NULL)
				continue;
			if (pOddsInfo->m_nOddsInfoKind == OI_MAIN)
			{
				m_Grid.SetItemFloat(2 + i * 3, nColNo, pOddsInfo->m_oVal1);
				m_Grid.SetItemFloat(3 + i * 3, nColNo, pOddsInfo->m_oVal2);
				if ((DWORD)pOddsInfo->fDraw != 0)
					m_Grid.SetItemFloat(4 + i * 3, nColNo, pOddsInfo->fDraw);
				break;
			}
		}
	}
	nColNo++;

	//Handicap
	//if (pGameInfo->GetCameCategory() != GC_CRICKET && pGameInfo->GetCameCategory() != GC_E_SPORTS && pGameInfo->GetCameCategory() != GC_SNOOKER)
	{
		nStartColNo = nColNo;
		int nHandicapCount = 0;
		for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
		{
			CGameData* pGameData = pGameInfo->m_aryGameData[i];
			if (!pGameData)
				ASSERT(0);
			for (j = 0; j < pGameData->m_aryOddsInfo.GetSize(); j++)
			{
				COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo.ElementAt(j);
				if (pOddsInfo == NULL)
					continue;
				if (pOddsInfo->m_nOddsInfoKind != OI_HANDICAP)
					continue;
				nHandicapCount++;
			}
		}

		if (nHandicapCount != 0)
		{
			for (i = 0; i < nHandicapCount; i++)
				m_Grid.SetColumnWidth(nColNo + i, COL_WIDTH);

			int nHandicapNo = 0;
			for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
			{
				CGameData* pGameData = pGameInfo->m_aryGameData[i];
				if (!pGameData)
					ASSERT(0);
				for (int jj = 0; jj < pGameData->m_aryOddsInfo.GetSize(); jj++)
				{
					COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[jj];
					if (pOddsInfo == NULL)
						continue;
					if (pOddsInfo->m_nOddsInfoKind != OI_HANDICAP)
						continue;
					//(+1.5, -1.5)
					CString strLabel = MakeHandicapLabel(pOddsInfo);
					int nPos = GetSameCaption(strLabel, nStartColNo, nColNo - nStartColNo);
					if (nPos == -1)
					{
						m_Grid.SetItemText(1, nColNo, strLabel);

						m_Grid.SetItemFloat(2 + i * 3, nColNo, pOddsInfo->m_oVal1);
						m_Grid.SetItemFloat(3 + i * 3, nColNo, pOddsInfo->m_oVal2);
						nColNo++;
					}
					else
					{
						m_Grid.SetItemFloat(2 + i * 3, nPos, pOddsInfo->m_oVal1);
						m_Grid.SetItemFloat(3 + i * 3, nPos, pOddsInfo->m_oVal2);
					}
				}
			}
			if (nColNo - nStartColNo >= 2)
				m_Grid.MergeCells(0, nStartColNo, 0, nColNo - 1);
			m_Grid.SetItemText(0, nStartColNo, _T("Handicap"));
		}
		//nColNo += (nHandicapCount * 2);
	}

	int nGoalsCount;
	//Goals(Over/Under)
	//if (pGameInfo->GetCameCategory() != GC_CRICKET && pGameInfo->GetCameCategory() != GC_DARTS && pGameInfo->GetCameCategory() != GC_SNOOKER)
	{
		nStartColNo = nColNo;
		nGoalsCount = 0;
		for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
		{
			CGameData* pGameData = pGameInfo->m_aryGameData[i];
			if (!pGameData)
				ASSERT(0);
			for (j = 0; j < pGameData->m_aryOddsInfo.GetSize(); j++)
			{
				COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo.ElementAt(j);

				if (pOddsInfo == NULL)
					continue;
				if (pOddsInfo->m_nOddsInfoKind != OI_GOAL_OU)
					continue;
				nGoalsCount++;
			}
		}

		if (nGoalsCount != 0)
		{
			for (i = 0; i < nGoalsCount; i++)
				m_Grid.SetColumnWidth(nColNo + i, COL_WIDTH);

			int nGoalsNo = 0;
			for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
			{
				CGameData* pGameData = pGameInfo->m_aryGameData[i];
				if (!pGameData)
					ASSERT(0);
				for (int jj = 0; jj < pGameData->m_aryOddsInfo.GetSize(); jj++)
				{
					COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[jj];
					if (pOddsInfo == NULL)
						continue;
					if (pOddsInfo->m_nOddsInfoKind != OI_GOAL_OU)
						continue;

					CString strLabel = MakeGoalLabel(pOddsInfo);
					int nPos = GetSameCaption(strLabel, nStartColNo, nColNo - nStartColNo);
					if (nPos == -1)
					{
						m_Grid.SetItemText(1, nColNo, strLabel);

						m_Grid.SetItemFloat(2 + i * 3, nColNo, pOddsInfo->m_oVal1);
						m_Grid.SetItemFloat(3 + i * 3, nColNo, pOddsInfo->m_oVal2);
						nColNo++;
					}
					else
					{
						m_Grid.SetItemFloat(2 + i * 3, nPos, pOddsInfo->m_oVal1);
						m_Grid.SetItemFloat(3 + i * 3, nPos, pOddsInfo->m_oVal2);
					}
				}
			}
			if (nColNo - nStartColNo >= 2)
				m_Grid.MergeCells(0, nStartColNo, 0, nColNo - 1);
			m_Grid.SetItemText(0, nStartColNo, _T("Goals"));
		}
		//nColNo += (nGoalsCount * 2);
	}

	//Goals(Odd/Even)
	//if (pGameInfo->GetCameCategory() != GC_CRICKET && pGameInfo->GetCameCategory() != GC_DARTS && pGameInfo->GetCameCategory() != GC_SNOOKER)
	{
		nStartColNo = nColNo;
		nGoalsCount = 0;
		for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
		{
			CGameData* pGameData = pGameInfo->m_aryGameData[i];
			if (!pGameData)
				ASSERT(0);
			for (j = 0; j < pGameData->m_aryOddsInfo.GetSize(); j++)
			{
				COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[j];
				if (pOddsInfo->m_nOddsInfoKind != OI_GOAL_OE)
					continue;
				nGoalsCount++;
			}
		}

		if (nGoalsCount != 0)
		{
			for (i = 0; i < nGoalsCount; i++)
				m_Grid.SetColumnWidth(nColNo, COL_WIDTH);

			//if (nGoalsCount >= 2)
			//	m_Grid.MergeCells(0, nColNo, 0, nColNo + 1);
			m_Grid.SetItemText(0, nColNo, _T("Goals"));

			int nGoalsNo = 0;
			for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
			{
				CGameData* pGameData = pGameInfo->m_aryGameData[i];
				if (!pGameData)
					ASSERT(0);
				for (int jj = 0; jj < pGameData->m_aryOddsInfo.GetSize(); jj++)
				{
					COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[jj];
					if (pOddsInfo->m_nOddsInfoKind != OI_GOAL_OE)
						continue;

					m_Grid.SetItemText(1, nColNo, _T("Odd/Evn"));

					m_Grid.SetItemFloat(2 + i * 3, nColNo, pOddsInfo->m_oVal1);
					m_Grid.SetItemFloat(3 + i * 3, nColNo, pOddsInfo->m_oVal2);
				}
			}
			nColNo++;
		}
	}

	//Goals(Team1)
	//	if (pGameInfo->GetCameCategory() != GC_CRICKET && pGameInfo->GetCameCategory() != GC_DARTS && 
	//		pGameInfo->GetCameCategory() != GC_E_SPORTS && pGameInfo->GetCameCategory() != GC_HANDBALL && 
	//		pGameInfo->GetCameCategory() != GC_RUGBY_SEVENS && pGameInfo->GetCameCategory() != GC_SNOOKER)
	{
		nStartColNo = nColNo;
		nGoalsCount = 0;
		for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
		{
			CGameData* pGameData = pGameInfo->m_aryGameData[i];
			if (!pGameData)
				ASSERT(0);
			for (j = 0; j < pGameData->m_aryOddsInfo.GetSize(); j++)
			{
				COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[j];
				if (pOddsInfo->m_nOddsInfoKind != OI_TEAM1_GOAL_OU)
					continue;
				nGoalsCount++;
			}
		}
		if (nGoalsCount != 0)
		{
			for (i = 0; i < nGoalsCount; i++)
				m_Grid.SetColumnWidth(nColNo + i, COL_WIDTH);

			int nGoalsNo = 0;
			for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
			{
				CGameData* pGameData = pGameInfo->m_aryGameData[i];
				if (!pGameData)
					ASSERT(0);
				for (int jj = 0; jj < pGameData->m_aryOddsInfo.GetSize(); jj++)
				{
					COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[jj];
					if (pOddsInfo->m_nOddsInfoKind != OI_TEAM1_GOAL_OU)
						continue;

					CString strLabel = MakeGoalLabel(pOddsInfo);
					int nPos = GetSameCaption(strLabel, nStartColNo, nColNo - nStartColNo);
					if (nPos == -1)
					{
						m_Grid.SetItemText(1, nColNo, strLabel);

						m_Grid.SetItemFloat(2 + i * 3, nColNo, pOddsInfo->m_oVal1);
						m_Grid.SetItemFloat(3 + i * 3, nColNo, pOddsInfo->m_oVal2);
						nColNo++;
					}
					else
					{
						m_Grid.SetItemFloat(2 + i * 3, nPos, pOddsInfo->m_oVal1);
						m_Grid.SetItemFloat(3 + i * 3, nPos, pOddsInfo->m_oVal2);
					}
				}
				if (nColNo - nStartColNo >= 2)
					m_Grid.MergeCells(0, nStartColNo, 0, nColNo - 1);
				m_Grid.SetItemText(0, nStartColNo, _T("HT"));
			}
			//nColNo += (nGoalsCount * 2);
		}
	}

	//Goals(Team2)
	//	if (pGameInfo->GetCameCategory() != GC_CRICKET && pGameInfo->GetCameCategory() != GC_DARTS &&
	//		pGameInfo->GetCameCategory() != GC_E_SPORTS && pGameInfo->GetCameCategory() != GC_HANDBALL &&
	//		pGameInfo->GetCameCategory() != GC_RUGBY_SEVENS && pGameInfo->GetCameCategory() != GC_SNOOKER)
	{
		nStartColNo = nColNo;
		nGoalsCount = 0;
		for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
		{
			CGameData* pGameData = pGameInfo->m_aryGameData[i];
			if (!pGameData)
				ASSERT(0);
			for (j = 0; j < pGameData->m_aryOddsInfo.GetSize(); j++)
			{
				COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[j];
				if (pOddsInfo->m_nOddsInfoKind != OI_TEAM2_GOAL_OU)
					continue;
				nGoalsCount++;
			}
		}
		if (nGoalsCount != 0)
		{
			for (i = 0; i < nGoalsCount; i++)
				m_Grid.SetColumnWidth(nColNo + i, COL_WIDTH);

			int nGoalsNo = 0;
			for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
			{
				CGameData* pGameData = pGameInfo->m_aryGameData[i];
				if (!pGameData)
					ASSERT(0);
				for (int jj = 0; jj < pGameData->m_aryOddsInfo.GetSize(); jj++)
				{
					COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[jj];
					if (pOddsInfo->m_nOddsInfoKind != OI_TEAM2_GOAL_OU)
						continue;

					CString strLabel = MakeGoalLabel(pOddsInfo);
					int nPos = GetSameCaption(strLabel, nStartColNo, nColNo - nStartColNo);
					if (nPos == -1)
					{
						m_Grid.SetItemText(1, nColNo, strLabel);

						m_Grid.SetItemFloat(2 + i * 3, nColNo, pOddsInfo->m_oVal1);
						m_Grid.SetItemFloat(3 + i * 3, nColNo, pOddsInfo->m_oVal2);
						nColNo++;
					}
					else
					{
						m_Grid.SetItemFloat(2 + i * 3, nPos, pOddsInfo->m_oVal1);
						m_Grid.SetItemFloat(3 + i * 3, nPos, pOddsInfo->m_oVal2);
					}
				}
			}
			if (nColNo - nStartColNo >= 2)
				m_Grid.MergeCells(0, nStartColNo, 0, nColNo - 1);
			m_Grid.SetItemText(0, nStartColNo, _T("AT"));
		}
		//nColNo += (nGoalsCount * 2);
	}

	m_Grid.SetColumnWidth(nColNo, 1000);
	m_Grid.MergeCells(0, nColNo, 1, nColNo);
	m_Grid.SetItemText(0, nColNo, _T("Game"));

	for (i = 0; i < pGameInfo->m_aryGameData.GetSize(); i++)
	{
		CGameData* pGameData = pGameInfo->m_aryGameData[i];
		if (!pGameData)
			ASSERT(0);

		m_Grid.MergeCells(2 + i * 3, nColNo, 4 + i * 3, nColNo);

		::ToUnicode(pGameData->m_MainInfo.m_szHTName, g_szHTeam, CP_ACP);
		::ToUnicode(pGameData->m_MainInfo.m_szATName, g_szATeam, CP_ACP);
		::ToUnicode(pGameData->m_MainInfo.m_szStage, g_szStage1, CP_ACP);

		::ToUnicode(pGameData->m_MainInfo.m_szLeague, g_szLeague, CP_ACP);

		TCHAR szChanged[0x40];
		_tcscpy_s(szChanged, _T(""));
		if (pGameData->m_bHAChanged)
			_tcscpy_s(szChanged, _T("[===============]"));
		_stprintf_s(g_szGame, _T("%s : %s [%s] (ID : %d, ([%s (%d : %d)] (%d/%d/%d) : (%d/%d/%d)) : %s"), g_szHTeam, g_szATeam, g_szLeague, pGameData->m_nGameID,
			g_szStage1, pGameData->m_MainInfo.m_nTime / 60, pGameData->m_MainInfo.m_nTime % 60,
			pGameData->m_MainInfo.m_HTScore.nScore, pGameData->m_MainInfo.m_HTScore.nScore1, pGameData->m_MainInfo.m_HTScore.nScore2,
			pGameData->m_MainInfo.m_ATScore.nScore, pGameData->m_MainInfo.m_ATScore.nScore1, pGameData->m_MainInfo.m_ATScore.nScore2, szChanged
			);
		m_Grid.SetItemText(2 + i * 3, nColNo, g_szGame);
	}
	nColNo++;

	m_Grid.SetFixedRowCount(m_nFixRows);
	m_Grid.SetFixedColumnCount(m_nFixCols);
}
