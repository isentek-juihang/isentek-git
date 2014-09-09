// SubDlgCompass.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgCompass.h"
#include "afxdialogex.h"


// CSubDlgCompass dialog

IMPLEMENT_DYNAMIC(CSubDlgCompass, CSAPrefsSubDlg)

CSubDlgCompass::CSubDlgCompass(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgCompass::IDD, pParent)
	, m_data()
	, m_TiltSource(CSubDlgCompass::REAL_TILT)
	, m_VittualRoll(0)
	, m_VittualPitch(0)
{
	for(int i=0; i<nFontNum; i++)
		m_pFont[i]= NULL;
	
	for(int i=0; i<nBrushNum; i++)
		m_pBrush[i] = NULL;
}

CSubDlgCompass::~CSubDlgCompass()
{
}

void CSubDlgCompass::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_COMPASS_X, m_data.x);
	DDX_Text(pDX, IDC_STATIC_COMPASS_Y, m_data.y);
	DDX_Text(pDX, IDC_STATIC_COMPASS_Z, m_data.z);
	DDX_Radio(pDX, IDC_RADIO_COMPASS_REAL_TILT, m_TiltSource);
	DDX_Control(pDX, IDC_SLIDER_COMPASS_VIRTUAL_PITCH, m_VirtualPitchCtl);
	DDX_Control(pDX, IDC_SLIDER_COMPASS_VIRTUAL_ROLL, m_VirtualRollCtl);
	DDX_Text(pDX, IDC_STATIC_COMPASS_VIRTUAL_ROLL, m_VittualRoll);
	DDX_Text(pDX, IDC_STATIC_COMPASS_VIRTUAL_PITCH, m_VittualPitch);
}


BEGIN_MESSAGE_MAP(CSubDlgCompass, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO_COMPASS_REAL_TILT, &CSubDlgCompass::OnBnClickedRadioCompassRealTilt)
	ON_BN_CLICKED(IDC_RADIO_COMPASS_VIRTUAL_TILT, &CSubDlgCompass::OnBnClickedRadioCompassVirtualTilt)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CSubDlgCompass message handlers

BOOL CSubDlgCompass::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here
	m_VirtualPitchCtl.SetRange(-90,90, true);
	m_VirtualRollCtl.SetRange(-180,180, true);
#if 0
	m_VirtualPitchCtl.SetPos(m_VittualPitch);
	m_VirtualRollCtl.SetPos(m_VittualRoll);
#else
	MyPitchUpdateData();
	MyRollUpdateData();
#endif
	m_VirtualPitchCtl.ShowWindow(SW_HIDE);
	m_VirtualRollCtl.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_COMPASS_VIRTUAL_PITCH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_COMPASS_VIRTUAL_ROLL)->ShowWindow(SW_HIDE);

	
	InitializeFont();

	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgCompass::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}

void CSubDlgCompass::OnBnClickedRadioCompassRealTilt()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_VirtualPitchCtl.ShowWindow(SW_HIDE);
	m_VirtualRollCtl.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_COMPASS_VIRTUAL_PITCH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_COMPASS_VIRTUAL_ROLL)->ShowWindow(SW_HIDE);
	UpdateData(FALSE);
}


void CSubDlgCompass::OnBnClickedRadioCompassVirtualTilt()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_VirtualPitchCtl.ShowWindow(SW_SHOW);
	m_VirtualRollCtl.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_COMPASS_VIRTUAL_PITCH)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_COMPASS_VIRTUAL_ROLL)->ShowWindow(SW_SHOW);
	UpdateData(FALSE);
}


