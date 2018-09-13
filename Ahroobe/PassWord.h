#pragma once


// PassWord 대화 상자입니다.

class PassWord : public CDialogEx
{
	DECLARE_DYNAMIC(PassWord)

public:
	PassWord(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PassWord();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedCancel();
	CString password;
	afx_msg void OnChangePassword();
};
