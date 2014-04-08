#pragma once


// CSubDlgMagnetometerApp dialog

class CSubDlgMagnetometerApp : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgMagnetometerApp)

public:
	CSubDlgMagnetometerApp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgMagnetometerApp();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_MAGNETOMETER_APP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

	#define OriginX 110
	#define OriginY	110

	#define RectangleW 230
	#define RectangleH 230

};
