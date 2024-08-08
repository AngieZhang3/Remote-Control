// CIpDlg.cpp : implementation file
//

#include "pch.h"
#include "RemoteCtrlServer.h"
#include "CIpDlg.h"
#include "afxdialogex.h"


// CIpDlg dialog

IMPLEMENT_DYNAMIC(CIpDlg, CDialogEx)

CIpDlg::CIpDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_IP, pParent)
{

}

CIpDlg::~CIpDlg()
{
}

void CIpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_editIP);
	DDX_Text(pDX, IDC_EDIT2, m_editPort);
}


BEGIN_MESSAGE_MAP(CIpDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CIpDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CIpDlg message handlers


BOOL CIpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_editIP = _T("127.0.0.1");
	m_editPort = _T("16666");
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CIpDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}
