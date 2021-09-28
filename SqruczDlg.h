// SqruczDlg.h : header file
//

#pragma once
#include "afxdtctl.h"
#include "ProgressDlg.h"

// CSqruczDlg dialog
class CSqruczDlg : public CDialog
{
// Construction
public:
	CSqruczDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SQRUCZ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMaindlgButtonClose();
	afx_msg void OnBnClickedMaindlgButtonHide();
	afx_msg void OnBnClickedMaindlgButtonGetfolder1();
	afx_msg void OnBnClickedMaindlgButtonGetfolder2();
protected:
	afx_msg void OnDestroy();
private:
	CDateTimeCtrl TimeCtrl;
public:
	afx_msg void OnDtnDatetimechangeMaindlgDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
private:
	afx_msg void OnTimer(UINT TimerId);
	void AddTrayIcon(bool add);
	afx_msg void OnClose(void);
	afx_msg LRESULT TrayMessage(WPARAM  wParam, LPARAM lParam);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
public:
	bool visible;
	afx_msg void OnBnClickedMaindlgButtonInfo();
	afx_msg void OnBnClickedMaindlgButtonCopynow();
	CProgressDlg* pProgressDlg;
private:
	bool CheckZeroTime(void);
	bool CopyData(void);
	unsigned int CopyAllObjFromDirectory(const CString& CSSourceFolderPath , const CString&  CSDestinationFolderPath);
	bool CopyFile(const CString& CSSourcePath , const CString& CSDestinationPath , const CString& CSFileName);
public:
	unsigned int GetObjNumber(const CString&  CSSourceFolderPath);
	bool DeleteFolderRec(CString szPath);
	bool FolderIsEmpty(CString szPath);
	afx_msg void OnEnChangeMaindlgEditSource();
	afx_msg void OnEnChangeMaindlgEditDest();
};
