// SubDlgGyroscopeData.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgGyroscopeData.h"
#include "afxdialogex.h"


// CSubDlgGyroscopeData dialog

IMPLEMENT_DYNAMIC(CSubDlgGyroscopeData, CSAPrefsSubDlg)

CSubDlgGyroscopeData::CSubDlgGyroscopeData(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgGyroscopeData::IDD, pParent)
	, m_raw()
	, m_offset()
	, m_data()
	, m_soft_data()
{

}

CSubDlgGyroscopeData::~CSubDlgGyroscopeData()
{
}

void CSubDlgGyroscopeData::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_STATIC_GYROSCOPE_X_RAW, m_raw.u.x);
//	DDX_Text(pDX, IDC_STATIC_GYROSCOPE_Y_RAW, m_raw.u.y);
//	DDX_Text(pDX, IDC_STATIC_GYROSCOPE_Z_RAW, m_raw.u.z);
//	DDX_Text(pDX, IDC_STATIC_GYROSCOPE_X_OFFSET, m_offset.x);
//	DDX_Text(pDX, IDC_STATIC_GYROSCOPE_Y_OFFSET, m_offset.y);
//	DDX_Text(pDX, IDC_STATIC_GYROSCOPE_Z_OFFSET, m_offset.z);
	DDX_Text(pDX, IDC_STATIC_GYROSCOPE_X, m_data.x);
	DDX_Text(pDX, IDC_STATIC_GYROSCOPE_Y, m_data.y);
	DDX_Text(pDX, IDC_STATIC_GYROSCOPE_Z, m_data.z);
	DDX_Text(pDX, IDC_STATIC_EGYROSCOPE_X, m_soft_data.x);
	DDX_Text(pDX, IDC_STATIC_EGYROSCOPE_Y, m_soft_data.y);
	DDX_Text(pDX, IDC_STATIC_EGYROSCOPE_Z, m_soft_data.z);
	DDX_Control(pDX, IDC_STATIC_GYRO_X, m_GyroWaveform[0]);
	DDX_Control(pDX, IDC_STATIC_GYRO_Y, m_GyroWaveform[1]);
	DDX_Control(pDX, IDC_STATIC_GYRO_Z, m_GyroWaveform[2]);
	DDX_Control(pDX, IDC_STATIC_EGYRO_X, m_SoftGyroWaveform[0]);
	DDX_Control(pDX, IDC_STATIC_EGYRO_Y, m_SoftGyroWaveform[1]);
	DDX_Control(pDX, IDC_STATIC_EGYRO_Z, m_SoftGyroWaveform[2]);
//	DDX_Control(pDX, IDC_STATIC_MAGNETOMETER_WAVEFORM3D, m_WaveformCtrl);
}


BEGIN_MESSAGE_MAP(CSubDlgGyroscopeData, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSubDlgGyroscopeData message handlers

BOOL CSubDlgGyroscopeData::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();
	// TODO: Add extra initialization here
	for(int i=0; i<3; i++) {
		m_GyroWaveform[i].Create();
		m_GyroWaveform[i].SetRange(15);		

		m_SoftGyroWaveform[i].Create();
		m_SoftGyroWaveform[i].SetRange(15);
	}

	m_GyroWaveform[0].SetEnable(true, false, false);
	m_SoftGyroWaveform[0].SetEnable(true, false, false);

	m_GyroWaveform[1].SetEnable(false, true, false);
	m_SoftGyroWaveform[1].SetEnable(false, true, false);

	m_GyroWaveform[2].SetEnable(false, false, true);
	m_SoftGyroWaveform[2].SetEnable(false, false, true);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgGyroscopeData::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}