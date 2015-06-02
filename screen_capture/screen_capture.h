
// screen_capture.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// Cscreen_captureApp:
// 有关此类的实现，请参阅 screen_capture.cpp
//

class Cscreen_captureApp : public CWinAppEx
{

	public:
//*******************************
	HWND m_hwndDlg;
//*******************************
public:
	Cscreen_captureApp();

// 重写
	public:
	virtual BOOL InitInstance();
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
// 实现

	DECLARE_MESSAGE_MAP()
};

extern Cscreen_captureApp theApp;