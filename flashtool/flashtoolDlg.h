
// flashtoolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CflashtoolDlg �Ի���
class CflashtoolDlg : public CDialogEx
{
// ����
public:
	CflashtoolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FLASHTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	void UpdateEdit(TCHAR *content);

	CButton MyButton;
	CString m_EditContent;

	static CflashtoolDlg* pInst;
	afx_msg void OnLbnSelchangeList2();
};
