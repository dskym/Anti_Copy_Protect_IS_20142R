
// Ahroobe.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "afxtempl.h"
#include "resource.h"		// �� ��ȣ�Դϴ�.

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
// �� Ŭ������ ������ ���ؼ��� Ahroobe.cpp�� �����Ͻʽÿ�.
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

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CAhroobeApp theApp;