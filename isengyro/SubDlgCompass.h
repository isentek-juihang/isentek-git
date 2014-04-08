#pragma once

//#include "./GUI/SA/SAPrefsSubDlg.h"
#include "SAPrefsSubDlg.h"
#include "iSensorsDlg.h"
#include "afxwin.h"
#include "afxcmn.h"

// CSubDlgCompass dialog

class CSubDlgCompass : public CSAPrefsSubDlg
{
	DECLARE_DYNAMIC(CSubDlgCompass)

	enum TILT_SOURCE{ REAL_TILT, VIRTUAL_TILT};

public:
	CSubDlgCompass(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlgCompass();

// Dialog Data
	enum { IDD = IDD_DIALOG_SUBDLG_COMPASS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedRadioCompassRealTilt();
	afx_msg void OnBnClickedRadioCompassVirtualTilt();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
public:
	//float m_X;
	sensors_vec_t m_data;	
	BOOL m_TiltSource;	
	CSliderCtrl m_VirtualPitchCtl;
	CSliderCtrl m_VirtualRollCtl;	
	void MyPitchUpdateData();
	void MyRollUpdateData();
	short m_VittualRoll;
	short m_VittualPitch;

	static const unsigned int nFontNum = 3;
	static const unsigned int nBrushNum = 3;
	CFont* m_pFont[nFontNum];
	CBrush* m_pBrush[nBrushNum];
	void InitializeFont(void);
};
