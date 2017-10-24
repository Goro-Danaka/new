
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SportsBetting.h"

#include "MainFrm.h"
#include "CaptchaDlg.h"
#include "SportsBettingView.h"
#include "Bet.h"
#include "BetSites\BetSite_188bet.h"
#include "BetSites\BetSite_18bet.h"
#include "BetSites\BetSite_1bet.h"
#include "BetSites\BetSite_1xbetchina.h"
#include "BetSites\BetSite_babibet.h"
#include "BetSites\BetSite_bet3000.h"
#include "BetSites\BetSite_bet365.h"
#include "BetSites\BetSite_bet888vip.h"
#include "BetSites\BetSite_betballer.h"
#include "BetSites\BetSite_bwin.h"
#include "BetSites\BetSite_marathonbet.h"
#include "BetSites\BetSite_tomato333.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_WM_SETTINGCHANGE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_STARTBETTING, &CMainFrame::OnStartbetting)
	ON_UPDATE_COMMAND_UI(ID_STARTBETTING, &CMainFrame::OnUpdateStartbetting)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ENGINE_TIME, &CMainFrame::OnUpdateEngineTime)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_BETTING_TIME, &CMainFrame::OnUpdateBettingTime)
	ON_COMMAND(ID_CLEARBETINFO, &CMainFrame::OnClearbetinfo)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_ENGINE_TIME,
	ID_INDICATOR_BETTING_TIME,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);

	::InitializeCriticalSection(&g_csGameInfoArray);
	::InitializeCriticalSection(&g_csTeamNameWnd);
	::InitializeCriticalSection(&g_csSameName);
	::InitializeCriticalSection(&g_csDiffName);

	g_aryGameInfo.RemoveAll();
	g_mapName.RemoveAll();

	g_site[SITE_188bet] = new CBetSite_188bet;
	g_site[SITE_18bet] = new CBetSite_18bet;
	g_site[SITE_1bet] = new CBetSite_1bet;
	g_site[SITE_1xbetchina] = new CBetSite_1xbetchina;
	g_site[SITE_babibet] = new CBetSite_babibet;
	g_site[SITE_bet3000] = new CBetSite_bet3000;
	g_site[SITE_bet365] = new CBetSite_bet365;
	g_site[SITE_bet888vip] = new CBetSite_bet888vip;
	g_site[SITE_betballer] = new CBetSite_betballer;
	g_site[SITE_bwin] = new CBetSite_bwin;
	g_site[SITE_marathonbet] = new CBetSite_marathonbet;
	g_site[SITE_tomato333] = new CBetSite_tomato333;

	m_treeGameInfo = NULL;
	m_Grid = NULL;

	m_bDoingBetting = FALSE;
	m_nBetStrategy = BS_SPATIAL;

	m_pMainView = NULL;

	for (int i = 0; i < GC_COUNT; i++)
	{
		m_hItemCategory[i] = NULL;
		m_bSetCategory[i] = TRUE;
	}
	m_hCurrentSelGame = NULL;
	m_pCurSelGameInfo = NULL;
	g_aryDiffNamePair.RemoveAll();

	m_hBetTread = INVALID_HANDLE_VALUE;
	m_bExitBetThreadFlg = FALSE;

	g_pBetInfo = new BetInfo;
#ifdef BET_SUBTHREAD_MODE
	m_bExitBetSubThreadFlg = FALSE;
	m_hCompletionPortBet = INVALID_HANDLE_VALUE;
	m_hBettingCompleteEvent = CreateEvent(nullptr, true, true, nullptr);

	m_nBetObjCount = 0;
#endif
}

CMainFrame::~CMainFrame()
{
	if (g_pBetInfo)
		delete g_pBetInfo;
}

CMainFrame* theFrame;

