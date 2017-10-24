
#pragma once

#include "ViewTree.h"
#include "..\GameInfo.h"

class CGamesView : public CDockablePane
{
// Construction
public:
	CGamesView();

	void AdjustLayout();
	void OnChangeVisualStyle();

// Attributes
protected:

	CViewTree m_wndGamesView;
	CImageList m_GamesViewImages;

protected:
	void InitTreeViewControl();

// Implementation
public:
	virtual ~CGamesView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSelChangedTree1(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

