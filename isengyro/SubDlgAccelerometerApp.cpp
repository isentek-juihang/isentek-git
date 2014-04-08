// SubDlgAccelerometerApp.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgAccelerometerApp.h"
#include "afxdialogex.h"


// CSubDlgAccelerometerApp dialog

IMPLEMENT_DYNAMIC(CSubDlgAccelerometerApp, CSAPrefsSubDlg)

CSubDlgAccelerometerApp::CSubDlgAccelerometerApp(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgAccelerometerApp::IDD, pParent)
{

}

CSubDlgAccelerometerApp::~CSubDlgAccelerometerApp()
{
}

void CSubDlgAccelerometerApp::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSubDlgAccelerometerApp, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSubDlgAccelerometerApp message handlers

BOOL CSubDlgAccelerometerApp::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here
	//m_MyStatic.SetSpeed(1);
	//m_MyStatic.Draw3DFunction();
	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgAccelerometerApp::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}