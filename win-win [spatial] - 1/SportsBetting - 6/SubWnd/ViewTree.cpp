
#include "..\stdafx.h"
#include "ViewTree.h"
#include "..\resource.h"
#include "..\global.h"
#include "..\MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
	m_hActiveItem = NULL;
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CViewTree::OnNMRClick)
	ON_COMMAND(ID_REMOVE, &CViewTree::OnRemove)
	ON_COMMAND(ID_SET_INVALID, &CViewTree::OnSetInvalid)
	ON_COMMAND(ID_SET_VALID, &CViewTree::OnSetValid)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

HTREEITEM CViewTree::GetInsertPos(CString strGameName, HTREEITEM hTreeParentItem)
{
	HTREEITEM hNextItem;
	if (!ItemHasChildren(hTreeParentItem))
		return TVI_LAST;

	HTREEITEM hChildItem = GetChildItem(hTreeParentItem);

	while (hChildItem != NULL)
	{
		CString str1 = GetItemText(hChildItem);
		if (strGameName < str1)
		{
			HTREEITEM hPrevItem = GetPrevSiblingItem(hChildItem);
			if (hPrevItem == NULL)
				return TVI_FIRST;
			else
				return hPrevItem;
		}

		hNextItem = GetNextItem(hChildItem, TVGN_NEXT);
		if (hNextItem == NULL)
			return TVI_LAST;

		CString str2 = GetItemText(hNextItem);
		if (strGameName < str2)
			return hChildItem;

		hChildItem = hNextItem;
	}
	return TVI_LAST;
}

void CViewTree::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACE0("CMyTreeCtrl::OnRClick()n");

	// Send WM_CONTEXTMENU to self 

	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());

	// Mark message as handled and suppress default handling 

	*pResult = 1;
}

void CViewTree::OnContextMenu(CWnd* pWnd, CPoint ptMousePos)
{
	// if Shift-F10 
	if (ptMousePos.x == -1 && ptMousePos.y == -1)
		ptMousePos = (CPoint)GetMessagePos();

	ScreenToClient(&ptMousePos);

	UINT uFlags;
	HTREEITEM htItem;

	htItem = HitTest(ptMousePos, &uFlags);

	if (htItem == NULL)
		return;

	for (int nCate = 0; nCate < GC_COUNT; nCate++)
	{
		if (htItem == theFrame->m_hItemCategory[nCate])
			return;
	}

	m_hActiveItem = htItem;

	CMenu menu;
	CMenu* pPopup;

	// the font popup is stored in a resource 
	menu.LoadMenu(IDR_POPUP_EXPLORER);
	pPopup = menu.GetSubMenu(0);
	ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, this);
}

void CViewTree::OnRemove()
{
	// TODO: Add your command handler code here
	CGameInfo* pGameInfo = (CGameInfo*)theFrame->m_treeGameInfo->GetItemData(m_hActiveItem);

	::EnterCriticalSection(&g_csGameInfoArray);
	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pGameInfo1 = g_aryGameInfo[i];
		if (pGameInfo == NULL)
			continue;
		if (pGameInfo == pGameInfo1)
		{
			delete pGameInfo;
			g_aryGameInfo[i] = NULL;
			theFrame->m_treeGameInfo->DeleteItem(m_hActiveItem);
			m_hActiveItem = NULL;
			break;
		}
	}
	::LeaveCriticalSection(&g_csGameInfoArray);
}

void CViewTree::OnSetInvalid()
{
	// TODO: Add your command handler code here
	CGameInfo* pGameInfo = (CGameInfo*)theFrame->m_treeGameInfo->GetItemData(m_hActiveItem);

	::EnterCriticalSection(&g_csGameInfoArray);
	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pGameInfo1 = g_aryGameInfo[i];
		if (pGameInfo == NULL)
			continue;
		if (pGameInfo == pGameInfo1)
		{
//			pGameInfo->m_bValid = FALSE;
			break;
		}
	}
	::LeaveCriticalSection(&g_csGameInfoArray);
}

void CViewTree::OnSetValid()
{
	// TODO: Add your command handler code here
	CGameInfo* pGameInfo = (CGameInfo*)theFrame->m_treeGameInfo->GetItemData(m_hActiveItem);

	::EnterCriticalSection(&g_csGameInfoArray);
	for (int i = 0; i < g_aryGameInfo.GetSize(); i++)
	{
		CGameInfo* pGameInfo1 = g_aryGameInfo[i];
		if (pGameInfo == NULL)
			continue;
		if (pGameInfo == pGameInfo1)
		{
//			pGameInfo->m_bValid = TRUE;
			break;
		}
	}
	::LeaveCriticalSection(&g_csGameInfoArray);
}


