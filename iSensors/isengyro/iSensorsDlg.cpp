
// iSensorsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iSensors.h"
#include "iSensorsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI SensorFusionProc(LPVOID lpParam);

CiSensorsDlg *g_pMain;

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CiSensorsDlg dialog




CiSensorsDlg::CiSensorsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CiSensorsDlg::IDD, pParent)
	, m_iSenI2c(NULL)	
	, bStopSignal(FALSE)
	, m_hThread(NULL)
//	, m_SingleDlg(FALSE)
	, m_SingleDlg(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iSenI2c = CreateIicObj();

	m_iCurPage = -1;
	m_pages.RemoveAll();
	
	m_pStartPage = NULL;
	
	m_csTitle = "Untitled";

	m_bValidateBeforeChangingPage = false;

	AddPage(m_SubDlgAccelerometer,	"Accelerometer");
	AddPage(m_SubDlgAccelerometerData,	"Accelerometer - Data", &m_SubDlgAccelerometer);
	AddPage(m_SubDlgAccelerometerApp,	"Accelerometer - App", &m_SubDlgAccelerometer);
	AddPage(m_SubDlgMagnetometer,	"Magnetometer");
	AddPage(m_SubDlgMagnetometerData,	"Magnetometer - Data", &m_SubDlgMagnetometer);
	AddPage(m_SubDlgMagnetometerApp,	"Magnetometer - App", &m_SubDlgMagnetometer);
	AddPage(m_SubDlgGyroscope,	"Gyroscope");
	AddPage(m_SubDlgGyroscopeData,	"Gyroscope - Data", &m_SubDlgGyroscope);
	AddPage(m_SubDlgGyroscopeApp,	"Gyroscope - App", &m_SubDlgGyroscope);
	AddPage(m_SubDlgCompass,	"Compass");

	// if we set this to true, it will call UpdateData on the current page,
	// before switching pages and the page switch will only happen if UpdateData succeeds.
	SetValidateBeforeChangingPages(true);
	SetStartPage(&m_SubDlgGyroscopeData);
//#ifdef _DEBUG
	RedirectIOToConsole();
//#endif
}

CiSensorsDlg::~CiSensorsDlg()
{
	// clean up
	for (int i=0;i<m_pages.GetSize();i++)
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
		if (pPS)
			delete pPS;
	}
}

void CiSensorsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PAGE, m_pageTree);
	DDX_Control(pDX, IDC_STATIC_DLG_FRAME, m_boundingFrame);
	DDX_Control(pDX, IDC_STATIC_CAPTION_BAR, m_captionBar);
	DDX_Control(pDX, IDC_BUTTON_START, m_StartingCtl);
	DDX_Control(pDX, IDC_LIST_MSG, m_MsgListCtl);
}

BEGIN_MESSAGE_MAP(CiSensorsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PAGE, &CiSensorsDlg::OnTvnSelchangedPageTree)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE_PAGE, &CiSensorsDlg::OnTvnGetdispinfoPageTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_PAGE, &CiSensorsDlg::OnTvnSelchangingPageTree)
	ON_MESSAGE(WM_CHANGE_PAGE, OnChangePage)
	ON_MESSAGE(WM_SET_FOCUS_WND, OnSetFocusWnd)
	ON_COMMAND(ID_SETTING_I2CUSB, &CiSensorsDlg::OnSettingI2cUsb)
	ON_BN_CLICKED(IDC_BUTTON_START, &CiSensorsDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDCANCEL, &CiSensorsDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CiSensorsDlg::OnBnClickedOk)
	ON_WM_CLOSE()	
	ON_COMMAND(ID_SETTING_SINGLEDIALOG, &CiSensorsDlg::OnSettingSingledialog)
	ON_UPDATE_COMMAND_UI(ID_SETTING_SINGLEDIALOG, &CiSensorsDlg::OnUpdateSettingSingledialog)
	ON_COMMAND(ID_HELP_ABOUTISENSORS, &CiSensorsDlg::OnHelpAboutIsensors)	
END_MESSAGE_MAP()


// CiSensorsDlg message handlers