#ifdef BET_SUBTHREAD_MODE
DWORD WINAPI GlobSubBetThreadProc(PVOID pParam)
{
	CMainFrame* pMainFrame = (CMainFrame*)pParam;
	return pMainFrame->SubBetThreadProc();
}
#endif

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	theFrame = this;
	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	//m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);

	// Allow user-defined toolbars operations:
	//InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);
	
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	//DockPane(&m_wndToolBar);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndGamesView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndGamesView);
	CDockablePane* pTabbedBar = NULL;
	m_wndSetting.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndSetting);
	m_wndTeamName.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndTeamName);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, 0, strCustomize, ID_VIEW_TOOLBAR);

	char szLine[MAX_TEAM_NAME * 3];
	FILE* fp = NULL;
	fopen_s(&fp, "./teamnames.ini", "r");
	if (fp)
	{
		while (!feof(fp))
		{
			strcpy_s(szLine, "");
			fgets(szLine, sizeof(szLine), fp);
			string s1, s2;
			SplitS(szLine, ':', s1, s2);
			if (s1 == "" || s2 == "")
				continue;
			::EnterCriticalSection(&g_csSameName);
			SetSlaveName((char*)s1.c_str(), (char*)s2.c_str());
			::LeaveCriticalSection(&g_csSameName);
		}
		fclose(fp);
	}
	SetCategory();
	CreateSiteThreads();

	for (int i = 0; i < SITE_COUNT; i++)
	{
		g_fBalance[i] = 0.0;
		g_fBalanceOld[i] = 0.0;
	}

#ifdef BET_SUBTHREAD_MODE
	m_hCompletionPortBet = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!m_hCompletionPortBet)
		return 0;

	for (int i = 0; i < 3; i++)
		m_hBetSubThread[i] = CreateThread(NULL, 0, ::GlobSubBetThreadProc, this, 0, NULL);
#endif
	return 0;
}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_bExitBetThreadFlg = TRUE;

#ifdef BET_SUBTHREAD_MODE
	m_bExitBetSubThreadFlg = TRUE;
	for (int i = 0; i < 3; ++i)
		WaitForThreadToTerminate(m_hBetSubThread[i]);
#endif
	WaitForThreadToTerminate(m_hBetTread);
	m_hBetTread = NULL;

	m_wndSetting.SaveBetHistory();
	int i;
	for (i = 0; i < SITE_COUNT; i++)
	{
		if (g_site[i])
		{
			g_site[i]->ExitThread();

			delete g_site[i];
			g_site[i] = NULL;
		}
	}

	for (i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pGameInfo = g_aryGameInfo[i];
		if (pGameInfo)
		{
			delete pGameInfo;
			g_aryGameInfo[i] = NULL;
		}
	}
	g_aryGameInfo.RemoveAll();
	g_mapName.RemoveAll();

	for (i = 0; i < g_aryDiffNamePair.GetSize(); i++)
	{
		NamePair* pNamePair = g_aryDiffNamePair[i];
		if (pNamePair)
		{
			delete pNamePair;
			g_aryDiffNamePair[i] = NULL;
		}
	}
	g_aryDiffNamePair.RemoveAll();

	if (m_pCurSelGameInfo)
	{
		delete m_pCurSelGameInfo;
		m_pCurSelGameInfo = NULL;
	}
	::DeleteCriticalSection(&g_csTeamNameWnd);
	::DeleteCriticalSection(&g_csGameInfoArray);
	::DeleteCriticalSection(&g_csSameName);
	::DeleteCriticalSection(&g_csDiffName);

	CFrameWndEx::OnClose();
}

int CMainFrame::CreateSiteThreads()
{
	char szID[MAX_ID], szPswd[MAX_PSWD], szEncPswd[MAX_ENC_PSWD];
	for (int i = 0; i < sizeof(g_szSiteName) / 4; i++)
	{
		//int nSiteFlag = ::GetPrivateProfileIntA("Site", g_szSiteName[i], 0, "./setting.ini");
		::GetPrivateProfileStringA("ID", g_szSiteName[i], "", szID, MAX_ID, "./setting.ini");
		::GetPrivateProfileStringA("PSWD", g_szSiteName[i], "", szEncPswd, MAX_ENC_PSWD, "./setting.ini");
		::GetDecPswd(szEncPswd, szPswd);
		//if (nSiteFlag == 1)
		//	g_site[i]->StartThread(szID, szPswd);
	}
	
	return 0;
}

void CMainFrame::SetCategory()
{
	for (int i = 0; i < sizeof(g_szGameCategory) / 4; i++)
	{
		int nCatFlag = ::GetPrivateProfileIntA("Category", g_szGameCategory[i], 1, "./setting.ini");
		m_bSetCategory[i] = nCatFlag;
	}
}

