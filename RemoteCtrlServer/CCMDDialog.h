#pragma once
#include "CTcpSocket.h"

// CCMDDialog 对话框

class CCMDDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CCMDDialog)

public:
	CCMDDialog(CWnd* pParent = nullptr);   // 标准构造函数
	//CCMDDialog(CTcpSocket* pSocket);
	void SetTcpSocket(CTcpSocket* pTcpSocket);
	virtual ~CCMDDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//CEdit m_editCommand;
	CEdit m_editResult;
private:
	CTcpSocket* m_pCMDSocket;
public:
	afx_msg void OnClickedBtnStart();
	afx_msg void OnClickedBtnExe();
	int ShowResult(LPBYTE pBuff, DWORD dwBuffSize);
	CString m_editCommand;
	CString m_editRlt;
	//x_msg void OnDestroy();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	virtual void OnOK();
};