BOOL CiSensorsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	g_pMain = this;
	
	InitializeGUI();

	m_iSenI2c->iSenOpenI2cMaster(CISenI2cMaster::I2CDEV_IOW);
	
	if(m_iSenI2c->GetI2cDevSel()==CISenI2cMaster::I2CDEV_NONE) {
		m_StatusBar.SetText(_T("I2C Master： None"), 0, 0);
	} else if(m_iSenI2c->GetI2cDevSel()==CISenI2cMaster::I2CDEV_AARDVARK) {
		m_StatusBar.SetText(_T("I2C Master： AARDVARK"), 0, 0);
	} else if(m_iSenI2c->GetI2cDevSel()==CISenI2cMaster::I2CDEV_IOW) {
		m_StatusBar.SetText(_T("I2C Master： IOW"), 0, 0);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CiSensorsDlg::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);
	
	// Don't let CDialog process the Escape key.
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	
	if (CWnd::PreTranslateMessage(pMsg))
		return TRUE;
	
	// don't translate dialog messages when 
	// application is in help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;
	
	// ensure the dialog messages will not
	// eat frame accelerators
	pFrameWnd = GetParentFrame();
	while (pFrameWnd != NULL)
	{
		if (pFrameWnd->PreTranslateMessage(pMsg))
			return TRUE;
		pFrameWnd = pFrameWnd->GetParentFrame();
	}
	
	return PreTranslateInput(pMsg);
	
}

void CiSensorsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CiSensorsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CiSensorsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CiSensorsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此加入特別建立的程式碼

	return 0;
}

void CiSensorsDlg::OnTvnSelchangedPageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	if (pNMTreeView->itemNew.lParam)
	{
		// find out which page was selected
		int iIdx = -1;
		for (int i=0;i<m_pages.GetSize();i++)
		{
			if (m_pages.GetAt(i)==(pageStruct *)pNMTreeView->itemNew.lParam)
			{
				iIdx = i;
				break;
			}
		}
		
		// show that page
		if ((iIdx >= 0) && (iIdx < m_pages.GetSize()))
		{
			pageStruct *pPS = (pageStruct *)m_pages.GetAt(iIdx);
			if (m_iCurPage!=iIdx)
			{
				PostMessage(WM_CHANGE_PAGE, iIdx);
			}
		}
	}

	*pResult = 0;
}


void CiSensorsDlg::OnTvnGetdispinfoPageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	// return the caption of the appropriate dialog
	if (pTVDispInfo->item.lParam)
	{
		if (pTVDispInfo->item.mask & TVIF_TEXT)
		{
			pageStruct *pPS = (pageStruct *)pTVDispInfo->item.lParam;
			wcscpy(pTVDispInfo->item.pszText, pPS->csCaption);
		}
	}

	*pResult = 0;
}


void CiSensorsDlg::OnTvnSelchangingPageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	// do we need to validate this page?
	if (m_bValidateBeforeChangingPage)
	{
		// is the current page a real page?
		if ((m_iCurPage >= 0) && (m_iCurPage < m_pages.GetSize()))
		{
			pageStruct *pPS = (pageStruct *)m_pages.GetAt(m_iCurPage);
			if (pPS)
			{
				ASSERT(pPS->pDlg);
				if (pPS->pDlg)
				{
					if (::IsWindow(pPS->pDlg->m_hWnd))
					{
						// now we know we have a valid window. let's see if the data on it is OK
						if (!pPS->pDlg->UpdateData())
						{  
							// bad data!
							
							// when we leave here, the tree wil grab focus again.
							// this is bad, because we want the control that cause the UpdateData
							// to fail to have focus (so the user knows what to fix). so, we'll
							// do some trickery...

							// find out who has focus
							CWnd *pLost = pPS->pDlg->GetFocus();
							
							if (pLost)
							{
								// post a message back to this window with the HWND of the
								// window that had focus (the one that broke the UpdateData).
								// the handler for WM_SET_FOCUS_WND will set the focus back to 
								// that control
								PostMessage(WM_SET_FOCUS_WND, (UINT)pLost->m_hWnd);
							}

							// don't switch pages
							*pResult = 1;

							return;
						}

						// if we're here, the dialog validated OK. we can switch pages.
					}
				}
			}
		}
	}

	// ok to switch pages
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////

