#pragma once


// Publisher ��ȭ �����Դϴ�.

class Publisher : public CDialogEx
{
	DECLARE_DYNAMIC(Publisher)

public:
	Publisher(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Publisher();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
