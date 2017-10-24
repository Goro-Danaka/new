#pragma once
#include "afxwin.h"


// CCaptchaDlg dialog

class CCaptchaDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCaptchaDlg)

public:
	CCaptchaDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCaptchaDlg();

	int m_nSiteID;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CAPTCHA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnPaint();
	CString m_strCaptcha;
};
