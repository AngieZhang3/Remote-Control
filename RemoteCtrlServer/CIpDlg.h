#pragma once


// CIpDlg dialog

class CIpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CIpDlg)

public:
	CIpDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CIpDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_IP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_editIP;
	CString m_editPort;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
};