void CMainFrame::SetCategory(int nIndex, BOOL bStart)
{
	m_bSetCategory[nIndex] = bStart;
	::WritePrivateProfileIntA("Category", g_szGameCategory[nIndex], bStart ? 1 : 0, "./setting.ini");
}

void CMainFrame::SetBetStrategy(int nIndex)
{
	m_nBetStrategy = nIndex;
	::WritePrivateProfileIntA("Bet Strategy", "method", nIndex, "./setting.ini");
}

void CMainFrame::StartThread(int nIndex, BOOL bStart, char* szID, char* szPswd)
{
	if (bStart)
	{
		if (!g_site[nIndex]->m_hThread)
		{
			g_site[nIndex]->StartThread(szID, szPswd);
			PostMessageW(MSG_UPDATE, (WPARAM)nIndex, (LPARAM)THREAD_START);
		}
	}
	else
	{
		if (g_site[nIndex]->m_hThread)
		{
			g_site[nIndex]->ExitThread();
			PostMessageW(MSG_UPDATE, (WPARAM)nIndex, (LPARAM)THREAD_STOP);
		}
	}

	//::WritePrivateProfileIntA("Site", g_szSiteName[nIndex], bStart ? 1 : 0, "./setting.ini");
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create games view
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndGamesView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create File View window\n");
		return FALSE; // failed to create
	}

	// Create setting window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_SETTING_WND);
	ASSERT(bNameValid);
	if (!m_wndSetting.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	CString strTeamNameWnd;
	bNameValid = strTeamNameWnd.LoadString(IDS_TEAMNAME_WND);
	ASSERT(bNameValid);
	if (!m_wndTeamName.Create(strTeamNameWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndGamesView.SetIcon(hFileViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndSetting.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndTeamName.SetIcon(hPropertiesBarIcon, FALSE);

}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
		return 0;

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
		return FALSE;

	return TRUE;
	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndSetting.UpdateFonts();
}


LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
	case MSG_UPDATE:
		UpdateInfo((int)wParam, (int)lParam);
		break;
	case MSG_UPDATE_EVENT_LIST:
		UpdateEventInfoList((int)wParam, (int)lParam);
		break;
	case MSG_UPDATE_EVENT_TREE:
		UpdateEventInfoTree((int)wParam);
		break;
	case MSG_CHECK_TEAMNAME:
		CheckTeamName(FALSE, wParam, lParam);
		break;
	case MSG_CAPTCHA:
		ShowCaptcha(wParam, lParam);
		break;
	case MSG_BETTING:
		{
			if (wParam == 0)
				m_wndStatusBar.SetWindowTextW(_T("Betting..."));
			else
				m_wndStatusBar.SetWindowTextW(_T(""));
		}
		break;
	default:
		break;
	}

	return CFrameWndEx::WindowProc(message, wParam, lParam);
}

void CMainFrame::ShowCaptcha(WPARAM wParam, LPARAM lParam)
{
	CCaptchaDlg dlg;
	dlg.m_nSiteID = (int)wParam;
	HANDLE theHandle = (HANDLE)lParam;
	if (dlg.DoModal() == IDOK)
	{
		CString s = dlg.m_strCaptcha.GetBuffer();
		::ToAscii(s.GetBuffer(), g_site[(int)wParam]->m_szCaptcha, CP_ACP);
	}
	SetEvent(theHandle);
}

void CMainFrame::CheckTeamName(BOOL bHAChanged, WPARAM wParam, LPARAM lParam)
{
	CMainInfo* pMainInfo1 = (CMainInfo*)wParam;
	CMainInfo* pMainInfo2 = (CMainInfo*)lParam;
	::EnterCriticalSection(&g_csTeamNameWnd);
	if (!m_wndTeamName.IsExistItem(*pMainInfo1))
		m_wndTeamName.AddItem(bHAChanged, *pMainInfo1, *pMainInfo2);
	::LeaveCriticalSection(&g_csTeamNameWnd);
}

void CMainFrame::UpdateEventInfoTree(int nSiteID)
{
	m_wndStatusBar.SetWindowTextW(g_wszSiteName[nSiteID]);

	if (UpdateTree(nSiteID, 0) == TRUE) // selected item removed
	{
		m_Grid->DeleteAllItems();
		if (m_pCurSelGameInfo)
		{
			delete m_pCurSelGameInfo;
			m_pCurSelGameInfo = NULL;
			m_hCurrentSelGame = NULL;
		}
	}
}

