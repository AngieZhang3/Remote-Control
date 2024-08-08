
// RemoteCtrlClientDlg.cpp: 实现文件
//


#include "pch.h"
#include "framework.h"
#include "RemoteCtrlClient.h"
#include "RemoteCtrlClientDlg.h"
#include "afxdialogex.h"
#include "log.h"
#include<windows.h>
#include <stdlib.h>
#include <vector>
#include <iostream >
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// 声明定时器ID
#define TIMER_ID_SCREENSHOT 1


//#define DEBUG
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	//	afx_msg void OnBtnStart();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//	ON_COMMAND(IDC_BTN_START, &CAboutDlg::OnBtnStart)
END_MESSAGE_MAP()


// CRemoteCtrlClientDlg 对话框


CRemoteCtrlClientDlg::CRemoteCtrlClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTECTRLCLIENT_DIALOG, pParent)
	, m_editIP(_T(""))
	, m_editPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteCtrlClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENTSTATS, m_listClientStats);
	DDX_Text(pDX, IDC_EDIT1, m_editIP);
	DDX_Text(pDX, IDC_EDIT2, m_editPort);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
}

BEGIN_MESSAGE_MAP(CRemoteCtrlClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CRemoteCtrlClientDlg::OnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CRemoteCtrlClientDlg::OnClickedBtnStop)
	ON_COMMAND(IDOK, &CRemoteCtrlClientDlg::OnIdok)
	ON_WM_TIMER()
	//ON_MESSAGE(WM_UPDATE_UI, &CRemoteCtrlClientDlg::OnUpdateUi)
	ON_MESSAGE(WM_DOWNLOAD_COMPLETE, &CRemoteCtrlClientDlg::OnDownloadComplete)
END_MESSAGE_MAP()


// CRemoteCtrlClientDlg 消息处理程序

BOOL CRemoteCtrlClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_editIP = "127.0.0.1";
	m_editPort = 16666;
	UpdateData(FALSE);
	m_btnStart.EnableWindow(!m_bClientIsRunnning);
	m_btnStop.EnableWindow(m_bClientIsRunnning);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRemoteCtrlClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteCtrlClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRemoteCtrlClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CAboutDlg::OnBtnStart()
//{
//	// TODO: 在此添加命令处理程序代码
//}

