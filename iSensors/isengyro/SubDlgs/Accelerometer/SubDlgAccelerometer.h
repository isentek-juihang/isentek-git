#pragma once

#include "SAPrefsSubDlg.h"
#include "iSensorsDlg.h"
#include "afxwin.h"


// CSubDlgAccelerometer dialog

class CSubDlgAccelerometer : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgAccelerometer)

	enum  { A_NONE, A_DMARD08, A_KXTF9 };
	enum  { A_ORIGINAL, A_AVIATION, A_ANDROID };
	enum  { A_LSB, A_MG };

public:
	CSubDlgAccelerometer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgAccelerometer();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_ACCELEROMETER };
	
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedRadioAccelerometerNone();
	afx_msg void OnBnClickedRadioAccelerometerDmt08();
	afx_msg void OnBnClickedRadioAccelerometerKxtf9();
	DECLARE_MESSAGE_MAP()
public:
	int m_ChipSelection;
	int m_UnitsPreference;
	int m_nCoordinateSystem;
	
	void FreezeUnitsPreferenceGroup(BOOL bFreeze);
	void FreezeCoordinateSystemGroup(BOOL bFreeze);
	void FreezeLayoutPatternGroup(BOOL bFreeze);

	CComboBox *m_LayoutPatternCtl;
	
	afx_msg void OnCbnSelchangeComboAccelerometerLayout();
};
