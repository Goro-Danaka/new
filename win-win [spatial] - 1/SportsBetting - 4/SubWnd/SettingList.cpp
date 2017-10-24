
#include "..\stdafx.h"

#include "SettingList.h"
#include "..\Resource.h"
#include "..\MainFrm.h"
#include "..\global.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingList1

CSettingList::CSettingList()
{
}

CSettingList::~CSettingList()
{
}

BEGIN_MESSAGE_MAP(CSettingList, CListCtrl)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSettingList message handlers

void CSettingList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}

CEditList::CEditList()
:m_FocusCell(-1)
, m_pEditor(NULL)
{
	m_nTabIndex = -1;
	m_aryType.RemoveAll();
}

CEditList::~CEditList()
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		char* p = (char*)GetItemData(i);
		if (p)
			delete p;
	}
	m_aryType.RemoveAll();
}

BEGIN_MESSAGE_MAP(CEditList, CListCtrl)
	ON_WM_LBUTTONDOWN()	// OnLButtonDown(UINT nFlags, CPoint point)
	ON_WM_LBUTTONUP()	// OnLButtonUp(UINT nFlags, CPoint point)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSettingList message handlers

void CEditList::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Find out what subitem was clicked
	int nRow, nCol;
	CellHitTest(point, nRow, nCol);

	// If not left-clicking on an actual row, then don't update focus cell
	if (nRow == -1)
	{
		CListCtrl::OnLButtonUp(nFlags, point);
		return;
	}

	if (GetFocus() != this)
		SetFocus();	// Force focus to finish editing

	UINT nFlag;
	int nItem = HitTest(point, &nFlag);
	if (nFlag == LVHT_ONITEMSTATEICON)
	{
		BOOL bCheck = GetCheck(nRow);
		
		if (m_nTabIndex == 0)
		{
			CString strID = GetItemText(nRow, 1);
			char szID[MAX_ID];
			::ToAscii((LPCTSTR)strID, szID, CP_ACP);
			char* szPswd = (char*)GetItemData(nRow);

			theFrame->StartThread(nRow, bCheck, szID, szPswd);
		}
		else if (m_nTabIndex == 1)
			theFrame->SetCategory(nRow, bCheck);
		else if (m_nTabIndex == 2)
		{
			for (int i = 0; i < BS_COUNT; i++)
			{
				if (i == nRow)
				{
					theFrame->SetBetStrategy(nRow);
					SetCheck(i, 1);
				}
				else
					SetCheck(i, 0);
			}
		}
	}

	CListCtrl::OnLButtonUp(nFlags, point);
	return;
}

void CEditList::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Find out what subitem was clicked
	int nRow, nCol;
	CellHitTest(point, nRow, nCol);

	// If not left-clicking on an actual row, then don't update focus cell
	if (nRow == -1)
	{
		CListCtrl::OnLButtonDown(nFlags, point);
		return;
	}

	if (GetFocus() != this)
		SetFocus();	// Force focus to finish editing

	int startEdit = OnClickEditStart(nRow, nCol, point, false);
	if (startEdit != 2)
	{
		// Update the focused cell before calling CListCtrl::OnLButtonDown()
		// as it might cause a row-repaint
		SetFocusCell(nCol);

		CListCtrl::OnLButtonDown(nFlags, point);

		// LVN_BEGINDRAG message can be fired when calling parent OnLButtonDown(),
		// this should not result in a start edit operation
		if (GetFocusCell() != nCol)
		{
			SetFocusCell(nCol);
			startEdit = 0;
		}

		// CListCtrl::OnLButtonDown() doesn't change row if clicking on subitem without fullrow selection
		if (!(GetExtendedStyle() & LVS_EX_FULLROWSELECT))
		{
			if (nRow != GetFocusRow())
			{
				SetFocusRow(nRow);
				if (!(GetKeyState(VK_CONTROL) < 0) && !(GetKeyState(VK_SHIFT) < 0))
				{
					SelectRow(-1, false);
					SelectRow(nRow, true);
				}
			}
		}

		// CListCtrl::OnLButtonDown() doesn't always cause a row-repaint
		// call our own method to ensure the row is repainted
		SetFocusCell(nCol, true);
	}

	if (startEdit != 0)
	{
		// This will steal the double-click event when double-clicking a cell that already have focus,
		// but we cannot guess after the first click, whether the user will click a second time.
		// A timer could be used but it would cause slugish behavior (http://blogs.msdn.com/oldnewthing/archive/2004/10/15/242761.aspx)
		EditCell(nRow, nCol, point);
	}
}