HTREEITEM CiSensorsDlg::FindHTREEItemForDlg(CSAPrefsSubDlg *pParent)
{
	// if you didn't specify a parent in AddPage(...) , the
	// dialog becomes a root-level entry
	if (pParent==NULL)
	{
		return TVI_ROOT;
	}
	else
	{
		DWORD dwHTree;
		if (m_dlgMap.Lookup(pParent, dwHTree))
		{
			return (HTREEITEM)dwHTree;
		}
		else
		{
			// you have specified a parent that has not 
			// been added to the tree - can't do that.
			ASSERT(FALSE);
			return TVI_ROOT;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

bool CiSensorsDlg::AddPage(CSAPrefsSubDlg &dlg, const char *pCaption, CSAPrefsSubDlg* pDlgParent /*=NULL*/)
{
	if (m_hWnd)
	{
		// can't add once the window has been created
		ASSERT(0);
		return false;
	}
	
	pageStruct *pPS = new pageStruct;
	pPS->pDlg = &dlg;
	pPS->id = dlg.GetID();
	pPS->csCaption = pCaption;
	pPS->pDlgParent = pDlgParent;
	
	m_pages.Add(pPS);
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CiSensorsDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;
	
	cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, NULL, NULL, NULL);
	cs.style |= WS_CLIPCHILDREN;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////

bool CiSensorsDlg::ShowPage(CSAPrefsSubDlg * pPage)
{
	// find that page
	for (int i=0;i<m_pages.GetSize();i++)
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
		ASSERT(pPS);
		if (pPS)
		{
			ASSERT(pPS->pDlg);
			if (pPS->pDlg == pPage)
			{
				ShowPage(i);
				m_iCurPage = i;
				return true;
			}
		}
	}
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////

bool CiSensorsDlg::ShowPage(int iPage)
{
	m_captionBar.SetWindowText(_T(""));
	
	// turn off the current page
	if ((m_iCurPage >= 0) && (m_iCurPage < m_pages.GetSize()))
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(m_iCurPage);
		ASSERT(pPS);
		if (pPS)
		{
			ASSERT(pPS->pDlg);
			if (pPS->pDlg)
			{
				if (::IsWindow(pPS->pDlg->m_hWnd))
				{
					pPS->pDlg->ShowWindow(SW_HIDE);
				}
			}
		}
		else
		{
			return false;
		}
	}
	
	// show the new one
	if ((iPage >= 0) && (iPage < m_pages.GetSize()))
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(iPage);
		ASSERT(pPS);
		
		if (pPS)
		{
			ASSERT(pPS->pDlg);
			if (pPS->pDlg)
			{
				
				// update caption bar
				m_captionBar.SetWindowText(pPS->csCaption);
				
				// if we haven't already, Create the dialog
				if (!::IsWindow(pPS->pDlg->m_hWnd))
				{
					pPS->pDlg->Create(pPS->pDlg->GetID(), this);
				}
				
				// move, show, focus
				if (::IsWindow(pPS->pDlg->m_hWnd))
				{
					pPS->pDlg->MoveWindow(m_frameRect.left, m_frameRect.top, m_frameRect.Width(), m_frameRect.Height());
					pPS->pDlg->ShowWindow(SW_SHOW);
					pPS->pDlg->SetFocus();
				}
				
				// change the tree
				
				// find this in our map
				HTREEITEM hItem = FindHTREEItemForDlg(pPS->pDlg);
				if (hItem)
				{
					// select it
					m_pageTree.SelectItem(hItem);
				}
				return true;
			}
		}
	}
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////

bool CiSensorsDlg::EndOK()
{
	bool bOK = true;
	
	CSAPrefsSubDlg * pPage = NULL;
	
	// first, UpdateData...
	for (int i=0;i<m_pages.GetSize();i++)
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
		ASSERT(pPS);
		if (pPS)
		{
			ASSERT(pPS->pDlg);
			if (pPS->pDlg)
			{
				if (::IsWindow(pPS->pDlg->m_hWnd))
				{
					if (!pPS->pDlg->UpdateData(TRUE))
					{
						bOK = false;
						pPage = pPS->pDlg;
						break;
					}
				}
			}
		}
	}
	
	// were there any UpdateData errors?
	if ((!bOK) && (pPage!=NULL))
	{
		ShowPage(pPage);
		return false;
	}
	
	// tell all of the sub-dialogs "OK"
	for (int i=0;i<m_pages.GetSize();i++)
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
		ASSERT(pPS);
		if (pPS)
		{
			ASSERT(pPS->pDlg);
			if (pPS->pDlg)
			{
				if (::IsWindow(pPS->pDlg->m_hWnd))
				{
					pPS->pDlg->OnOK();
				}
			}
		}
	}
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CiSensorsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if (EndOK())
	{
		CDialogEx::OnOK();
	}
	
}

void CiSensorsDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(m_hThread!=NULL) {
		fprintf(stdout, "!!! Thread wait stop\n");
		WaitForSingleObject(m_hThread, INFINITE);
		fprintf(stdout, "!!! Thread stop\n");
		CloseHandle(m_hThread);
	}

	CDialogEx::OnClose();
}

void CiSensorsDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	
	// tell all of the sub-dialogs "Cancel"
	for (int i=0; i<m_pages.GetSize(); i++)
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
		ASSERT(pPS);
		
		if (pPS)
		{
			ASSERT(pPS->pDlg);
			if (pPS->pDlg)
			{
				if (::IsWindow(pPS->pDlg->m_hWnd))
				{
					pPS->pDlg->OnCancel();
				}
			}
		}
	}

	CDialogEx::OnCancel();
}


/////////////////////////////////////////////////////////////////////////////

void CiSensorsDlg::EndSpecial(UINT res, bool bOk)
{
	if (bOk)
	{
		EndOK();
	}
	
	EndDialog(res);
}


/////////////////////////////////////////////////////////////////////////////
#if 0
void CiSensorsDlg::OnPhelp() 
{
	// simulate the property sheet method of sending Help (with WM_NOTIFY)
	if ((m_iCurPage >= 0) && (m_iCurPage < m_pages.GetSize()))
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(m_iCurPage);
		ASSERT(pPS);
		ASSERT(pPS->pDlg);
		if (pPS)
		{
			if (pPS->pDlg)
			{
				if (::IsWindow(pPS->pDlg->m_hWnd))
				{
					// help!
					NMHDR nm;
					nm.code=PSN_HELP;
					nm.hwndFrom=m_hWnd;
					nm.idFrom=CSAPrefsDialog::IDD;
					pPS->pDlg->SendMessage(WM_NOTIFY, 0, (long)&nm);
				}
			}
		}
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////

LONG CiSensorsDlg::OnChangePage(UINT u, LONG l)
{
	if (ShowPage(u))
	{
		m_iCurPage = u;   
	}
	
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////

LONG CiSensorsDlg::OnSetFocusWnd(UINT u, LONG l)
{
	if (::IsWindow((HWND)u))
	{
		::SetFocus((HWND)u);
	}

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void __cdecl CiSensorsDlg::wPrintMsg(_In_z_ _Printf_format_string_ const wchar_t *format, ...)
{
	static DWORD ListBoxIndex = 0;

	if(ListBoxIndex>0xffffff)
		ListBoxIndex = 0;

	m_MsgListCtl.InsertString(ListBoxIndex, format);
	m_MsgListCtl.SetCurSel(ListBoxIndex);
	ListBoxIndex++;
}

void __cdecl CiSensorsDlg::wPrintMsg( _In_z_ _Printf_format_string_ const wchar_t *owner, _In_z_ _Printf_format_string_ const wchar_t *format, ...)
{
	wchar_t space[] = _T(" ");
	wchar_t message[256] = _T("");
	wcscat(message, owner);
	wcscat(message, space);
	wcscat(message, format);
	wPrintMsg(message);
}

void CiSensorsDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	DWORD dwData;
	DWORD dwThreadId;
	CString btnStr;

	UpdateData(TRUE);	

	if(!m_iSenI2c->CheckI2c()) {
		//Should display i2c error
		//	m_StatusBar.SetText(_T("I2C Master： None"), 0, 0);
		AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return;
	}
	
	m_StartingCtl.GetWindowText(btnStr);
	
	if(btnStr==_T("Start measuring")) {
		bStopSignal = FALSE;
		m_StartingCtl.SetWindowText(_T("Stop measuring"));

		m_SubDlgAccelerometer.UpdateData();
		m_SubDlgMagnetometer.UpdateData();
		m_SubDlgGyroscope.UpdateData();

		if(m_hThread!=NULL)
			CloseHandle(m_hThread);
			m_hThread = CreateThread(
						NULL, 0,
						SensorFusionProc,
						&dwData,
						0,
						&dwThreadId);
		
		if(m_hThread == NULL)	{
			AfxMessageBox(_T("Create Thread Error"));
			ExitProcess(0);
		}
		
	} else if( btnStr==_T("Stop measuring") ) {
		bStopSignal = TRUE;

		m_StartingCtl.SetWindowText(_T("Start measuring"));
	}

	UpdateData(FALSE);
}





