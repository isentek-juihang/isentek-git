// SubDlgMagnetometerData.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgMagnetometerData.h"
#include "afxdialogex.h"


// CSubDlgMagnetometerData dialog

IMPLEMENT_DYNAMIC(CSubDlgMagnetometerData, CSAPrefsSubDlg)

CSubDlgMagnetometerData::CSubDlgMagnetometerData(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgMagnetometerData::IDD, pParent)
	, m_raw()
	, m_offset()
	, m_data()
	, m_MagneticFieldStrength(0)
{

}

CSubDlgMagnetometerData::~CSubDlgMagnetometerData()
{
}

void CSubDlgMagnetometerData::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_X_RAW, m_raw.u.x);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Y_RAW, m_raw.u.y);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Z_RAW, m_raw.u.z);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_X_OFFSET, m_offset.x);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Y_OFFSET, m_offset.y);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Z_OFFSET, m_offset.z);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_X, m_data.x);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Y, m_data.y);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Z, m_data.z);	
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_MAGNETIC_FIELDS_DATA, m_MagneticFieldStrength);
}


BEGIN_MESSAGE_MAP(CSubDlgMagnetometerData, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSubDlgMagnetometerData message handlers

BOOL CSubDlgMagnetometerData::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgMagnetometerData::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}