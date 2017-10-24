
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "SubWnd\GamesView.h"
#include "SubWnd\SettingWnd.h"
#include "SubWnd\TeamNameWnd.h"

#include "BetSites\BetSite.h"
//#include "GameInfo.h"
#include "global.h"
#include "GridCtrl\GridCtrl.h"

#define THREAD_START	2
#define THREAD_STOP		1

class CGameInfo;
class CMainFrame : public CFrameWndEx
{
	
public: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	HTREEITEM m_hItemCategory[GC_COUNT];
	HTREEITEM m_hCurrentSelGame;

	CViewTree* m_treeGameInfo;
	CGridCtrl* m_Grid;

	CGameInfo* m_pCurSelGameInfo;
	BOOL m_bDoingBetting;

	BOOL m_bSetCategory[GC_COUNT];
	int m_nBetStrategy;

	HANDLE m_hBetTread;
	BOOL m_bExitBetThreadFlg;

#ifdef BET_SUBTHREAD_MODE
	BOOL m_bExitBetSubThreadFlg;
	HANDLE m_hCompletionPortBet;
	HANDLE m_hBetSubThread[3];
	HANDLE m_hBettingCompleteEvent;
	int m_nBetObjCount;
#endif
// Operations
public:
	void SetCategory();
	void SetCategory(int nIndex, BOOL bStart);
	void SetBetStrategy(int nIndex);
	void StartThread(int nIndex, BOOL bStart, char* szID, char* szPswd);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

	void UpdateList(CGameInfo* pGameInfo, BOOL bNewTreeItem = TRUE);
	BOOL UpdateTree(int nSiteID, int nIsExitThread);
	void UpdateInfo(int nSiteID, int nIsExitThread);
	void UpdateEventInfoList(int nSiteID, int nEventID);
	void UpdateEventInfoTree(int nSiteID);
	void CheckTeamName(BOOL bHAChanged, WPARAM wParam = 0, LPARAM lParam = 0);
	void ShowCaptcha(WPARAM wParam = 0, LPARAM lParam = 0);

	DWORD BetThreadProc();
#ifdef BET_SUBTHREAD_MODE
	DWORD SubBetThreadProc();
#endif

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
public:
	CGamesView        m_wndGamesView;
	CSettingWnd       m_wndSetting;
	CTeamNameWnd	  m_wndTeamName;
public:
	CView*			  m_pMainView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnUpdateEngineTime(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBettingTime(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	int CreateSiteThreads();
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	afx_msg void OnClose();
	afx_msg void OnStartbetting();
	afx_msg void OnUpdateStartbetting(CCmdUI *pCmdUI);
	afx_msg void OnClearbetinfo();
};

extern CMainFrame* theFrame;

