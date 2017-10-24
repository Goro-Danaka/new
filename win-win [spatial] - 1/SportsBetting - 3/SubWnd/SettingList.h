
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CSettingList window
enum COL_TYPE
{
	CT_NONE = 0,
	CT_CHECK = 1,
	CT_EDIT,
	CT_PSWD,
	CT_LOGIN_STATUS,
};

class CSettingList : public CListCtrl
{
	// Construction
public:
	CSettingList();

	// Implementation
public:
	virtual ~CSettingList();

protected:
	afx_msg void OnViewOutput();

	//{{AFX_MSG(CSettingList)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CEditList : public CSettingList
{
	// Construction
public:
	CEditList();

	// Implementation
public:
	virtual ~CEditList();

	int m_nTabIndex;

	void InsertColumnEx(int nCol, LPCTSTR szText, int nWidth, int nType = CT_NONE);
	UINT CellHitTest(const CPoint& pt, int& nRow, int& nCol) const;

	virtual void SetFocusCell(int nCol, BOOL bRedraw = false);
	inline int GetFocusCell() const { return m_FocusCell; }
	void SetFocusRow(int nRow);
	int GetFocusRow() const;

	BOOL SelectRow(int nRow, bool bSelect);

	virtual CWnd* EditCell(int nRow, int nCol, CPoint pt);

	virtual int OnClickEditStart(int nRow, int nCol, CPoint pt, bool bDblClick);
	virtual CWnd* OnEditBegin(int nRow, int nCol);
	virtual CWnd* OnEditBegin(int nRow, int nCol, CPoint pt);
	virtual BOOL OnEditComplete(int nRow, int nCol, CWnd* pEditor, LV_DISPINFO* pLVDI);

	CFont* GetCellFont();

	virtual CRect GetCellEditRect(int nRow, int nCol);
	virtual int GetFirstVisibleColumn();
	int GetCellFontHeight();

	CEdit* CreateEdit(int nRow, int nCol, DWORD dwStyle, const CRect& rect, CFont& CellFont);
	CEdit* CreatePswdEdit(int nRow, int nCol, DWORD dwStyle, const CRect& rect, CFont& CellFont);

	BOOL GetCellRect(int nRow, int nCol, int nCode, CRect& rect);

public:
	int m_FocusCell;			//!< Column currently having focus (-1 means entire row)
	CWnd* m_pEditor;			//!< Cell value editor currently in use
	CFont m_CellFont;			//!< Current font to draw rows

	CUIntArray	m_aryType;

	//{{AFX_MSG(CSettingList)
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

