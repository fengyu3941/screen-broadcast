
// screen_captureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "screen_capture.h"
#include "screen_captureDlg.h"
#include <vector>

//for dsound
#include <mmsystem.h>
#include <dsound.h>

//for screen
#include "CatchScreenDlg.h"


//CPP调用C

#ifdef __cplusplus
extern "C" {
#endif
#include "ffmpegmfc.h"
	//上下文
	extern FFMFCContext *ffmfcctx;
#ifdef __cplusplus
}
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

char video_profile_names[7][50] = {"baseline -level 3.0",
"baseline -level 3.1",
"main -level 3.1",
"main -level 4.0",
"high -level 4.0",
"high -level 4.1",
"high -level 4.2"};

char video_rate_names[10][50] = {"ultrafast",
"superfast",
"veryfast",
"faster",
"fast",
"medium",
"slow",
"slower",
"veryslow",
"placebo"};


//转码线程
//convert thread
UINT Thread_ffmpeg(LPVOID lpParam){

	Cscreen_captureDlg *dlg=(Cscreen_captureDlg *)lpParam;
	ffmfcctx->mainhwnd = dlg->GetSafeHwnd();
	//当前线程的句柄
	ffmfc_ffmpeg(ffmfcctx, dlg->charlist_size, dlg->charlist);

	return 0;
}

//-----------------------------------------------------------------------------
// Name: DSoundEnumCallback()
// Desc: Enumeration callback called by DirectSoundEnumerate
//-----------------------------------------------------------------------------
INT_PTR CALLBACK DSoundEnumCallback( GUID* pGUID, LPSTR strDesc, LPSTR strDrvName,
									VOID* pContext )
{
	// Set aside static storage space for 20 audio drivers
	static GUID  AudioDriverGUIDs[20];
	static DWORD dwAudioDriverIndex = 0;

	GUID* pTemp  = NULL;

	if( pGUID )
	{
		if( dwAudioDriverIndex >= 20 )
			return TRUE;

		pTemp = &AudioDriverGUIDs[dwAudioDriverIndex++];
		memcpy( pTemp, pGUID, sizeof(GUID) );
	}
	else
	{
		return TRUE;
	}


	HWND hSoundDeviceCombo = (HWND)pContext;

	// Add the string to the combo box
	SendMessage( hSoundDeviceCombo, CB_ADDSTRING, 
		0, (LPARAM) (LPCTSTR) strDesc );

	// Get the index of the string in the combo box
	INT nIndex = (INT)SendMessage( hSoundDeviceCombo, CB_FINDSTRING, 
		0, (LPARAM) (LPCTSTR) strDesc );

	// Set the item data to a pointer to the static guid stored in AudioDriverGUIDs
	SendMessage( hSoundDeviceCombo, CB_SETITEMDATA, 
		nIndex, (LPARAM) pTemp );

	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cscreen_captureDlg 对话框




Cscreen_captureDlg::Cscreen_captureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cscreen_captureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

void Cscreen_captureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_VIDEO_CODEC, combobox_video_codec);
	DDX_Control(pDX, IDC_COMBO_VIDEO_PROFILE, combobox_video_profile);
	DDX_Control(pDX, IDC_COMBO_VIDEO_RATE, combobox_video_rate);
	DDX_Control(pDX, IDC_EDIT_VIDEO_QUALITY, edit_video_quality);
	DDX_Control(pDX, IDC_EDIT_VIDEO_FPS, edit_video_fps);
	DDX_Control(pDX, IDC_COMBO_AUDIO_DEVICE, combobox_audio_device);
	DDX_Control(pDX, IDC_COMBO_AUDIO_CODEC, combobox_audio_codec);
	DDX_Control(pDX, IDC_EDIT_AUDIO_CHAN2, edit_audio_chan);
	DDX_Control(pDX, IDC_EDIT_AUDIO_KBPS, edit_audio_kbps);
	DDX_Control(pDX, IDC_EDIT_AUDIO_SAMPLE_RATE, edit_audio_sample);
	DDX_Control(pDX, IDC_COMBO_MUX, combobox_mux);
	DDX_Control(pDX, IDC_EDIT_RTMP_URL, edit_rtmp_url);
}

