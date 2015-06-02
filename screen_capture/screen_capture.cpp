
// screen_capture.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "screen_capture.h"
#include "screen_captureDlg.h"
#include "CatchScreenDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cscreen_captureApp

BEGIN_MESSAGE_MAP(Cscreen_captureApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Cscreen_captureApp 构造

Cscreen_captureApp::Cscreen_captureApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_hwndDlg=NULL;
}


// 唯一的一个 Cscreen_captureApp 对象

Cscreen_captureApp theApp;


// Cscreen_captureApp 初始化

BOOL Cscreen_captureApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	Cscreen_captureDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

//********************************************************************************
#define SHIFTED 0x8000 
//********************************************************************************

BOOL Cscreen_captureApp::ProcessMessageFilter(int code, LPMSG lpMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_hwndDlg!=NULL)
	{   //判断消息，如果消息是从对话框发出的或者其子控件发出的，就进行处理
		if((lpMsg->hwnd==m_hwndDlg) || ::IsChild(m_hwndDlg,lpMsg->hwnd))
		{
			//如果消息是WM_KEYDOWN
			//用方向键调整位置
			if(lpMsg->message==WM_KEYDOWN)
			{
				CRect rect(0,0,0,0);
				CCatchScreenDlg * pDlg=(CCatchScreenDlg *)AfxGetMainWnd();
				
				rect=pDlg->m_rectTracker.m_rect;

				if(pDlg->m_bFirstDraw)
				{
					
					//如果Shift键按下则方向键调整大小
					BOOL isShifeDowm=FALSE;
					int nVirtKey;
					nVirtKey = GetKeyState(VK_SHIFT); 
					if (nVirtKey & SHIFTED) 
						isShifeDowm=TRUE;

					switch(lpMsg->wParam)
					{
					case VK_UP:
						//如果按下Shift,则只调整一边
						if(!isShifeDowm)
							rect.top-=1;
						rect.bottom-=1;
						pDlg->m_rectTracker.m_rect=rect;
						pDlg->PaintWindow();
						break;
					case VK_DOWN:
						if(!isShifeDowm)
							rect.top+=1;
						rect.bottom+=1;
						pDlg->m_rectTracker.m_rect=rect;
						pDlg->PaintWindow();
						break;
					case VK_LEFT:
						if(!isShifeDowm)
							rect.left-=1;
						rect.right-=1;
						pDlg->m_rectTracker.m_rect=rect;
						pDlg->PaintWindow();
						break;
					case VK_RIGHT:
						if(!isShifeDowm)
							rect.left+=1;
						rect.right+=1;
						pDlg->m_rectTracker.m_rect=rect;
						pDlg->PaintWindow();
						break;
					}
				}
			}
			
		}
	} 
	return CWinApp::ProcessMessageFilter(code, lpMsg);
}
//********************************************************************************