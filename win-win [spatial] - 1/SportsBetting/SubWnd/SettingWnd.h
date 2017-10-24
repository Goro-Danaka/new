
#pragma once

#include "SettingList.h"
#include "..\global.h"

class CSettingWnd : public CDockablePane
{
// Construction
public:
	CSettingWnd();

	void UpdateFonts();

// Attributes
public:
	CMFCTabCtrl	m_wndTabs;

	CEditList m_wndSites;
	CEditList m_wndCategory;
	CEditList m_wndBetStrategy;
	CSettingList m_wndBetHistory;

	CImageList m_ImageList;

protected:
	void AdjustHorzScroll(CListBox& wndListBox);

// Implementation
public:
	virtual ~CSettingWnd();

	int m_nStateImageIdx;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	void InitSites();
	void InitCategory();
	void InitBetStrategy();
	void InitBetHistory();
	void DisplayBetStatus(BetStatusDisplay* betState);
	void SaveBetHistory();
	void SaveBetHistory(FILE* fp, int nRow);

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