BEGIN_MESSAGE_MAP(Cscreen_captureDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_REGION, &Cscreen_captureDlg::OnBnClickedButtonChooseRegion)
	ON_BN_CLICKED(IDC_BUTTON_START_TRANSFER, &Cscreen_captureDlg::OnBnClickedButtonStartTransfer)
	ON_BN_CLICKED(IDC_BUTTON_STOP_TRANSFER, &Cscreen_captureDlg::OnBnClickedButtonStopTransfer)

	ON_MESSAGE(WM_SETERRORMEG, &Cscreen_captureDlg::OnSetErrorMsg)
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILE, &Cscreen_captureDlg::OnBnClickedButtonSavefile)
END_MESSAGE_MAP()


// Cscreen_captureDlg 消息处理程序

BOOL Cscreen_captureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	init_edit_and_member();



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cscreen_captureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cscreen_captureDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cscreen_captureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cscreen_captureDlg::OnBnClickedButtonChooseRegion()
{
	// TODO: 在此添加控件通知处理程序代码
	crop_x = 0;
	crop_y = 0;
	crop_width = 0;
	crop_height = 0;

	ShowWindow(SW_HIDE);  
	Sleep(500);
	CCatchScreenDlg dlg;

	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
		crop_x = dlg.final_rect.left;
		crop_y = dlg.final_rect.top;
		crop_width = dlg.final_rect.Width();
		crop_height = dlg.final_rect.Height();

		if (0 != crop_height%16)
		{
			crop_height = (crop_height/16)*16;
		}

		if (0!=crop_width%16)
		{
			crop_width = (crop_width/16)*16;
		}
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	ShowWindow(SW_SHOWNORMAL);  
}

void Cscreen_captureDlg::OnBnClickedButtonStartTransfer()
{
	// TODO: 在此添加控件通知处理程序代码
	ffmfcctx->dlgcmd = DLGCMD_PLAY;
	ffmfcctx->error_size = 0;
	memset(ffmfcctx->error_msg, 0, 1024);

	get_member_from_edit();
	disable_edit_and_button();
	pThreadffmpeg = AfxBeginThread(Thread_ffmpeg,this);//开启线程

}

void Cscreen_captureDlg::OnBnClickedButtonStopTransfer()
{
	// TODO: 在此添加控件通知处理程序代码
	ffmfcctx->dlgcmd=DLGCMD_STOP;
	::WaitForSingleObject(pThreadffmpeg->m_hThread,INFINITE);//程序在此一直挂起。  
	pThreadffmpeg = NULL;
	enable_edit_and_button();
}

void Cscreen_captureDlg::init_audio_device_vector()
{
	HWND hCaptureDeviceCombo = GetDlgItem(IDC_COMBO_AUDIO_DEVICE)->GetSafeHwnd();
	DirectSoundCaptureEnumerate( (LPDSENUMCALLBACK)DSoundEnumCallback,
		(VOID*)hCaptureDeviceCombo );

	// Select the first device in the combo box
	::SendMessage( hCaptureDeviceCombo, CB_SETCURSEL, 0, 0 );
}

void Cscreen_captureDlg::init_edit_and_member()
{
	init_audio_device_vector();

	crop_x = 0;
	crop_y = 0;
	crop_width = 0;
	crop_height = 0;

	video_codec = "libx264";	
	video_profile = video_profile_names[0];
	video_rate = video_rate_names[0];
	video_quality = 20;
	video_fps = 25;

	audio_device_name = "";
	audio_codec = "aac";
	audio_chan = 2;
	audio_kbps = 64;
	audio_sample = 44100;

	mux_type = "flv";
	rtmp_url = "";

	combobox_video_codec.AddString("libx264"); 
	combobox_video_codec.SetCurSel(0);
	combobox_video_codec.EnableWindow(FALSE);

	{
		for (int i=0;i<6;i++)
		{
			combobox_video_profile.InsertString(i,video_profile_names[i]);
		}
		combobox_video_profile.SetCurSel(0);
		combobox_video_profile.EnableWindow(FALSE);
	}

	{
		for (int i=0;i<10;i++)
		{
			combobox_video_rate.InsertString(i, video_rate_names[i]);			
		}
		combobox_video_rate.SetCurSel(0);
	}

	char str_tmp[128];
	sprintf(str_tmp, "%d",20);
	edit_video_quality.SetWindowText(str_tmp);


	sprintf(str_tmp, "%d",25);
	edit_video_fps.SetWindowText(str_tmp);


	int device_count = combobox_audio_device.GetCount();
	combobox_audio_device.InsertString(device_count,"none");
	combobox_audio_device.SetCurSel(0);

	combobox_audio_codec.AddString("acc"); 
	combobox_audio_codec.SetCurSel(0);
	combobox_audio_codec.EnableWindow(FALSE);

	sprintf(str_tmp, "%d",2);
	edit_audio_chan.SetWindowText(str_tmp);

	sprintf(str_tmp, "%d",64);
	edit_audio_kbps.SetWindowText(str_tmp);

	sprintf(str_tmp, "%d",44100);
	edit_audio_sample.SetWindowText(str_tmp);


	combobox_mux.AddString("flv"); 
	combobox_mux.SetCurSel(0);
	combobox_mux.EnableWindow(FALSE);

	edit_rtmp_url.SetWindowText("test.flv");

	enable_edit_and_button();

	pThreadffmpeg = NULL;
	charlist_size = 0;
	charlist = NULL;

	ffmfcctx = new FFMFCContext;
	ffmfcctx->error_msg = new char[1024];

}

