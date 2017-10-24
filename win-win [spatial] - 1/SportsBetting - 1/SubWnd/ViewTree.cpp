
#include "..\stdafx.h"
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
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