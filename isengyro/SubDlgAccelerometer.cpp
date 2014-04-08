// SubDlgIST8301.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgAccelerometer.h"
#include "afxdialogex.h"


// CSubDlgAccelerometer dialog

IMPLEMENT_DYNAMIC(CSubDlgAccelerometer, CSAPrefsSubDlg)

CSubDlgAccelerometer::CSubDlgAccelerometer(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgAccelerometer::IDD, pParent)
#ifdef D_SL
	, m_ChipSelection(A_DMARD08)
	, m_UnitsPreference(A_MG)
	, m_nCoordinateSystem(A_AVIATION)
#else
	, m_ChipSelection(0)
	, m_UnitsPreference(0)
	, m_nCoordinateSystem(A_ORIGINAL)
#endif
{

}

CSubDlgAccelerometer::~CSubDlgAccelerometer()
{
}

void CSubDlgAccelerometer::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_ACCELEROMETER_NONE, m_ChipSelection);
	DDX_Radio(pDX, IDC_RADIO_ACCELEROMETER_LSB, m_UnitsPreference);	
	DDX_Radio(pDX, IDC_RADIO_ACCELEROMETER_COORD_ORG, m_nCoordinateSystem);
}


BEGIN_MESSAGE_MAP(CSubDlgAccelerometer, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO_ACCELEROMETER_DMT08, &CSubDlgAccelerometer::OnBnClickedRadioAccelerometerDmt08)
	ON_BN_CLICKED(IDC_RADIO_ACCELEROMETER_NONE, &CSubDlgAccelerometer::OnBnClickedRadioAccelerometerNone)
	ON_BN_CLICKED(IDC_RADIO_ACCELEROMETER_KXTF9, &CSubDlgAccelerometer::OnBnClickedRadioAccelerometerKxtf9)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCELEROMETER_LAYOUT, &CSubDlgAccelerometer::OnCbnSelchangeComboAccelerometerLayout)
END_MESSAGE_MAP()


// CSubDlgAccelerometer message handlers
BOOL CSubDlgAccelerometer::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here
	m_LayoutPatternCtl = (CComboBox *)GetDlgItem(IDC_COMBO_ACCELEROMETER_LAYOUT);
	m_LayoutPatternCtl->InsertString(0, _T("LAYOUT_PATTERN_1"));
	m_LayoutPatternCtl->InsertString(1, _T("LAYOUT_PATTERN_2"));
	m_LayoutPatternCtl->InsertString(2, _T("LAYOUT_PATTERN_3"));
	m_LayoutPatternCtl->InsertString(3, _T("LAYOUT_PATTERN_4"));
	m_LayoutPatternCtl->InsertString(4, _T("LAYOUT_PATTERN_5"));
	m_LayoutPatternCtl->InsertString(5, _T("LAYOUT_PATTERN_6"));
	m_LayoutPatternCtl->InsertString(6, _T("LAYOUT_PATTERN_7"));
	m_LayoutPatternCtl->InsertString(7, _T("LAYOUT_PATTERN_8"));

#ifdef D_SL
	m_LayoutPatternCtl->SetCurSel(2) ;	
	if(m_ChipSelection==A_NONE)
		OnBnClickedRadioAccelerometerNone();
	if(m_ChipSelection==A_DMARD08)
		OnBnClickedRadioAccelerometerDmt08();
	if(m_ChipSelection==A_KXTF9)
		OnBnClickedRadioAccelerometerKxtf9();
#else
	m_LayoutPatternCtl->SetCurSel(0) ;	
	if(m_ChipSelection==0)
		OnBnClickedRadioAccelerometerNone();
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgAccelerometer::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}

void CSubDlgAccelerometer::OnBnClickedRadioAccelerometerNone()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	FreezeUnitsPreferenceGroup(false);
	FreezeCoordinateSystemGroup(false);
	FreezeLayoutPatternGroup(false);

	
	UpdateData(FALSE);
}


void CSubDlgAccelerometer::OnBnClickedRadioAccelerometerDmt08()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	FreezeUnitsPreferenceGroup(true);
	FreezeCoordinateSystemGroup(true);
	FreezeLayoutPatternGroup(true);


	UpdateData(FALSE);
}


void CSubDlgAccelerometer::OnBnClickedRadioAccelerometerKxtf9()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	FreezeUnitsPreferenceGroup(true);
	FreezeCoordinateSystemGroup(true);
	FreezeLayoutPatternGroup(true);

	UpdateData(FALSE);
}

void CSubDlgAccelerometer::FreezeUnitsPreferenceGroup(BOOL bFreeze)
{
	GetDlgItem(IDC_STATIC_ACCELEROMETER_UNITS_GROUP)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_ACCELEROMETER_LSB)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_ACCELEROMETER_MG)->EnableWindow(bFreeze);
}

void CSubDlgAccelerometer::FreezeCoordinateSystemGroup(BOOL bFreeze)
{
	GetDlgItem(IDC_STATIC_ACCELEROMETER_COORDINATE_SYSTEM)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_ACCELEROMETER_COORD_ORG)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_ACCELEROMETER_COORD_AVIATION)->EnableWindow(bFreeze);
	GetDlgItem(IDC_RADIO_ACCELEROMETER_COORD_ANDROID)->EnableWindow(bFreeze);	
}

void CSubDlgAccelerometer::FreezeLayoutPatternGroup(BOOL bFreeze)
{
	GetDlgItem(IDC_STATIC_ACCELEROMETER_LAYOUT_PATTERN)->EnableWindow(bFreeze);
	GetDlgItem(IDC_COMBO_ACCELEROMETER_LAYOUT)->EnableWindow(bFreeze);
}

void CSubDlgAccelerometer::OnCbnSelchangeComboAccelerometerLayout()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	
	UpdateData(FALSE);
}