int CRemoteCtrlClientDlg::WorkThread(CRemoteCtrlClientDlg* pThis) {
	LOGI("WorkThread start");
	CTcpSocket& TcpSocket = pThis->m_TcpSocket;

	while (!pThis->m_bStopThread) {
		stPacketHdr hdr;
		if (TcpSocket.Receive((char*)&hdr, sizeof(hdr)) < 0) {
			pThis->ShowMsg(TcpSocket.GetErrMsg());
			break;
		}

		//处理消息

		switch (hdr.nCmd) {
		case CMD_GETDESKTOP:
			pThis->StartScreenshotTimer();
			pThis->ShowMsg("StartScreenshotTimer");
			break;
		case CMD_STOPDESKTOP:
			pThis->StopScreenshotTimer();
			pThis->ShowMsg("StopScreenshotTimer");
			break;
		case CMD_MOUSE:

			if (hdr.nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[hdr.nLen];
				while (nRecved < hdr.nLen)
				{
					int nRet = pThis->m_TcpSocket.Receive(
						(char*)(pBuff + nRecved), hdr.nLen - nRecved);
					nRecved += nRet;
				}
				pThis->ProcessMouseEvent(pBuff);
			}
			
			break;
		case CMD_KEYBOARD:
			if (hdr.nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[hdr.nLen];
				while (nRecved < hdr.nLen)
				{
					int nRet = pThis->m_TcpSocket.Receive(
						(char*)(pBuff + nRecved), hdr.nLen - nRecved);
					nRecved += nRet;
				}
				pThis->ProcessKeybdEvent(pBuff);
			}
		
			break;
		case CMD_CMDINIT:
			pThis->InitCmd();
			break;
		case CMD_CMDCOMMAND:

			if (hdr.nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[hdr.nLen];
				while (nRecved < hdr.nLen)
				{
					int nRet = pThis->m_TcpSocket.Receive(
						(char*)(pBuff + nRecved), hdr.nLen - nRecved);
					nRecved += nRet;
				}
				pThis->HandleCmd(pBuff, hdr.nLen);
			}

			break;
		case CMD_CMDSTOP:
			pThis->StopCmd();
			break;
		case CMD_FILEBROWSE:
			pThis->GetLogicalDrive();
			break;
		case CMD_FILEPATH:
			if (hdr.nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[hdr.nLen];
				while (nRecved < hdr.nLen)
				{
					int nRet = pThis->m_TcpSocket.Receive(
						(char*)(pBuff + nRecved), hdr.nLen - nRecved);
					nRecved += nRet;
				}
				pThis->GetDirectory(pBuff, hdr.nLen);
			}
		
			break;
		case CMD_GETFILES:
			if (hdr.nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[hdr.nLen];
				while (nRecved < hdr.nLen)
				{
					int nRet = pThis->m_TcpSocket.Receive(
						(char*)(pBuff + nRecved), hdr.nLen - nRecved);
					nRecved += nRet;
				}
				pThis->GetFiles(pBuff, hdr.nLen);
			}
	
			break;
		case CMD_DOWNLOADFILE:
			if (hdr.nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[hdr.nLen];
				while (nRecved < hdr.nLen)
				{
					int nRet = pThis->m_TcpSocket.Receive(
						(char*)(pBuff + nRecved), hdr.nLen - nRecved);
					nRecved += nRet;
				}
				pThis->DownloadFiles(pBuff, hdr.nLen, pThis);
			}
		
			break;
		case CMD_UPLOADSTART:
			if (hdr.nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[hdr.nLen];
				while (nRecved < hdr.nLen)
				{
					int nRet = pThis->m_TcpSocket.Receive(
						(char*)(pBuff + nRecved), hdr.nLen - nRecved);
					nRecved += nRet;
				}
				pThis->GetDstPath(pBuff, hdr.nLen);
			}

			break;
		case CMD_UPLOADFILE:
			if (hdr.nLen > 0)
			{
				LPBYTE pBuff = NULL;
				int nRecved = 0;
				pBuff = new BYTE[hdr.nLen];
				while (nRecved < hdr.nLen)
				{
					int nRet = pThis->m_TcpSocket.Receive(
						(char*)(pBuff + nRecved), hdr.nLen - nRecved);
					nRecved += nRet;
				}
				pThis->WriteFiles(pBuff, hdr.nLen);
			}

			break;
		case CMD_UPLOADCOMPLETE:
			pThis->CompleteUpload();
			break;
		case CMD_DISCONNECT:
			pThis->OnClickedBtnStop();
			break;
		default:
			break;
		}
		if (pThis->m_bStopThread)
			break;
	}


	LOGI("WorkThread end");
	return 0;
}

void CRemoteCtrlClientDlg::OnClickedBtnStart()
{
	UpdateData(TRUE);

	////这里要用到switch
	//stPacketHdr Header = { CMD_LOGIN, sizeof(stPacket) };
	//stPacket Packet;


	m_listClientStats.ResetContent();
	if (m_TcpSocket.GetSocket() != INVALID_SOCKET) {
		ShowMsg("Server already started");
		return;
	}

	int nPort = m_editPort;
	CString strIP = m_editIP;

	if (m_TcpSocket.CreateSocket() < 0) {
		ShowMsg(m_TcpSocket.GetErrMsg());
		goto SAFE_EXIT;
	}


	if (nPort < 0 || nPort > 65535 || strIP.IsEmpty())
	{
		AfxMessageBox(_T("Please enter valid IP and port"));
		return;
	}

	if (m_TcpSocket.Connect(strIP, nPort) < 0) {
		ShowMsg(m_TcpSocket.GetErrMsg());
		goto SAFE_EXIT;
	}


	//Create WorkThread
	m_bStopThread = false;
	m_pThread = new std::thread(CRemoteCtrlClientDlg::WorkThread, this);
	m_pThread->detach();


	ShowMsg("Successfully connected to the server.");
	m_bClientIsRunnning = TRUE;
	m_btnStart.EnableWindow(!m_bClientIsRunnning);
	m_btnStop.EnableWindow(m_bClientIsRunnning);

	return;

SAFE_EXIT:
	m_TcpSocket.Close();
	ShowMsg("m_TcpSocket has been closed");

}

#if 1
void CRemoteCtrlClientDlg::ShowMsg(const char* pMsg) {
	m_CriticalSection.Lock();
	m_listClientStats.InsertString(m_listClientStats.GetCount(), pMsg);
	m_CriticalSection.Unlock();
}
#endif // 0