void CMainFrame::UpdateEventInfoList(int nSiteID, int nEventID)
{
	CGameInfo* pGameInfo;
	if (m_hCurrentSelGame)
	{
		pGameInfo = (CGameInfo*)m_treeGameInfo->GetItemData(m_hCurrentSelGame);
		CGameData* pGameData = pGameInfo->GetGameData(nSiteID);
		if (pGameData != NULL)
		{
			if (pGameData->m_nGameID == nEventID)
				UpdateList(pGameInfo, FALSE);
		}
	}
}

void CMainFrame::UpdateInfo(int nSiteID, int nIsExitThread)
{
	m_wndStatusBar.SetWindowTextW(g_wszSiteName[nSiteID]);
	if (nIsExitThread == THREAD_STOP || nIsExitThread == THREAD_START)
	{
		m_wndTeamName.SendMessageW(WM_COMMAND, ID_DELETE);
	}
	CGameInfo* pGameInfo;
	if (UpdateTree(nSiteID, nIsExitThread) == TRUE) // selected item removed
	{
		m_Grid->DeleteAllItems();
		if (m_pCurSelGameInfo)
		{
			delete m_pCurSelGameInfo;
			m_pCurSelGameInfo = NULL;
			m_hCurrentSelGame = NULL;
		}
	}
	else
	{
		if (m_hCurrentSelGame)
		{
			pGameInfo = (CGameInfo*)m_treeGameInfo->GetItemData(m_hCurrentSelGame);
			UpdateList(pGameInfo, FALSE);
		}
	}
}

void CMainFrame::UpdateList(CGameInfo* pGameInfo, BOOL bNewTreeItem)
{
	((CSportsBettingView*)m_pMainView)->UpdateList(pGameInfo, bNewTreeItem);
}

BOOL CMainFrame::UpdateTree(int nSiteID, int nIsExitThread)
{
	return g_site[nSiteID]->UpdateTree(nIsExitThread);
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// TODO: Add your specialized code here and/or call the base class
	CFrameWndEx::OnUpdateFrameTitle(bAddToTitle);
	SetWindowText(_T("Sports Betting"));
}

DWORD WINAPI GlobBetThreadProc(PVOID pParam)
{
	CMainFrame* pMainFrame = (CMainFrame*)pParam;
	return pMainFrame->BetThreadProc();
}

#ifdef NO_ENGINE
int GetRandInfo(int a)
{
	srand((unsigned int)time(NULL));
	int ret = rand() % a;
	return ret;
}

#define BET_SITE_COUNT 5

void AvailSiteProc(int nSiteIndex[], int nBettingMoney[], int nOddsKind)
{
	int nAliveSite = 0;
	for (int i = 0; i < BET_SITE_COUNT; i++)
	{
		if (!g_site[nSiteIndex[i]]->m_hThread)
			nSiteIndex[i] = -1;
		else
			nAliveSite++;
	}

	int k = 0, k1 = 0;
	while (k < BET_SITE_COUNT)
	{
		if (nSiteIndex[k] == -1)
		{
			int dif = 0;
			for (k1 = k + 1; k1 < BET_SITE_COUNT; k1++)
			{
				if (nSiteIndex[k1] == -1)
					continue;
				dif = k1 - k;
				break;
			}
			for (k1 = k + dif; k1 < BET_SITE_COUNT; k1++)
			{
				nSiteIndex[k1 - dif] = nSiteIndex[k1];
				nBettingMoney[k1 - dif] = nBettingMoney[k1];
				nSiteIndex[k1] = -1;
			}
		}
		k++;
	}

	if (nOddsKind == OI_MAIN)
	{
		while (nAliveSite > 3)
		{
			int nIndex = GetRandInfo(3);
			if (nSiteIndex[nIndex] != -1)
			{
				nSiteIndex[nIndex] = -1;
				nAliveSite--;
			}
		}
	}
	else
	{
		while (nAliveSite > 2)
		{
			int nIndex = GetRandInfo(2);
			if (nSiteIndex[nIndex] != -1)
			{
				nSiteIndex[nIndex] = -1;
				nAliveSite--;
			}
		}
	}

	k = 0, k1 = 0;
	while (k < BET_SITE_COUNT)
	{
		if (nSiteIndex[k] == -1)
		{
			int dif = 0;
			for (k1 = k + 1; k1 < BET_SITE_COUNT; k1++)
			{
				if (nSiteIndex[k1] == -1)
					continue;
				dif = k1 - k;
				break;
			}
			for (k1 = k + dif; k1 < BET_SITE_COUNT; k1++)
			{
				nSiteIndex[k1 - dif] = nSiteIndex[k1];
				nBettingMoney[k1 - dif] = nBettingMoney[k1];
				nSiteIndex[k1] = -1;
			}
		}
		k++;
	}
}

