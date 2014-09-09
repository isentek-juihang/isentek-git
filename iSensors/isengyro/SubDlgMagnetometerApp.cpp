// SubDlgMagnetometerApp.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgMagnetometerApp.h"
#include "afxdialogex.h"


// CSubDlgMagnetometerApp dialog

IMPLEMENT_DYNAMIC(CSubDlgMagnetometerApp, CSAPrefsSubDlg)

CSubDlgMagnetometerApp::CSubDlgMagnetometerApp(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgMagnetometerApp::IDD, pParent)

{

}

CSubDlgMagnetometerApp::~CSubDlgMagnetometerApp()
{
}

void CSubDlgMagnetometerApp::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSubDlgMagnetometerApp, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CSubDlgMagnetometerApp message handlers

BOOL CSubDlgMagnetometerApp::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgMagnetometerApp::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}

void CSubDlgMagnetometerApp::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CSAPrefsSubDlg::OnPaint() for painting messages

}