void CRemoteCtrlClientDlg::OnClickedBtnStop()
{

	//u_long mode = 1; // 1 to enable non-blocking socket
	//ioctlsocket(m_TcpSocket.GetSocket(), FIONBIO, &mode);

	ShowMsg("Stopping client...");
	stPacketHdr hdr;
	hdr.nCmd = CMD_DISCONNECT;
	hdr.nLen = 0;
	if (m_TcpSocket.Sent((char*)&hdr, sizeof(hdr)) < 0)
	{
		LOGE("Sent");
	}
	m_bStopThread = true;
	m_TcpSocket.Close();
	ShowMsg("Client successfully disconnected from the server.");
	m_bClientIsRunnning = FALSE;
	m_btnStart.EnableWindow(!m_bClientIsRunnning);

}


void CRemoteCtrlClientDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}



int CRemoteCtrlClientDlg::GetScreenshot()
{
	//1. Screenshot
	//1.1  Get Desktop window and DC
	CWnd* pWnd = GetDesktopWindow();
	if (pWnd == nullptr)
	{
		ShowMsg(m_TcpSocket.GetErrMsg());
		LOGE("GetDesktopWindow");
		return -1;
	}


	CDC* pDC = pWnd->GetDC();
	if (pDC == nullptr)
	{
		ShowMsg(m_TcpSocket.GetErrMsg());
		LOGE("GetDC");
		pWnd->ReleaseDC(pDC);
		return -1;
	}

	//1.2 Get the width and height of desktop
	CRect rc;
	pWnd->GetClientRect(rc);
	int nHeight = rc.Height();
	int nWidth = rc.Width();

	//1.3  Create Memory DC
	CDC MemDC;
	if (!MemDC.CreateCompatibleDC(pDC))
	{
		ShowMsg(m_TcpSocket.GetErrMsg());
		LOGE("CreateCompatibleDC");
		pWnd->ReleaseDC(pDC);
		return -1;
	}





	////1.4 创建 CImage
	//CImage screenshotImg;
	//if (screenshotImg.Create(nWidth, nHeight, 32) == 0)
	//{
	//	ShowMsg(m_TcpSocket.GetErrMsg());
	//	LOGE("Create");
	//	return -1;
	//}

	//MemDC.SelectObject(screenshotImg);
	//MemDC.BitBlt(0, 0, nWidth, nHeight, pDC, 0, 0, SRCCOPY);

#if 1
	//1.4 Create CBitmap
	CBitmap* pBmp = new CBitmap;
	if (pBmp == nullptr)
	{
		LOGE("pBitmap");
		pWnd->ReleaseDC(pDC);
		return -1;
	}

	pBmp->CreateCompatibleBitmap(pDC, nWidth, nHeight);
	MemDC.SelectObject(pBmp);
	MemDC.BitBlt(0, 0, nWidth, nHeight, pDC, 0, 0, SRCCOPY);

#endif // 0

#ifdef DEBUG
	//CString strFileName = _T("screenshot.png");
	//screenshotImg.Save(strFileName);

	//CImage screenshotImg;
	//if (screenshotImg.Create(nWidth, nHeight, 32) == 0)
	//{
	//	ShowMsg(m_TcpSocket.GetErrMsg());
	//	LOGE("Create");
	//	return -1;
	//}
	//// Use a new CDC for the CImage
	//CDC MemDCCopy;
	//MemDCCopy.CreateCompatibleDC(pDC);

	//MemDCCopy.SelectObject(screenshotImg);
	//MemDCCopy.BitBlt(0, 0, nWidth, nHeight, pDC, 0, 0, SRCCOPY);

	//CString strFileName = _T("screenshot.png");
	//screenshotImg.Save(strFileName);


#endif // DEBUG


	//2.  Get image data and calculate size
	DWORD nImgDataSize = nWidth * nHeight * sizeof(COLORREF);

	ImagePacket* pImagePacket = (ImagePacket*)malloc(sizeof(ImagePacket) + nImgDataSize);
	if (pImagePacket == NULL)
	{
		LOGE("malloc");
		pWnd->ReleaseDC(pDC);
		delete pBmp;
		return -1;
	}
	BYTE* pImageData = (BYTE*)malloc(nImgDataSize);
	if (pImageData == NULL)
	{
		LOGE("malloc");
		pWnd->ReleaseDC(pDC);
		delete pBmp;
		free(pImagePacket);
		return -1;
	}
	DWORD nRet = pBmp->GetBitmapBits(nImgDataSize, pImageData);
	if (nRet == 0)
	{
		LOGE("GetBitmapBits");
		pWnd->ReleaseDC(pDC);
		delete pBmp;
		free(pImagePacket);
		free(pImageData);
		return -1;
	}


	//3.Create and fill the header and screenshot data packet structure
	stPacketHdr hdr;
	hdr.nCmd = CMD_DESKTOPDATA;
	hdr.nLen = sizeof(ImagePacket) + nImgDataSize;

	pImagePacket->m_width = nWidth;
	pImagePacket->m_height = nHeight;
	pImagePacket->m_dataSize = nImgDataSize;
	memcpy(pImagePacket->m_imageBuffer, pImageData, nImgDataSize);

	//发送
	int nSentHdr = m_TcpSocket.Sent((char*)&hdr, sizeof(hdr));
	if (nSentHdr < 0)
	{
		LOGE("sent");
		pWnd->ReleaseDC(pDC);
		delete pBmp;
		free(pImagePacket);
		free(pImageData);
		return -1;
	}
	int nSentPacket = m_TcpSocket.Sent((char*)pImagePacket, hdr.nLen);
	if (nSentPacket < 0) {
		LOGE("sent");
		pWnd->ReleaseDC(pDC);
		delete pBmp;
		free(pImagePacket);
		free(pImageData);
		return -1;
	}


	free(pImageData);
	free(pImagePacket);


	// Release DC
	pWnd->ReleaseDC(pDC);
	delete pBmp;

	return 0;

}


void CRemoteCtrlClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (nIDEvent == TIMER_ID_SCREENSHOT)
	{
		// Execute GetScreenShot every 500ms
		GetScreenshot();
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CRemoteCtrlClientDlg::StartScreenshotTimer()
{
	// set up timer, 500ms interval
	SetTimer(TIMER_ID_SCREENSHOT, 500, nullptr);
}

void CRemoteCtrlClientDlg::StopScreenshotTimer()
{
	KillTimer(TIMER_ID_SCREENSHOT);
}

DWORD WINAPI CRemoteCtrlClientDlg::DownloadThreadProc(LPVOID lParam)
{
	ThreadParams* pThreadParam = (ThreadParams*)lParam;
	CRemoteCtrlClientDlg* pThis = pThreadParam->m_pDlg;
	char szPath[MAX_PATH];
	strcpy_s(szPath, strlen((char*)(pThreadParam->m_pBuff)) + 1, (char*)pThreadParam->m_pBuff);

	CFile srcFile;
	static CFileException ex;
	char szBuf[0x8000];
	try {
		if (!srcFile.Open(szPath, CFile::typeBinary | CFile::modeReadWrite, &ex)) {
			char* pError = new char[MAXBYTE];
			ex.GetErrorMessage(pError, MAXBYTE);
			pThis->ShowMsg(pError);
			return -1;
		}
		else {
			//read file
			ULONGLONG ullFileSize = srcFile.GetLength();
			ULONGLONG ullCopyByte = 0;
			//send the total size of the file to server
			stPacketHdr hdr;
			hdr.nCmd = CMD_DOWNLOADSTART;
			hdr.nLen = sizeof(ULONGLONG);
			pThis->m_TcpSocket.Sent((char*)&hdr, sizeof(hdr));
			pThis->m_TcpSocket.Sent((char*)&ullFileSize, hdr.nLen);

			while (ullCopyByte <= ullFileSize) {
				int nByte = srcFile.Read(szBuf, sizeof(szBuf));
				if (nByte <= 0)
				{
					break;
				}

				//send 
				stPacketHdr hdr;
				hdr.nCmd = CMD_DOWNLOADFILE;
				hdr.nLen = nByte;
				pThis->m_TcpSocket.Sent((char*)&hdr, sizeof(hdr));
				pThis->m_TcpSocket.Sent((char*)&szBuf, nByte);
			}
		
			hdr.nCmd = CMD_DNCOMPLETE;
			hdr.nLen = 0;
			pThis->m_TcpSocket.Sent((char*)&hdr, sizeof(hdr));
			::PostMessage(pThis->m_hWnd, WM_DOWNLOAD_COMPLETE, 0, 0);
		}
	}
	catch (CException* e)
	{
		char* pError = new char[MAXBYTE];
		e->GetErrorMessage(pError, MAXBYTE);
		LOGE(pError);
	}

	srcFile.Close();

	return 0;
}

int CRemoteCtrlClientDlg::InitCmd()
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	//Create anonymous 
	if (!CreatePipe(&m_hOutputRd, &m_hOutputWr, &saAttr, 0))
		LOGE("CreatePipe");
	if (!CreatePipe(&m_hInputRd, &m_hInputWr, &saAttr, 0))
		LOGE("CreatePipe");

	//创建子进程
	m_piProcInfo;
	STARTUPINFO siStartInfo;
	ZeroMemory(&m_piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdOutput = m_hOutputWr;
	siStartInfo.hStdError = m_hOutputWr;
	siStartInfo.hStdInput = m_hInputRd;
	siStartInfo.dwFlags = STARTF_USESTDHANDLES |
		STARTF_USESHOWWINDOW | STARTF_USESIZE | STARTF_USEPOSITION;
	siStartInfo.wShowWindow = SW_HIDE;
	siStartInfo.dwX = 0;
	siStartInfo.dwY = 0;
	siStartInfo.dwXSize = 800;
	siStartInfo.dwYSize = 800;


	//继承补丁
	if (!CreateProcess("C:\\Windows\\System32\\cmd.exe",
		NULL,       // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          //inherit handle
		CREATE_NO_WINDOW,              // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&m_piProcInfo))
		return -1;


	DWORD dwBytes = 0;
	char  strBuff[4096] = { 0 };
	DWORD dwBytesRead = 0;
	Sleep(100);
	while (PeekNamedPipe(m_hOutputRd, NULL, 0, NULL, &dwBytes, NULL)
		&& dwBytes > 0)
	{
		if (!ReadFile(m_hOutputRd, strBuff, 0x1000, &dwBytesRead, NULL) || dwBytesRead <= 0)
		{
			return -1;
			LOGE("ReadFile");
		}
		Sleep(100);

		stPacketHdr hdr;
		hdr.nCmd = CMD_CMD;
		hdr.nLen = dwBytesRead + 1;
		m_TcpSocket.Sent((char*)&hdr, sizeof(hdr));
		m_TcpSocket.Sent(strBuff, dwBytesRead + 1);

		memset(strBuff, 0, sizeof(strBuff));  //clean up buffer
	}
	//Sleep(100);
	return 0;
}


int CRemoteCtrlClientDlg::HandleCmd(LPBYTE pCmd, DWORD dwSize)
{
	DWORD dwBytesWr = 0;

	if (!WriteFile(                    //Write to pipe
		m_hInputWr,
		pCmd,
		dwSize,
		&dwBytesWr,
		NULL) || dwBytesWr <= 0)
	{
		return -1;
		LOGE("WriteFile");
	}
	Sleep(100);
	DWORD dwBytes = 0;
	char  strBuff[0x10000] = { 0 };
	DWORD dwBytesRead = 0;
	while (PeekNamedPipe(m_hOutputRd, NULL, 0, NULL, &dwBytes, NULL)
		&& dwBytes > 0) //Loop to determine whether there is data in the pipeline within 1 second. If there is, continue looping, otherwise exit
	{
		if (!ReadFile(   //Read From pipe
			m_hOutputRd,
			strBuff,
			0x10000,
			&dwBytesRead,
			NULL) || dwBytesRead <= 0)
		{
			return -1;
			LOGE("ReadFile")
		}
		Sleep(1000);

		stPacketHdr hdr;
		hdr.nCmd = CMD_CMD;
		hdr.nLen = dwBytesRead + 1;
		m_TcpSocket.Sent((char*)&hdr, sizeof(hdr));
		m_TcpSocket.Sent(strBuff, dwBytesRead + 1);

		memset(strBuff, 0, sizeof(strBuff));  //clear buffer
	}
	return 0;
}

int CRemoteCtrlClientDlg::StopCmd()
{
	// closeHandle
	if (m_hOutputRd != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hOutputRd);
		m_hOutputRd = INVALID_HANDLE_VALUE;
	}

	if (m_hOutputWr != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hOutputWr);
		m_hOutputWr = INVALID_HANDLE_VALUE;
	}

	if (m_hInputRd != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hInputRd);
		m_hInputRd = INVALID_HANDLE_VALUE;
	}

	if (m_hInputWr != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hInputWr);
		m_hInputWr = INVALID_HANDLE_VALUE;
	}

	// close process handle
	if (m_piProcInfo.hProcess != NULL)
	{
		CloseHandle(m_piProcInfo.hProcess);
		m_piProcInfo.hProcess = NULL;
	}

	if (m_piProcInfo.hThread != NULL)
	{
		CloseHandle(m_piProcInfo.hThread);
		m_piProcInfo.hThread = NULL;
	}
	ShowMsg("cmd is closed");
	return 0;
}