CGameInfo* GetBetCandidate(CArrayGameInfo& aryGameInfo, BetInfo* pBetInfo, int nSiteIndex[], int nOddsKind, float fOVal[])
{
	CGameInfo* pGameInfo = NULL;
	int nObjCount = 0;

	char szHTeam[MAX_TEAM_NAME];
	char szATeam[MAX_TEAM_NAME];
	char szALeague[MAX_LEAGUE_NAME];
	strcpy_s(szHTeam, MAX_TEAM_NAME, "");
	strcpy_s(szATeam, MAX_TEAM_NAME, "");
	strcpy_s(szALeague, MAX_LEAGUE_NAME, "");
	
	for (int i = 0; i < 3; i++)
	{
		if (nSiteIndex[i] != -1 && g_site[nSiteIndex[i]]->m_hThread)
		{
			pBetInfo->BetVal[i].bValid = TRUE;
			pBetInfo->BetVal[i].nBookMaker = nSiteIndex[i];
			nObjCount++;
		}
		else
			pBetInfo->BetVal[i].bValid = FALSE;
	}
	if (nOddsKind != OI_MAIN)
		pBetInfo->BetVal[2].bValid = FALSE;

	for (int i = 0; i < aryGameInfo.GetSize(); i++)
	{
		pGameInfo = aryGameInfo[i];
		if (pGameInfo == NULL)
			continue;
		if (pGameInfo->m_MainInfo.m_nGameCategory != pBetInfo->nCategory)
			continue;

		if (pGameInfo->m_BetState.bIsTotalBettingComplete)
			continue;

		//if (strstr(pGameInfo->m_MainInfo.m_szHTName, "Toronto") == 0)
		//	continue;

		for (int j = 0; j < 3; j++)
		{
			if (pGameInfo->m_BetState.nWasPartialBetting[j] == PB_SUCCESS || pGameInfo->m_BetState.nWasPartialBetting[j] == PB_DONT_BET)
			{
				pBetInfo->BetVal[j].bValid = FALSE;
			}
			if (pGameInfo->m_BetState.nWasPartialBetting[j] == PB_DO_BET && pGameInfo->m_BetState.nBookMakers[j] != -1)
			{
				pBetInfo->nOddsKind = pGameInfo->m_BetState.nOddsKind;
				//fOVal[j] = pGameInfo->m_BetState.fOdds[j];
				CGameData* pGameData = pGameInfo->GetGameData(pBetInfo->BetVal[j].nBookMaker);
				if (pGameData == NULL)
					continue;
				int k;
				for (k = 0; k < pGameData->m_aryOddsInfo.GetSize(); k++)
				{
					COddsInfo* pOddsInfo = pGameData->m_aryOddsInfo[k];
					if (pOddsInfo->m_nOddsInfoKind != pGameInfo->m_BetState.nOddsKind)
						continue;
					if (pGameInfo->m_BetState.nOddsKind != OI_MAIN && pGameInfo->m_BetState.nOddsKind != OI_GOAL_OE &&
						pOddsInfo->m_hVal1 != pGameInfo->m_BetState.m_hVal)
						continue;

					if (pGameInfo->m_BetState.nOddsKind != OI_MAIN && pGameInfo->m_BetState.nOddsKind != OI_GOAL_OE)
						pBetInfo->hVal1 = pGameInfo->m_BetState.m_hVal;

					if (j == 0)
						fOVal[j] = pOddsInfo->m_oVal1;
					else if (j == 1)
						fOVal[j] = pOddsInfo->m_oVal2;
					else if (j == 2)
						fOVal[j] = pOddsInfo->fDraw;
					break;
				}
				if (k == pGameData->m_aryOddsInfo.GetSize())
					continue;
				return pGameInfo;
			}
		}
	}

	for (int i = 0; i < aryGameInfo.GetSize(); i++)
	{
		pGameInfo = aryGameInfo[i];
		/*if (strstr(pGameInfo->m_MainInfo.m_szHTName, "Feralpi") == 0)
		continue;*/
		if (pGameInfo->m_MainInfo.m_nGameCategory != pBetInfo->nCategory)
			continue;

		if (pGameInfo->m_BetState.bIsTotalBettingComplete)
			continue;

		//if (strstr(pGameInfo->m_MainInfo.m_szHTName, "Toronto") == 0)
		//	continue;

		int nObjCount1 = 0, nObjCount2 = 0, nObjCount3 = 0;
		for (int j = 0; j < 3; j++)
		{
			if (pBetInfo->BetVal[j].bValid && pGameInfo->m_BetState.nWasPartialBetting[j] == PB_DO_BET)
				nObjCount1++;
			CGameData* pGameData = pGameInfo->GetGameData(pBetInfo->BetVal[j].nBookMaker);
			if (pGameData == NULL)
				continue;
			if (pBetInfo->BetVal[j].bValid && pGameData)
				nObjCount2++;
		}
		if (nObjCount != nObjCount1 || nObjCount != nObjCount2)
			continue;

		for (int j = 0; j < 3; j++)
		{
			if (!pBetInfo->BetVal[j].bValid)
				continue;
			CGameData* pGameData = pGameInfo->GetGameData(pBetInfo->BetVal[j].nBookMaker);
			ASSERT(pGameData != 0);
			BOOL bFound = FALSE;
			for (int k = 0; k < pGameData->m_aryOddsInfo.GetSize(); k++)
			{
				COddsInfo* pOdds = pGameData->m_aryOddsInfo[k];
				if (pOdds->m_nOddsInfoKind != nOddsKind)
					continue;

				if (nOddsKind == OI_MAIN || nOddsKind == OI_GOAL_OE)
				{
					if (j == 0)
						fOVal[j] = pOdds->m_oVal1;
					else if (j == 1)
						fOVal[j] = pOdds->m_oVal2;
					else if (j == 2)
						fOVal[j] = pOdds->fDraw;
					bFound = TRUE;
					break;
				}
				else
				{
					if (pBetInfo->hVal1 == 0xFFFF)
					{
						pBetInfo->hVal1 = pOdds->m_hVal1;
						if (j == 0)
							fOVal[j] = pOdds->m_oVal1;
						else if (j == 1)
							fOVal[j] = pOdds->m_oVal2;
						else if (j == 2)
							fOVal[j] = pOdds->fDraw;
						bFound = TRUE;
						break;
					}
					else
					{
						if (pOdds->m_hVal1 == pBetInfo->hVal1)
						{
							if (j == 0)
								fOVal[j] = pOdds->m_oVal1;
							else if (j == 1)
								fOVal[j] = pOdds->m_oVal2;
							else if (j == 2)
								fOVal[j] = pOdds->fDraw;
							bFound = TRUE;
							break;
						}
					}
				}
			}
			if (bFound)
				nObjCount3++;
		}
		if (nObjCount != nObjCount3)
			continue;

		strcpy_s(szHTeam, MAX_TEAM_NAME, pGameInfo->m_MainInfo.m_szHTName);
		strcpy_s(szATeam, MAX_TEAM_NAME, pGameInfo->m_MainInfo.m_szATName);
		strcpy_s(szALeague, MAX_LEAGUE_NAME, pGameInfo->m_MainInfo.m_szLeague);
		if (strcmp(szHTeam, "") == 0 || strcmp(szATeam, "") == 0)
			continue;
		return pGameInfo;
	}
	return NULL;
}

