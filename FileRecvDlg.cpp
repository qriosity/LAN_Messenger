// FileRecvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FileRecvDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileRecvDlg dialog


CFileRecvDlg::CFileRecvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileRecvDlg::IDD, pParent)
{
	//m_ClientSocket == INVALID_SOCKET;
	m_ClientSocket = INVALID_SOCKET;
}

CFileRecvDlg::~CFileRecvDlg()
{
	if (INVALID_SOCKET != m_ClientSocket)
	{
		closesocket(m_ClientSocket);
	}
}


void CFileRecvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileRecvDlg)
	DDX_Control(pDX, IDC_STC_Info, m_wndInfo);
	DDX_Control(pDX, IDC_PROGRESS1, m_wndProgress);
	DDX_Control(pDX, IDC_BTN_RECV, m_btnRecv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileRecvDlg, CDialog)
	//{{AFX_MSG_MAP(CFileRecvDlg)
	ON_BN_CLICKED(IDC_BTN_RECV, OnBtnRecv)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_RECV_CLOSE, &CFileRecvDlg::OnBnClickedBtnRecvClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileRecvDlg message handlers

void CFileRecvDlg::OnBtnRecv()
{
	// TODO: Add your control notification handler code here

	CFileDialog dlg(FALSE, NULL, m_sendFile.szFileName);//FALSE 保存文件
	if (IDOK != dlg.DoModal())
	{
		return;
	}

	GetDlgItem(IDC_BTN_RECV)->EnableWindow(FALSE);

	//得到路径名称
	m_strFilePath = dlg.GetPathName();

	//创建线程
	AfxBeginThread(FileRecvThread, this);

}

//接受数据的线程  分段读取  大数据量的分发与接受
UINT CFileRecvDlg::FileRecvThread(LPVOID pParam)
{
	// 一个指向当前控件的指针
	CFileRecvDlg* pThis = (CFileRecvDlg*)pParam;
	// 获取文件的总大小
	DWORD nFileLen = pThis->m_sendFile.dwFileLength;

	//创建文件
	CFile file;
	// 打开文件失败的话（如无写入权限等）
	if (FALSE == file.Open(pThis->m_strFilePath,
		CFile::modeCreate | CFile::modeWrite))
	{
		return 0;
	}
	//设置进度条
	pThis->m_wndProgress.SetRange32(0, nFileLen);
	pThis->m_wndInfo.SetWindowText(_T("正在接受数据......"));
	//接受数据
	CHAR szBuf[4096] = { 0 }; // 设置一个4k的临时缓冲区
	DWORD nLeft = nFileLen;   // 文件的总大小
	while (nLeft > 0)
	{
		//计算接受的数据量
		DWORD nRecv = 4096;	 // 剩余的文件大小
		if (nLeft < nRecv)   // 如果总文件大小小于4k,则将剩余文件大小赋值给剩余文件大小
		{
			nRecv = nLeft;
		}

		//接收数据
		pThis->RecvData(szBuf, nRecv);
		//recv(pThis->m_ClientSocket, szBuf, nLeft, 0);
		//写入文件
		file.Write(szBuf, nRecv);
		//计算剩余数据
		nLeft = nLeft - nRecv;

		//设置进度条
		pThis->m_wndProgress.SetPos(nFileLen - nLeft);

	}
	pThis->m_wndInfo.SetWindowText(_T("接受数据成功"));


	//关闭文件
	file.Close();

	return 0;
}

//接受数据  循环接受 不是一次接受完毕
BOOL CFileRecvDlg::RecvData(LPVOID pData, DWORD nLen)  // 临时缓冲区 临时缓冲区剩余大小
{

	LPSTR pTmpData = (LPSTR)pData;	// 临时缓冲区
	int nLeft = nLen;				// 剩余文件大小
	while (nLeft > 0)
	{
		int nRecv = recv(m_ClientSocket, pTmpData, nLeft, 0); // recv返回的是接受的内容的大小
		if (SOCKET_ERROR == nRecv)
		{
			return FALSE;
		}
		nLeft -= nRecv;			// 更新临时缓冲区剩余大小

		pTmpData += nRecv;		// 更新临时缓冲区已经使用的大小

	}

	return TRUE;
}

BOOL CFileRecvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	//接受包头以及发送的文件信息
	NETHEADER header = { 0 };
	RecvData((LPSTR)&header, sizeof(header));
	RecvData((LPSTR)&m_sendFile, sizeof(m_sendFile));

	//获得了文件信息，可以将文件信息显示在窗体上
	SetWindowText(m_strIP);
	m_wndInfo.SetWindowText(_T("请接收文件"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CFileRecvDlg::OnBnClickedBtnRecvClose()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileRecvDlg::OnOK();
}
