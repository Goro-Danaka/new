
#include "..\stdafx.h"

#include "SettingWnd.h"
#include "..\Resource.h"
#include "..\MainFrm.h"
#include "..\global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

CSettingWnd::CSettingWnd()
{
}

CSettingWnd::~CSettingWnd()
{
}

BEGIN_MESSAGE_MAP(CSettingWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CSettingWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LVS_REPORT;

	if (!m_wndSites.Create(dwStyle, rectDummy, &m_wndTabs, IDC_LIST_SITES) ||
		!m_wndCategory.Create(dwStyle, rectDummy, &m_wndTabs, IDC_LIST_CATEGORY) ||
		!m_wndBetStrategy.Create(dwStyle, rectDummy, &m_wndTabs, IDC_LIST_BETSTRATEGY) ||
		!m_wndBetHistory.Create(dwStyle, rectDummy, &m_wndTabs, IDC_LIST_BETHISTORY))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	InitSites();
	InitCategory();
	InitBetStrategy();
	InitBetHistory();
	//InitBalance();

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// Attach list windows to tab:
	bNameValid = strTabName.LoadString(IDS_SITES);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndSites, strTabName, (UINT)0);

	bNameValid = strTabName.LoadString(IDS_CATEGORY);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndCategory, strTabName, (UINT)0);

	bNameValid = strTabName.LoadString(IDS_BETSTRATEGY);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndBetStrategy, strTabName, (UINT)0);

	bNameValid = strTabName.LoadString(IDS_BETHISTORY);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndBetHistory, strTabName, (UINT)0);
	return 0;
}

