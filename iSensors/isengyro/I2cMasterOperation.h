#pragma once

#include "./Interface/iSenInterfaceLinker.h"
#include "./Interface/I2C/ISenI2cMaster.h"

// CI2cMasterOperation dialog

class CI2cMasterOperation : public CDialogEx
{
	DECLARE_DYNAMIC(CI2cMasterOperation)

public:
	CI2cMasterOperation(CWnd* pParent = NULL);   // standard constructor
	virtual ~CI2cMasterOperation();

// Dialog Data
	enum { IDD = IDD_DIALOG_I2CMASTER_OPERATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioI2cmasterNone();
	afx_msg void OnBnClickedRadioI2cmasterAardvark();
	afx_msg void OnBnClickedRadioI2cmasterIow();
	afx_msg void OnBnClickedRadioI2cmasterFtdi();
	int m_nI2cDev;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	void UpdateStatusBar(void);
};
