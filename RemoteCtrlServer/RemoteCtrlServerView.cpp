
// RemoteCtrlServerView.cpp: CRemoteCtrlServerView 类的实现
//

#include "pch.h"
#include "framework.h"
#include "log.h"
#include <Gdiplus.h>

// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "RemoteCtrlServer.h"
#endif

#include "RemoteCtrlServerDoc.h"
#include "RemoteCtrlServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_DISPLAY_DESKTOP_IMAGE (WM_USER + 1)
// 声明定时器ID
#define TIMER_ID_SCREENSHOT 1
// CRemoteCtrlServerView


IMPLEMENT_DYNCREATE(CRemoteCtrlServerView, CView)

BEGIN_MESSAGE_MAP(CRemoteCtrlServerView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CRemoteCtrlServerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_CONNECT, &CRemoteCtrlServerView::OnConnect)
	ON_COMMAND(ID_DISCONNECT, &CRemoteCtrlServerView::OnDisconnect)
	ON_WM_DESTROY()
	ON_COMMAND(ID_DESKTOP, &CRemoteCtrlServerView::OnDesktop)
	ON_COMMAND(ID_STOPDESKTOP, &CRemoteCtrlServerView::OnStopdesktop)
	ON_MESSAGE(WM_DISPLAY_DESKTOP_IMAGE, &CRemoteCtrlServerView::OnDisplayDesktopImage)
	//ON_WM_TIMER()
	ON_COMMAND(ID_CMD, &CRemoteCtrlServerView::OnCmd)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, &CRemoteCtrlServerView::OnUpdateConnectServer)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, &CRemoteCtrlServerView::OnUpdateDisconnectServer)
	//ON_BN_CLICKED(IDC_BTN_CONNECT, &CRemoteCtrlServerView::OnBnClickedBtnConnect)
	//ON_COMMAND(ID_MOUSECTRL, &CRemoteCtrlServerView::OnMousectrl)
	ON_COMMAND(ID_MOUSECONTROL_START, &CRemoteCtrlServerView::OnMousecontrolStart)
	ON_COMMAND(ID_MOUSECONTROL_STOP, &CRemoteCtrlServerView::OnMousecontrolStop)
	ON_COMMAND(ID_KEYBOARDCONTROL_START, &CRemoteCtrlServerView::OnKeyboardcontrolStart)
	ON_COMMAND(ID_KEYBOARDCONTROL_STOP, &CRemoteCtrlServerView::OnKeyboardcontrolStop)
	ON_COMMAND(ID_FILE, &CRemoteCtrlServerView::OnFile)
	//ON_MESSAGE(WM_CMD_CLOSE, &CRemoteCtrlServerView::OnCmdClose)
END_MESSAGE_MAP()

// CRemoteCtrlServerView 构造/析构

CRemoteCtrlServerView::CRemoteCtrlServerView() noexcept
{
	// TODO: 在此处添加构造代码

}

CRemoteCtrlServerView::~CRemoteCtrlServerView()
{
}

BOOL CRemoteCtrlServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}



void CRemoteCtrlServerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CRemoteCtrlServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CRemoteCtrlServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CRemoteCtrlServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CRemoteCtrlServerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CRemoteCtrlServerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	//#ifndef SHARED_HANDLERS
	//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
	//#endif
	return;
}


// CRemoteCtrlServerView 诊断

#ifdef _DEBUG
void CRemoteCtrlServerView::AssertValid() const
{
	CView::AssertValid();
}

void CRemoteCtrlServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRemoteCtrlServerDoc* CRemoteCtrlServerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRemoteCtrlServerDoc)));
	return (CRemoteCtrlServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CRemoteCtrlServerView Message handler


void CRemoteCtrlServerView::OnConnect()
{
	//check if socket already exist, if not, create one
	if (m_TcpSocket.GetSocket() != INVALID_SOCKET) {
		LOGI("GetSocket");
		AfxMessageBox(_T("Already connected to a client!"));
		return;
	}

	//m_pIPDlg = new IPDLg();
	//m_pIPDlg->Create(IDD_IPDIALOG, this);
	//ShowWindow(SW_SHOWNORMAL);

	CString strIP;
	CString strPort;
	int nPort;
	if (m_ipDlg.DoModal() == IDOK)
	{
		strIP = m_ipDlg.m_editIP;
		strPort = m_ipDlg.m_editPort;
		nPort = _ttoi(strPort);



		if (m_TcpSocket.CreateSocket() < 0) {
			LOGE("CreateSocket");
			m_TcpSocket.Close();
			return;
		}

		if (m_TcpSocket.Bind(strIP, nPort) < 0) {
			LOGE("Bind")
				m_TcpSocket.Close();
			return;
		}

		if (m_TcpSocket.Listen() < 0) {
			LOGE("Listen");
			m_TcpSocket.Close();
			return;
		}

		m_bIsListening = true; 
		m_pAcceptThread = new std::thread(CRemoteCtrlServerView::AcceptThread, this, &m_TcpSocket);
		m_pAcceptThread->detach();
	}

}



int CRemoteCtrlServerView::RecvThread(CRemoteCtrlServerView* pThis, CTcpSocket* p) {
	LOGI("RecvThreadStart");
	while (pThis->m_bIsSocketConnect) {
		stPacketHdr hdr = { 0 };
		if (p->Receive((char*)&hdr, sizeof(hdr)) < 0) {
			LOGE("RecvThreadReceive");
			break;
		}
		if (pThis->m_bStopRequest) {
			break;
		}

		// Message Handler
		int nLen = hdr.nLen;

		switch (hdr.nCmd) {
		case CMD_DESKTOPDATA:      //desktop screenshot packet
			// Receive data
			if (nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[nLen];
				while (nRecved < nLen)
				{
					int nRet = p->Receive(
						(char*)(pBuff + nRecved), nLen - nRecved);
					if (nRet < 0) {
						LOGE("Receive");
						return -1;
					}
					nRecved += nRet;
				}
				//sent message to primary thread to display message (update UI)
				pThis->PostMessage(WM_DISPLAY_DESKTOP_IMAGE, 0, reinterpret_cast<LPARAM>(pBuff));
	
			}

			break;
		case CMD_CMD:

			if (nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[nLen];
				while (nRecved < nLen)
				{
					int nRet = p->Receive(
						(char*)(pBuff + nRecved), nLen - nRecved);
					if (nRet < 0) {
						LOGE("Receive");
						return -1;
					}
					nRecved += nRet;
				}
				pThis->m_pCmdDlg->ShowResult(pBuff, nLen);
			}

			break;
		case CMD_FILEBROWSE:
			if (nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[nLen];
				while (nRecved < nLen)
				{
					int nRet = p->Receive(
						(char*)(pBuff + nRecved), nLen - nRecved);
					if (nRet < 0) {
						LOGE("Receive");
						return -1;
					}
					nRecved += nRet;
				}
				pThis->m_pCFileDlg->InitTreeCtrl(pBuff, nLen);
			}

			break;
		case CMD_FILEPATH:
			if (nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[nLen];
				while (nRecved < nLen)
				{
					int nRet = p->Receive(
						(char*)(pBuff + nRecved), nLen - nRecved);
					if (nRet < 0) {
						LOGE("Receive");
						return -1;
					}
					nRecved += nRet;
				}
				pThis->m_pCFileDlg->InsertTreeNode(pBuff, nLen);
			}

			break;
		case CMD_GETFILES:
			if (nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[nLen];
				pBuff = new BYTE[nLen];
				while (nRecved < nLen)
				{
					int nRet = p->Receive(
						(char*)(pBuff + nRecved), nLen - nRecved);
					if (nRet < 0) {
						LOGE("Receive");
						return -1;
					}
					nRecved += nRet;
				}
				pThis->m_pCFileDlg->InsertFileList(pBuff, nLen);
			}

			break;
		case CMD_DOWNLOADSTART:
			if (nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[nLen];
				while (nRecved < nLen)
				{
					int nRet = p->Receive(
						(char*)(pBuff + nRecved), nLen - nRecved);
					if (nRet < 0) {
						LOGE("Receive");
						return -1;
					}
					nRecved += nRet;
				}
				pThis->m_pCFileDlg->SaveFileSize(pBuff, nLen);
			}

			break;
		case CMD_DOWNLOADFILE:
			if (nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[nLen];
				while (nRecved < nLen)
				{
					int nRet = p->Receive(
						(char*)(pBuff + nRecved), nLen - nRecved);
					if (nRet < 0) {
						LOGE("Receive");
						return -1;
					}
					nRecved += nRet;
				}
				pThis->m_pCFileDlg->WriteFiles(pBuff, nLen);
			}

			break;
		case CMD_DNCOMPLETE:
			pThis->m_pCFileDlg->CloseDNFile();
			break;
		case CMD_DISCONNECT:
			pThis->OnDisconnect();
			break;
		default:
			break;
		}
	}
	LOGI("WorkThreadEnd");


	return 0;
}

void CRemoteCtrlServerView::OnDisconnect()
{
	if (m_TcpSocket.GetSocket() == INVALID_SOCKET) {
		AfxMessageBox("Server not connected");
		return;
	}
	if (m_pTcpSocket->GetSocket() != INVALID_SOCKET) {
		stPacketHdr hdr;
		hdr.nCmd = CMD_DISCONNECT;
		hdr.nLen = 0;
		if (m_pTcpSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
		{
			LOGE("Sent");
		}
	}
	// 设置停止标志
	m_bIsSocketConnect = false;
	m_bIsListening = false;
	m_bStopRequest = true;


	// 停止桌面控制
	if (m_bIsDesktop == TRUE) {
		OnStopdesktop();
		m_bIsDesktop = false;
	}

	// 关闭客户端套接字
	if (m_pTcpSocket != NULL && m_pTcpSocket->GetSocket() != INVALID_SOCKET) {
		m_pTcpSocket->Close();
	}

	// 关闭主套接字
	m_TcpSocket.Close();

	AfxMessageBox("Successfully disconnected.");

	// 发送消息通知更新 UI
	AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(0, CN_UPDATE_COMMAND_UI));
		

}


void CRemoteCtrlServerView::OnDestroy()
{
	OnDisconnect();
	CView::OnDestroy();


}


void CRemoteCtrlServerView::OnDesktop()
{
	stPacketHdr hdr;
	hdr.nCmd = CMD_GETDESKTOP;
	uint32_t nLen = 0;
	if (m_pTcpSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
	{
		LOGE("Sent");
	}
	m_bIsDesktop = TRUE;
	//SetTimer(TIMER_ID_SCREENSHOT, 500, nullptr);
	//m_bNewData = false;
}


void CRemoteCtrlServerView::OnStopdesktop()
{
	stPacketHdr hdr;
	hdr.nCmd = CMD_STOPDESKTOP;
	hdr.nLen = 0;
	if (m_pTcpSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
	{
		LOGE("Sent");
	}

	// Clear the client area
	CDC* pDC = GetDC();
	if (pDC != nullptr)
	{
		CRect rect;
		GetClientRect(&rect);

		// Fill the client area with the background color (e.g., white)
		pDC->FillSolidRect(&rect, RGB(255, 255, 255));
		InvalidateRect(&rect);
		ReleaseDC(pDC);
	}

	m_bIsDesktop = FALSE;
	m_bIsMouseCtrl = false;
	m_bIsKeyboardCtrl = false;

}


afx_msg LRESULT CRemoteCtrlServerView::OnDisplayDesktopImage(WPARAM wParam, LPARAM lParam)
{
	ImagePacket* pImagePacket = (ImagePacket*)(lParam);
	if (pImagePacket == nullptr)
	{
		LOGE("pImagePacket is nullptr");
		return -1;
	}

	int nWidth = pImagePacket->m_width;
	int nHeight = pImagePacket->m_height;
	int nImgDataSize = pImagePacket->m_dataSize;
	BYTE* pImageData = new BYTE[nImgDataSize];

	if (pImageData == nullptr)
	{
		LOGE("Failed to allocate memory for image data");
		delete pImagePacket;
		return -1;
	}
	memcpy(pImageData, pImagePacket->m_imageBuffer, nImgDataSize);
	// Create a CBitmap object and load the received image
	CBitmap recBmp;
	CClientDC dc(this);
	recBmp.DeleteObject();
	if (!recBmp.CreateCompatibleBitmap(&dc, nWidth, nHeight))
	{
		LOGE("CreateCompatibleBitmap failed");
		delete[] pImageData;
		delete pImagePacket;
		return -1;
	}

	// Set bitmap bits directly into the CBitmap
	if (!recBmp.SetBitmapBits(nImgDataSize, pImageData))
	{
		LOGE("SetBitmapBits failed");
		delete[] pImageData;
		delete pImagePacket;
		return -1;
	}

	// Display the received image on the view
	CDC* pDC = GetDC();
	if (pDC == nullptr)
	{
		LOGE("GetDC failed");
		delete[] pImageData;
		delete pImagePacket;
		return -1;
	}
	CRect rect;
	GetClientRect(&rect);

	// Draw the image using StretchBlt
	CDC memDC;
	if (!memDC.CreateCompatibleDC(pDC))
	{
		LOGE("CreateCompatibleDC failed");
		ReleaseDC(pDC);
		delete[] pImageData;
		delete pImagePacket;
		return -1;
	}

	// Select the received bitmap into the memory DC
	CBitmap* pOldBitmap = memDC.SelectObject(&recBmp);

	// set screen zoom ratio
	double xScale = static_cast<double>(rect.Width()) / nWidth;
	double yScale = static_cast<double>(rect.Height()) / nHeight;
	m_scale = min(xScale, yScale);

	int newWidth = static_cast<int>(nWidth * m_scale);
	int newHeight = static_cast<int>(nHeight * m_scale);

	if (!pDC->StretchBlt(0, 0, newWidth, newHeight, &memDC, 0, 0, nWidth, nHeight, SRCCOPY))
	{
		LOGE("StretchBlt failed");
		memDC.SelectObject(pOldBitmap);
		ReleaseDC(pDC);
		delete[] pImageData;
		delete pImagePacket;
		return -1;
	}


	// Cleanup
	memDC.SelectObject(pOldBitmap);
	ReleaseDC(pDC);
	delete[] pImageData;
	delete pImagePacket;
	return 0;



}





void CRemoteCtrlServerView::OnDraw(CDC* /*pDC*/)
{
	// TODO: 在此添加专用代码和/或调用基类
}


void CRemoteCtrlServerView::OnCmd()
{

	//execute CMD
	if (m_bIsSocketConnect != true)
	{
		AfxMessageBox("Not connected to client");
		return;
	}

	m_pCmdDlg = new CCMDDialog();
	m_pCmdDlg->SetTcpSocket(m_pTcpSocket);
	m_pCmdDlg->Create(IDD_DIALOG1, this);
	m_pCmdDlg->ShowWindow(SW_SHOWNORMAL);
}


void CRemoteCtrlServerView::OnUpdateConnectServer(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bIsSocketConnect); //Enable or disable menu items based on connection status
}

void CRemoteCtrlServerView::OnUpdateDisconnectServer(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsSocketConnect); //Enable or disable menu items based on connection status
}






void CRemoteCtrlServerView::OnMousecontrolStart()
{
	m_bIsMouseCtrl = true;
}


void CRemoteCtrlServerView::OnMousecontrolStop()
{
	m_bIsMouseCtrl = false;
}


BOOL CRemoteCtrlServerView::PreTranslateMessage(MSG* pMsg)
{
	stPacketHdr hdr;
	if (m_bIsMouseCtrl == true) {
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_XBUTTONDBLCLK:
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:


			hdr.nCmd = CMD_MOUSE;
			hdr.nLen = sizeof(MousePacket);

			//fill in the MousePacket Struct
			MousePacket mousePkt;
			mousePkt.m_message = pMsg->message;
			mousePkt.m_pt = pMsg->pt;
			mousePkt.m_wParam = pMsg->wParam;
			ScreenToClient(&mousePkt.m_pt);
			mousePkt.m_pt.x = static_cast<LONG>(mousePkt.m_pt.x / m_scale);
			mousePkt.m_pt.y = static_cast<LONG>(mousePkt.m_pt.y / m_scale);
			//send mouse message to client
			int nSentHdr = m_pTcpSocket->Sent((char*)&hdr, sizeof(hdr));
			if (nSentHdr < 0) {
				LOGE("SentHdr");
				return -1;
			}
			int nSentMsPacket = m_pTcpSocket->Sent((char*)&mousePkt, hdr.nLen);
			if (nSentMsPacket < 0) {
				LOGE("SentMsPacket");
				return -1;
			}
			break;

		}
	}
	if (m_bIsKeyboardCtrl == true) {
		switch (pMsg->message) {
		case WM_KEYDOWN:
		case WM_KEYUP:
			//fill in header

			hdr.nCmd = CMD_KEYBOARD;
			hdr.nLen = sizeof(KeybdPacket);

			// fill in keybd packet
			KeybdPacket keybdPkt;
			keybdPkt.m_msg = pMsg->message;
			keybdPkt.m_lParam = pMsg->lParam;
			keybdPkt.m_wParam = pMsg->wParam;

			//send keyboard message to client
			int nSentHdr = m_pTcpSocket->Sent((char*)&hdr, sizeof(hdr));
			if (nSentHdr < 0) {
				LOGE("SentHdr");
				return -1;
			}
			int nSentKeybdPacket = m_pTcpSocket->Sent((char*)&keybdPkt, hdr.nLen);
			if (nSentKeybdPacket < 0) {
				LOGE("SentKeybdPacket");
				return -1;
			}
			break;
		}
	}

	return CView::PreTranslateMessage(pMsg);
}


