#pragma once


// CSubDlgMagnetometer dialog

class CSubDlgMagnetometer : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgMagnetometer)

	enum  { M_NONE, M_AKM8975, M_IST8301 };
	enum  { M_ORIGINAL, M_AVIATION, M_ANDROID };
	enum  { M_LSB, M_MT, M_MG};

public:
	CSubDlgMagnetometer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgMagnetometer();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_MAGNETOMETER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()
public:
	int m_ChipSelection;
	int m_UnitsPreference;
	int m_nCoordinateSystem;

	afx_msg void OnBnClickedRadioMagnetometerIst8301();
	afx_msg void OnBnClickedRadioMagnetometerAkm8975();
	afx_msg void OnBnClickedRadioMagnetometerNone();

	void FreezeUnitsPreferenceGroup(BOOL bFreeze);
	void FreezeCoordinateSystemGroup(BOOL bFreeze);
};
