// SubDlgAccelerometerData.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgAccelerometerData.h"
#include "afxdialogex.h"


// CSubDlgAccelerometerData dialog

IMPLEMENT_DYNAMIC(CSubDlgAccelerometerData, CSAPrefsSubDlg)

CSubDlgAccelerometerData::CSubDlgAccelerometerData(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgAccelerometerData::IDD, pParent)
	, m_raw()
	, m_offset()
	, m_data()
	, m_bCalibration(false)
	, m_fRoll(0)
	, m_fPitch(0)
{

}

CSubDlgAccelerometerData::~CSubDlgAccelerometerData()
{
}

void CSubDlgAccelerometerData::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_X_RAW, m_raw.u.x);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_Y_RAW, m_raw.u.y);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_Z_RAW, m_raw.u.z);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_X_OFFSET, m_offset.x);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_Y_OFFSET, m_offset.y);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_Z_OFFSET, m_offset.z);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_X, m_data.x);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_Y, m_data.y);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_Z, m_data.z);	
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_ROLL_DATA, m_fRoll);
	DDX_Text(pDX, IDC_STATIC_ACCELEROMETER_PITCH_DATA, m_fPitch);
}


BEGIN_MESSAGE_MAP(CSubDlgAccelerometerData, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_ACCELEROMETER_CALIBRATION, &CSubDlgAccelerometerData::OnBnClickedButtonAccelerometerCalibration)
END_MESSAGE_MAP()


// CSubDlgAccelerometerData message handlers

BOOL CSubDlgAccelerometerData::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgAccelerometerData::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}

void CSubDlgAccelerometerData::OnBnClickedButtonAccelerometerCalibration()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_ACCELEROMETER_CALIBRATION)->EnableWindow(false);
	m_bCalibration = true;
}
