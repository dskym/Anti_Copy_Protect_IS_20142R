#pragma once


// PassWord ��ȭ �����Դϴ�.

class PassWord : public CDialogEx
{
	DECLARE_DYNAMIC(PassWord)

public:
	PassWord(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~PassWord();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedCancel();
	CString password;
	afx_msg void OnChangePassword();
};