void CRemoteCtrlClientDlg::StopThread()
{
	m_bStopThread = true;
	if (m_pThread) {
		if (m_pThread->joinable()) {
			m_pThread->join(); // Wait for the thread to finish
		}
		delete m_pThread;
		m_pThread = nullptr;
	}
}

void CRemoteCtrlClientDlg::ProcessMouseEvent(LPBYTE pBuff)
{
	MousePacket* pMousePacket = (MousePacket*)pBuff;

	//map the received point to screen coordinates
	POINT pt = pMousePacket->m_pt;
	HWND hwnd = ::GetDesktopWindow();
	::ClientToScreen(hwnd, &pt);

	DWORD dwFlags = 0;
	DWORD mouseData = 0;
	//DWORD dwFlags = MOUSEEVENTF_ABSOLUTE;
	//LONG dx = (pt.x * 65535) / GetSystemMetrics(SM_CXSCREEN);
	//LONG dy = (pt.y * 65535) / GetSystemMetrics(SM_CYSCREEN);
	//DWORD mouseData = 0;

	switch (pMousePacket->m_message) {
	case WM_LBUTTONDOWN:
		dwFlags |= MOUSEEVENTF_LEFTDOWN;
		break;
	case WM_LBUTTONUP:
		dwFlags |= MOUSEEVENTF_LEFTUP;
		break;
	case WM_LBUTTONDBLCLK:
		dwFlags |= MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
	case WM_RBUTTONDOWN:
		dwFlags |= MOUSEEVENTF_RIGHTDOWN;
		break;
	case WM_RBUTTONUP:
		dwFlags |= MOUSEEVENTF_RIGHTUP;
	case WM_RBUTTONDBLCLK:
		dwFlags |= MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP;
	case WM_MBUTTONDOWN:
		dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
		break;
	case WM_MBUTTONUP:
		dwFlags |= MOUSEEVENTF_MIDDLEUP;
		break;
	case WM_MBUTTONDBLCLK:
		dwFlags |= MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP;
		break;
	case WM_MOUSEMOVE:
		dwFlags |= MOUSEEVENTF_MOVE;
		break;
	case WM_MOUSEWHEEL:
		mouseData = GET_WHEEL_DELTA_WPARAM(pMousePacket->m_wParam);
		dwFlags |= MOUSEEVENTF_WHEEL;
		break;
	case WM_MOUSEHWHEEL:
		dwFlags |= MOUSEEVENTF_HWHEEL;
		mouseData = GET_WHEEL_DELTA_WPARAM(pMousePacket->m_wParam);
		break;
	case WM_XBUTTONDOWN:
		dwFlags |= MOUSEEVENTF_XDOWN;
		mouseData = XBUTTON1;
		break;
	case WM_XBUTTONUP:
		dwFlags |= MOUSEEVENTF_XUP;
		mouseData = XBUTTON1;
		break;
	case WM_XBUTTONDBLCLK:
		dwFlags |= MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP;
		mouseData = XBUTTON1;
		break;
	default:
		break;
	}

	//send mouse event
	//SendInput(1, &input, sizeof(INPUT));
	mouse_event(dwFlags, pt.x, pt.y, mouseData, 0);
	if (pMousePacket->m_message == WM_MOUSEMOVE)
	{
		SetCursorPos(pt.x, pt.y);
	}

	delete[] pBuff;

}