BOOL SetGameInfoArray1(CArrayGameInfo& aryGameInfo, BetInfo* pBetInfo, int nBetStrategy, float* fBalance, BOOL bBettingState, BOOL& bBettingCompleted)
{
	int nOddsKind = GetRandInfo(OI_TEAM2_GOAL_OU) + 1;//GetRandInfo(OI_TEAM2_GOAL_OU + 1);
	pBetInfo->nCategory = (GetRandInfo(2) == 0) ? GC_FOOTBALL : GC_TENNIS;
	pBetInfo->nOddsKind = nOddsKind;
	pBetInfo->hVal1 = 0xFFFF;

	int nSiteIndex[BET_SITE_COUNT];
	for (int i = 0; i < BET_SITE_COUNT; i++)
		nSiteIndex[i] = -1;

	int nBettingMoney[BET_SITE_COUNT];
	nBettingMoney[0] = 10;
	nBettingMoney[1] = 10;
	nBettingMoney[2] = 2;
	nBettingMoney[3] = 10;
	nBettingMoney[4] = 10;

	////////////////
	nSiteIndex[0] = SITE_188bet;
	nSiteIndex[1] = SITE_18bet;
	nSiteIndex[2] = SITE_1xbetchina;
	nSiteIndex[3] = SITE_babibet;
	nSiteIndex[4] = SITE_bet888vip;
	////////////////
	AvailSiteProc(nSiteIndex, nBettingMoney, nOddsKind);

	float fOVal[3];
	CGameInfo* pGameInfo = GetBetCandidate(aryGameInfo, pBetInfo, nSiteIndex, nOddsKind, fOVal);
	if (pGameInfo == NULL)
		return FALSE;
	for (int k = 0; k < 3; k++)
	{
		if (pBetInfo->BetVal[k].bValid && pGameInfo->m_BetState.nWasPartialBetting[k] != PB_DONT_BET)
		{
			CGameData* pGameData = pGameInfo->GetGameData(pBetInfo->BetVal[k].nBookMaker);
			pBetInfo->BetVal[k].dwGameID = pGameData->m_nGameID;
			strcpy_s(pBetInfo->szHTeam, MAX_TEAM_NAME, pGameData->m_pGameInfo->m_MainInfo.m_szHTName);
			//if (strcmp(pBetInfo->szHTeam, "Astana") == 0)
			//	_asm int 3
			strcpy_s(pBetInfo->szATeam, MAX_TEAM_NAME, pGameData->m_pGameInfo->m_MainInfo.m_szATName);
			strcpy_s(pBetInfo->szLeague, MAX_LEAGUE_NAME, pGameData->m_pGameInfo->m_MainInfo.m_szLeague);
			pBetInfo->HTScore = pGameData->m_pGameInfo->m_MainInfo.m_HTScore;
			pBetInfo->ATScore = pGameData->m_pGameInfo->m_MainInfo.m_ATScore;
			if (pBetInfo->BetVal[k].dwGameID == -1)
				ASSERT(0);
			pBetInfo->BetVal[k].oVal = fOVal[k];
			pBetInfo->BetVal[k].nBettingMoney = nBettingMoney[k];
		}
	}
	return TRUE;
}
#endif

