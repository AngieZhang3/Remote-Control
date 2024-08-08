#pragma once

#pragma pack(push)
#pragma pack(1)

#include <Windows.h>

#define CMD_GETDESKTOP               0
#define CMD_DESKTOPDATA             1
#define CMD_STOPDESKTOP             2
#define CMD_MOUSE                         3
#define CMD_KEYBOARD                   4
#define CMD_CMD                              5
#define CMD_CMDINIT                       6
#define CMD_CMDCOMMAND         7
#define CMD_FILEBROWSE                8
#define CMD_FILEPATH                       9
#define CMD_GETFILES                       10
#define CMD_DOWNLOADFILE          11
#define CMD_DNCOMPLETE              12
#define CMD_DOWNLOADSTART      13
#define CMD_UPLOADFILE                 14  
#define CMD_UPLOADCOMPLETE     15
#define CMD_UPLOADSTART              16
#define CMD_CMDSTOP                      17
#define CMD_DISCONNECT                 18


typedef struct _stPacketHdr {
	short nCmd;
	DWORD nLen;
}stPacketHdr;


typedef struct  _stImagePacket {
    DWORD m_width;
    DWORD m_height;
    DWORD m_dataSize;
    BYTE m_imageBuffer[1];
} ImagePacket;

typedef struct _stMousePacket {
    DWORD m_message;
    POINT m_pt;
    WPARAM m_wParam;
}MousePacket;

typedef struct _stKeybdPacket {
    DWORD m_msg;
    WPARAM m_wParam;
    LPARAM m_lParam;
}KeybdPacket;

typedef struct _stFileTreeNode {
    DWORD m_nFileNameLen;
    BOOL m_isDirectory;
    char m_fileName[1]; //placeholder for flexible array member
}FileTreeNode;

typedef struct _stFileInfo {
    DWORD m_nFileNameLen;
    BOOL m_isDirectory;
    ULONGLONG m_fileSize;
    FILETIME m_dateModified;
    char m_szFileName[1];
}FileInfo;
#pragma pack(pop)