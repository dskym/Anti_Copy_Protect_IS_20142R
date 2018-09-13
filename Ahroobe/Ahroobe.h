
// Ahroobe.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "afxtempl.h"
#include "resource.h"		// 주 기호입니다.

#define FILE_DEVICE_UNKNOWN  0x00000022
#define METHOD_BUFFERED 0x0000
#define FILE_ANY_ACCESS 0x0000
#define CTL_CODE( DeviceType, Function, Method, Access ) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))

#define DRIVERINSTALL 1
#define DRIVERSTART 2
#define DRIVERSTOP 3
#define DRIVERREMOVE 4
	
typedef struct _Node {
	WCHAR sOriginalFile[256];
	WCHAR sCopyFile[256];
}	Node;

// CAhroobeApp:
// 이 클래스의 구현에 대해서는 Ahroobe.cpp을 참조하십시오.
//

class CAhroobeApp : public CWinApp
{
	
public:
	static CList<Node*, Node*&> fileList;
	static UINT uTimerID;
	static DWORD dwDriverStatus;

	CAhroobeApp();	

	static void communicate();
	static void ClearClipBoard();
	static BOOL LoaderStart();	
	static BOOL LoaderStop();	

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()
};

extern CAhroobeApp theApp;