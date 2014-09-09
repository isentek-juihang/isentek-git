#pragma once

#include "ISenTekAssistantDlg.h"
#include "afxwin.h"

// CConfigurationDlg 對話方塊

class CConfigurationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigurationDlg)

public:
	CConfigurationDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConfigurationDlg();

// 對話方塊資料
	enum { IDD = IDD_DIALOG_CONFIGURATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bMSensorI2CSlaveAdd;
	int m_nGSelection;
	int m_nMSelection;
	float m_fAngleAlpha;
	float m_Q11;
	float m_Q12;
	float m_Q13;
	float m_Q21;
	float m_Q22;
	float m_Q23;
	float m_Q31;
	float m_Q32;
	float m_Q33;
	float m_R11;
	float m_R12;
	float m_R13;
	float m_R21;
	float m_R22;
	float m_R23;
	float m_R31;
	float m_R32;
	float m_R33;

	float m_G11;
	float m_G12;
	float m_G13;
	float m_G21;
	float m_G22;
	float m_G23;
	float m_G31;
	float m_G32;
	float m_G33;
	float m_G41;
	float m_G42;
	float m_G43;
	float m_G51;
	float m_G52;
	float m_G53;
	float m_G61;
	float m_G62;
	float m_G63;

	float m_moving;
	float m_datanumber;
	float m_datarange;
	float m_hardstd;
	float m_softstd;
	float m_pitchlimit;
//	float m_GzTh2;
	int m_fUAlpha;
	float m_nMdThX;
	float m_nMdThY;
	float m_nMdThZ;
	int m_GchangeTh;
	int m_MradiusTh;
	afx_msg void OnEnChangeEditMxTh();
	afx_msg void OnEnChangeEditMyTh();
	afx_msg void OnEnChangeEditMzTh();
	afx_msg void OnEnChangeEditR11();
	BOOL m_bGsensorCalibrationSW;
	afx_msg void OnBnClickedButtonGxdown();
	afx_msg void OnBnClickedButtonGxup();
	afx_msg void OnBnClickedButtonGydown();
	afx_msg void OnBnClickedButtonGyup();
	afx_msg void OnBnClickedButtonGzdown();
	afx_msg void OnBnClickedButtonGzup();
	BOOL m_bFirG;
	BOOL m_bFirM;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnSetfocusRadioIst8301();
	afx_msg void OnBnSetfocusRadioIst8303();
private:
	void OnMSensorSelectionChanged(void);
};
