
#include "..\stdafx.h"

#include "TeamNameWnd.h"
#include "..\Resource.h"
#include "..\MainFrm.h"
#include "..\SportsBetting.h"
#include "..\global.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar
CTeamNameWnd::CTeamNameWnd()
{
	m_nComboHeight = 0;
	m_aryTeamName.RemoveAll();
}

CTeamNameWnd::~CTeamNameWnd()
{
	for (int i = 0; i < m_aryTeamName.GetSize(); i++)
	{
		if (m_aryTeamName[i])
		{
			delete m_aryTeamName[i];
			m_aryTeamName[i] = NULL;
		}
	}
	m_aryTeamName.RemoveAll();
}

BEGIN_MESSAGE_MAP(CTeamNameWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_SAVE, OnSave)
	ON_UPDATE_COMMAND_UI(ID_SAVE, OnUpdateSave)
	ON_COMMAND(ID_YES, OnYes)
	ON_UPDATE_COMMAND_UI(ID_YES, OnUpdateYes)
	ON_COMMAND(ID_NO, OnNo)
	ON_UPDATE_COMMAND_UI(ID_NO, OnUpdateNo)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnTooltipHandler)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CTeamNameWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndTeamNameList.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CTeamNameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

	m_wndObjectCombo.AddString(_T("Compare Team Name"));
	m_wndObjectCombo.AddString(_T("Equal Team Names"));
	m_wndObjectCombo.SetCurSel(0);

	CRect rectCombo;
	m_wndObjectCombo.GetClientRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();

	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LVS_REPORT;
	if (!m_wndTeamNameList.Create(dwStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetShowTooltips(true);
	m_wndToolBar.EnableToolTips(true);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

BOOL CTeamNameWnd::OnTooltipHandler(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT nID = static_cast<int>(pNMHDR->idFrom);
	CString strTip = _T("Tool");
	CString toolTips[] = {_T("Yes_Save"), _T("Yes"), _T("No"), _T("Delete")};
	int size = sizeof(toolTips);
	strTip = ((int)nID > size / 4)? _T("Tool Tip Here"): toolTips[nID - 1];

	wcscpy_s(pTTT->szText, strTip.GetBuffer(0));
	return TRUE;
}

void CTeamNameWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CTeamNameWnd::OnSave()
{
	OnYes();

	char szLine[MAX_TEAM_NAME * 3];
	FILE* fp = NULL;
	fopen_s(&fp, "./teamnames.ini", "w");
	if (fp == NULL)
		return;
	::EnterCriticalSection(&g_csSameName);
	CStringA slave;
	POSITION pos = g_mapName.GetStartPosition();
	while (pos != NULL)
	{
		CStringA master;
		g_mapName.GetNextAssoc(pos, slave, master);
		if (slave == master)
			continue;
		sprintf_s(szLine, "%s:%s\n", (const char*)slave, (const char*)master);
		fputs(szLine, fp);
	}
	::LeaveCriticalSection(&g_csSameName);
	fclose(fp);
}

void CTeamNameWnd::OnUpdateSave(CCmdUI* /* pCmdUI */)
{
}

void CTeamNameWnd::OnYes()
{
	POSITION pos = m_wndTeamNameList.GetFirstSelectedItemPosition();
	int nIndex = m_wndTeamNameList.GetNextSelectedItem(pos);
	if (nIndex == -1)
		return;

	::EnterCriticalSection(&g_csTeamNameWnd);
	int nPos = (int)m_wndTeamNameList.GetItemData(nIndex);
	ASSERT(m_aryTeamName[nPos] != NULL);
	TEAMNAME* pTeamName = new TEAMNAME(*m_aryTeamName[nPos]);
	::LeaveCriticalSection(&g_csTeamNameWnd);

	::EnterCriticalSection(&g_csSameName);
	if (pTeamName->info1.m_bMaster)
	{
		SetSlaveName(pTeamName->info2.m_szhtName, pTeamName->info1.m_szhtName);
		SetSlaveName(pTeamName->info2.m_szatName, pTeamName->info1.m_szatName);
	}
	else if (pTeamName->info2.m_bMaster)
	{
		SetSlaveName(pTeamName->info1.m_szhtName, pTeamName->info2.m_szhtName);
		SetSlaveName(pTeamName->info1.m_szatName, pTeamName->info2.m_szatName);
	}
	else
		ASSERT(0);

	::LeaveCriticalSection(&g_csSameName);
	delete pTeamName;

	::EnterCriticalSection(&g_csTeamNameWnd);
	m_wndTeamNameList.DeleteItem(nIndex);
	delete m_aryTeamName[nPos];
	m_aryTeamName[nPos] = NULL;
	::LeaveCriticalSection(&g_csTeamNameWnd);
}

void CTeamNameWnd::OnUpdateYes(CCmdUI* pCmdUI)
{
}

void CTeamNameWnd::OnNo()
{
	POSITION pos = m_wndTeamNameList.GetFirstSelectedItemPosition();
	int nIndex = m_wndTeamNameList.GetNextSelectedItem(pos);
	if (nIndex == -1)
		return;

	::EnterCriticalSection(&g_csTeamNameWnd);
	int nPos = (int)m_wndTeamNameList.GetItemData(nIndex);
	ASSERT(m_aryTeamName[nPos] != NULL);
	TEAMNAME* pTeamName = new TEAMNAME(*m_aryTeamName[nPos]);
	::LeaveCriticalSection(&g_csTeamNameWnd);

	::EnterCriticalSection(&g_csSameName);
	if (strcmp(pTeamName->info1.m_szhtName, pTeamName->info2.m_szhtName) != 0)
	{
		NamePair* pNamePair1 = new NamePair;
		strcpy_s(pNamePair1->strTeam1, MAX_TEAM_NAME, pTeamName->info1.m_szhtName);
		strcpy_s(pNamePair1->strTeam2, MAX_TEAM_NAME, pTeamName->info2.m_szhtName);
		g_aryDiffNamePair.Add(pNamePair1);
	}
	if (strcmp(pTeamName->info1.m_szatName, pTeamName->info2.m_szatName) != 0)
	{
		NamePair* pNamePair2 = new NamePair;
		strcpy_s(pNamePair2->strTeam1, MAX_TEAM_NAME, pTeamName->info1.m_szatName);
		strcpy_s(pNamePair2->strTeam2, MAX_TEAM_NAME, pTeamName->info2.m_szatName);
		g_aryDiffNamePair.Add(pNamePair2);
	}
	::LeaveCriticalSection(&g_csSameName);
	delete pTeamName;

	::EnterCriticalSection(&g_csTeamNameWnd);
	m_wndTeamNameList.DeleteItem(nIndex);
	delete m_aryTeamName[nPos];
	m_aryTeamName[nPos] = NULL;
	::LeaveCriticalSection(&g_csTeamNameWnd);
}

void CTeamNameWnd::OnUpdateNo(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CTeamNameWnd::OnDelete()
{
	::EnterCriticalSection(&g_csTeamNameWnd);
	while (m_wndTeamNameList.GetItemCount() > 0)
	{
		int nPos = (int)m_wndTeamNameList.GetItemData(0);
		m_wndTeamNameList.DeleteItem(0);
		delete m_aryTeamName[nPos];
		m_aryTeamName[nPos] = NULL;
	}
	::LeaveCriticalSection(&g_csTeamNameWnd);
}

void CTeamNameWnd::OnUpdateDelete(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CTeamNameWnd::InitPropList()
{
	LVCOLUMN list;
	list.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
	list.fmt = LVCFMT_LEFT;
	list.cx = 100;
	list.pszText = _T("Category");
	list.iSubItem = 0;
	//Inserts the column
	m_wndTeamNameList.InsertColumn(0, &list);

	list.cx = 300;
	list.pszText = _T("Team1/Players1");
	list.iSubItem = 1;
	//Inserts the column
	m_wndTeamNameList.InsertColumn(1, &list);

	list.cx = 300;
	list.pszText = _T("Team2/Players2");
	list.iSubItem = 2;
	m_wndTeamNameList.InsertColumn(2, &list);

	ListView_SetExtendedListViewStyle(m_wndTeamNameList.GetSafeHwnd(), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CTeamNameWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndTeamNameList.SetFocus();
}

void CTeamNameWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CTeamNameWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndTeamNameList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}

void CTeamNameWnd::AddItem(BOOL bHAChanged, CMainInfo& info1, CMainInfo& info2)
{
	ASSERT(info1.m_nGameCategory == info2.m_nGameCategory);
	TEAMNAME* tn = new TEAMNAME;
	tn->bHAChanged = bHAChanged;
	tn->info1 = info1;
	tn->info2 = info2;
	PostMessage(MSG_ADD_TEAMNAME, (WPARAM)tn);
}

BOOL CTeamNameWnd::IsExistItem(CMainInfo& info)
{
	int nGameCategory = info.m_nGameCategory;
	char* ht = info.m_szhtName;
	char* at = info.m_szatName;
	TEAMNAME* tn = new TEAMNAME;
	for (int i = 0; i < m_aryTeamName.GetSize(); i++)
	{
		::EnterCriticalSection(&g_csTeamNameWnd);
		if (m_aryTeamName[i] == NULL)
		{
			::LeaveCriticalSection(&g_csTeamNameWnd);
			continue;
		}

		ASSERT(m_aryTeamName[i] != NULL);
		*tn = *m_aryTeamName[i];
		::LeaveCriticalSection(&g_csTeamNameWnd);

		if (tn->info1.m_nGameCategory == nGameCategory)
		{
			if ((strcmp(info.m_szhtName, tn->info1.m_szhtName) == 0 && strcmp(info.m_szatName, tn->info1.m_szatName) == 0) ||
				(strcmp(info.m_szhtName, tn->info2.m_szhtName) == 0 && strcmp(info.m_szatName, tn->info2.m_szatName) == 0) ||
				(strcmp(info.m_szhtName, tn->info1.m_szatName) == 0 && strcmp(info.m_szatName, tn->info1.m_szhtName) == 0) ||
				(strcmp(info.m_szhtName, tn->info1.m_szatName) == 0 && strcmp(info.m_szatName, tn->info1.m_szhtName) == 0))
			//if (IsEqualGameName(info, tn->info1, 0, 0) || IsEqualGameName(info, tn->info2, 0, 0))
			{
				delete tn;
				return TRUE;
			}
		}
	}
	delete tn;
	return FALSE;
}

LRESULT CTeamNameWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
	case MSG_ADD_TEAMNAME:
	{
		TEAMNAME* tn = (TEAMNAME*)wParam;
		int nCount = m_wndTeamNameList.GetItemCount();

		TCHAR wht1[MAX_TEAM_NAME], wat1[MAX_TEAM_NAME], wht2[MAX_TEAM_NAME], wat2[MAX_TEAM_NAME], wLeague1[MAX_LEAGUE_NAME], wLeague2[MAX_LEAGUE_NAME];
		::ToUnicode(tn->info1.m_szHTName, wht1, CP_ACP);
		::ToUnicode(tn->info1.m_szATName, wat1, CP_ACP);
		::ToUnicode(tn->info2.m_szHTName, wht2, CP_ACP);
		::ToUnicode(tn->info2.m_szATName, wat2, CP_ACP);
		::ToUnicode(tn->info1.m_szLeague, wLeague1, CP_ACP);
		::ToUnicode(tn->info2.m_szLeague, wLeague2, CP_ACP);
		CString str1, str2;
		str1.Format(_T("[%d:%d]%s:%s[%s]"), tn->info1.m_HTScore.nScore, tn->info1.m_ATScore.nScore, wht1, wat1, wLeague1);
		str2.Format(_T("[%d:%d]%s:%s[%s]"), tn->info2.m_HTScore.nScore, tn->info2.m_ATScore.nScore, wht2, wat2, wLeague2);

		SetListItemText(m_wndTeamNameList, 0, nCount, g_wszGameCategory[tn->info1.m_nGameCategory]);
		SetListItemText(m_wndTeamNameList, 1, nCount, str1);
		SetListItemText(m_wndTeamNameList, 2, nCount, str2);
		int nPos = -1;
		::EnterCriticalSection(&g_csTeamNameWnd);
		for (int i = 0; i < m_aryTeamName.GetSize(); i ++)
		{
			if (m_aryTeamName[i] == NULL)
			{
				nPos = i;
				break;
			}
		}
		if (nPos != -1)
			m_aryTeamName[nPos] = tn;
		else
		{
			nPos = m_aryTeamName.GetSize();
			m_aryTeamName.Add(tn);
		}
		::LeaveCriticalSection(&g_csTeamNameWnd);

		m_wndTeamNameList.SetItemData(nCount, (DWORD_PTR)nPos);
	}
		break;
	default:
		break;
	}
	return CDockablePane::WindowProc(message, wParam, lParam);
}
