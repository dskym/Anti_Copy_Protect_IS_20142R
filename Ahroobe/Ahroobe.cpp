
// Ahroobe.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//
#include "stdafx.h"
#include "Ahroobe.h"
#include "AhroobeDlg.h"
#include "loader.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAhroobeApp

BEGIN_MESSAGE_MAP(CAhroobeApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAhroobeApp generation

wchar_t sysfile[100] = L"InfoSec2";
Load_Driver loader = Load_Driver(sysfile);

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

DWORD CAhroobeApp::dwDriverStatus = 0;
UINT CAhroobeApp::uTimerID = 0;
CList<Node*, Node*&> CAhroobeApp::fileList(10);

CAhroobeApp::CAhroobeApp()
{
	if(loader.InstallDriver())
	{
		CAhroobeApp::dwDriverStatus = DRIVERINSTALL;

		if(loader.StartDriver())
		{
			CAhroobeApp::dwDriverStatus = DRIVERSTART;

			CAhroobeApp::communicate();

			//clean the clipboard
			CAhroobeApp::ClearClipBoard();
		}
		else
		{
			AfxMessageBox(TEXT("Fail to start"));
		}
	}
	else
	{
		AfxMessageBox(TEXT("Fail to install"));
	}
}


// CAhroobeApp object

CAhroobeApp theApp;

// CAhroobeApp intializing

BOOL CAhroobeApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	
	CWinApp::InitInstance();

	AfxEnableControlContainer();

	//Shell manager
	CShellManager *pShellManager = new CShellManager;

	// MFC control theme
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("Program from local"));

	CAhroobeDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// IDOK button is deleted
	}
	else if (nResponse == IDCANCEL)
	{
		// IDCANCEL button is deleted also
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog box has not been generated well.\n");
		TRACE(traceAppMsg, 0, "Warning to use MFC control. #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS cannot be done.\n");
	}

	// delete shell manager
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// destroy application
	return FALSE;
}

BOOL CAhroobeApp::LoaderStart()
{
	BOOL status;

	status = loader.StartDriver();

	CAhroobeApp::communicate();

	return status;
}

BOOL CAhroobeApp::LoaderStop()
{
	return loader.StopDriver();
}


//Driver, communication with application
void CAhroobeApp::communicate()
{
	int i;
	POSITION pos;

	HANDLE handle;
	WCHAR DeviceLink[] = L"\\\\.\\dskym";
	DWORD dwRet;

	Node* target;

	handle = CreateFileW(DeviceLink, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(handle == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(TEXT("Get Device Handle Fail!"));
		printf("Get Device Handle Fail! : 0x%x\n", GetLastError());

		return;
	}
	
	pos = CAhroobeApp::fileList.GetHeadPosition();

	for(i=0;i<CAhroobeApp::fileList.GetSize();++i)
	{
		target = (Node*)CAhroobeApp::fileList.GetNext(pos);

		if(!DeviceIoControl(handle,	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4000, METHOD_BUFFERED, FILE_ANY_ACCESS), target, sizeof(Node), 0, 0, &dwRet, 0))
		{
			AfxMessageBox(TEXT("DeviceIoControl Fail!"));
			printf("DeviceIoControl Fail!\n");

			CloseHandle(handle);

			return;
		}
	}

	CloseHandle(handle);
}

void CAhroobeApp::ClearClipBoard()
{
	uTimerID = SetTimer(NULL, NULL, 100, TimerProc);
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	OpenClipboard(NULL);

	if(IsClipboardFormatAvailable(CF_TEXT))
		EmptyClipboard();

	CloseClipboard();
}