#ifdef BET_SUBTHREAD_MODE
struct BET_COMPLETION_KEY
{
	int nBookMaker;
	int nBetObjNo;
};
#endif

DWORD CMainFrame::BetThreadProc()
{
	CArrayGameInfo aryGameInfo;
	BOOL bBettingCompleted = FALSE;
	while (TRUE)
	{
		if (!m_bExitBetThreadFlg && bBettingCompleted)
			break;
		BOOL bFound = FALSE;
		aryGameInfo.RemoveAll();

		GetSystemTime(&g_EngineStartTime);

		::EnterCriticalSection(&g_csGameInfoArray);
		CopyGameInfo(g_aryGameInfo, aryGameInfo);
		::LeaveCriticalSection(&g_csGameInfoArray);

		g_pBetInfo->Init();
#ifdef NO_ENGINE
		if (SetGameInfoArray1(aryGameInfo, g_pBetInfo, m_nBetStrategy, g_fBalance, m_bExitBetThreadFlg, bBettingCompleted))
#else
		if (SetGameInfoArray(aryGameInfo, g_pBetInfo, m_nBetStrategy, g_fBalance, m_bExitBetThreadFlg, bBettingCompleted))
#endif
		{
			PostMessage(MSG_BETTING, 0);
			OutDbgStrAA("\nSetGameInfoArray");
			GetSystemTime(&g_BettingStartTime);
#ifdef BET_SUBTHREAD_MODE
			ResetEvent(m_hBettingCompleteEvent);
			for (int j = 0; j < 3; j++)
			{
				if (g_pBetInfo->BetVal[j].bValid)
				{
					g_pBetInfo->BetVal[j].oVal_orig = g_pBetInfo->BetVal[j].oVal;
					g_pBetInfo->BetVal[j].oVal_cur = g_pBetInfo->BetVal[j].oVal;
					m_nBetObjCount++;
				}
			}
			ResetEvent(m_hBettingCompleteEvent);
#endif
			for (int j = 0; j < 3; j++)
			{
				if (g_pBetInfo->BetVal[j].bValid)
				{
					int nBookMaker = g_pBetInfo->BetVal[j].nBookMaker;
#ifdef BET_SUBTHREAD_MODE
					BET_COMPLETION_KEY* pBetKey = new BET_COMPLETION_KEY;
					pBetKey->nBookMaker = nBookMaker;
					pBetKey->nBetObjNo = j;
					PostQueuedCompletionStatus(m_hCompletionPortBet, 0, (DWORD)pBetKey, NULL);
#else
					g_site[nBookMaker]->DoBetting(g_pBetInfo, j);
#endif
				}
			}
#ifdef BET_SUBTHREAD_MODE
			WaitForSingleObject(m_hBettingCompleteEvent, INFINITE);
#endif
			PostMessage(MSG_BETTING, 1);
			GetSystemTime(&g_BettingEndTime);
		}
		for (int i = 0; i < aryGameInfo.GetSize(); i++)
			delete aryGameInfo[i];
		aryGameInfo.RemoveAll();
	}
	return 0;
}

