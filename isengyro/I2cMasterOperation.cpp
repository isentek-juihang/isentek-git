// I2cMasterOperation.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "I2cMasterOperation.h"
#include "afxdialogex.h"

#include "iSensorsDlg.h"

// CI2cMasterOperation dialog

IMPLEMENT_DYNAMIC(CI2cMasterOperation, CDialogEx)

CI2cMasterOperation::CI2cMasterOperation(CWnd* pParent /*=NULL*/)
	: CDialogEx(CI2cMasterOperation::IDD, pParent)
	, m_nI2cDev(0)
{
}

CI2cMasterOperation::~CI2cMasterOperation()
{
}

void CI2cMasterOperation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_I2CMASTER_NONE, m_nI2cDev);
}


BEGIN_MESSAGE_MAP(CI2cMasterOperation, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_I2CMASTER_NONE, &CI2cMasterOperation::OnBnClickedRadioI2cmasterNone)
	ON_BN_CLICKED(IDC_RADIO_I2CMASTER_AARDVARK, &CI2cMasterOperation::OnBnClickedRadioI2cmasterAardvark)
	ON_BN_CLICKED(IDC_RADIO_I2CMASTER_IOW, &CI2cMasterOperation::OnBnClickedRadioI2cmasterIow)
	ON_BN_CLICKED(IDC_RADIO_I2CMASTER_FTDI, &CI2cMasterOperation::OnBnClickedRadioI2cmasterFtdi)
	ON_BN_CLICKED(IDOK, &CI2cMasterOperation::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CI2cMasterOperation::OnBnClickedCancel)
END_MESSAGE_MAP()


// CI2cMasterOperation message handlers

BOOL CI2cMasterOperation::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: Add extra initialization here
#ifdef AARDVARK_SUPPORT
	GetDlgItem(IDC_RADIO_I2CMASTER_AARDVARK)->EnableWindow(TRUE);
#else
	GetDlgItem(IDC_RADIO_I2CMASTER_AARDVARK)->EnableWindow(FALSE);
#endif
#ifdef IOW_SUPPORT
	GetDlgItem(IDC_RADIO_I2CMASTER_IOW)->EnableWindow(TRUE);
#else
	GetDlgItem(IDC_RADIO_I2CMASTER_IOW)->EnableWindow(FALSE);
#endif

	CiSensorsDlg *pParent = (CiSensorsDlg *)GetParent();
	m_nI2cDev = pParent->m_iSenI2c->GetI2cDevSel();

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CI2cMasterOperation::OnBnClickedRadioI2cmasterNone()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	
	CiSensorsDlg *pParent = (CiSensorsDlg *)GetParent();

	if(pParent->m_iSenI2c->GetI2cDevSel() != CISenI2cMaster::I2CDEV_NONE)
		m_nI2cDev = pParent->m_iSenI2c->iSenCloseI2cMaster();	
	
	UpdateStatusBar();

	UpdateData(FALSE);
}


void CI2cMasterOperation::OnBnClickedRadioI2cmasterAardvark()
{
	// TODO: Add your control notification handler code here
	UpdateData();
#ifdef AARDVARK_SUPPORT
	CiSensorsDlg *pParent = (CiSensorsDlg *)GetParent();
	m_nI2cDev = pParent->m_iSenI2c->iSenOpenI2cMaster(CISenI2cMaster::I2CDEV_AARDVARK);	
#endif
	UpdateStatusBar();

	UpdateData(FALSE);
}


void CI2cMasterOperation::OnBnClickedRadioI2cmasterIow()
{
	// TODO: Add your control notification handler code here
	UpdateData();
#ifdef IOW_SUPPORT
	CiSensorsDlg *pParent = (CiSensorsDlg *)GetParent();
	m_nI2cDev = pParent->m_iSenI2c->iSenOpenI2cMaster(CISenI2cMaster::I2CDEV_IOW);
	
#endif
	UpdateStatusBar();

	UpdateData(FALSE);
}


void CI2cMasterOperation::OnBnClickedRadioI2cmasterFtdi()
{
	// TODO: Add your control notification handler code here
}

void CI2cMasterOperation::UpdateStatusBar(void)
{
	CiSensorsDlg *pParent = (CiSensorsDlg *)GetParent();

	if(pParent->m_iSenI2c->GetI2cDevSel()==CISenI2cMaster::I2CDEV_NONE) {
		pParent->m_StatusBar.SetText(_T("I2C Master¡G None"), 0, 0);
	} else if(pParent->m_iSenI2c->GetI2cDevSel()==CISenI2cMaster::I2CDEV_AARDVARK) {
		pParent->m_StatusBar.SetText(_T("I2C Master¡G AARDVARK"), 0, 0);
	} else if(pParent->m_iSenI2c->GetI2cDevSel()==CISenI2cMaster::I2CDEV_IOW) {
		pParent->m_StatusBar.SetText(_T("I2C Master¡G IOW"), 0, 0);
	}
}

void CI2cMasterOperation::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CI2cMasterOperation::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}
