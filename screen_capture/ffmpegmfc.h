#ifndef __FFMPEG_MFC_H__
#define __FFMPEG_MFC_H__

#define WM_SETERRORMEG WM_USER+1
//
typedef enum Dlgcmd{
	DLGCMD_PLAY,
	DLGCMD_PROGRESS,
	DLGCMD_PAUSE,
	DLGCMD_STOP
};

/* select an input stream for an output stream */
typedef struct FFMFCContext {

	//主对话框句柄
	void *mainhwnd;

	//主程序命令
	int dlgcmd;

	//错误值
	char* error_msg;
	int   error_size;

} FFMFCContext;
//FFMPEG修改后main函数
int ffmfc_ffmpeg(void *param,int argc, char **argv);
void writeErrorMsg(const char* format, ...);
int ANSIToUTF8(char* pszCode, char* UTF8code);
int UTF8ToANSI(char* UTF8code, char* pszCode);
#endif