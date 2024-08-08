// CFileDlg.cpp : implementation file
//

#include "pch.h"
#include "RemoteCtrlServer.h"
#include "CFileDlg.h"
#include "afxdialogex.h"
#include "Proto.h"
#include "log.h"
#include <vector>
#include <iostream>


// CFileDlg dialog

IMPLEMENT_DYNAMIC(CFileDlg, CDialogEx)

CFileDlg::CFileDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FILE, pParent)
	//, m_csDownloadPath(_T(""))
	//, m_csUploadFile(_T(""))
	, m_editClientFilePath(_T(""))
{

}

CFileDlg::~CFileDlg()
{
}

void CFileDlg::SetSocket(CTcpSocket* pTcpSocket)
{
	m_pFileSocket = pTcpSocket;
}

void CFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_lstCtrl);
	DDX_Control(pDX, IDC_TREE1, m_treeCtrl);
	//DDX_Text(pDX, IDC_EDIT1, m_csDownloadPath);
	//DDX_Text(pDX, IDC_EDIT2, m_csUploadFile);
	DDX_Text(pDX, IDC_EDIT_CLIENTPATH, m_editClientFilePath);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressCtrl);
}


BEGIN_MESSAGE_MAP(CFileDlg, CDialogEx)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, &CFileDlg::OnItemexpandingTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CFileDlg::OnSelchangedTree1)
	//ON_BN_CLICKED(IDC_BUTTON1, &CFileDlg::OnBnClickedButton1)
//	ON_BN_CLICKED(IDC_BTN_UPLOADSEL, &CFileDlg::OnBnClickedBtnUploadsel)
	//ON_BN_CLICKED(IDC_BTN_DOWNLOAD, &CFileDlg::OnBnClickedBtnDownload)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CFileDlg::OnDblclkList2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &CFileDlg::OnNMCustomdrawProgress1)
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CFileDlg::OnItemchangedList2)
	ON_COMMAND(ID_CONTEXTMENU_UPLOAD, &CFileDlg::OnContextmenuUpload)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CFileDlg::OnNMRClickList2)
	ON_COMMAND(ID_CONTEXTMENU_DOWNLOAD, &CFileDlg::OnContextmenuDownload)
	//ON_MESSAGE(WM_SET_PROGRESS, &CFileDlg::OnSetProgress)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPLOAD_COMPLETE, &CFileDlg::OnUploadComplete)
END_MESSAGE_MAP()


// CFileDlg message handlers


BOOL CFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	if (!SendBrowseRequest())
	{
		AfxMessageBox(_T("Fail to open client's file explorer"));
		return FALSE;
	}
	InitListCtrl();
	m_lstCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_SHOWSELALWAYS);
	SetTimer(2, 100, NULL);
	return TRUE;
	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CFileDlg::InitTreeCtrl(LPBYTE pBuff, DWORD dwBuffSize)
{

	pBuff[dwBuffSize - 1] = '\0';
	std::vector<std::string> drives;
	char* tempStr = _strdup(reinterpret_cast <char*>(pBuff));
	if (tempStr == nullptr)
	{
		LOGE("strdup");
		return FALSE;
	}
	char* token = strtok(tempStr, "-");
	while (token != nullptr) {
		drives.push_back(token);
		token = strtok(nullptr, "-");
	}
	free(tempStr); // strdup used malloc so have to free

	for (const auto& drive : drives)
	{
		TVINSERTSTRUCT tvInsert = { 0 };
		tvInsert.item.mask = TVIF_TEXT | TVIF_CHILDREN;
		tvInsert.hParent = NULL;
		tvInsert.hInsertAfter = NULL;
		tvInsert.item.iImage = 0;
		tvInsert.item.iSelectedImage = 0;
		tvInsert.item.pszText = const_cast<char*>(drive.c_str());
		tvInsert.item.cchTextMax = MAX_PATH; // 260
		tvInsert.item.cChildren = 1;  //show +

		HTREEITEM hItem = m_treeCtrl.InsertItem(&tvInsert);
	}

	return TRUE;
}


