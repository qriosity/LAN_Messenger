#pragma once

#include "stdafx.h"

#define PORT_USERSVR 8989
#define PORT_FILESVR 8990

// 数据包的包头
typedef struct _tagNetHeader
{
	DWORD  dwVersion;
	DWORD  dwCmdID;
	DWORD  dwDataLength;
}NETHEADER, *LPNETHEADER;

// UDP数据报
typedef struct _tagUDPPacket
{
	NETHEADER	header;
	CHAR		szData[256];
}UDPPACKET, *LPUDPPACKET;

// 用户广播数据包
#define NETCMDID_USERBROADCAST 1
typedef struct _tagUserBroadcast
{
	NETHEADER	header;
	CHAR		szName[128];
	//CHAR		szSign[128];
}USERBROADCAST, *LPUSERBROADCAST;

// 用户正常退出数据包
#define NETCMDID_USERQUIT 2
typedef struct _tagUserQuit
{
	NETHEADER	header;
	DWORD		dwExitCode;
}USERQUIT, *LPUSERQUIT;

// 聊天数据
#define NETCMDID_USERCHAT 3
typedef struct _tagUserChat
{
	NETHEADER	header;
	char		szChat[256];
}USERCHAT, *LPUSERCHAT;

// 发送文件  
#define  NETCMDID_SENDFILE  4
typedef struct  _tagSendFile
{
	CHAR  szFileName[256];
	DWORD dwFileLength;
}SENDFILE, *LPSENDFILE;