void CRemoteCtrlServerView::OnKeyboardcontrolStart()
{
	m_bIsKeyboardCtrl = true;
}


void CRemoteCtrlServerView::OnKeyboardcontrolStop()
{
	m_bIsKeyboardCtrl = false;
}


void CRemoteCtrlServerView::OnFile()
{
	if (m_bIsSocketConnect != true)
	{
		AfxMessageBox("Not connected to client");
		return;
	}

	m_pCFileDlg = new CFileDlg();
	m_pCFileDlg->SetSocket(m_pTcpSocket);
	m_pCFileDlg->Create(IDD_DIALOG_FILE, this);
	m_pCFileDlg->ShowWindow(SW_SHOWNORMAL);



}


int CRemoteCtrlServerView::AcceptThread(CRemoteCtrlServerView* pView, CTcpSocket* pSocket)
{
	while (pView->m_bIsListening)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(pSocket->GetSocket(), &readfds);
		timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		int result = select(0, &readfds, NULL, NULL, &timeout);
		if (result > 0 && FD_ISSET(pSocket->GetSocket(), &readfds))
		{
			pView->m_pTcpSocket = pSocket->Accept();
			if (pView->m_pTcpSocket == nullptr) {
				LOGE("Accept");
				pSocket->Close();
				return -1;
			}
			pView->m_bIsSocketConnect = true;
			if (pView->m_bIsSocketConnect){
				pView->m_pThread = new std::thread(CRemoteCtrlServerView::RecvThread, pView, pView->m_pTcpSocket);
				pView->m_pThread->detach();
			}

			AfxMessageBox("Successfully connected to a client");
			if (!pView->m_bIsListening) {
				break;
			}
			if (pView->m_bStopRequest) {
				break;
			}
		}
		
		

	}


	//AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(0, CN_UPDATE_COMMAND_UI));
	return 0;
}


//afx_msg LRESULT CRemoteCtrlServerView::OnCmdClose(WPARAM wParam, LPARAM lParam)
//{
//	m_bIsCMD = false; 
//	
//	stPacketHdr hdr;
//	hdr.nCmd = CMD_CMDSTOP;
//	hdr.nLen = 0;
//	if (m_pTcpSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
//	{
//		LOGE("sent");
//	}
//	return 0;
//}
