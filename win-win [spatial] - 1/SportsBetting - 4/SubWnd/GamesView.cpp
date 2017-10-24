
#include "..\stdafx.h"
#include "..\mainfrm.h"
#include "GamesView.h"
#include "..\Resource.h"
#include "..\SportsBetting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CGamesView

CGamesView::CGamesView()
{
}

CGamesView::~CGamesView()
{
}

BEGIN_MESSAGE_MAP(CGamesView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CGamesView::OnSelChangedTree1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CGamesView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

	if (!m_wndGamesView.Create(dwViewStyle, rectDummy, this, IDC_TREE1))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	theFrame->m_treeGameInfo = &m_wndGamesView;

	// Load view images:
	m_GamesViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndGamesView.SetImageList(&m_GamesViewImages, TVSIL_NORMAL);

	OnChangeVisualStyle();

	// Fill in some static tree view data (dummy code, nothing magic here)
	InitTreeViewControl();
	AdjustLayout();

	return 0;
}

void CGamesView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CGamesView::InitTreeViewControl()
{
	CString str;
	for (int i = 0; i < GC_COUNT; i++)
	{
		str.Format(_T("%s(0)"), GetGameCategory(i));
		int nImageNo = GetImageNo(i);
		theFrame->m_hItemCategory[i] = m_wndGamesView.InsertItem(str, nImageNo, nImageNo, TVI_ROOT);
	}
}

void CGamesView::OnSelChangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pnmtv1 = (LPNMTREEVIEW)pNMHDR;

	HTREEITEM hItem = theFrame->m_treeGameInfo->GetSelectedItem();
	if (hItem)
	{
		CGameInfo* pGameInfo = (CGameInfo*)theFrame->m_treeGameInfo->GetItemData(hItem);
		if (pGameInfo)
		{
			if (theFrame->m_treeGameInfo->GetParentItem(hItem) != NULL) // No Category
			{
				theFrame->m_hCurrentSelGame = hItem;
				theFrame->UpdateList(pGameInfo);
			}
		}
	}
	*pResult = 0;
}

void CGamesView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndGamesView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CGamesView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndGamesView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CGamesView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndGamesView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CGamesView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndGamesView.SetFocus();
}

void CGamesView::OnChangeVisualStyle()
{
	m_GamesViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_GamesViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_GamesViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndGamesView.SetImageList(&m_GamesViewImages, TVSIL_NORMAL);
}

