#pragma once


// CSubDlgGyroscopeApp dialog

class CSubDlgGyroscopeApp : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgGyroscopeApp)

public:
	CSubDlgGyroscopeApp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgGyroscopeApp();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_GYROSCOPE_APP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()
};
