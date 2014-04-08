// SubDlgGyroscope.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgGyroscope.h"
#include "afxdialogex.h"


// CSubDlgGyroscope dialog

IMPLEMENT_DYNAMIC(CSubDlgGyroscope, CSAPrefsSubDlg)

CSubDlgGyroscope::CSubDlgGyroscope(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgGyroscope::IDD, pParent)
	, m_ChipSelection(0)
	, m_UnitsPreference(0)
	, m_nCoordinateSystem(0)
{

}

CSubDlgGyroscope::~CSubDlgGyroscope()
{
}

void CSubDlgGyroscope::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_GYROSCOPE_NONE, m_ChipSelection);
}


BEGIN_MESSAGE_MAP(CSubDlgGyroscope, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO_GYROSCOPE_NONE, &CSubDlgGyroscope::OnBnClickedRadioGyroscopeNone)
	ON_BN_CLICKED(IDC_RADIO_GYROSCOPE_L3G4200D, &CSubDlgGyroscope::OnBnClickedRadioGyroscopeL3g4200d)
END_MESSAGE_MAP()


// CSubDlgGyroscope message handlers

BOOL CSubDlgGyroscope::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgGyroscope::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}

void CSubDlgGyroscope::OnBnClickedRadioGyroscopeNone()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	UpdateData(FALSE);
}


void CSubDlgGyroscope::OnBnClickedRadioGyroscopeL3g4200d()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	UpdateData(FALSE);
}
