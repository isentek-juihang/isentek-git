// SubDlgMagnetometer.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgMagnetometer.h"
#include "afxdialogex.h"


// CSubDlgMagnetometer dialog

IMPLEMENT_DYNAMIC(CSubDlgMagnetometer, CSAPrefsSubDlg)

CSubDlgMagnetometer::CSubDlgMagnetometer(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgMagnetometer::IDD, pParent)
#ifdef D_SL
	, m_ChipSelection(M_IST8301)
	, m_UnitsPreference(M_LSB)
	, m_nCoordinateSystem(M_AVIATION)
#else
	, m_ChipSelection(0)
	, m_UnitsPreference(0)
	, m_nCoordinateSystem(M_ORIGINAL)
#endif
{

}

CSubDlgMagnetometer::~CSubDlgMagnetometer()
{
}

void CSubDlgMagnetometer::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_MAGNETOMETER_NONE, m_ChipSelection);
	DDX_Radio(pDX, IDC_RADIO_MAGNETOMETER_LSB, m_UnitsPreference);
	DDX_Radio(pDX, IDC_RADIO_MAGNETOMETER_COORD_ORG, m_nCoordinateSystem);
}


BEGIN_MESSAGE_MAP(CSubDlgMagnetometer, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO_MAGNETOMETER_NONE, &CSubDlgMagnetometer::OnBnClickedRadioMagnetometerNone)
	ON_BN_CLICKED(IDC_RADIO_MAGNETOMETER_IST8301, &CSubDlgMagnetometer::OnBnClickedRadioMagnetometerIst8301)
	ON_BN_CLICKED(IDC_RADIO_MAGNETOMETER_AKM8975, &CSubDlgMagnetometer::OnBnClickedRadioMagnetometerAkm8975)	
END_MESSAGE_MAP()


// CSubDlgMagnetometer message handlers

BOOL CSubDlgMagnetometer::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here
#ifdef D_SL
	if(m_ChipSelection==M_NONE)
		OnBnClickedRadioMagnetometerNone();
	if(m_ChipSelection==M_AKM8975)
		OnBnClickedRadioMagnetometerAkm8975();
	if(m_ChipSelection==M_IST8301)
		OnBnClickedRadioMagnetometerIst8301();
#else
	if(m_ChipSelection==0)
		OnBnClickedRadioMagnetometerNone();
#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgMagnetometer::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}

void CSubDlgMagnetometer::OnBnClickedRadioMagnetometerNone()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	FreezeUnitsPreferenceGroup(false);
	FreezeCoordinateSystemGroup(false);
	UpdateData(FALSE);
}

void CSubDlgMagnetometer::OnBnClickedRadioMagnetometerAkm8975()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	FreezeUnitsPreferenceGroup(true);
	FreezeCoordinateSystemGroup(true);
	UpdateData(FALSE);
}

void CSubDlgMagnetometer::OnBnClickedRadioMagnetometerIst8301()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	FreezeUnitsPreferenceGroup(true);
	FreezeCoordinateSystemGroup(true);
	UpdateData(FALSE);
}

void CSubDlgMagnetometer::FreezeUnitsPreferenceGroup(BOOL bFreeze)
{
	GetDlgItem(IDC_STATIC_MAGNETOMETER_UNITS_GROUP)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_MAGNETOMETER_LSB)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_MAGNETOMETER_MT)->EnableWindow(bFreeze);
}

void CSubDlgMagnetometer::FreezeCoordinateSystemGroup(BOOL bFreeze)
{
	GetDlgItem(IDC_STATIC_MAGNETOMETER_COORDINATE_SYSTEM)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_MAGNETOMETER_COORD_ORG)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_MAGNETOMETER_COORD_AVIATION)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_MAGNETOMETER_COORD_ANDROID)->EnableWindow(bFreeze);
}