void Cscreen_captureDlg::get_member_from_edit()
{
	int nIndex = 0;
	CString strCBText;
	
	nIndex = combobox_video_profile.GetCurSel();
	combobox_video_profile.GetLBText( nIndex, strCBText);
	video_profile = strCBText;


	nIndex = combobox_video_rate.GetCurSel();
	combobox_video_rate.GetLBText( nIndex, strCBText);
	video_rate = strCBText;

	edit_video_quality.GetWindowText(strCBText);
	video_quality = atoi(strCBText);

	edit_video_fps.GetWindowText(strCBText);
	video_fps = atoi(strCBText);

	nIndex = combobox_audio_device.GetCurSel();
	combobox_audio_device.GetLBText( nIndex, strCBText);
	audio_device_name = strCBText;

	edit_audio_chan.GetWindowText(strCBText);
	audio_chan = atoi(strCBText);

	edit_audio_kbps.GetWindowText(strCBText);
	audio_kbps = atoi(strCBText);

	edit_audio_sample.GetWindowText(strCBText);
	audio_sample = atoi(strCBText);

	edit_rtmp_url.GetWindowText(strCBText);
	rtmp_url = strCBText;


	//charlistsize, charlist
	//ffmpeg.exe
	//-f gdigrab [ -offset_x 10 -offset_y 20 -video_size 640x480] -i desktop -c:v libx264 -profile:v baseline -preset ultrafast -crf 20 -r
	//25 -tune zerolatency 16必选 6可选
	//-f dshow -i audio="麦克风 (Realtek High Definition Au" -strict 2  -c:a aac -ar 44100 -ab 48k -ac 2 14可选
	//-f flv rtmp:// 3必选

	if (charlist)
	{
		//析构
		for (int i=0;i<charlist_size;i++)
		{
			delete [](charlist[i]);
		}
		delete []charlist;
		charlist_size = 0;
		charlist = NULL;

	}

	charlist_size = 1;
	charlist_size += 12;
	charlist_size += 6;
	if (crop_width ==0 || crop_height==0)
	{
		crop_x = 0;
		crop_y = 0;
		crop_width = ::GetSystemMetrics(SM_CXSCREEN);
		crop_height = ::GetSystemMetrics(SM_CYSCREEN);;
	}
	if (audio_device_name!="none")
	{
		charlist_size += 14;
	}
	else
	{	
		charlist_size += 2;
	}
	charlist_size += 3;

	charlist = new char*[charlist_size];
	{
		for (int i=0;i<charlist_size;i++)
		{
			charlist[i] = new char[512];
			memset(charlist[i], 0, 32);
		}
	}

	sprintf(charlist[0], "%s", "ffmpeg.exe");
	sprintf(charlist[1], "%s", "-f");
	sprintf(charlist[2], "%s", "gdigrab");
	sprintf(charlist[3], "%s", "-offset_x");
	sprintf(charlist[4], "%d", crop_x);
	sprintf(charlist[5], "%s", "-offset_y");
	sprintf(charlist[6], "%d", crop_y);
	sprintf(charlist[7], "%s", "-video_size");
	sprintf(charlist[8], "%dx%d", crop_width,crop_height);
	sprintf(charlist[9], "%s","-i");
	sprintf(charlist[10], "%s","desktop");



	if (audio_device_name!="none")
	{		
		sprintf(charlist[11], "%s","-f");
		sprintf(charlist[12], "%s","dshow");
		sprintf(charlist[13], "%s","-i");
		sprintf(charlist[14], "audio=%s",audio_device_name.c_str());
		sprintf(charlist[15], "%s","-strict");
		sprintf(charlist[16], "%s","experimental");
		sprintf(charlist[17], "%s","-c:a");
		sprintf(charlist[18], "%s",audio_codec.c_str());
		sprintf(charlist[19], "%s","-ar");
		sprintf(charlist[20], "%d",audio_sample);
		sprintf(charlist[21], "%s","-ab");
		sprintf(charlist[22], "%dk",audio_kbps);
		sprintf(charlist[23], "%s","-ac");
		sprintf(charlist[24], "%d",audio_chan);


		sprintf(charlist[25], "%s","-c:v");
		sprintf(charlist[26], "%s",video_codec.c_str());
		//sprintf(charlist[27], "%s","-profile:v");
		//sprintf(charlist[28], "%s",video_profile.c_str());
		sprintf(charlist[27], "%s","-preset");
		sprintf(charlist[28], "%s",video_rate.c_str());
		//sprintf(charlist[29], "%s","-crf");
		//sprintf(charlist[30], "%d",video_quality);
		//sprintf(charlist[31], "%s", "-r");
		//sprintf(charlist[32], "%d", video_fps);
		sprintf(charlist[29], "%s","-pix_fmt");
		sprintf(charlist[30], "%s","yuv420p");
		sprintf(charlist[31], "%s","-tune");
		sprintf(charlist[32], "%s","zerolatency");
		sprintf(charlist[33], "%s","-f");
		sprintf(charlist[34], "%s",mux_type.c_str());
		sprintf(charlist[35], "%s",rtmp_url.c_str());
	}
	else
	{
		sprintf(charlist[11], "%s","-strict");
		sprintf(charlist[12], "%s","experimental");
		sprintf(charlist[13], "%s","-c:v");
		sprintf(charlist[14], "%s",video_codec.c_str());
		//sprintf(charlist[15], "%s","-profile:v");
		//sprintf(charlist[16], "%s",video_profile.c_str());
		sprintf(charlist[15], "%s","-preset");
		sprintf(charlist[16], "%s",video_rate.c_str());
		//sprintf(charlist[17], "%s","-crf");
		//sprintf(charlist[18], "%d",video_quality);
		//sprintf(charlist[19], "%s", "-r");
		//sprintf(charlist[20], "%d", video_fps);
		sprintf(charlist[17], "%s","-pix_fmt");
		sprintf(charlist[18], "%s","yuv420p");
		sprintf(charlist[19], "%s","-tune");
		sprintf(charlist[20], "%s","zerolatency");
		sprintf(charlist[21], "%s","-f");
		sprintf(charlist[22], "%s",mux_type.c_str());
		sprintf(charlist[23], "%s",rtmp_url.c_str());
	}
}

