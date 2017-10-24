// EditEx.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\SportsBetting.h"
#include "EditEx.h"
#include "..\global.h"

// CEditEx

//------------------------------------------------------------------------
// CEditEx (For internal use)
//------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	//{{AFX_MSG_MAP(CEditEx)
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------
//! CEditEx - Constructor
//------------------------------------------------------------------------
CEditEx::CEditEx()
{

}

CEditEx::CEditEx(CEditList* pList, int nRow, int nCol)
:m_Row(nRow)
, m_Col(nCol)
, m_Completed(false)
, m_Modified(false)
, m_InitialModify(true)
, m_pList(pList)
{
}

//------------------------------------------------------------------------
//! The cell value editor was closed and the entered should be saved.
//!
//! @param bSuccess Should the entered cell value be saved
//------------------------------------------------------------------------
void CEditEx::EndEdit(bool bSuccess)
{
	// Avoid two messages if key-press is followed by kill-focus
	if (m_Completed)
		return;

	m_Completed = true;

	// Send Notification to parent of ListView ctrl
	CString str;
	GetWindowText(str);

	LV_DISPINFO dispinfo = { 0 };
	if (bSuccess && m_Modified)
	{
		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.pszText = str.GetBuffer(0);
		dispinfo.item.cchTextMax = str.GetLength();
	}
	ShowWindow(SW_HIDE);
	//CGridColumnTraitImage::SendEndLabelEdit(*GetParent(), m_Row, m_Col, dispinfo);
	m_pList->SetItemText(m_Row, m_Col, str.GetBuffer(0));
	char* szID = new char[MAX_ID];
	::ToAscii(str.GetBuffer(0), szID, CP_ACP);
	::WritePrivateProfileStringA("ID", g_szSiteName[m_Row], szID, "./setting.ini");
	PostMessage(WM_CLOSE);
}

//------------------------------------------------------------------------
//! WM_KILLFOCUS message handler called when CEdit is loosing focus
//! to other control. Used register that cell value editor should close.
//!
//! @param pNewWnd Pointer to the window that receives the input focus (may be NULL or may be temporary).
//------------------------------------------------------------------------
void CEditEx::OnKillFocus(CWnd *pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	EndEdit(true);
}

//------------------------------------------------------------------------
//! Called by the default OnNcDestroy (WM_NCDESTROY) message handler, 
//! when CEdit window has been be destroyed. 
//! Used to delete the inplace CEdit editor object as well.
//! This is necessary when the CEdit is created dynamically.
//------------------------------------------------------------------------
void CEditEx::PostNcDestroy()
{
	CEdit::PostNcDestroy();
	delete this;
}

//------------------------------------------------------------------------
//! EN_CHANGE notification handler to monitor text modifications
//------------------------------------------------------------------------
void CEditEx::OnEnChange()
{
	if (!m_InitialModify)
		m_Modified = true;
	else
		m_InitialModify = false;
}

//------------------------------------------------------------------------
//! Hook to proces windows messages before they are dispatched.
//! Catch keyboard events that can should cause the cell value editor to close
//!
//! @param pMsg Points to a MSG structure that contains the message to process
//! @return Nonzero if the message was translated and should not be dispatched; 0 if the message was not translated and should be dispatched.
//------------------------------------------------------------------------
BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
		{
			if (GetStyle() & ES_WANTRETURN)
				break;

			EndEdit(true);
			return TRUE;
		}
		case VK_TAB: 
			EndEdit(true); 
			return FALSE;
		case VK_ESCAPE: 
			EndEdit(false); 
			return TRUE;
		case 0x56:
		case 0x76:
			if (GetKeyState(VK_CONTROL) & 0x80)
			{
				SetWindowText(::GetClipboard());
			}
			break;
		}
		break;
	};
	case WM_MOUSEWHEEL: 
		EndEdit(true); 
		return FALSE;	// Don't steal event
	}
	return CEdit::PreTranslateMessage(pMsg);
}

CPswdEditEx::CPswdEditEx()
{

}

CPswdEditEx::CPswdEditEx(CEditList* pList, int nRow, int nCol)
{
	m_Row = nRow;
	m_Col = nCol;
	m_Completed = false;
	m_Modified = false;
	m_InitialModify = true;
	m_pList = pList;
}

void CPswdEditEx::EndEdit(bool bSuccess)
{
	// Avoid two messages if key-press is followed by kill-focus
	if (m_Completed)
		return;

	m_Completed = true;

	// Send Notification to parent of ListView ctrl
	CString str;
	GetWindowText(str);

	LV_DISPINFO dispinfo = { 0 };
	if (bSuccess && m_Modified)
	{
		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.pszText = str.GetBuffer(0);
		dispinfo.item.cchTextMax = str.GetLength();
	}
	ShowWindow(SW_HIDE);
	//CGridColumnTraitImage::SendEndLabelEdit(*GetParent(), m_Row, m_Col, dispinfo);

//	char* p = (char*)m_pList->GetItemData(m_Row);
//	if (p)
///		delete p;

	char* szPswd = new char[MAX_PSWD];
	::ToAscii(str.GetBuffer(0), szPswd, CP_ACP);

	if (str != "")
	{
		BOOL bFlag = FALSE;
		for (int i = 0; i < str.GetLength(); i++)
		{
			if (str.GetAt(i) != _T('*'))
			{
				bFlag = TRUE;
				break;
			}
		}
		if (!bFlag)
			return;
	}
	m_pList->SetItemData(m_Row, (DWORD_PTR)szPswd);
	CString s = _T("");
	for (int i = 0; i < str.GetLength(); i++)
		s += _T('*');
	m_pList->SetItemText(m_Row, m_Col, s);

	char* szEncPswd = new char[MAX_ENC_PSWD];
	GetEncPswd(szPswd, szEncPswd);
	//delete szPswd;

	::WritePrivateProfileStringA("PSWD", g_szSiteName[m_Row], szEncPswd, "./setting.ini");
	PostMessage(WM_CLOSE);
}