BOOL CFileDlg::SendBrowseRequest()
{
	stPacketHdr hdr;
	hdr.nCmd = CMD_FILEBROWSE;
	hdr.nLen = 0;
	if (m_pFileSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
	{
		LOGE("sent");
		return FALSE;
	}
	return TRUE;
}


void CFileDlg::OnItemexpandingTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	char szPath[MAX_PATH];
	szPath[0] = '\0';
	GetItemPath(pNMTreeView->itemNew.hItem, szPath, 0, MAX_PATH);

	m_hCurrentTreeItem = pNMTreeView->itemNew.hItem;

	// send szPath to client
	// client to traverse the file path to get file name 
	stPacketHdr hdr;
	hdr.nCmd = CMD_FILEPATH;
	hdr.nLen = strlen(szPath) + 1;
	if (m_pFileSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
	{
		LOGE("sent");
	}
	if (m_pFileSocket->Sent((char*)&szPath, hdr.nLen) < 0)
	{
		LOGE("sent");
	}
	*pResult = 0;
}


BOOL CFileDlg::GetItemPath(HTREEITEM hRoot, char* lpBuf, DWORD dwLen, DWORD dwMaxLen)
{
	BOOL bRet = FALSE;
	if (!hRoot) {
		return FALSE;
	}
	CString csText = m_treeCtrl.GetItemText(hRoot);
	bRet = GetItemPath(m_treeCtrl.GetParentItem(hRoot), lpBuf, dwLen, dwMaxLen);
	strcat_s(lpBuf, dwMaxLen, csText.GetBuffer(0));
	strcat_s(lpBuf, dwMaxLen, "\\");
	return bRet;
}

BOOL CFileDlg::InsertTreeNode(LPBYTE pBuff, DWORD dwBuffSize)
{

	LPBYTE p = pBuff;
	while (p < pBuff + dwBuffSize) {
		DWORD fileNameLength;
		memcpy(&fileNameLength, p, sizeof(DWORD));
		p += sizeof(DWORD);

		//allocate memory for FileTreeNode
		size_t nodeSize = sizeof(FileTreeNode) + fileNameLength * sizeof(char);
		FileTreeNode* fileNode = (FileTreeNode*)malloc(nodeSize);
		if (fileNode == nullptr) {
			LOGE("malloc");
			continue;
		}
		fileNode->m_nFileNameLen = fileNameLength;

		memcpy(&fileNode->m_isDirectory, p, sizeof(BOOL));
		p += sizeof(BOOL);

		memcpy(&fileNode->m_fileName, p, fileNode->m_nFileNameLen);
		p += fileNode->m_nFileNameLen;
		fileNode->m_fileName[fileNode->m_nFileNameLen] = '\0'; // Ensure null-terminated string
		//check if node already exists, if yes, don't insert. 
		CString csNodeName = fileNode->m_fileName;
		if (CheckExistingNode(m_treeCtrl, m_hCurrentTreeItem, csNodeName))
		{
			continue;
		}

		//insert into tree control

		TVINSERTSTRUCT tvInsert = { 0 };
		tvInsert.item.mask = TVIF_TEXT | TVIF_CHILDREN;
		tvInsert.hParent = m_hCurrentTreeItem;
		tvInsert.hInsertAfter = TVI_LAST;
		tvInsert.item.pszText = fileNode->m_fileName;
		tvInsert.item.cchTextMax = fileNode->m_nFileNameLen + 1;
		if (fileNode->m_isDirectory) {
			tvInsert.item.cChildren = 1;
		}
		else {
			tvInsert.item.cChildren = 0;
		}
		HTREEITEM hitem = m_treeCtrl.InsertItem(&tvInsert);
		free(fileNode);

	}
	m_treeCtrl.Invalidate();
	m_treeCtrl.UpdateWindow();
	return TRUE;
}


int CFileDlg::InitListCtrl()
{
	m_lstCtrl.SetExtendedStyle(m_lstCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_lstCtrl.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 200);
	m_lstCtrl.InsertColumn(1, _T("Date modified"), LVCFMT_LEFT, 200);
	m_lstCtrl.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 200);
	m_lstCtrl.InsertColumn(3, _T("Size"), LVCFMT_LEFT, 200);
	return 0;
}