UINT CEditList::CellHitTest(const CPoint& pt, int& nRow, int& nCol) const
{
	nRow = -1;
	nCol = -1;

	LVHITTESTINFO lvhti = { 0 };
	lvhti.pt = pt;
	nRow = ListView_SubItemHitTest(m_hWnd, &lvhti);	// SubItemHitTest is non-const
	nCol = lvhti.iSubItem;
	if (!(lvhti.flags & LVHT_ONITEM))
		nRow = -1;

	return lvhti.flags;
}

void CEditList::SetFocusCell(int nCol, BOOL bRedraw)
{
	m_FocusCell = nCol;
	if (bRedraw)
	{
		int nFocusRow = GetFocusRow();
		if (nFocusRow >= 0)
		{
			CRect itemRect;
			VERIFY(GetItemRect(nFocusRow, itemRect, LVIR_BOUNDS));
			InvalidateRect(itemRect);
			UpdateWindow();
		}
	}
}

int CEditList::GetFocusRow() const
{
	return GetNextItem(-1, LVNI_FOCUSED);
}

void CEditList::SetFocusRow(int nRow)
{
	SetItemState(nRow, LVIS_FOCUSED, LVIS_FOCUSED);
}

BOOL CEditList::SelectRow(int nRow, bool bSelect)
{
	return SetItemState(nRow, (UINT)(bSelect ? LVIS_SELECTED : 0), LVIS_SELECTED);
}

CWnd* CEditList::EditCell(int nRow, int nCol, CPoint pt)
{
	if (nCol == -1 || nRow == -1)
		return NULL;

	m_pEditor = OnEditBegin(nRow, nCol, pt);
	if (m_pEditor == NULL)
		return NULL;

	m_pEditor->SetFont(&afxGlobalData.fontRegular);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo = { 0 };
	dispinfo.hdr.hwndFrom = m_hWnd;
	dispinfo.hdr.idFrom = (UINT_PTR)GetDlgCtrlID();
	dispinfo.hdr.code = LVN_BEGINLABELEDIT;

	dispinfo.item.mask = LVIF_PARAM;
	dispinfo.item.iItem = nRow;
	dispinfo.item.iSubItem = nCol;
	dispinfo.item.lParam = (LPARAM)GetItemData(nRow);
	if (GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&dispinfo) == TRUE)
	{
		// Parent didn't want to start edit
		OnEditComplete(nRow, nCol, NULL, NULL);
		m_pEditor->PostMessage(WM_CLOSE);
		m_pEditor = NULL;
		return NULL;
	}

	// Show editor
	m_pEditor->ShowWindow(SW_SHOWNORMAL);
	m_pEditor->SetFocus();
	return m_pEditor;
}

CWnd* CEditList::OnEditBegin(int nRow, int nCol)
{
	return OnEditBegin(nRow, nCol, CPoint(-1, -1));
}

