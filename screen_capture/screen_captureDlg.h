
// screen_captureDlg.h : 头文件
//

#pragma once
#include <string>
#include "afxwin.h"

// Cscreen_captureDlg 对话框
class Cscreen_captureDlg : public CDialog
{
// 构造
public:
	Cscreen_captureDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SCREEN_CAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CWinThread *pThreadffmpeg;

	int charlist_size;
	char** charlist;
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
	afx_msg void OnBnClickedButtonChooseRegion();
	afx_msg void OnBnClickedButtonStartTransfer();
	afx_msg void OnBnClickedButtonStopTransfer();

	//设置进度的消息响应函数
	//Callback of Set Progress Message
	LRESULT OnSetErrorMsg(WPARAM wParam, LPARAM lParam );

private:
	void init_audio_device_vector();
	void init_edit_and_member();
	void get_member_from_edit();
	void disable_edit_and_button();
	void enable_edit_and_button();

private:
	//音视频编码封装传输参数
	int crop_x;
	int crop_y;
	int crop_width;
	int crop_height;
	std::string video_codec;
	std::string video_profile;
	int    video_fps;
	std::string video_rate;
	int    video_quality;

	std::string audio_device_name;
	std::string audio_codec;
	int    audio_chan;
	int	   audio_kbps;
	int	   audio_sample;

	std::string mux_type;
	std::string rtmp_url;

private:
	CComboBox combobox_video_codec;
	CComboBox combobox_video_profile;
	CComboBox combobox_video_rate;
	CEdit edit_video_quality;
	CEdit edit_video_fps;
	CComboBox combobox_audio_device;
	CComboBox combobox_audio_codec;
	CEdit edit_audio_chan;
	CEdit edit_audio_kbps;
	CEdit edit_audio_sample;
	CComboBox combobox_mux;
	CEdit edit_rtmp_url;
public:
	afx_msg void OnBnClickedButtonSavefile();
};
