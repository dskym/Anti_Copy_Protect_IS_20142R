// Publisher.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Ahroobe.h"
#include "Publisher.h"
#include "afxdialogex.h"


// Publisher 대화 상자입니다.

IMPLEMENT_DYNAMIC(Publisher, CDialogEx)

Publisher::Publisher(CWnd* pParent /*=NULL*/)
	: CDialogEx(Publisher::IDD, pParent)
{

}

Publisher::~Publisher()
{
}

void Publisher::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Publisher, CDialogEx)
END_MESSAGE_MAP()


// Publisher 메시지 처리기입니다.