void CSubDlgCompass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	BOOL changed = true;
	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_COMPASS_VIRTUAL_ROLL:
		switch(nSBCode) {
		case SB_PAGELEFT:
		case SB_PAGERIGHT:
			m_VittualRoll = m_VirtualRollCtl.GetPos();
			break;
		case SB_THUMBPOSITION:
			m_VittualRoll = nPos;
			break;	
		default:
			changed=FALSE;
		}
		break;
	default:
		changed=FALSE;
	}
	if(changed)
		MyRollUpdateData();
	CSAPrefsSubDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CSubDlgCompass::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	BOOL changed = true;
	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_COMPASS_VIRTUAL_PITCH:
		switch(nSBCode) {
		case SB_PAGELEFT:
		case SB_PAGERIGHT:
			m_VittualPitch = m_VirtualPitchCtl.GetPos()*-1;
			break;
		case SB_THUMBPOSITION:
			m_VittualPitch = nPos*-1;
			break;	
		default:
			changed=FALSE;
		}
		break;
	default:
		changed=FALSE;
	}
	if(changed)
		MyPitchUpdateData();
	CSAPrefsSubDlg::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSubDlgCompass::MyPitchUpdateData()
{
	m_VirtualPitchCtl.SetPos(m_VittualPitch*-1);
	UpdateData(FALSE);
}

void CSubDlgCompass::MyRollUpdateData()
{
	m_VirtualRollCtl.SetPos(m_VittualRoll);
	UpdateData(FALSE);
}

void CSubDlgCompass::InitializeFont(void)
{
	//FW_NORMAL
#if 1
	//Font[0]
	m_pFont[0] = new CFont;
	VERIFY(m_pFont[0]->CreateFontW( 52, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman"))); 
	GetDlgItem(IDC_STATIC_COMPASS_X)->SetFont(m_pFont[0]);

	//m_pFont[0]->DeleteObject();
#else
	CFont m_Font;
	VERIFY(m_Font.CreateFontW(  28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman")));
	GetDlgItem(IDC_STATIC_COMPASS_X)->SetFont(&m_Font);

	VERIFY(m_Font.CreateFontW(  52, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman"))); 
	GetDlgItem(IDC_STATIC_COMPASS_Y)->SetFont(&m_Font);

	
#endif
#if 0
	//Font[1]
	m_pFont[1] = new CFont;
	VERIFY(m_pFont[1]->CreateFontW( 16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman"))); 
	
	GetDlgItem(IDC_STATIC_M_SENINFO_GROUP)->SetFont(m_pFont[1]);
	GetDlgItem(IDC_STATIC_G_SENINFO_GROUP)->SetFont(m_pFont[1]);
	GetDlgItem(IDC_STATIC_CALIBRATION_GROUP)->SetFont(m_pFont[1]);
	GetDlgItem(IDC_STATIC_ANGLES_GROUP)->SetFont(m_pFont[1]);

	GetDlgItem(IDC_STATIC_MAGNETIC_FIELDS)->SetFont(m_pFont[1]);
	/**/
	//Font[2]
	m_pFont[2] = new CFont;
	VERIFY(m_pFont[2]->CreateFontW( 28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
					   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman"))); 
	GetDlgItem(IDC_EDIT_ROLL_DATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_PITCH_DATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_MAGNETIC_FIELDS)->SetFont(m_pFont[2]);

	GetDlgItem(IDC_STATIC_M_RAW)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_CALIBRATED)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_MX)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_MY)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_MZ)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_MXDATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_MYDATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_MZDATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_CMXDATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_CMYDATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_CMZDATA)->SetFont(m_pFont[2]);	

	GetDlgItem(IDC_STATIC_CAX)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_CAY)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_CAZ)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_CA_XDATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_CA_YDATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_CA_ZDATA)->SetFont(m_pFont[2]);	

	GetDlgItem(IDC_STATIC_GX)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_GY)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_GZ)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_GXDATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_GYDATA)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_EDIT_GZDATA)->SetFont(m_pFont[2]);

	GetDlgItem(IDC_STATIC_ROLL)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_PITCH)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_ANGLE)->SetFont(m_pFont[2]);
	

	/*GetDlgItem(IDC_STATIC_M_SENINFO_GROUP)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_G_SENINFO_GROUP)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_CALIBRATION_GROUP)->SetFont(m_pFont[2]);
	GetDlgItem(IDC_STATIC_ANGLES_GROUP)->SetFont(m_pFont[2]);*/
#endif
}
