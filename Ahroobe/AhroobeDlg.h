
// AhroobeDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CAhroobeDlg dialog box
class CAhroobeDlg : public CDialogEx
{
public:
	CAhroobeDlg(CWnd* pParent = NULL);	

// data
	enum { IDD = IDD_AHROOBE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV



protected:
	HICON m_hIcon;


	// message map

	BOOL PreTranslateMessage(MSG* pMsg);
	void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	
	afx_msg void OnBnClickedButton1();
	CListBox file_list;
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedRes();
};

