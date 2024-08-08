
// RemoteCtrlClientDlg.h: 头文件
//

#pragma once
#include "CTcpSocket.h"
#include "proto.h"
#include <thread>
#define WM_DOWNLOAD_COMPLETE (WM_USER + 1)


// CRemoteCtrlClientDlg 对话框
class CRemoteCtrlClientDlg : public CDialogEx
{

// 构造
public:

	CRemoteCtrlClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECTRLCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listClientStats;
	CTcpSocket m_TcpSocket;
	CCriticalSection m_CriticalSection;
	std::thread* m_pThread;
	afx_msg void OnClickedBtnStart();
	void ShowMsg(const char* pMsg);
	afx_msg void OnClickedBtnStop();
	afx_msg void OnIdok();
	static int WorkThread(CRemoteCtrlClientDlg* pThis);
	int GetScreenshot();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void StartScreenshotTimer();
	void StopScreenshotTimer();
	static DWORD WINAPI DownloadThreadProc(LPVOID lpParam);

	HANDLE m_hOutputRd;
	HANDLE m_hOutputWr;
	HANDLE m_hInputRd;
	HANDLE m_hInputWr;

	int InitCmd();
	int HandleCmd(LPBYTE pCmd, DWORD dwSize);
	int StopCmd();
	CString m_editIP;
	int m_editPort;
	CButton m_btnStart;
	CButton m_btnStop;
	BOOL m_bClientIsRunnning = FALSE;
	BOOL m_bStopThread = FALSE;// signal to stop thread
	CString m_csDstPath;
	void StopThread();
	void ProcessMouseEvent(LPBYTE pBuff);
	void ProcessKeybdEvent(LPBYTE pBuff);
	int GetDirectory(LPBYTE pBuff, DWORD nLen);
	int GetFiles(LPBYTE pBuff, DWORD nLen);
	int DownloadFiles(LPBYTE pBuff, DWORD nLen, CRemoteCtrlClientDlg* pThis);
	int GetDstPath(LPBYTE pBuff, DWORD nLen);
	int WriteFiles(LPBYTE pBuff, DWORD nLen);
	void CompleteUpload();
	HANDLE m_hDownloadThread; 
	PROCESS_INFORMATION m_piProcInfo;
protected:
//	afx_msg LRESULT OnUpdateUi(WPARAM wParam, LPARAM lParam);
public:
	BOOL GetLogicalDrive();
protected:
	afx_msg LRESULT OnDownloadComplete(WPARAM wParam, LPARAM lParam);
private: 
	typedef struct _stThreadParams {
		CRemoteCtrlClientDlg* m_pDlg;
		BYTE* m_pBuff;
	} ThreadParams;
};