void CMainFrame::OnStartbetting()
{
	// TODO: Add your command handler code here
	m_bDoingBetting = !m_bDoingBetting;
	if (m_bDoingBetting) // start betting
	{
		m_bExitBetThreadFlg = FALSE;
		m_hBetTread = ::CreateThread(NULL, 0, GlobBetThreadProc, this, 0, NULL);
	}
	else // stop betting
	{
		m_bExitBetThreadFlg = TRUE;
		WaitForThreadToTerminate(m_hBetTread);
		m_hBetTread = NULL;
	}
}

void CMainFrame::OnUpdateStartbetting(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_bDoingBetting)
		pCmdUI->SetText(_T("Stop Betting"));
	else
		pCmdUI->SetText(_T("Start Betting"));
}

#ifdef BET_SUBTHREAD_MODE
DWORD CMainFrame::SubBetThreadProc()
{
	ULONG CompletionKey;
	DWORD NumBytesSent = 0;
	LPOVERLAPPED ol;
	OutDbgStrAA("\nGetQueuedCompletionStatus0");
	while (!m_bExitBetSubThreadFlg)
	{
		if (!GetQueuedCompletionStatus(m_hCompletionPortBet, &NumBytesSent, &CompletionKey, &ol, 200))
			continue;
		OutDbgStrAA("\nGetQueuedCompletionStatus");
		BET_COMPLETION_KEY* pKey = (BET_COMPLETION_KEY*)CompletionKey;
		if (pKey == NULL)
			break;
		g_site[pKey->nBookMaker]->DoBetting(g_pBetInfo, pKey->nBetObjNo);
		OutDbgStrAA("\nGetQueuedCompletionStatus2");
		delete pKey;
	}
	OutDbgStrAA("\nGetQueuedCompletionStatus10");
	return 0;
}
#endif

void CMainFrame::OnUpdateEngineTime(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString strPage;
	strPage.Format(_T("Engine Time : "));
	pCmdUI->SetText(strPage);
}

void CMainFrame::OnUpdateBettingTime(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString strPage;
	strPage.Format(_T("Betting Time : "));
	pCmdUI->SetText(strPage);
}

void CMainFrame::OnClearbetinfo()
{
	CGameInfo* pGameInfo;
	::EnterCriticalSection(&g_csGameInfoArray);
	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		pGameInfo = g_aryGameInfo[i];
		if (pGameInfo == NULL)
			continue;

		pGameInfo->m_BetState.init();
	}
	::LeaveCriticalSection(&g_csGameInfoArray);
}
