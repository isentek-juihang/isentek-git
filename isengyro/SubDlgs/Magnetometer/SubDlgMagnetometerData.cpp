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
	, m_MagneticFieldStrength(0)
	, m_pFont(NULL)
{
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			m_SoftMtx[i][j] = (_T(""));

	for(int i=0; i<3; i++) {
		m_raw[i] = (_T(""));
		m_offset[i] = (_T(""));
		m_data[i] = (_T(""));
	}
}

CSubDlgMagnetometerData::~CSubDlgMagnetometerData()
{
	//Delete Font Object
	if(m_pFont!=NULL) {
		m_pFont->DeleteObject();
		delete m_pFont;
		m_pFont = NULL;
	}
}

void CSubDlgMagnetometerData::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_X_RAW, m_raw[0]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Y_RAW, m_raw[1]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Z_RAW, m_raw[2]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_X_OFFSET, m_offset[0]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Y_OFFSET, m_offset[1]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Z_OFFSET, m_offset[2]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_X, m_data[0]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Y, m_data[1]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_Z, m_data[2]);	
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_MAGNETIC_FIELDS_DATA, m_MagneticFieldStrength);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_SOFT_MTX00, m_SoftMtx[0][0]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_SOFT_MTX01, m_SoftMtx[0][1]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_SOFT_MTX02, m_SoftMtx[0][2]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_SOFT_MTX10, m_SoftMtx[1][0]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_SOFT_MTX11, m_SoftMtx[1][1]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_SOFT_MTX12, m_SoftMtx[1][2]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_SOFT_MTX20, m_SoftMtx[2][0]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_SOFT_MTX21, m_SoftMtx[2][1]);
	DDX_Text(pDX, IDC_STATIC_MAGNETOMETER_SOFT_MTX22, m_SoftMtx[2][2]);
}


BEGIN_MESSAGE_MAP(CSubDlgMagnetometerData, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSubDlgMagnetometerData message handlers

BOOL CSubDlgMagnetometerData::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here
	m_pFont = new CFont;
	VERIFY(m_pFont->CreateFontW( 52, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman"))); 
	GetDlgItem(IDC_STATIC_MAGNETOMETER_Z_RAW)->SetFont(m_pFont);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgMagnetometerData::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}