CWnd* CEditList::OnEditBegin(int nRow, int nCol, CPoint pt)
{
	int nCount = GetItemCount();
	if (nRow >= nCount)
		return NULL;
	if (m_aryType.GetSize() == 0)
		return NULL;
	if (m_aryType[nCol] == CT_NONE)
		return NULL;

	// Get position of the cell to edit
	CRect rectCell = GetCellEditRect(nRow, nCol);

	// Get the text-style of the cell to edit
	DWORD dwStyle = ES_AUTOHSCROLL | ES_NOHIDESEL | WS_BORDER;
	HDITEM hditem = { 0 };
	hditem.mask = HDI_FORMAT;
	VERIFY(GetHeaderCtrl()->GetItem(nCol, &hditem));
	if (hditem.fmt & HDF_CENTER)
		dwStyle |= ES_CENTER;
	else if (hditem.fmt & HDF_RIGHT)
		dwStyle |= ES_RIGHT;
	else
		dwStyle |= ES_LEFT;

	LOGFONT lf = { 0 };
	GetFont()->GetLogFont(&lf);
	if (static_cast<HFONT>(m_CellFont))
		VERIFY(m_CellFont.DeleteObject());
	VERIFY(m_CellFont.CreateFontIndirect(&lf));

	CEdit* pEdit = NULL;
	if (m_aryType[nCol] == CT_EDIT)
		pEdit = CreateEdit(nRow, nCol, dwStyle, rectCell, m_CellFont);
	else if (m_aryType[nCol] == CT_PSWD)
		pEdit = CreatePswdEdit(nRow, nCol, dwStyle, rectCell, m_CellFont);
	//VERIFY(pEdit != NULL);
	if (pEdit == NULL)
		return NULL;

	// Configure font
	pEdit->SetFont(GetCellFont());

	// First column (Label) doesn't have a margin when imagelist is assigned
	if (nCol == 0 && GetImageList(LVSIL_SMALL) != NULL)
		pEdit->SetMargins(0, 0);
	else
		// First column (Label) doesn't have a margin when checkboxes are enabled
	if (nCol == 0 && GetExtendedStyle() & LVS_EX_CHECKBOXES)
		pEdit->SetMargins(1, 0);
	else
		// Label column doesn't have margin when not first in column order
	if (nCol == 0 && GetFirstVisibleColumn() != nCol)
		pEdit->SetMargins(1, 0);
	else
	if (dwStyle & ES_CENTER)
		pEdit->SetMargins(0, 0);
	else
	if (dwStyle & ES_RIGHT)
		pEdit->SetMargins(0, 7);
	else
		pEdit->SetMargins(4, 0);

	pEdit->SetLimitText(UINT_MAX);

	CString cellText = GetItemText(nRow, nCol);
	pEdit->SetWindowText(cellText);
	pEdit->SetSel(0, -1, 0);
	return pEdit;
}

CFont* CEditList::GetCellFont()
{
	LOGFONT lf = { 0 };
	GetFont()->GetLogFont(&lf);
	if (static_cast<HFONT>(m_CellFont))
		VERIFY(m_CellFont.DeleteObject());
	VERIFY(m_CellFont.CreateFontIndirect(&lf));

	return &m_CellFont;
}

BOOL CEditList::OnEditComplete(int nRow, int nCol, CWnd* pEditor, LV_DISPINFO* pLVDI)
{
	return TRUE;
}

int CEditList::OnClickEditStart(int nRow, int nCol, CPoint pt, bool bDblClick)
{
	if (GetKeyState(VK_CONTROL) < 0)
		return 0;	// Row selection should not trigger cell edit
	if (GetKeyState(VK_SHIFT) < 0)
		return 0;	// Row selection should not trigger cell edit

	int nCount = GetItemCount();
	if (nRow >= nCount)
		return NULL;
	if (m_aryType.GetSize() == 0 || m_aryType[nCol] == CT_NONE)
		return NULL;

	return 1;
}

void CEditList::InsertColumnEx(int nCol, LPCTSTR szText, int nWidth, int nType)
{
	LVCOLUMN list;
	list.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
	list.fmt = LVCFMT_LEFT;
	list.cx = nWidth;
	list.pszText = (LPWSTR)szText;
	list.iSubItem = nCol;
	//Inserts the column
	InsertColumn(nCol, &list);

	if (nType == CT_NONE)
		return;

	if (nCol >= m_aryType.GetSize())
		m_aryType.SetAtGrow(nCol, nType);
	else
		m_aryType.SetAt(nCol, nType);
}

