// CaptchaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SportsBetting.h"
#include "CaptchaDlg.h"
#include "afxdialogex.h"
#include "global.h"

// CCaptchaDlg dialog

IMPLEMENT_DYNAMIC(CCaptchaDlg, CDialogEx)

CCaptchaDlg::CCaptchaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CAPTCHA, pParent)
	, m_strCaptcha(_T(""))
{
	m_nSiteID = -1;
}

CCaptchaDlg::~CCaptchaDlg()
{
}

void CCaptchaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strCaptcha);
}


BEGIN_MESSAGE_MAP(CCaptchaDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCaptchaDlg::OnBnClickedOk)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CCaptchaDlg message handlers


BOOL CCaptchaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CCaptchaDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CDialogEx::OnOK();
}


void CCaptchaDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages
	if (m_nSiteID == -1)
		return;

	SetWindowText(g_wszSiteName[m_nSiteID]);
	CDC* pDC = GetDC();

	int width, height = 0;
	CImage* img = new CImage();

	TCHAR szFile[MAX_PATH];
	_stprintf_s(szFile, _T("./%s_captcha.png"), g_wszSiteName[m_nSiteID]);
	HRESULT hResult = img->Load(szFile);
	if (hResult != S_OK)
		return;
	
	width = img->GetWidth();
	height = img->GetHeight();
	img->Draw(*pDC, 0, 0, width, height, 0, 0, width, height);
	DeleteFile(szFile);
}
