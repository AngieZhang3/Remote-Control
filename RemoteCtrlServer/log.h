#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define DEBUG
#ifdef DEBUG
#define LOGD(fmt, ...)  printf(fmt, __VA_ARGS__)
#else
#define LOGD(fmt, ...) 
#endif

#define LOGI(func)  { \
    CString strInfo; \
    strInfo.Format("[RemoteCtrl][+] %-20s %s ok\n", __FUNCTION__,func);\
  OutputDebugString(strInfo);  \
}

#define LOGE(func)  { \
  LPVOID lpMsgBuf = GetErrorMsg(); \
CString strError; \
  strError.Format("[RemoteCtrl][-] %-20s %s Error:%-6d %s file:%s line:%d\n", \
       __FUNCTION__, \
       func, \
       WSAGetLastError(), \
       (char*)lpMsgBuf, \
       __FILE__,  \
       __LINE__); \
    OutputDebugString(strError); \
  LocalFree(lpMsgBuf);\
}


#define LOGW(func)  { \
  LPVOID lpMsgBuf = GetErrorMsg(); \
CString strWarning;\
strWarning.Format("[RemoteCtrl][-] %-20s %s Error:%-6d %s file:%s line:%d\n", \
    __FUNCTION__, \
    func, \
    WSAGetLastError(), \
    (char*)lpMsgBuf, \
    __FILE__, \
    __LINE__))\
  OutputDebugString(strWarning);\
  LocalFree(lpMsgBuf);\
}


LPVOID GetErrorMsg();