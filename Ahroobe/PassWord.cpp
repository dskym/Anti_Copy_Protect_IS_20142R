// PassWord.cpp 
//

#include "stdafx.h"
#include "Ahroobe.h"
#include "PassWord.h"
#include "afxdialogex.h"
CString something;
// PassWord dialog box

IMPLEMENT_DYNAMIC(PassWord, CDialogEx)

PassWord::PassWord(CWnd* pParent /*=NULL*/)
	: CDialogEx(PassWord::IDD, pParent)
	, password(_T(""))
{

}

PassWord::~PassWord()
{
}

void PassWord::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PASSWORD, password);
}


BEGIN_MESSAGE_MAP(PassWord, CDialogEx)
	ON_BN_CLICKED(IDOK, &PassWord::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &PassWord::OnEnChangeEdit1)
	ON_BN_CLICKED(IDCANCEL, &PassWord::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_PASSWORD, &PassWord::OnChangePassword)
END_MESSAGE_MAP()


// PassWord message


void PassWord::OnBnClickedOk()
{

	GetDlgItem(IDC_PASSWORD)->GetWindowText(something);
	//password is temporary 1234.
	if (something == "1234")
	{
		//after checking password, driver stop
		if(CAhroobeApp::dwDriverStatus == DRIVERSTART)
		{
			if(CAhroobeApp::LoaderStop())
			{
				CAhroobeApp::dwDriverStatus = DRIVERSTOP;
				KillTimer(CAhroobeApp::uTimerID);
				SetDlgItemText(IDMSG, _T("paused"));
				AfxMessageBox(TEXT("Completed!!"));
			}
			else
				AfxMessageBox(TEXT("fail!!"));
		}
		else
		{
			AfxMessageBox(TEXT("DriverStatus is not started."));
		}
	}
	else
		AfxMessageBox(TEXT("Wrong password!!"));

	CDialogEx::OnOK();
}


void PassWord::OnEnChangeEdit1()
{
	
}


void PassWord::OnBnClickedCancel()
{
	//cancel button
	CDialogEx::OnCancel();
	
}


void PassWord::OnChangePassword()
{

	
}