BOOL CFileDlg::InsertFileList(LPBYTE pBuff, DWORD dwBuffSize)
{
	//deserialize FileInfo
	LPBYTE p = pBuff;
	while (p < pBuff + dwBuffSize) {
		DWORD fileNameLength;
		memcpy(&fileNameLength, p, sizeof(DWORD));
		p += sizeof(DWORD);

		//allocate memory for FileInfo structure
		size_t fileInfoSize = sizeof(FileInfo) + fileNameLength * sizeof(char);
		FileInfo* fileInfo = (FileInfo*)malloc(fileInfoSize);
		if (fileInfo == nullptr) {
			LOGE("malloc");
			continue;
		}
		fileInfo->m_nFileNameLen = fileNameLength;

		memcpy(&fileInfo->m_isDirectory, p, sizeof(BOOL));
		p += sizeof(BOOL);

		memcpy(&fileInfo->m_fileSize, p, sizeof(ULONGLONG));
		p += sizeof(ULONGLONG);

		memcpy(&fileInfo->m_dateModified, p, sizeof(FILETIME));
		p += sizeof(FILETIME);

		memcpy(&fileInfo->m_szFileName, p, fileInfo->m_nFileNameLen);
		p += fileInfo->m_nFileNameLen;

		fileInfo->m_szFileName[fileInfo->m_nFileNameLen] = '\0'; // Ensure null-terminated string
		//insert into listControl
		CString csFileName = fileInfo->m_szFileName;
		int nItem = m_lstCtrl.InsertItem(0, csFileName);
		// deserialize and format FILETIME
		SYSTEMTIME stUTC, stLocal;
		FileTimeToSystemTime(&fileInfo->m_dateModified, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		CString csFormattedTime;
		csFormattedTime.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),
			stLocal.wMonth, stLocal.wDay, stLocal.wYear,
			stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
		m_lstCtrl.SetItemText(nItem, 1, csFormattedTime);

		//type
		CString csType;
		if (fileInfo->m_isDirectory == TRUE) {
			csType = _T("File folder");
		}
		else {
			csType = _T("File");
		}
		m_lstCtrl.SetItemText(nItem, 2, csType);

		//Size
		CString csFormattedSize;
		/*if (fileInfo->m_fileSize < 1024) {
			csFormattedSize.Format(_T("%llu B"), fileInfo->m_fileSize);
		}*/
		if (fileInfo->m_fileSize < 1024 * 1024) {
			csFormattedSize.Format(_T("%llu KB"), (fileInfo->m_fileSize) / 1024);
		}
		else if (fileInfo->m_fileSize < 1024 * 1024 * 1024) {
			csFormattedSize.Format(_T("%llu MB"), (fileInfo->m_fileSize) / (1024 * 1024));
		}
		else {
			csFormattedSize.Format(_T("%llu GB"), (fileInfo->m_fileSize) / (1024 * 1024 * 1024));
		}
		m_lstCtrl.SetItemText(nItem, 3, csFormattedSize);

		free(fileInfo);

	}
	//m_lstCtrl.Invalidate();
	//m_lstCtrl.UpdateWindow();
	return TRUE;
}

BOOL CFileDlg::WriteFiles(LPBYTE pBuff, DWORD dwBuffSize)
{

//	CString csFilePath = m_csDownloadPath + "\\" + m_csDNFileName;
	CFile dstFile;
	CFileException ex;

	try {
		if (!dstFile.Open(m_csDNLDPath.GetBuffer(), CFile::typeBinary | CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite, &ex))
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
			while (dwCopyBytes < dwBuffSize) {
				DWORD dwBytesToWrite = min(chunkSize, dwBuffSize - dwCopyBytes);
				dstFile.Write(pBuff + dwCopyBytes, dwBytesToWrite);
				dwCopyBytes += dwBytesToWrite;
			}
			m_ullTransferredSize += chunkSize;
			//::PostMessage(this->GetSafeHwnd(), WM_SET_PROGRESS, WPARAM(chunkSize), 0);
		}
	}
		catch (CException *e) {
			char* pError = new char[MAXBYTE];
			e->GetErrorMessage(pError, MAXBYTE);
			OutputDebugString(pError);
			dstFile.Close();
			return FALSE;
		}
		dstFile.Close();
		return TRUE;
	}

