#pragma once

#include "CTcpSocket.h"
// CFileDlg dialog
#define WM_UPLOAD_COMPLETE (WM_USER + 3)
class CFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileDlg)

public:
	CFileDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFileDlg();
	void SetSocket(CTcpSocket* pTcpSocket);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstCtrl;
	CTreeCtrl m_treeCtrl;
private: 
	CTcpSocket* m_pFileSocket;
	HTREEITEM m_hCurrentTreeItem;
	CString m_csDNFileName;
	CString m_csUploadFilePath;
	CString m_csUploadDstPath;
	int m_nSelectedIndex;
	ULONGLONG m_ullTotalFileSize;
	ULONGLONG m_ullTransferredSize = 0;
	HANDLE m_hUploadThread;
public:
	virtual BOOL OnInitDialog();
	BOOL InitTreeCtrl(LPBYTE pBuff, DWORD dwBuffSize);
	BOOL SendBrowseRequest();
	afx_msg void OnItemexpandingTree1(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL GetItemPath(HTREEITEM hRoot, char* lpBuf, DWORD dwLen, DWORD dwMaxLen);
	BOOL InsertTreeNode(LPBYTE pBuff, DWORD dwBuffSize);
	int InitListCtrl();
	BOOL InsertFileList(LPBYTE pBuff, DWORD dwBuffSize);
	BOOL WriteFiles(LPBYTE pBuff, DWORD dwBuffSize);
	BOOL SaveFileSize(LPBYTE pBuff, DWORD dwBuffSize); //total size of the file to be downloaded
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);

	//afx_msg void OnBnClickedButton1();
	CString m_csDNLDPath;
	//CString m_csUploadFile;
	CString m_csSelectedDir;//selected file path
	//afx_msg void OnBnClickedBtnUploadsel();
	//afx_msg void OnBnClickedBtnDownload();
	afx_msg void OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL CheckExistingNode(CTreeCtrl& treeCtrl, HTREEITEM hParent, CString& nodeText);
	void CloseDNFile();
	afx_msg void OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult);
	CString m_editClientFilePath;
	//afx_msg void OnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextmenuUpload();
	afx_msg void OnNMRClickList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextmenuDownload();
	int UploadFiles();
	static DWORD WINAPI UploadThreadProc(LPVOID lpParam);
protected:
	//afx_msg LRESULT OnSetProgress(WPARAM wParam, LPARAM lParam);
public:
	CProgressCtrl m_progressCtrl;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	afx_msg LRESULT OnUploadComplete(WPARAM wParam, LPARAM lParam);
};
