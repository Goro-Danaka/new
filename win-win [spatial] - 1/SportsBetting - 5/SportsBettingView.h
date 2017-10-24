
// SportsBettingView.h : interface of the CSportsBettingView class
//

#pragma once
#include "GridCtrl\GridCtrl.h"
#include "GameInfo.h"
#include "SportsBettingDoc.h"

class CSportsBettingView : public CView
{
protected: // create from serialization only
	CSportsBettingView();
	DECLARE_DYNCREATE(CSportsBettingView)

// Attributes
public:
	CSportsBettingDoc* GetDocument() const;

// Operations
public:
	CGridCtrl m_Grid;

	int		m_nFixCols;
	int		m_nFixRows;
	int		m_nCols;
	int		m_nRows;

	void DeleteAllItems();

	void UpdateList(CGameInfo* pGameInfo, BOOL bNewTreeItem = TRUE);
	BOOL IsTableStructureChanged(CGameInfo* pGameInfo);
	int GetSameCaption(CString strLabel, int nStartColNo, int nCount);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnDraw(CDC* pDC);

// Implementation
public:
	virtual ~CSportsBettingView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void InitListControl();
};

#ifndef _DEBUG  // debug version in SportsBettingView.cpp
inline CSportsBettingDoc* CSportsBettingView::GetDocument() const
   { return reinterpret_cast<CSportsBettingDoc*>(m_pDocument); }
#endif

