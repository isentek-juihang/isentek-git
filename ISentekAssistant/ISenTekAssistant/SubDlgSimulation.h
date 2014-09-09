#pragma once

#include "SAPrefsSubDlg.h"
#include "ISenTekAssistantDlg.h"
#include "afxwin.h"

// CSubDlgSimulation 對話方塊

class CSubDlgSimulation : public CSAPrefsSubDlg
{
	CFont m_font;

	DECLARE_DYNAMIC(CSubDlgSimulation)

public:
	CSubDlgSimulation(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CSubDlgSimulation();

// 對話方塊資料
	enum { IDD = IDD_DIALOG_SUBDLG_SIMULATION };
	enum FUN_MODE{CALIBRATION_MODE, SIMULATION_MODE};
	enum {LEFT, RIGHT};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSimulation();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	BOOL m_unit;


	CButton m_SimulatingCtl;
	int m_nFunSel;
	
	void ShowMagneticSensorInfo(bool bShow);
	void ShowAccelerometerInfo(bool bShow);
	void ShowGyroInfo(bool bShow);
	void ShowCalibrationInfo(bool bShow);
	void ShowAnglesInfo(bool bShow);
	void LoadImage(void);
	void ChangeImage(int Angle, int ndirection);
	void CreateCompassGuiThread(void);
	void LoadImageThread(void);

	CDC *mdc,*mdc1;
	RECT ClientRect;
	CBitmap  *bgbmp,*tempbmp, *maskBmp;
	CBitmap *bitmap[360];

	float m_ng[3];
	int m_nM[3];
	int m_nG[3];
	int m_nCalibratedM[3];
	float m_fOptim[3];
	int m_AngleData;
	float m_fRoll;
	float m_fPitch;
	
	unsigned int CompassW, CompassH;
	unsigned int CompassX, CompassY;
	unsigned int BgW, BgH;
	unsigned int ImageX, ImageY;
	HANDLE hCompassGuiThread;
	int m_nCurrentIndex;
	bool bImageReady;
	CComboBox *m_OdrCtl;
	CComboBox *m_OdrCtl1;
	afx_msg void OnCbnSelchangeComboSmpgSpeed();
	afx_msg void OnCbnSelchangeComboSmpgSoftorhard();
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	
	
	
	
	float m_nU;
	float m_nV11;
	float m_nV21;
	float m_nV12;
	float m_nV22;

	float m_Sc11;
	float m_Sc12;
	float m_Sc13;
	float m_Sc21;
	float m_Sc22;
    float m_Sc23;
	float m_Sc31;
	float m_Sc32;
	float m_Sc33;

	float m_radius;
	afx_msg void OnEnChangeEditDirTh();
	CString m_CalibrationIndication;
	afx_msg void OnBnClickedStaticGSeninfoGroup2();
	afx_msg void OnStnClickedStaticMydata();
	afx_msg void OnBnClickedStaticAnglesGroup();
	afx_msg void OnBnClickedBtnAuto();
	
	CButton m_Auto;
	

	
	
	afx_msg void OnBnClickedRadioLsb();
	afx_msg void OnBnClickedRadioMt();
};
