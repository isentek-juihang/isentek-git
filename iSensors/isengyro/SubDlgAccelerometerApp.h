#pragma once


// CSubDlgAccelerometerApp dialog

class CSubDlgAccelerometerApp : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgAccelerometerApp)

public:
	CSubDlgAccelerometerApp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgAccelerometerApp();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_ACCELEROMETER_APP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()
};
