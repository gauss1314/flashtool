
// flashtoolDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CflashtoolDlg 对话框
class CflashtoolDlg : public CDialogEx
{
// 构造
public:
	CflashtoolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FLASHTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	void UpdateEdit(wchar_t *content);

	CString m_EditContent;
	CEdit m_edit;
	CProgressCtrl pctrl;
	static CflashtoolDlg* pInst;
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnBnClickedButton2();
protected:
	afx_msg LRESULT OnFlashMsg(WPARAM wParam, LPARAM lParam);
};