void Cscreen_captureDlg::disable_edit_and_button()
{
	//combobox_video_profile.EnableWindow(FALSE);
	combobox_video_rate.EnableWindow(FALSE);
	edit_video_quality.EnableWindow(FALSE);
	edit_video_fps.EnableWindow(FALSE);
	combobox_audio_device.EnableWindow(FALSE);
	edit_audio_chan.EnableWindow(FALSE);
	edit_audio_kbps.EnableWindow(FALSE);
	edit_audio_sample.EnableWindow(FALSE);
	edit_rtmp_url.EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CHOOSE_REGION)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_TRANSFER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SAVEFILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_TRANSFER)->EnableWindow(TRUE);


}

void Cscreen_captureDlg::enable_edit_and_button()
{
	//combobox_video_profile.EnableWindow(TRUE);
	combobox_video_rate.EnableWindow(TRUE);
	edit_video_quality.EnableWindow(TRUE);
	edit_video_fps.EnableWindow(TRUE);
	combobox_audio_device.EnableWindow(TRUE);
	edit_audio_chan.EnableWindow(TRUE);
	edit_audio_kbps.EnableWindow(TRUE);
	edit_audio_sample.EnableWindow(TRUE);
	edit_rtmp_url.EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CHOOSE_REGION)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_START_TRANSFER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SAVEFILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP_TRANSFER)->EnableWindow(FALSE);
}

LRESULT Cscreen_captureDlg::OnSetErrorMsg( WPARAM wParam, LPARAM lParam )
{
	::MessageBox(NULL, ffmfcctx->error_msg ,"ERROR", MB_OK);
	return 0;
}

void Cscreen_captureDlg::OnBnClickedButtonSavefile()
{
	// TODO: 在此添加控件通知处理程序代码

	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		edit_rtmp_url.SetWindowText(FilePathName);
	}
}
