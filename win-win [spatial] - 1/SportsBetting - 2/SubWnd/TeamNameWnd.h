
#pragma once

#include "..\global.h"
#include "..\MainInfo.h"

class CTeamNameToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

struct TEAMNAME
{
	//int nCategory;
	//char ht1[MAX_TEAM_NAME];
	//char at1[MAX_TEAM_NAME];
	//char ht2[MAX_TEAM_NAME];
	//char at2[MAX_TEAM_NAME];
	//int hs1, as1, hs2, as2, time1, time2;
	BOOL bHAChanged;
	CMainInfo info1;
	CMainInfo info2;

	TEAMNAME()
	{
		bHAChanged = FALSE;
	}

	TEAMNAME(TEAMNAME& tn)
	{
		bHAChanged = tn.bHAChanged;
		info1 = tn.info1;
		info2 = tn.info2;
	}

	TEAMNAME& operator=(TEAMNAME& tn)
	{
		bHAChanged = tn.bHAChanged;
		info1 = tn.info1;
		info2 = tn.info2;
		return *this;
	}
};

typedef CArray<TEAMNAME*, TEAMNAME*> CTeamNameArray;
typedef CMap<string, string&, string, string&> CTeamNameMap;

class CTeamNameWnd : public CDockablePane
{
// Construction
public:
	CTeamNameWnd();

	CTeamNameArray m_aryTeamName;
	void AdjustLayout();

// Attributes
public:
	void AddItem(BOOL bHAChanged, CMainInfo& info1, CMainInfo& info2);
	BOOL IsExistItem(CMainInfo& info);
protected:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CTeamNameToolBar m_wndToolBar;
	CListCtrl m_wndTeamNameList;

// Implementation
public:
	virtual ~CTeamNameWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSave();
	afx_msg void OnUpdateSave(CCmdUI* pCmdUI);
	afx_msg void OnYes();
	afx_msg void OnUpdateYes(CCmdUI* pCmdUI);
	afx_msg void OnNo();
	afx_msg void OnUpdateNo(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	//afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()


	void InitPropList();
	void SetPropListFont();
	BOOL OnTooltipHandler(UINT id, NMHDR * pNMHDR, LRESULT * pResult);

	int m_nComboHeight;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