void CRemoteCtrlClientDlg::ProcessKeybdEvent(LPBYTE pBuff)
{
	KeybdPacket* pKeybdPkt = (KeybdPacket*)pBuff;
	BYTE bVk = static_cast<BYTE> (pKeybdPkt->m_wParam);


	switch (pKeybdPkt->m_msg) {
	case WM_KEYDOWN:
		keybd_event(bVk, MapVirtualKey(bVk, 0), 0, 0);
		break;
	case WM_KEYUP:
		keybd_event(bVk, MapVirtualKey(bVk, 0), KEYEVENTF_KEYUP, 0);
		break;
	default:
		break;
	}

	delete[] pBuff;

}

int CRemoteCtrlClientDlg::GetDirectory(LPBYTE pBuff, DWORD nLen)
{
	std::vector<FileTreeNode*> lstFileNode;
	char szPath[MAX_PATH];
	strcpy_s(szPath, (size_t)nLen, (char*)pBuff);
	CFileFind finder;
	strcat_s(szPath, MAX_PATH, "*.*");
	BOOL bWorking = finder.FindFile(szPath);
	while (bWorking) {
		bWorking = finder.FindNextFile();
		CString csText = finder.GetFileName();
		if (csText == "." || csText == "..")
			continue;
		DWORD fileNameLength = csText.GetLength();
		size_t nodeSize = sizeof(FileTreeNode) + fileNameLength * sizeof(char);
		FileTreeNode* pFileNode = (FileTreeNode*)malloc(nodeSize);
		if (pFileNode == nullptr) {
			LOGE("malloc");
			continue;
		}
		pFileNode->m_nFileNameLen = fileNameLength;
		pFileNode->m_isDirectory = finder.IsDirectory();
		strcpy_s(pFileNode->m_fileName, fileNameLength + 1, csText.GetBuffer());
		lstFileNode.push_back(pFileNode);
	}

	//calculate the total size needed for the serialized data
	size_t totalSize = 0;
	for (auto fileNode : lstFileNode) {
		totalSize += sizeof(FileTreeNode) + fileNode->m_nFileNameLen * sizeof(char);

	}

	// Allocate buffer
	std::vector<char> buffer(totalSize);
	char* p = buffer.data();

	for (auto fileNode : lstFileNode) {
		// serialize fileNameLen
		memcpy(p, &fileNode->m_nFileNameLen, sizeof(DWORD));
		//serialize m_bIsDirectory
		p += sizeof(DWORD);
		memcpy(p, &fileNode->m_isDirectory, sizeof(BOOL));
		p += sizeof(BOOL);
		memcpy(p, &fileNode->m_fileName, fileNode->m_nFileNameLen * sizeof(char));
		p += fileNode->m_nFileNameLen * sizeof(char);

	}

	// send data
	stPacketHdr hdr;
	hdr.nCmd = CMD_FILEPATH;
	hdr.nLen = static_cast<DWORD>(totalSize);
	m_TcpSocket.Sent((char*)&hdr, sizeof(hdr));
	m_TcpSocket.Sent(buffer.data(), hdr.nLen);

	// free allocated memory
	for (auto fileNode : lstFileNode)
	{
		free(fileNode);
	}
	return 0;
}