CRect CEditList::GetCellEditRect(int nRow, int nCol)
{
	// Get position of the cell to edit
	CRect rectCell;
	VERIFY(GetCellRect(nRow, nCol, LVIR_LABEL, rectCell));

	// Adjust cell rectangle according to grid-lines
	if (GetExtendedStyle() & LVS_EX_GRIDLINES)
		rectCell.bottom -= ::GetSystemMetrics(SM_CXBORDER);

	if (nCol == 0 && GetImageList(LVSIL_SMALL) != NULL)
	{
		// Add margin to cell image
		rectCell.left += ::GetSystemMetrics(SM_CXBORDER);
	}
	else
	{
		// Overlap the focus rectangle, unless we are first in column order
		if (nCol != GetFirstVisibleColumn())
			rectCell.left -= ::GetSystemMetrics(SM_CXBORDER);
	}

	// Check if there is enough room for normal margin
	int requiredHeight = GetCellFontHeight();
	requiredHeight += 2 * ::GetSystemMetrics(SM_CXEDGE);
	if (requiredHeight > rectCell.Height())
		rectCell.bottom = rectCell.top + requiredHeight;

	return rectCell;
}

BOOL CEditList::GetCellRect(int nRow, int nCol, int nCode, CRect& rect)
{
	if (GetSubItemRect(nRow, nCol, nCode, rect) == FALSE)
		return FALSE;

	if (nCode == LVIR_BOUNDS)
	{
		// Find the left and right of the cell-rectangle using the CHeaderCtrl
		CRect colRect;
		if (GetHeaderCtrl()->GetItemRect(nCol, colRect) == FALSE)
			return FALSE;

		if (nCol == 0)
		{
			// Fix bug where LVIR_BOUNDS gives the entire row for nCol==0
			CRect labelRect;
			if (GetSubItemRect(nRow, nCol, LVIR_LABEL, labelRect) == FALSE)
				return FALSE;

			rect.right = labelRect.right;
			rect.left = labelRect.right - colRect.Width();
		}
		else
		{
			// Fix bug when width is smaller than subitem image width
			rect.right = rect.left + colRect.Width();
		}
	}

	if (nCode == LVIR_ICON)
	{
		if (nCol > 0 && !(GetExtendedStyle() & LVS_EX_SUBITEMIMAGES))
			return FALSE;	// no image in subitem

		return TRUE;
	}

	if (nCode == LVIR_LABEL)
	{
		if (nCol > 0)
		{
			if (!(GetExtendedStyle() & LVS_EX_SUBITEMIMAGES))
				return TRUE;	// no image in subitem

			if (GetImageList(LVSIL_SMALL) == NULL)
				return TRUE;	// no image in subitem

			CRect iconRect;
			if (GetSubItemRect(nRow, nCol, LVIR_ICON, iconRect) == FALSE)
				return FALSE;

			rect.left += iconRect.Width();
		}
		else
		{
			if (GetExtendedStyle() & LVS_EX_CHECKBOXES)
				return TRUE;	// First column gets correct margin when imagelist is assigned

			if (GetImageList(LVSIL_SMALL) != NULL)
				return TRUE;	// First column gets correct margin when checkboxes are enabled

			// Label column gets extra margin when no subitem images or checkbox (Remove this)
			return GetCellRect(nRow, nCol, LVIR_BOUNDS, rect);
		}
	}

	return TRUE;
}

int CEditList::GetFirstVisibleColumn()
{
	int nColCount = GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColCount; ++i)
	{
		int nCol = GetHeaderCtrl()->OrderToIndex(i);
		return nCol;
	}
	return -1;
}

int CEditList::GetCellFontHeight()
{
	const CString testText = _T("yjpÍÁ");

	CRect rcRequired = CRect(0, 0, 0, 0);

	CClientDC dc(this);
	dc.DrawText(testText, &rcRequired, DT_CALCRECT | DT_SINGLELINE);

	return rcRequired.Height();
}

CEdit* CEditList::CreateEdit(int nRow, int nCol, DWORD dwStyle, const CRect& rect, CFont& CellFont)
{
	CEditEx* pEdit = new CEditEx(this, nRow, nCol);
	VERIFY(pEdit->Create(WS_CHILD | dwStyle, rect, this, 0));
	return pEdit;
}

CEdit* CEditList::CreatePswdEdit(int nRow, int nCol, DWORD dwStyle, const CRect& rect, CFont& CellFont)
{
	CPswdEditEx* pEdit = new CPswdEditEx(this, nRow, nCol);
	VERIFY(pEdit->Create(WS_CHILD | dwStyle | ES_PASSWORD, rect, this, 0));
	pEdit->SetPasswordChar(_T('*'));
	return pEdit;
}
