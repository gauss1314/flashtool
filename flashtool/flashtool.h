
// flashtool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CflashtoolApp: 
// �йش����ʵ�֣������ flashtool.cpp
//

class CflashtoolApp : public CWinApp
{
public:
	CflashtoolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CflashtoolApp theApp;