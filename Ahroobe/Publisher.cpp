// Publisher.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Ahroobe.h"
#include "Publisher.h"
#include "afxdialogex.h"


// Publisher ��ȭ �����Դϴ�.

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


// Publisher �޽��� ó�����Դϴ�.
