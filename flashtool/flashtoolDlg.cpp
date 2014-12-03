
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

void ShowMessage(wchar_t *Buf);
unsigned int __stdcall ThreadFunc(void *param);
int GetProgressPace(std::wstring filepath);
#define WM_FLASHMSG WM_USER+1
int pace;

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
	DDX_Text(pDX, IDC_EDIT1, m_EditContent);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_PROGRESS1, pctrl);
}

BEGIN_MESSAGE_MAP(CflashtoolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CflashtoolDlg::OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_LIST2, &CflashtoolDlg::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON2, &CflashtoolDlg::OnBnClickedButton2)
	ON_MESSAGE(WM_FLASHMSG, &CflashtoolDlg::OnFlashMsg)
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

	pctrl.SetRange(0, 100);
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
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("All Files (*.*)|*.*||"), NULL);
	if (dlg.DoModal() == IDOK)
	{
		std::wstring filePathName;
		filePathName = dlg.GetPathName(); //文件名保存在了filePathName里
		pace = GetProgressPace(filePathName);
		unsigned int tid;
		tmpStr = new wchar_t[filePathName.size() + 1];
		memset(tmpStr, 0, filePathName.size());
		wcsncpy_s(tmpStr, filePathName.size() + 1, filePathName.c_str(), filePathName.size());
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void *)tmpStr, 0, &tid);
	}
	else
	{
		return;
	}
}

void CflashtoolDlg::UpdateEdit(wchar_t *content)
{
	::SendMessage(this->m_hWnd, WM_FLASHMSG, (LPARAM)content, 0);
}

void ShowMessage(wchar_t *Buf)
{
	CflashtoolDlg::pInst->UpdateEdit(Buf);
}

void CflashtoolDlg::OnLbnSelchangeList2()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CflashtoolDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	std::wstring filePathName;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("All Files (*.*)|*.*||"), NULL);
	if (dlg.DoModal() == IDOK)
	{
		filePathName = dlg.GetPathName();
		iniFile(filePathName);
	}
	else
	{
		return;
	}
}

unsigned int __stdcall ThreadFunc(void *param)
{
	unsigned int result = 0;
	FlashImg((wchar_t *)param, ShowMessage);
	return result;
}

afx_msg LRESULT CflashtoolDlg::OnFlashMsg(WPARAM wParam, LPARAM lParam)
{
	//char *inStr = (char *)wParam;
	wchar_t outStr[1024];
	wcsncpy_s(outStr, 1024, (wchar_t *)wParam, wcslen((wchar_t *)wParam));
	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, inStr, strlen(inStr) + 1, outStr, 1024);
	m_EditContent += (char *)outStr;
	UpdateData(FALSE);

	// 使控件总是自动滚动到最新一行
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->LineScroll(pEdit->GetLineCount());

	// 刷新进度条
	CflashtoolDlg::pctrl.SetPos(CflashtoolDlg::pctrl.GetPos() + pace);
	return 0;
}

CflashtoolDlg::~CflashtoolDlg()
{
	delete[] tmpStr;
	CloseHandle(m_hThread);
}

int GetProgressPace(std::wstring filepath)
{
	HZIP hz = OpenZip((void*)filepath.c_str(), 0, ZIP_FILENAME);
	if (hz == NULL)
	{
		CloseZip(hz);
		return -1;
	}
	ZIPENTRYW ze;
	GetZipItem(hz, -1, &ze);
	int nNum = ze.index;
	bool bOK = TRUE;
	int tmp = 0;
	for (int zi = 0; zi < nNum; zi++)
	{
		GetZipItem(hz, zi, &ze);
		if (IsImg(ze.name))
			tmp++;
	}
	CloseZip(hz);
	return 100 % (tmp + 1) ? 100 / (tmp + 1) + 1 : 100 / (tmp + 1);
}