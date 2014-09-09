#pragma once

#include "./GUI/Waveform/WaveformCtrl.h"

// CSubDlgGyroscopeData dialog

class CSubDlgGyroscopeData : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgGyroscopeData)

public:
	CSubDlgGyroscopeData(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgGyroscopeData();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_GYROSCOPE_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()

public:

	CWaveformCtrl	m_GyroWaveform[3];
	CWaveformCtrl	m_SoftGyroWaveform[3];
};
