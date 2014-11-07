
// flashtoolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "flashtool.h"
#include "flashtoolDlg.h"
#include "afxdialogex.h"
#include "flash\flash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ShowMessage(TCHAR *Buf);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CflashtoolDlg 对话框



CflashtoolDlg::CflashtoolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CflashtoolDlg::IDD, pParent)
	, m_EditContent(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CflashtoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, MyButton);
	DDX_Text(pDX, IDC_EDIT1, m_EditContent);
}

BEGIN_MESSAGE_MAP(CflashtoolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CflashtoolDlg::OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_LIST2, &CflashtoolDlg::OnLbnSelchangeList2)
END_MESSAGE_MAP()


// CflashtoolDlg 消息处理程序
CflashtoolDlg* CflashtoolDlg::pInst = NULL;


BOOL CflashtoolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// TODO:  在此添加额外的初始化代码
	pInst = this;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CflashtoolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CflashtoolDlg::OnPaint()
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
HCURSOR CflashtoolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CflashtoolDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	std::wstring filePathName;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("All Files (*.*)|*.*||"), NULL);
	if (dlg.DoModal() == IDOK)
	{
		filePathName = dlg.GetPathName(); //文件名保存在了filePathName里
		FlashImg(filePathName, ShowMessage);
		//MessageBox(NULL, L"刷机", MB_OK);
	}
	else
	{
		return;
	}
}

void CflashtoolDlg::UpdateEdit(TCHAR *content)
{
	m_EditContent += (char *)content;
	/*int len = WideCharToMultiByte(CP_ACP, 0, content, wcslen(content), NULL, 0, NULL, NULL);
	char *m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, content, wcslen(content), m_char, len, NULL, NULL);
	m_char[len] = '\0';*/
	UpdateData(FALSE);
}



void ShowMessage(TCHAR *Buf)
{
	CflashtoolDlg::pInst->UpdateEdit(Buf);
}

void CflashtoolDlg::OnLbnSelchangeList2()
{
	// TODO:  在此添加控件通知处理程序代码
}