void CSettingWnd::InitSites()
{
	ListView_SetExtendedListViewStyle(m_wndSites.GetSafeHwnd(), LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	ListView_SetCheckState(m_wndSites.GetSafeHwnd(), 0, TRUE);

	m_wndSites.m_nTabIndex = 0;
	m_wndSites.InsertColumnEx(0, _T("Book Makers"), 200);
	m_wndSites.InsertColumnEx(1, _T("User Name"), 200, CT_EDIT);
	m_wndSites.InsertColumnEx(2, _T("Password"), 200, CT_PSWD);
	m_wndSites.InsertColumnEx(3, _T("Log In Status"), 100, CT_LOGIN_STATUS);
	m_wndSites.InsertColumnEx(4, _T("Balance"), 100, CT_LOGIN_STATUS);

	TCHAR sz[MAX_BOOKMAKER];
	for (int i = 0; i < sizeof(g_szSiteName) / 4; i++)
	{
		char* szID = new char[MAX_ID];
		char* szPswd = new char[MAX_PSWD];
		char* szEncPswd = new char[MAX_ENC_PSWD];

		::ToUnicode(g_szSiteName[i], sz, CP_ACP);
		SetListItemText(m_wndSites, 0, i, sz);

		//int nSiteFlag = ::GetPrivateProfileIntA("Site", g_szSiteName[i], 0, "./setting.ini");
		//if (nSiteFlag)
		//	m_wndSites.SetCheck(i, nSiteFlag);
		::GetPrivateProfileStringA("ID", g_szSiteName[i], "", szID, MAX_ID, "./setting.ini");
		::ToUnicode(szID, sz, CP_ACP);
		SetListItemText(m_wndSites, 1, i, sz);

		::GetPrivateProfileStringA("PSWD", g_szSiteName[i], "", szEncPswd, MAX_ENC_PSWD, "./setting.ini");

		::GetDecPswd(szEncPswd, szPswd);
		delete szEncPswd;

		CString s = _T("");
		for (int i = 0; i < (int)strlen(szPswd); i++)
			s += _T('*');
		SetListItemText(m_wndSites, 2, i, s);
		m_wndSites.SetItemData(i, (DWORD_PTR)szPswd);
	}
}

void CSettingWnd::InitCategory()
{
	ListView_SetExtendedListViewStyle(m_wndCategory.GetSafeHwnd(), LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	ListView_SetCheckState(m_wndCategory.GetSafeHwnd(), 0, TRUE);

	m_wndCategory.m_nTabIndex = 1;
	m_wndCategory.InsertColumnEx(0, _T("Category"), 200);

	int i;
	for (i = 0; i < GC_COUNT; i++)
	{
		TCHAR sz[0x30];
		::ToUnicode(g_szGameCategory[i], sz, CP_ACP);
		SetListItemText(m_wndCategory, 0, i, sz);

		int nCatFlag = ::GetPrivateProfileIntA("Category", g_szGameCategory[i], 1, "./setting.ini");
		if (nCatFlag)
			m_wndCategory.SetCheck(i, nCatFlag);
	}
}

void CSettingWnd::InitBetStrategy()
{
	ListView_SetExtendedListViewStyle(m_wndBetStrategy.GetSafeHwnd(), LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	ListView_SetCheckState(m_wndBetStrategy.GetSafeHwnd(), 0, TRUE);

	m_wndBetStrategy.m_nTabIndex = 2;
	m_wndBetStrategy.InsertColumnEx(0, _T("Bet Strategy"), 200);

	int i;
	for (i = 0; i < BS_COUNT; i++)
	{
		SetListItemText(m_wndBetStrategy, 0, i, g_wszBetStrategy[i]);
	}
	int nBetStrategy = ::GetPrivateProfileIntA("Bet Strategy", "method", 0, "./setting.ini");
	m_wndBetStrategy.SetCheck(nBetStrategy, 1);
}

void CSettingWnd::InitBetHistory()
{
	LVCOLUMN list;
	list.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
	list.fmt = LVCFMT_LEFT;

	list.cx = 120;
	list.pszText = _T("Time");
	list.iSubItem = 0;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 80;
	list.pszText = _T("Category");
	list.iSubItem = 1;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 300;
	list.pszText = _T("Game");
	list.iSubItem = 2;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 150;
	list.pszText = _T("League");
	list.iSubItem = 3;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 80;
	list.pszText = _T("Odd Type");
	list.iSubItem = 4;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 70;
	list.pszText = _T("Value");
	list.iSubItem = 5;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 50;
	list.pszText = _T("Odds");
	list.iSubItem = 6;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 50;
	list.pszText = _T("Orig");
	list.iSubItem = 7;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 50;
	list.pszText = _T("Object");
	list.iSubItem = 8;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 100;
	list.pszText = _T("BookMaker");
	list.iSubItem = 9;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 50;
	list.pszText = _T("Stakes");
	list.iSubItem = 10;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 70;
	list.pszText = _T("Balance");
	list.iSubItem = 11;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 70;
	list.pszText = _T("Engine(s)");
	list.iSubItem = 12;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 70;
	list.pszText = _T("Betting(s)");
	list.iSubItem = 13;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 160;
	list.pszText = _T("States");
	list.iSubItem = 14;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	list.cx = 70;
	list.pszText = _T("Repeat");
	list.iSubItem = 15;
	m_wndBetHistory.InsertColumn(list.iSubItem, &list);

	ListView_SetExtendedListViewStyle(m_wndBetHistory.GetSafeHwnd(), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CSettingWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void CSettingWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void CSettingWnd::UpdateFonts()
{
	m_wndSites.SetFont(&afxGlobalData.fontRegular);
	m_wndBetHistory.SetFont(&afxGlobalData.fontRegular);
}

LRESULT CSettingWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
	case MSG_CUR_BALANCE:
		{
			int nSiteID = (int)wParam;
			float fBal = (float)lParam / 100;
			TCHAR sz[20];
			if (lParam != -1)
			{
				_stprintf_s(sz, _T("%.2f CNY"), fBal);
				SetListItemText(m_wndSites, 4, nSiteID, sz);
			}
			else
				SetListItemText(m_wndSites, 4, nSiteID, _T(""));
		}
		break;
	case MSG_LOGIN_STATUS:
		{
			int nSiteID = (int)wParam;
			if (lParam == (LPARAM)1)
				SetListItemText(m_wndSites, 3, nSiteID, _T("Success"));
			else if (lParam == (LPARAM)2)
				SetListItemText(m_wndSites, 3, nSiteID, _T("Fail"));
			else
			{
				SetListItemText(m_wndSites, 3, nSiteID, _T(""));
				m_wndSites.SetCheck(nSiteID, 0);
			}
		}
		break;
	case MSG_DISP_BET_STATUS:
		{
			BetStatusDisplay* betState = (BetStatusDisplay*)wParam;
			DisplayBetStatus(betState);
			delete betState;
		}
		break;
	default:
		break;
	}
	return CDockablePane::WindowProc(message, wParam, lParam);
}

void CSettingWnd::SaveBetHistory(FILE* fp, int nRow)
{
	CString strLine = _T("");
	for (int i = 0; i < 16; i++)
	{
		strLine += m_wndBetHistory.GetItemText(nRow, i);
		if (i == 15)
			strLine += _T("\n");
		else
			strLine += _T(", ");
	}
	fputws(strLine.GetBuffer(), fp);
}

void CSettingWnd::SaveBetHistory()
{
	BOOL bRet = CreateDirectoryA("./log", NULL);
	if (!bRet)
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
			return;
	}
	int nRow = m_wndBetHistory.GetItemCount();
	if (nRow == 0)
		return;
	CStringA strFileName;
	SYSTEMTIME time;
	GetSystemTime(&time);
	strFileName.Format("./log/%.4d-%.2d-%.2d-%.2d-%.2d-%.2d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	FILE* fp = NULL;
	fopen_s(&fp, strFileName, "w");
	if (fp == NULL)
		return;
	for (int i = 0; i < nRow; i++)
		SaveBetHistory(fp, i);
	fclose(fp);
}

CString GetBetResult(int nResult)
{
	if (nResult >= sizeof(g_wszBetResult) / 4)
		return _T("Unknown");
	return g_wszBetResult[nResult];
}

void CSettingWnd::DisplayBetStatus(BetStatusDisplay* betState)
{
	int nRow = m_wndBetHistory.GetItemCount();

	if (nRow == BET_HISTORY_SAVE_LIMITCOUNT)
	{
		SaveBetHistory();
		m_wndBetHistory.DeleteAllItems();
		nRow = 0;
	}

	TCHAR szHTeam[MAX_TEAM_NAME], szATeam[MAX_TEAM_NAME], szLeague[MAX_LEAGUE_NAME];
	ToUnicode(betState->szHTeam, szHTeam, CP_ACP);
	ToUnicode(betState->szATeam, szATeam, CP_ACP);
	ToUnicode(betState->szLeague, szLeague, CP_ACP);
	SYSTEMTIME time;
	GetSystemTime(&time);
	CString strTime;
	strTime.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	SetListItemText(m_wndBetHistory, 0, nRow, strTime);
	SetListItemText(m_wndBetHistory, 1, nRow, GetGameCategory(betState->nCategory));
	CString strGameName;
	strGameName.Format(_T("%s / %s"), szHTeam, szATeam);
	SetListItemText(m_wndBetHistory, 2, nRow, strGameName);
	SetListItemText(m_wndBetHistory, 3, nRow, szLeague);
	SetListItemText(m_wndBetHistory, 4, nRow, g_wszOddKind[betState->nOddKind]);
	CString strOddVal;
	if (betState->nOddKind != OI_MAIN && betState->nOddKind != OI_GOAL_OE)
	{
		strOddVal.Format(_T("%.2f"), betState->fHVal);
		SetListItemText(m_wndBetHistory, 5, nRow, strOddVal);
	}
	strOddVal.Format(_T("%.2f"), betState->fOVal_cur);
	SetListItemText(m_wndBetHistory, 6, nRow, strOddVal);
	strOddVal.Format(_T("%.2f"), betState->fOVal_orig);
	SetListItemText(m_wndBetHistory, 7, nRow, strOddVal);
	strOddVal.Format(_T("%d"), betState->nObj);
	SetListItemText(m_wndBetHistory, 8, nRow, strOddVal);
	SetListItemText(m_wndBetHistory, 9, nRow, g_wszSiteName[betState->nBookMaker]);
	CString strStake;
	strStake.Format(_T("%d"), betState->nBettingMoney);
	SetListItemText(m_wndBetHistory, 10, nRow, strStake);
	CString strBalance;
	strBalance.Format(_T("%.2f"), betState->fBalance);
	SetListItemText(m_wndBetHistory, 11, nRow, strBalance);

	strTime.Format(_T("%d"), betState->dwEngineTime);
	SetListItemText(m_wndBetHistory, 12, nRow, strTime);
	strTime.Format(_T("%d"), betState->dwBettingTime);
	SetListItemText(m_wndBetHistory, 13, nRow, strTime);

	CString strResult = GetBetResult(betState->nResult);
	SetListItemText(m_wndBetHistory, 14, nRow, strResult);

	strTime.Format(_T("%d"), betState->nRepeat);
	SetListItemText(m_wndBetHistory, 15, nRow, strTime);

	m_wndBetHistory.EnsureVisible(nRow, FALSE);
}