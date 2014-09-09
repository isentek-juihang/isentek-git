#pragma once


// CSubDlgMagnetometerData dialog

class CSubDlgMagnetometerData : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgMagnetometerData)

public:
	CSubDlgMagnetometerData(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgMagnetometerData();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_MAGNETOMETER_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()

public:
/*	short m_MX;
	short m_MY;
	short m_MZ;
	
	short m_MX_Offset;
	short m_MY_Offset;
	short m_MZ_Offset;
	short m_MX_Raw;
	short m_MY_Raw;
	short m_MZ_Raw;*/

	raw_data m_raw;

	sensors_vec_t m_offset;

	sensors_vec_t m_data;
	float m_MagneticFieldStrength;
};
