#pragma once


// CSubDlgGyroscope dialog

class CSubDlgGyroscope : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgGyroscope)

	enum  { GR_NONE, GR_L3G4200D };
	enum  { GR_ORIGINAL, GR_AVIATION, GR_ANDROID };
	enum  { GR_LSB, GR_MG };

public:
	CSubDlgGyroscope(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgGyroscope();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_GYROSCOPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()

public:
	int m_ChipSelection;
	int m_UnitsPreference;
	int m_nCoordinateSystem;
	afx_msg void OnBnClickedRadioGyroscopeNone();
	afx_msg void OnBnClickedRadioGyroscopeL3g4200d();
};