int CRemoteCtrlClientDlg::GetFiles(LPBYTE pBuff, DWORD nLen)
{
	std::vector<FileInfo*> lstFileInfo;
	char szPath[MAX_PATH];
	strcpy_s(szPath, (size_t)nLen, (char*)pBuff);
	CFileFind finder;
	strcat_s(szPath, MAX_PATH, "*.*");
	//traverse directory
	BOOL bWorking = finder.FindFile(szPath);
	while (bWorking) {
		bWorking = finder.FindNextFile();
		CString csText = finder.GetFileName();
		if (csText == "." || csText == "..")
			continue;
		DWORD fileNameLength = csText.GetLength();
		size_t fileInfoSize = sizeof(FileInfo) + fileNameLength * sizeof(char);
		FileInfo* pFileInfo = (FileInfo*)malloc(fileInfoSize);
		if (pFileInfo == nullptr) {
			LOGE("malloc");
			continue;
		}
		//fill in FileInfo structure
		pFileInfo->m_nFileNameLen = fileNameLength;
		pFileInfo->m_isDirectory = finder.IsDirectory();
		pFileInfo->m_fileSize = finder.GetLength();
		finder.GetLastWriteTime(&pFileInfo->m_dateModified);
		strcpy_s(pFileInfo->m_szFileName, fileNameLength + 1, csText.GetBuffer());
		lstFileInfo.push_back(pFileInfo);
	}

	//calculate the total size needed for the serialized data
	size_t totalSize = 0;
	for (auto fileInfo : lstFileInfo) {
		totalSize += sizeof(FileInfo) + fileInfo->m_nFileNameLen * sizeof(char);
	}

	// Allocate buffer
	std::vector<char> buffer(totalSize);
	char* p = buffer.data();

	for (auto fileInfo : lstFileInfo) {
		// serialize fileNameLen
		memcpy(p, &fileInfo->m_nFileNameLen, sizeof(DWORD));
		p += sizeof(DWORD);
		//serialize m_bIsDirectory
		memcpy(p, &fileInfo->m_isDirectory, sizeof(BOOL));
		p += sizeof(BOOL);
		//serialize m_fileSize
		memcpy(p, &fileInfo->m_fileSize, sizeof(ULONGLONG));
		p += sizeof(ULONGLONG);
		//serialize dataModified;
		memcpy(p, &fileInfo->m_dateModified, sizeof(FILETIME));
		p += sizeof(FILETIME);
		//serialize file name
		memcpy(p, &fileInfo->m_szFileName, fileInfo->m_nFileNameLen * sizeof(char));
		p += fileInfo->m_nFileNameLen * sizeof(char);

	}

	// send data
	stPacketHdr hdr;
	hdr.nCmd = CMD_GETFILES;
	hdr.nLen = static_cast<DWORD>(totalSize);
	m_TcpSocket.Sent((char*)&hdr, sizeof(hdr));
	m_TcpSocket.Sent(buffer.data(), hdr.nLen);

	// free allocated memory
	for (auto fileInfo : lstFileInfo)
	{
		free(fileInfo);
	}

	return 0;
}