BOOL CFileDlg::SaveFileSize(LPBYTE pBuff, DWORD dwBuffSize)
{
	m_ullTotalFileSize = *((ULONGLONG*)pBuff);
	m_progressCtrl.ShowWindow(SW_NORMAL);
	return 0;
}


	void CFileDlg::OnSelchangedTree1(NMHDR * pNMHDR, LRESULT * pResult)
	{
		m_lstCtrl.DeleteAllItems();
		LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
		char szPath[MAX_PATH];
		szPath[0] = '\0';
		GetItemPath(pNMTreeView->itemNew.hItem, szPath, 0, MAX_PATH);
		m_csSelectedDir = szPath;// save current directory 
		//show selected path
		m_editClientFilePath = m_csSelectedDir;
		UpdateData(FALSE);
		stPacketHdr hdr;
		hdr.nCmd = CMD_GETFILES;
		hdr.nLen = strlen(szPath) + 1;
		if (m_pFileSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
		{
			LOGE("sent");
		}
		if (m_pFileSocket->Sent((char*)&szPath, hdr.nLen) < 0)
		{
			LOGE("sent");
		}

		*pResult = 0;
	}


	//void CFileDlg::OnBnClickedButton1()
	//{
	//	//use SHBrowseForFolder function to choose the destination folder
	//	BROWSEINFO bi = { 0 };
	//	bi.lpszTitle = _T("Select the destination folder");
	//	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	//	if (pidl != NULL)
	//	{
	//		TCHAR path[MAX_PATH];
	//		if (SHGetPathFromIDList(pidl, path))
	//		{
	//			m_csDownloadPath = path;
	//			UpdateData(FALSE);
	//		}

	//		CoTaskMemFree(pidl);
	//	}

	//}


	//void CFileDlg::OnBnClickedBtnUploadsel()
	//{
	//	CFileDialog file(TRUE);
	//	if (file.DoModal() == IDOK) {
	//		m_csUploadFile = file.GetPathName();
	//		UpdateData(FALSE);
	//	}    
	//}


	//void CFileDlg::OnBnClickedBtnDownload()
	//{
	//	//get selected fileName
	//	/*int nItem = m_lstCtrl.GetNextItem(-1, LVNI_SELECTED);*/
	//	//if no files selected or if a folder is selected, ask the user to select  a file
	//	if (m_nSelectedIndex != -1)
	//	{
	//		m_lstCtrl.SetItemState(m_nSelectedIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	//		m_lstCtrl.EnsureVisible(m_nSelectedIndex, FALSE); // 确保项可见
	//	}
	//	if (m_nSelectedIndex == -1 || m_lstCtrl.GetItemText(m_nSelectedIndex, 2) == CString(_T("File"))) {
	//		AfxMessageBox(_T("please select a file"));
	//		m_csDownloadPath = " ";
	//		UpdateData(FALSE);
	//		return;
	//	}

	//	m_csDNFileName = m_lstCtrl.GetItemText(m_nSelectedIndex, 0);
	//	CString filePath = m_csSelectedDir + m_csDNFileName;
	//	int nLen = filePath.GetLength() + 1;
	//	char szFilePath[MAX_PATH];
	//	strcpy_s(szFilePath, nLen, filePath.GetBuffer());

	//	stPacketHdr hdr;
	//	hdr.nCmd = CMD_DOWNLOADFILE;
	//	hdr.nLen = nLen;
	//	if (m_pFileSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
	//	{
	//		LOGE("sent");
	//	}
	//	if (m_pFileSocket->Sent((char*)szFilePath, hdr.nLen) < 0)
	//	{
	//		LOGE("sent");
	//	}

	//	return;

	//}

	//show subfolders and files when double click the folders in list control
	void CFileDlg::OnDblclkList2(NMHDR * pNMHDR, LRESULT * pResult)
	{

		LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
		if (m_lstCtrl.GetItemText(pNMItemActivate->iItem, 2) == _T("File")) {
			return;
		}

		if (pNMItemActivate->iItem != -1) {
			CString csFileName = m_lstCtrl.GetItemText(pNMItemActivate->iItem, 0);
			m_csSelectedDir = m_csSelectedDir + csFileName + "\\";
		}
		//show current path
		m_editClientFilePath = m_csSelectedDir; 
		UpdateData(FALSE);

		stPacketHdr hdr;
		hdr.nCmd = CMD_GETFILES;
		hdr.nLen = strlen(m_csSelectedDir.GetBuffer()) + 1;
		if (m_pFileSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
		{
			LOGE("sent");
		}
		if (m_pFileSocket->Sent((char*)(m_csSelectedDir.GetBuffer(hdr.nLen)), hdr.nLen) < 0)
		{
			LOGE("sent");
		}

		m_lstCtrl.DeleteAllItems();
		*pResult = 0;
	}


	BOOL CFileDlg::CheckExistingNode(CTreeCtrl & treeCtrl, HTREEITEM hParent, CString & nodeText)
	{
		HTREEITEM hChild = treeCtrl.GetChildItem(hParent);
		while (hChild)
		{
			CString existingText = treeCtrl.GetItemText(hChild);
			if (existingText == nodeText)
			{
				return TRUE;
			}
			hChild = treeCtrl.GetNextSiblingItem(hChild);
		}
		return FALSE;
	}

	void CFileDlg::CloseDNFile()
	{
		
		AfxMessageBox("File download completed");
		m_progressCtrl.ShowWindow(SW_HIDE);
		m_progressCtrl.SetPos(0);
		m_ullTransferredSize = 0;

	}


	void CFileDlg::OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult)
	{
		LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
		// TODO: Add your control notification handler code here
		*pResult = 0;
	}


	//void CFileDlg::OnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult)
	//{
	//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	//	if (pNMLV->uChanged & LVIF_STATE)
	//	{
	//		if (pNMLV->uNewState & LVIS_SELECTED)
	//		{
	//			// 保存选中项的索引
	//			m_nSelectedIndex = pNMLV->iItem;
	//		}
	//	}
	//	*pResult = 0;
	//}


	void CFileDlg::OnContextmenuUpload()
	{

		//check if selected destination path valid.
		if (m_nSelectedIndex == -1 || m_lstCtrl.GetItemText(m_nSelectedIndex, 2) == CString(_T("File"))) {
			AfxMessageBox(_T("please select a folder"));
			return;
		}
	
		CFileDialog file(TRUE);
		if (file.DoModal() == IDOK) {
			m_csUploadFilePath = file.GetPathName();
			m_csUploadDstPath = m_editClientFilePath + m_lstCtrl.GetItemText(m_nSelectedIndex, 0) + "\\" + file.GetFileName();
			UploadFiles();
		}
	}


	void CFileDlg::OnNMRClickList2(NMHDR* pNMHDR, LRESULT* pResult)
	{
		LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

		CPoint clickPoint;
		GetCursorPos(&clickPoint);

		//Convert screen coordinates to client coordinates
		CPoint clientPoint = clickPoint;
		m_lstCtrl.ScreenToClient(&clientPoint);

		//Determine the line clicked
		int selectedItem = m_lstCtrl.HitTest(clientPoint);
		if (selectedItem != -1)
		{
			m_lstCtrl.SetItemState(selectedItem, LVIS_SELECTED, LVIS_SELECTED);

			//Create and load menu
			CMenu menu;
			menu.LoadMenu(IDR_MENU1);

			CMenu* pContextMenu = menu.GetSubMenu(0);

			//display context menu
			if (pContextMenu) {
				pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, clickPoint.x, clickPoint.y, this);
			}
		}

		m_nSelectedIndex = selectedItem;
		*pResult = 0;
	}


	void CFileDlg::OnContextmenuDownload()
	{


		//use SHBrowseForFolder function to choose the destination folder
		BROWSEINFO bi = { 0 };
		bi.lpszTitle = _T("Select the destination folder");
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

		if (pidl != NULL)
		{
			TCHAR path[MAX_PATH];
			if (SHGetPathFromIDList(pidl, path)) 
			{
				m_csDNLDPath = path;
				UpdateData(FALSE);
			}

			CoTaskMemFree(pidl);
		}

		//if no files selected or if a folder is selected, ask the user to select  a file
		if (m_nSelectedIndex == -1 || m_lstCtrl.GetItemText(m_nSelectedIndex, 2) == CString(_T("File folder"))) {
			AfxMessageBox(_T("please select a file"));
			return;
		}

		m_csDNFileName = m_lstCtrl.GetItemText(m_nSelectedIndex, 0);
		m_csDNLDPath = m_csDNLDPath + "\\" + m_csDNFileName; // Path for saving the downloaded file
		CString filePath = m_csSelectedDir + m_csDNFileName;
		int nLen = filePath.GetLength() + 1;
		char szFilePath[MAX_PATH];
		strcpy_s(szFilePath, nLen, filePath.GetBuffer());

		stPacketHdr hdr;
		hdr.nCmd = CMD_DOWNLOADFILE;
		hdr.nLen = nLen;
		if (m_pFileSocket->Sent((char*)&hdr, sizeof(hdr)) < 0)
		{
			LOGE("sent");
		}
		if (m_pFileSocket->Sent((char*)szFilePath, hdr.nLen) < 0)
		{
			LOGE("sent");
		}

		return;


	}

	int CFileDlg::UploadFiles()
	{
		m_progressCtrl.ShowWindow(SW_NORMAL);
		m_hUploadThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UploadThreadProc, (LPVOID)this, 0, NULL);
		return 0;
	}

	DWORD WINAPI CFileDlg::UploadThreadProc(LPVOID lpParam)
	{
		CFileDlg* pThis = (CFileDlg*)lpParam;

		CFile srcFile;
		static CFileException ex;
		char szBuf[0x8000];
		try {
			if (!srcFile.Open(pThis->m_csUploadFilePath.GetBuffer(), CFile::typeBinary | CFile::modeReadWrite, &ex)) {
				char* pError = new char[MAXBYTE];
				ex.GetErrorMessage(pError, MAXBYTE);
				LOGI("src.open");
				return -1;
			}
			else {
				//read file              
				ULONGLONG ullFileSize = srcFile.GetLength();
				//save the total length of the file
				pThis->m_ullTotalFileSize = ullFileSize;
				ULONGLONG ullCopyByte = 0;
				//send the destination folder path to the client
		
				stPacketHdr hdr;
				hdr.nCmd = CMD_UPLOADSTART;
				hdr.nLen = pThis->m_csUploadDstPath.GetLength() + 1;
				pThis->m_pFileSocket->Sent((char*)&hdr, sizeof(hdr));
				pThis->m_pFileSocket->Sent((char*)pThis->m_csUploadDstPath.GetBuffer(), hdr.nLen);

				while (ullCopyByte < ullFileSize) {
					int nByte = srcFile.Read(szBuf, sizeof(szBuf));
					if (nByte <= 0)
					{
						break;
					}

					//send 
					stPacketHdr hdr;
					hdr.nCmd = CMD_UPLOADFILE;
					hdr.nLen = nByte;
					pThis->m_pFileSocket->Sent((char*)&hdr, sizeof(hdr));
					pThis->m_pFileSocket->Sent((char*)&szBuf, nByte);
					pThis->m_ullTransferredSize += nByte;
				}
		
				hdr.nCmd = CMD_UPLOADCOMPLETE;
				hdr.nLen = 0;
				pThis->m_pFileSocket->Sent((char*)&hdr, sizeof(hdr));
				::PostMessage(pThis->m_hWnd, WM_UPLOAD_COMPLETE, 0, 0);
			}
		}
		catch (CException* e)
		{
			char* pError = new char[MAXBYTE];
			e->GetErrorMessage(pError, MAXBYTE);
			LOGE("src.Open");
		}

		srcFile.Close();
		return 0;
	}


	//afx_msg LRESULT CFileDlg::OnSetProgress(WPARAM wParam, LPARAM lParam)
	//{
	//	m_transferredSize += (DWORD)wParam;
	//	m_progressCtrl.SetPos((double)(m_transferredSize / m_ullDownloadFileSize)*100);
	//	return 0;
	//}


	void CFileDlg::OnTimer(UINT_PTR nIDEvent)
	{
		// TODO: Add your message handler code here and/or call default
		if (nIDEvent == 2) {
			m_progressCtrl.SetPos(((double)m_ullTransferredSize / m_ullTotalFileSize) * 100);
		}
		CDialogEx::OnTimer(nIDEvent);
	}


	afx_msg LRESULT CFileDlg::OnUploadComplete(WPARAM wParam, LPARAM lParam)
	{

		AfxMessageBox("File upload completed");
		m_progressCtrl.ShowWindow(SW_HIDE);
		m_progressCtrl.SetPos(0);
		m_ullTransferredSize = 0;
		WaitForSingleObject(m_hUploadThread, INFINITE);
		CloseHandle(m_hUploadThread);
		return 0;
	}
