#pragma once

#include "SettingList.h"
// CEditEx

class CEditEx : public CEdit
{
public:
	CEditEx(CEditList* pList, int nRow, int nCol);
	virtual void EndEdit(bool bSuccess);

protected:
	afx_msg void OnKillFocus(CWnd *pNewWnd);
	afx_msg void OnEnChange();

	virtual void PostNcDestroy();
	virtual	BOOL PreTranslateMessage(MSG* pMsg);

public:
	int		m_Row;					//!< The index of the row being edited
	int		m_Col;					//!< The index of the column being edited
	bool	m_Completed;			//!< Ensure the editor only reacts to a single close event
	bool	m_Modified;				//!< Register if text was modified while the editor was open
	bool	m_InitialModify;		//!< Initial text modication should not set that the editor text was modified

	CEditList* m_pList;

	DECLARE_MESSAGE_MAP();

public:
	CEditEx();
	CEditEx(const CEditEx&);
	CEditEx& operator=(const CEditEx&);
};

class CPswdEditEx : public CEditEx
{
public:
	CPswdEditEx(CEditList* pList, int nRow, int nCol);
	virtual void EndEdit(bool bSuccess);

public:
	CPswdEditEx();
};

