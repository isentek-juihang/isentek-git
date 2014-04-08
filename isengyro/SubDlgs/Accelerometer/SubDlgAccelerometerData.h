#pragma once

#include "SensorsHeader.h"

// CSubDlgAccelerometerData dialog

class CSubDlgAccelerometerData : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgAccelerometerData)

public:
	CSubDlgAccelerometerData(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgAccelerometerData();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_ACCELEROMETER_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonAccelerometerCalibration();
	DECLARE_MESSAGE_MAP()

public:

	raw_data m_raw;

	sensors_vec_t m_offset;
	sensors_vec_t m_data;	

	bool m_bCalibration;
	float m_fRoll;
	float m_fPitch;
};
