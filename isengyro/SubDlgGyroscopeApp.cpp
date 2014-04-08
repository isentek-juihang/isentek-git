// SubDlgGyroscopeApp.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "SubDlgGyroscopeApp.h"
#include "afxdialogex.h"


// CSubDlgGyroscopeApp dialog

IMPLEMENT_DYNAMIC(CSubDlgGyroscopeApp, CSAPrefsSubDlg)

CSubDlgGyroscopeApp::CSubDlgGyroscopeApp(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgGyroscopeApp::IDD, pParent)
{

}

CSubDlgGyroscopeApp::~CSubDlgGyroscopeApp()
{
}

void CSubDlgGyroscopeApp::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSubDlgGyroscopeApp, CSAPrefsSubDlg)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSubDlgGyroscopeApp message handlers

BOOL CSubDlgGyroscopeApp::OnInitDialog()
{
	CSAPrefsSubDlg::OnInitDialog();

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CSubDlgGyroscopeApp::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSAPrefsSubDlg::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}