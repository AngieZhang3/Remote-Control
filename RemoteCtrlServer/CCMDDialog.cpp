// CCMDDialog.cpp: 实现文件
//

#include "pch.h"
#include "RemoteCtrlServer.h"
#include "CCMDDialog.h"
#include "afxdialogex.h"
#include "Proto.h"
#include"log.h"

// CCMDDialog 对话框

IMPLEMENT_DYNAMIC(CCMDDialog, CDialogEx)

CCMDDialog::CCMDDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_editCommand(_T(""))
	, m_editRlt(_T(""))
{

}

//CCMDDialog::CCMDDialog(CTcpSocket* pSocket)
//{
//	m_pCMDSocket = pSocket; 
//}

void CCMDDialog::SetTcpSocket(CTcpSocket* pTcpSocket)
{
	m_pCMDSocket = pTcpSocket;
}

CCMDDialog::~CCMDDialog()
{
}

void CCMDDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT_COMMAND, m_editCommand);
	DDX_Control(pDX, IDC_EDIT_RESULT, m_editResult);
	DDX_Text(pDX, IDC_EDIT_COMMAND, m_editCommand);
	DDX_Text(pDX, IDC_EDIT_RESULT, m_editRlt);
}


BEGIN_MESSAGE_MAP(CCMDDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_START, &CCMDDialog::OnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_EXE, &CCMDDialog::OnClickedBtnExe)
	//_WM_DESTROY(), &CCMDDialog::OnDestroy)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCMDDialog 消息处理程序


void CCMDDialog::OnClickedBtnStart()
{
	stPacketHdr hdr;
	hdr.nCmd = CMD_CMDINIT;
	hdr.nLen = 0;
	if (m_pCMDSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
	{
		LOGE("sent");
	}
}


void CCMDDialog::OnClickedBtnExe()
{
	UpdateData(TRUE);
	m_editCommand += "\r\n";

	stPacketHdr hdr;
	hdr.nCmd = CMD_CMDCOMMAND;
	hdr.nLen = m_editCommand.GetLength();  // Don't use GetLength() + 1, will cause error
	if (m_pCMDSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
	{
		LOGE("sent");
	}

	if ((m_pCMDSocket->Sent(m_editCommand.GetBuffer(), hdr.nLen)) < 0)
	{
		LOGE("sent");
	}

	m_editCommand = " ";
	UpdateData(FALSE);
}


int CCMDDialog::ShowResult(LPBYTE pBuff, DWORD dwBuffSize)
{
	CString strExisting;
	CString strBuff;
	strBuff += (char*)pBuff;

	GetDlgItemText(IDC_EDIT_RESULT, strExisting);
	SetDlgItemText(IDC_EDIT_RESULT, strExisting + strBuff);
	//m_editResult.LineScroll(m_editResult.GetLineCount());//自动滚屏
	return 0;
}




//void CCMDDialog::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//	CWnd* pMainWnd = AfxGetMainWnd();
//	if (pMainWnd) {
//		pMainWnd->PostMessage(WM_CMD_CLOSE);
//	}
//
//
//
//}


void CCMDDialog::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
}


void CCMDDialog::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	stPacketHdr hdr;
	hdr.nCmd = CMD_CMDSTOP;
	hdr.nLen = 0;
	if (m_pCMDSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
	{
		LOGE("sent");
	}
	CDialogEx::OnClose();
}


void CCMDDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::OnOK();
}
