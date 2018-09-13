
// AhroobeDlg.cpp 
//

#include "stdafx.h"
#include "Ahroobe.h"
#include "AhroobeDlg.h"
#include "afxdialogex.h"
#include "PassWord.h"
#include "Publisher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//  CAboutDlg dialog box

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// dialog box data
	enum { IDD = IDD_AHROOBE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAfxIdbChecklistbox95();
	afx_msg void OnTeam();
	afx_msg void OnBnClickedRes();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_COMMAND(AFX_IDB_CHECKLISTBOX_95, &CAboutDlg::OnAfxIdbChecklistbox95)
	ON_COMMAND(ID_TEAM, &CAboutDlg::OnTeam)
	ON_BN_CLICKED(IDRES, &CAboutDlg::OnBnClickedRes)
END_MESSAGE_MAP()


// CAhroobeDlg dialog box



CAhroobeDlg::CAhroobeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAhroobeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAhroobeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDLIST, file_list);
}

BEGIN_MESSAGE_MAP(CAhroobeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDADD, &CAhroobeDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDPAUSE, &CAhroobeDlg::OnBnClickedPause)
	ON_BN_CLICKED(IDDEL, &CAhroobeDlg::OnBnClickedDel)
	ON_BN_CLICKED(IDRES, &CAhroobeDlg::OnBnClickedRes)
END_MESSAGE_MAP()


BOOL CAhroobeDlg::PreTranslateMessage(MSG* pMsg)
{
	// prevent for termination on purpose, (ex) enter, esc, alt+f4
	if (pMsg->message == WM_KEYDOWN)
	{
		//enter, esc
		if (pMsg->wParam == VK_ESCAPE || VK_RETURN)
			return TRUE;
	}
	//alt+f4
	else if (pMsg->message == WM_SYSKEYDOWN){
		if (VK_F4 == pMsg->wParam)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CAhroobeDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();


   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   // prevent for termination on purpose x button in SystemMenu (disable it)
   pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
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


   SetIcon(m_hIcon, TRUE);         // big icon
   SetIcon(m_hIcon, FALSE);      // small icon
   
   //icon is not shown in 작업표시줄
   ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

   if (CAhroobeApp::dwDriverStatus == DRIVERSTART)
   {
      SetDlgItemText(IDMSG, _T("Started"));
      GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
      GetDlgItem(IDADD)->EnableWindow(FALSE);
      GetDlgItem(IDDEL)->EnableWindow(FALSE);
   }
   else
   {
      SetDlgItemText(IDMSG, _T("Stopped"));
   }
   return TRUE;  
}

void CAhroobeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CAhroobeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

	
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CAhroobeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAboutDlg::OnAfxIdbChecklistbox95()
{
}


void CAhroobeDlg::OnBnClickedButton1()
{
	// add button event of file manager.
	static TCHAR BASED_CODE szFilter[] = _T("모든 파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.*"),_T(""), OFN_HIDEREADONLY, szFilter);

	Node* fileNode;
	CString pathName;
	CString compareString;
	CString fileName;
	int SelectIndex;

	int i;

	if (IDOK == dlg.DoModal())
	{
		pathName = dlg.GetPathName();

		for(i=0;i<file_list.GetCount();++i)
		{
			file_list.GetText(i, compareString);

			if(pathName == compareString)
				break;
		}

		if(i == file_list.GetCount())
		{
			fileNode = (Node*)malloc(sizeof(Node));

			fileName = pathName.Right(pathName.GetLength() - pathName.ReverseFind('\\') - 1);

			wcscpy(fileNode->sOriginalFile, pathName.GetBuffer());
			//fileName = L"Copy" + fileName;
			wcscpy(fileNode->sCopyFile, fileName.GetBuffer());

			SelectIndex = file_list.AddString(pathName);		
			file_list.SetCurSel(SelectIndex);

			CAhroobeApp::fileList.AddTail(fileNode);
		}
		else
			AfxMessageBox(TEXT("Existed in file list."));
	}
}

void CAhroobeDlg::OnBnClickedDel()
{
	// delete button event of file manager.
	CString pathName;
	int nList = 0;
	POSITION pos;
	POSITION filePos;
	int i;
	Node* target;

	pos = CAhroobeApp::fileList.GetHeadPosition();

	for(i=0;i<CAhroobeApp::fileList.GetSize();++i)
	{
		target = (Node*)CAhroobeApp::fileList.GetNext(pos);

		file_list.GetText(file_list.GetCurSel(), pathName);

		if(!wcscmp(target->sOriginalFile, pathName.GetBuffer()))
		{
			nList = file_list.GetCurSel();
			file_list.DeleteString(nList);

			filePos = CAhroobeApp::fileList.Find(target);
			CAhroobeApp::fileList.RemoveAt(filePos);
		}
	}
}

void CAhroobeDlg::OnBnClickedPause()
{
   //driver pause button.
   // another dialog Password should be shown.-> PassWord.cpp
   PassWord dlg;
   if (CAhroobeApp::dwDriverStatus == DRIVERSTART)
   {
      dlg.DoModal();
      if (CAhroobeApp::dwDriverStatus == DRIVERSTOP){
         SetDlgItemText(IDMSG, _T("Stopped"));
         GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
		GetDlgItem(IDADD)->EnableWindow(TRUE);
		GetDlgItem(IDDEL)->EnableWindow(TRUE);
		Invalidate();
      }
      else
         SetDlgItemText(IDMSG, _T("StopFailed"));
   }
   else
   {
      AfxMessageBox(TEXT("DriverStatus is not started."));
   }
}

void CAboutDlg::OnTeam()
{
	// nothing..
	Publisher dlg;
	dlg.DoModal();
}

void CAhroobeDlg::OnBnClickedRes()
{

   //driver restart button.
   if(CAhroobeApp::dwDriverStatus == DRIVERSTOP)
   {
      if(CAhroobeApp::LoaderStart())
      {
         CAhroobeApp::dwDriverStatus = DRIVERSTART;
         CAhroobeApp::ClearClipBoard();
         SetDlgItemText(IDMSG, _T("Loaded"));
         AfxMessageBox(TEXT("Started"));
         GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		 GetDlgItem(IDADD)->EnableWindow(FALSE);
		 GetDlgItem(IDDEL)->EnableWindow(FALSE);       
		 Invalidate();
      }
      else
         AfxMessageBox(TEXT("StartFailed!"));
   }
   else
   {
      AfxMessageBox(TEXT("DriverStatus is not stopped."));
   }
}

void CAboutDlg::OnBnClickedRes()
{
	

}