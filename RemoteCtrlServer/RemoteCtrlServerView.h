
// RemoteCtrlServerView.h: CRemoteCtrlServerView 类的接口
//

#pragma once
#include <thread>
#include <vector>
#include "Proto.h"
#include "CCMDDialog.h"
#include "CIpDlg.h"
#include "CFileDlg.h"


class CRemoteCtrlServerView : public CView
{
protected: // 仅从序列化创建
	CRemoteCtrlServerView() noexcept;
	DECLARE_DYNCREATE(CRemoteCtrlServerView)

// 特性
public:
	CRemoteCtrlServerDoc* GetDocument() const;

// 操作
public:

// 重写
public:
//	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CRemoteCtrlServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnConnect();
	std::vector<std::thread*> m_WorkThreads;
	//static int AcceptThread(CRemoteCtrlServerView* pThis);
	//static int WorkThread(CRemoteCtrlServerView* pThis, CTcpSocket* p);
	 static int RecvThread(CRemoteCtrlServerView * pThis, CTcpSocket* p);
private: 
	CTcpSocket m_TcpSocket; //listening socket
	CTcpSocket* m_pTcpSocket;	//Acceptted socket
	std::thread* m_pThread;
	std::thread* m_pAcceptThread; // thread to accept client's socket 
	//bool m_bNewData;
	//bool m_bIsCMD = false;
	bool m_bIsSocketConnect = false;
	CCMDDialog *m_pCmdDlg;
	CIpDlg m_ipDlg;
	bool m_bIsMouseCtrl = false;
	POINT m_lastMousePoint; 
	bool m_bIsKeyboardCtrl = false;
	bool m_bIsDesktop = false;
	bool m_bIsListening = false;
	bool m_bStopRequest = false;
	double m_scale; // screen zoom ration
	CFileDlg* m_pCFileDlg;
public:
	afx_msg void OnDisconnect();
	afx_msg void OnDestroy();
	afx_msg void OnDesktop();
	afx_msg void OnStopdesktop();
	//update status of connect menu
	afx_msg void OnUpdateConnectServer(CCmdUI* pCmdUI);
	//update status of disconnect menu
	afx_msg void OnUpdateDisconnectServer(CCmdUI* pCmdUI);

protected:
	afx_msg LRESULT OnDisplayDesktopImage(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	ImagePacket* m_pImgPacket = nullptr;
	virtual void OnDraw(CDC* /*pDC*/);
	afx_msg void OnCmd();
//	afx_msg void OnBnClickedBtnConnect();
//	afx_msg void OnMousectrl();
	//void SetMouseHookThread();

	afx_msg void OnMousecontrolStart();
//	static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
//	static DWORD WINAPI SetMouseHookThread(LPVOID lpParam);
	afx_msg void OnMousecontrolStop();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKeyboardcontrolStart();
	afx_msg void OnKeyboardcontrolStop();
	afx_msg void OnFile();
	static int AcceptThread(CRemoteCtrlServerView* pView, CTcpSocket* pSocket);
protected:
	//afx_msg LRESULT OnCmdClose(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // RemoteCtrlServerView.cpp 中的调试版本
inline CRemoteCtrlServerDoc* CRemoteCtrlServerView::GetDocument() const
   { return reinterpret_cast<CRemoteCtrlServerDoc*>(m_pDocument); }
#endif