int CRemoteCtrlClientDlg::DownloadFiles(LPBYTE pBuff, DWORD nLen, CRemoteCtrlClientDlg* pThis)
{
	ThreadParams* pThreadParam = new ThreadParams; 
	pThreadParam->m_pDlg = pThis;
	pThreadParam->m_pBuff = pBuff;
	m_hDownloadThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DownloadThreadProc, pThreadParam, 0, NULL);

	return 0;
}

int CRemoteCtrlClientDlg::GetDstPath(LPBYTE pBuff, DWORD nLen)
{
	m_csDstPath = CString(pBuff);
	return 0;
}

int CRemoteCtrlClientDlg::WriteFiles(LPBYTE pBuff, DWORD nLen)
{
	CFile dstFile;
	CFileException ex;

	try {
		if (!dstFile.Open(m_csDstPath, CFile::typeBinary | CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite, &ex))
		{
			char* pError = new char[MAXBYTE];
			ex.GetErrorMessage(pError, MAXBYTE);
			AfxMessageBox(pError);
			LOGE("dstFile.Open");
			return FALSE;
		}
		else {
			dstFile.SeekToEnd();
			DWORD dwCopyBytes = 0;
			DWORD chunkSize = 0x8000;// write 0x8000 every time
			while (dwCopyBytes < nLen) {
				DWORD dwBytesToWrite = min(chunkSize, nLen - dwCopyBytes);
				dstFile.Write(pBuff + dwCopyBytes, dwBytesToWrite);
				dwCopyBytes += dwBytesToWrite;
			}
			//m_ullTransferredSize += chunkSize;
			//::PostMessage(this->GetSafeHwnd(), WM_SET_PROGRESS, WPARAM(chunkSize), 0);
		}
	}
	catch (CException* e) {
		char* pError = new char[MAXBYTE];
		e->GetErrorMessage(pError, MAXBYTE);
		ShowMsg(pError);
		dstFile.Close();
		return FALSE;
	}
	dstFile.Close();
	return TRUE;
	return 0;
}

void CRemoteCtrlClientDlg::CompleteUpload()
{
	ShowMsg("Complete file upload!");
}

BOOL CRemoteCtrlClientDlg::GetLogicalDrive()
{
	char szLogicalDrive[4096];  //[0x1000] one page
	if (GetLogicalDriveStrings(sizeof(szLogicalDrive), szLogicalDrive) == 0)
	{
		ShowMsg(_T("Fail to get logical drive"));
		return FALSE;
	}

	// convert logical drive string: replace the "\" after drive with "-", 
	// remove "\0" and concat the strings
	//-> C:-E:-
	std::string outputDrives;
	char* pLogical = szLogicalDrive;
	while (*pLogical) {
		outputDrives += pLogical;
		outputDrives.back() = '-';
		pLogical += strlen(pLogical) + 1;
	}
	stPacketHdr hdr;
	hdr.nCmd = CMD_FILEBROWSE;
	hdr.nLen = outputDrives.length() + 1;
	m_TcpSocket.Sent((char*)&hdr, sizeof(hdr));
	m_TcpSocket.Sent((char*)outputDrives.c_str(), hdr.nLen);
	return TRUE;
}


afx_msg LRESULT CRemoteCtrlClientDlg::OnDownloadComplete(WPARAM wParam, LPARAM lParam)
{
	ShowMsg(_T("File download completed"));
	WaitForSingleObject(m_hDownloadThread, INFINITE);
	CloseHandle(m_hDownloadThread);
	return 0;
}
