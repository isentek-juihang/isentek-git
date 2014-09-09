
// ISenTekAssistantDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ISenTekAssistant.h"
#include "ISenTekAssistantDlg.h"

//#include "afxdialogex.h"
#include "Total.h"
#include <math.h>

#include "IST8301Def.h"
#include "IST8301RegTable.h"
#include "ISenTekAssistantDlg.h"

#define INT_PROCESS

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI SimulationProc(LPVOID lpParam);

int g_AngleData = 0;

float g_fOptimX(0.0f);
float g_fOptimY(0.0f);
float g_fOptimZ(0.0f);

int IST8301_I2CSLAVEADD = 0x0E;
int DMARD08_I2CSLAVEADD = 0x1C;

#define FILE_BUFFER_SIZE 0xA0000
#define CURRENT_PATH _T(".\\")

static CISenTekAssistantDlg *g_pMain;

// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
	, m_FileVersion(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_FILEVER, m_FileVersion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CISenTekAssistantDlg dialog

CISenTekAssistantDlg::CISenTekAssistantDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CISenTekAssistantDlg::IDD, pParent)	
	, m_bAardvarkDisplaying(0)
	, bStopSimulation(FALSE)
	, m_bSwDebugMode(true)
	, m_nOffsetX(128)
	, m_nOffsetY(128)
	, m_nOffsetZ(128)
	, m_nOffsetDriftX(0)
	, m_nOffsetDriftY(0)
	, m_nOffsetDriftZ(0)
	, m_nInterruptThresholdValue(0)
	, m_nAccelerometerSel(G_KXTF9)
	, m_fAngleAlpha(1.00f)
	, m_fRollAlpha(0.5f)
	, m_fPitchAlpha(0.5f)
	, m_fGsenAlpha(0.25f)
	, Gz(1000)
	, GzTh(10000.0f)
//	, GzTh2(10000.0f)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_brush.CreateSolidBrush(RGB(50, 50, 80));

	m_iCurPage = -1;
	m_pages.RemoveAll();
	
	m_pStartPage = NULL;
	
	m_csTitle = "Untitled";

	m_bValidateBeforeChangingPage = false;

	AddPage(SimulationSubDlg,		"Demo");
	
	for(int i=0; i<3; i++)
		m_nMdTh[i] = 5;

//	SetValidateBeforeChangingPages(true);
}

CISenTekAssistantDlg::~CISenTekAssistantDlg()
{
	for (int i=0;i<m_pages.GetSize();i++)
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
		if (pPS)
			delete pPS;
	}
}

void CISenTekAssistantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PAGE, m_pageTree);
	DDX_Control(pDX, IDC_STATIC_DLG_FRAME, m_boundingFrame);
	DDX_Control(pDX, IDC_STATIC_CAPTION_BAR, m_captionBar);
}

BEGIN_MESSAGE_MAP(CISenTekAssistantDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_INITMENUPOPUP()
	ON_BN_CLICKED(IDC_BUTTON1, &CISenTekAssistantDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_WM_KEYUP()
	ON_WM_CTLCOLOR()
//	ON_WM_TIMER()
	ON_COMMAND(ID_USB_SETTING_AARDVARK, &CISenTekAssistantDlg::OnSettingUsbAardvark)
	ON_UPDATE_COMMAND_UI(ID_USB_SETTING_AARDVARK, &CISenTekAssistantDlg::OnUpdateSettingUsbAardvark)
	ON_COMMAND(ID_SETTING_CONFIGURATION, &CISenTekAssistantDlg::OnSettingConfiguration)
	ON_COMMAND(ID_HELP_ABOUT, &CISenTekAssistantDlg::OnHelpAbout)
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PAGE, &CISenTekAssistantDlg::OnTvnSelchangedPageTree)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE_PAGE, &CISenTekAssistantDlg::OnTvnGetdispinfoPageTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_PAGE, &CISenTekAssistantDlg::OnTvnSelchangingPageTree)
	ON_MESSAGE(WM_CHANGE_PAGE, OnChangePage)
	ON_MESSAGE(WM_SET_FOCUS_WND, OnSetFocusWnd)
END_MESSAGE_MAP()


// CISenTekAssistantDlg message handlers

BOOL CISenTekAssistantDlg::OnInitDialog()
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

	m_iSenI2cMaster.iSenOpenI2cMaster(CISenI2cMaster::nDefaultI2C);

	InitializeGUI();	

	LoadCalibrationFile(&g_fOptimX, &g_fOptimY, &g_fOptimZ);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CISenTekAssistantDlg::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);
	
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	
	if (CWnd::PreTranslateMessage(pMsg))
		return TRUE;
	
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;
	
	pFrameWnd = GetParentFrame();
	while (pFrameWnd != NULL)
	{
		if (pFrameWnd->PreTranslateMessage(pMsg))
			return TRUE;
		pFrameWnd = pFrameWnd->GetParentFrame();
	}
	
	return PreTranslateInput(pMsg);
	
}

void CISenTekAssistantDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CISenTekAssistantDlg::OnPaint()
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

HCURSOR CISenTekAssistantDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CISenTekAssistantDlg::OnClose()
{

	CDialogEx::OnClose();
}

int CISenTekAssistantDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}

void CISenTekAssistantDlg::OnTvnSelchangedPageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (pNMTreeView->itemNew.lParam)
	{
		int iIdx = -1;
		for (int i=0;i<m_pages.GetSize();i++)
		{
			if (m_pages.GetAt(i)==(pageStruct *)pNMTreeView->itemNew.lParam)
			{
				iIdx = i;
				break;
			}
		}
		
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


void CISenTekAssistantDlg::OnTvnGetdispinfoPageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

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


void CISenTekAssistantDlg::OnTvnSelchangingPageTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	if (m_bValidateBeforeChangingPage)
	{
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
						if (!pPS->pDlg->UpdateData())
						{  
							CWnd *pLost = pPS->pDlg->GetFocus();
							
							if (pLost)
							{
								PostMessage(WM_SET_FOCUS_WND, (UINT)pLost->m_hWnd);
							}

							*pResult = 1;

							return;
						}

					}
				}
			}
		}
	}

	*pResult = 0;
}

HTREEITEM CISenTekAssistantDlg::FindHTREEItemForDlg(CSAPrefsSubDlg *pParent)
{
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
			ASSERT(FALSE);
			return TVI_ROOT;
		}
	}
}

bool CISenTekAssistantDlg::AddPage(CSAPrefsSubDlg &dlg, const char *pCaption, CSAPrefsSubDlg* pDlgParent /*=NULL*/)
{
	if (m_hWnd)
	{
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

BOOL CISenTekAssistantDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;
	
	cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, NULL, NULL, NULL);
	cs.style |= WS_CLIPCHILDREN;
	return TRUE;
}


bool CISenTekAssistantDlg::ShowPage(CSAPrefsSubDlg * pPage)
{
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

bool CISenTekAssistantDlg::ShowPage(int iPage)
{
	m_captionBar.SetWindowText(_T(""));
	
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
	
	if ((iPage >= 0) && (iPage < m_pages.GetSize()))
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(iPage);
		ASSERT(pPS);
		
		if (pPS)
		{
			ASSERT(pPS->pDlg);
			if (pPS->pDlg)
			{
				
				m_captionBar.SetWindowText(pPS->csCaption);
				
				if (!::IsWindow(pPS->pDlg->m_hWnd))
				{
					pPS->pDlg->Create(pPS->pDlg->GetID(), this);
				}
				
				if (::IsWindow(pPS->pDlg->m_hWnd))
				{
					pPS->pDlg->MoveWindow(m_frameRect.left, m_frameRect.top, m_frameRect.Width(), m_frameRect.Height());
					pPS->pDlg->ShowWindow(SW_SHOW);
					pPS->pDlg->SetFocus();
				}
				
				
				HTREEITEM hItem = FindHTREEItemForDlg(pPS->pDlg);
				if (hItem)
				{
					m_pageTree.SelectItem(hItem);
				}
				return true;
			}
		}
	}
	
	return false;
}

void CISenTekAssistantDlg::OnOK() 
{
	if (EndOK())
	{
		CDialog::OnOK();
	}
}

bool CISenTekAssistantDlg::EndOK()
{
	bool bOK = true;
	
	CSAPrefsSubDlg * pPage = NULL;
	
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
	
	if ((!bOK) && (pPage!=NULL))
	{
		ShowPage(pPage);
		return false;
	}
	
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


void CISenTekAssistantDlg::OnCancel() 
{
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

	CDialog::OnCancel();
}



void CISenTekAssistantDlg::EndSpecial(UINT res, bool bOk)
{
	if (bOk)
	{
		EndOK();
	}
	
	EndDialog(res);
}



LONG CISenTekAssistantDlg::OnChangePage(UINT u, LONG l)
{
	if (ShowPage(u))
	{
		m_iCurPage = u;   
	}
	
	return 0L;
}


LONG CISenTekAssistantDlg::OnSetFocusWnd(UINT u, LONG l)
{
	if (::IsWindow((HWND)u))
	{
		::SetFocus((HWND)u);
	}

	return 0L;
}

void CISenTekAssistantDlg::OnBnClickedButton1()
{
	// TODO: 在此加入控制項告知處理常式程式碼

}

bool CISenTekAssistantDlg::LoadCalibrationFile(float *OptimX, float *OptimY, float *OptimZ)
{
	FILE *file;
	if ((file=fopen("Result.csv","r"))==NULL) {
		*OptimX = *OptimY = *OptimZ = 0.0f;
		return false;
	} else {
		fscanf(file, "%f,%f,%f", OptimX, OptimY, OptimZ);
		fclose(file);
		return true;
	}
}

float CISenTekAssistantDlg::GetYawAngle(float fMX, float fMY, float fMZ, float fRoll, float fPitch)
{
	float yaw = 0.0;
	
	yaw = atan2((sin(fRoll)*fMZ)-(cos(fRoll)*fMY), 
		(fMX*cos(fPitch))+(sin(fPitch)*fMY*sin(fRoll))+(fMZ*sin(fPitch)*cos(fRoll)));

	return yaw;
}

DWORD WINAPI SimulationProc(LPVOID lpParam)
{
	CString tmpStr;

	int nODR = 2;
	int nPresetTime = 0;

	float yaw=0.0;
	static unsigned int lastAngle=0;
	int nLastG[3] = {0};
	int nLastM[3] = {0};
	int nG[3] = {0};
	int nM[3] = {0};
	bool bABC = false;

	float fRoll  =0.0f;
	float fPitch =0.0f;

	float fLastRoll  =0.0f;
	float fLastPitch =0.0f;
	float fPreviousYaw=0.0;
	float G_Dis = 0.0f;

	char file_out[FILE_BUFFER_SIZE]={0};
	int fileIndex=0;	

	CFile file(_T("Simulation.csv"), CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite);

	fileIndex = sprintf( file_out, "Sensor, X Data, Y Data, Z Data, Roll, Picth, yaw angle\r\n"); 

	g_pMain->m_iSenI2cMaster.IST8301_SoftReset();

	switch (g_pMain->m_nAccelerometerSel)
	{
	case CISenTekAssistantDlg::G_NONE:
		break;
	case CISenTekAssistantDlg::G_KXTF9:
		if(!g_pMain->m_iSenI2cMaster.KXTF9_Chipset_Init())
			g_pMain->m_nAccelerometerSel = CISenTekAssistantDlg::G_NONE;
		break;
	
	case CISenTekAssistantDlg::G_DMARD08:
		break;
	}

	nODR = g_pMain->SimulationSubDlg.m_OdrCtl->GetCurSel();
	nPresetTime = 0;
	g_pMain->m_iSenI2cMaster.IST8301_ChipsetInit(  g_pMain->m_nOffsetX,
									g_pMain->m_nOffsetY,
									g_pMain->m_nOffsetZ,
									g_pMain->m_nOffsetDriftX,
									g_pMain->m_nOffsetDriftY,
									g_pMain->m_nOffsetDriftZ,
									nODR,
									nPresetTime,
									g_pMain->m_nInterruptThresholdValue );
	
	
	while(1)
	{
		switch (g_pMain->m_nAccelerometerSel)
		{
		case CISenTekAssistantDlg::G_NONE:
			for(int i=0; i<3; i++) {
				nLastG[i] = 0;
				nG[i] = 0;
			}
			fRoll = 0.0f;
			fPitch = 0.0f;
			break;

		case CISenTekAssistantDlg::G_KXTF9:
			if(g_pMain->m_iSenI2cMaster.KXTF9_ReadSensorData(&nG[1], &nG[0], &nG[2])<0) {
				g_pMain->m_nAccelerometerSel = CISenTekAssistantDlg::G_NONE;
				for(int i=0; i<3; i++) {
					nLastG[i] = 0;
					nG[i] = 0;
				}
				fRoll = 0.0f;
				fPitch = 0.0f;
			}
			break;
	
		case CISenTekAssistantDlg::G_DMARD08:
			if(!g_pMain->m_iSenI2cMaster.DMARD08_ReadSensorData(&nG[1], &nG[0], &nG[2])) {
				g_pMain->m_nAccelerometerSel = CISenTekAssistantDlg::G_NONE;
				for(int i=0; i<3; i++) {
					nLastG[i] = 0;
					nG[i] = 0;
				}
				fRoll = 0.0f;
				fPitch = 0.0f;
			} else {
				nG[1] *= -1;
				nG[2] *= -1;
			}
			break;
		}

		while( !g_pMain->m_iSenI2cMaster.IST8301_CheckDataReady() && !g_pMain->bStopSimulation)
		{
			Sleep(1);
		}		
		
		nM[0] = g_pMain->m_iSenI2cMaster.IST8301_GetXYZ(IST8301_DATAY)*-1;
		nM[1] = g_pMain->m_iSenI2cMaster.IST8301_GetXYZ(IST8301_DATAX)*-1;				
		nM[2] = g_pMain->m_iSenI2cMaster.IST8301_GetXYZ(IST8301_DATAZ)*-1;

		g_pMain->SimulationSubDlg.m_nM[0] = nM[0];
		g_pMain->SimulationSubDlg.m_nM[1] = nM[1];
		g_pMain->SimulationSubDlg.m_nM[2] = nM[2];

		g_pMain->SimulationSubDlg.m_nG[0] = nG[0];
		g_pMain->SimulationSubDlg.m_nG[1] = nG[1];
		g_pMain->SimulationSubDlg.m_nG[2] = nG[2];

		//g_pMain->SimulationSubDlg.UpdateData(FALSE);		//Debug
		
		nM[0] -= (int)g_fOptimX;
		nM[1] -= (int)g_fOptimY;
		nM[2] -= (int)g_fOptimZ;

		g_pMain->SimulationSubDlg.m_nCalibratedM[0] = nM[0];
		g_pMain->SimulationSubDlg.m_nCalibratedM[1] = nM[1];
		g_pMain->SimulationSubDlg.m_nCalibratedM[2] = nM[2];

		nG[0] = (1-g_pMain->m_fGsenAlpha)*nLastG[0]+ g_pMain->m_fGsenAlpha*nG[0];
		nG[1] = (1-g_pMain->m_fGsenAlpha)*nLastG[1]+ g_pMain->m_fGsenAlpha*nG[1];
		nG[2] = (1-g_pMain->m_fGsenAlpha)*nLastG[2]+ g_pMain->m_fGsenAlpha*nG[2];

		nM[0] = (1-g_pMain->m_fGsenAlpha)*nLastM[0]+ g_pMain->m_fGsenAlpha*nM[0];
		nM[1] = (1-g_pMain->m_fGsenAlpha)*nLastM[1]+ g_pMain->m_fGsenAlpha*nM[1];
		nM[2] = (1-g_pMain->m_fGsenAlpha)*nLastM[2]+ g_pMain->m_fGsenAlpha*nM[2];


		fRoll = atan2((float)nG[1], (float)nG[2]);
		fPitch = atan2(-( nG[0]), nG[1]*sin(fRoll)+nG[2]*cos(fRoll));

		int tmpDist1 = abs(nM[0] - nLastM[0]);
		int tmpDist2 = abs(nM[1] - nLastM[1]);
		int tmpDist3 = abs(nM[2] - nLastM[2]);
		
	if(tmpDist1>g_pMain->m_nMdTh[0] && tmpDist2>g_pMain->m_nMdTh[1] && tmpDist3>g_pMain->m_nMdTh[2]) 
	{
			G_Dis = 0.0f;
			for(int i=0; i<3; i++)
				G_Dis += pow((float)nG[i], 2);
			G_Dis = pow(G_Dis, 0.5f);

			if( (abs(G_Dis-g_pMain->Gz) > g_pMain->GzTh) && g_pMain->m_nAccelerometerSel!=CISenTekAssistantDlg::G_NONE) 
			{
				fPitch = atan2(-( nG[0]), nG[1]*sin(fRoll)+nG[2]*cos(fRoll));
				fPitch = (1-g_pMain->m_fPitchAlpha)*fLastPitch+ g_pMain->m_fPitchAlpha*fPitch;
				float tmpLastRoll = fLastRoll;
				do {
					if(fRoll<0)
						fRoll += 2*PI;

					tmpLastRoll = fLastRoll;

					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}
					fRoll = (1-g_pMain->m_fRollAlpha)*tmpLastRoll+ g_pMain->m_fRollAlpha*fRoll;
				
					if(fRoll>2*PI)
						fRoll -= 2*PI;

					tmpLastRoll = fLastRoll;
					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}

					g_pMain->SimulationSubDlg.m_nABC = abs((int)(fRoll-tmpLastRoll));
					g_pMain->SimulationSubDlg.UpdateData(FALSE);
				} while(abs((int)(fRoll-tmpLastRoll))>(g_pMain->SimulationSubDlg.m_unDoubleFilterTh*PI/180));

				if(fRoll>2*PI)
					fRoll -= 2*PI;
				yaw = fPreviousYaw;
			}
		else
		 {
				
				fPitch = atan2(-( nG[0]), nG[1]*sin(fRoll)+nG[2]*cos(fRoll));
				fPitch = (1-g_pMain->m_fPitchAlpha)*fLastPitch+ g_pMain->m_fPitchAlpha*fPitch;
				float tmpLastRoll = fLastRoll;
				do {
					if(fRoll<0)
						fRoll += 2*PI;

					tmpLastRoll = fLastRoll;

					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}
					fRoll = (1-g_pMain->m_fRollAlpha)*tmpLastRoll+ g_pMain->m_fRollAlpha*fRoll;
				
					if(fRoll>2*PI)
						fRoll -= 2*PI;

					tmpLastRoll = fLastRoll;
					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}

					g_pMain->SimulationSubDlg.m_nABC = abs((int)(fRoll-tmpLastRoll));
					g_pMain->SimulationSubDlg.UpdateData(FALSE);
				} while(abs((int)(fRoll-tmpLastRoll))>(g_pMain->SimulationSubDlg.m_unDoubleFilterTh*PI/180));

				if(fRoll>2*PI)
					fRoll -= 2*PI;

				

				yaw = g_pMain->GetYawAngle((float)nM[0], (float)nM[1], (float)nM[2], fRoll, fPitch);
				
		   }

		}
		else if (tmpDist1<g_pMain->m_nMdTh[0] && tmpDist2>g_pMain->m_nMdTh[1] && tmpDist3>g_pMain->m_nMdTh[2]) 
		{
			fPitch = atan2(-( nG[0]), nG[1]*sin(fRoll)+nG[2]*cos(fRoll));
				fPitch = (1-g_pMain->m_fPitchAlpha)*fLastPitch+ g_pMain->m_fPitchAlpha*fPitch;
				float tmpLastRoll = fLastRoll;
				do {
					if(fRoll<0)
						fRoll += 2*PI;

					tmpLastRoll = fLastRoll;

					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}
					fRoll = (1-g_pMain->m_fRollAlpha)*tmpLastRoll+ g_pMain->m_fRollAlpha*fRoll;
				
					if(fRoll>2*PI)
						fRoll -= 2*PI;

					tmpLastRoll = fLastRoll;
					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}

					g_pMain->SimulationSubDlg.m_nABC = abs((int)(fRoll-tmpLastRoll));
					g_pMain->SimulationSubDlg.UpdateData(FALSE);
				} while(abs((int)(fRoll-tmpLastRoll))>(g_pMain->SimulationSubDlg.m_unDoubleFilterTh*PI/180));

				if(fRoll>2*PI)
					fRoll -= 2*PI;
		//	yaw = g_pMain->GetYawAngle((float)nM[0], (float)nM[1], (float)nM[2], fRoll, fPitch);
		    yaw= fPreviousYaw;
    } 
	else if (tmpDist1>g_pMain->m_nMdTh[0] && tmpDist2<g_pMain->m_nMdTh[1] && tmpDist3>g_pMain->m_nMdTh[2]) //pitch
	{
			float tmpLastRoll = fLastRoll;
				do {
					if(fRoll<0)
						fRoll += 2*PI;

					tmpLastRoll = fLastRoll;

					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}
					fRoll = (1-g_pMain->m_fRollAlpha)*tmpLastRoll+ g_pMain->m_fRollAlpha*fRoll;
				
					if(fRoll>2*PI)
						fRoll -= 2*PI;

					tmpLastRoll = fLastRoll;
					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}

					g_pMain->SimulationSubDlg.m_nABC = abs((int)(fRoll-tmpLastRoll));
					g_pMain->SimulationSubDlg.UpdateData(FALSE);
				} while(abs((int)(fRoll-tmpLastRoll))>(g_pMain->SimulationSubDlg.m_unDoubleFilterTh*PI/180));

				if(fRoll>2*PI)
					fRoll -= 2*PI;
		//	yaw = g_pMain->GetYawAngle((float)nM[0], (float)nM[1], (float)nM[2], fRoll, fPitch);
		    yaw= fPreviousYaw;
	}
	else
	{
			
				fPitch = atan2(-( nG[0]), nG[1]*sin(fRoll)+nG[2]*cos(fRoll));
				fPitch = (1-g_pMain->m_fPitchAlpha)*fLastPitch+ g_pMain->m_fPitchAlpha*fPitch;
			float tmpLastRoll = fLastRoll;
				do {
					if(fRoll<0)
						fRoll += 2*PI;

					tmpLastRoll = fLastRoll;

					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}
					fRoll = (1-g_pMain->m_fRollAlpha)*tmpLastRoll+ g_pMain->m_fRollAlpha*fRoll;
				
					if(fRoll>2*PI)
						fRoll -= 2*PI;

					tmpLastRoll = fLastRoll;
					if(abs((int)(fLastRoll-fRoll))>PI)
					{
						if(fLastRoll<fRoll)
							tmpLastRoll += 2*PI;
						else if (fRoll<fLastRoll)
							fRoll += 2*PI;
					}

					g_pMain->SimulationSubDlg.m_nABC = abs((int)(fRoll-tmpLastRoll));
					g_pMain->SimulationSubDlg.UpdateData(FALSE);
				} while(abs((int)(fRoll-tmpLastRoll))>(g_pMain->SimulationSubDlg.m_unDoubleFilterTh*PI/180));

				if(fRoll>2*PI)
					fRoll -= 2*PI;

			//fPitch = fLastPitch;
			yaw = fPreviousYaw;
	}
		
		switch (g_pMain->m_nAccelerometerSel)
		{
		case CISenTekAssistantDlg::G_NONE:
			break;
		case CISenTekAssistantDlg::G_KXTF9:
			fileIndex += sprintf( file_out+fileIndex, "%C, %d, %d, %d, %f, %f\r\n", 'G', nG[0], nG[1], nG[2], fRoll, fPitch); 
			break;	
		case CISenTekAssistantDlg::G_DMARD08:
			fileIndex += sprintf( file_out+fileIndex, "%C, %d, %d, %d, %f, %f\r\n", 'G', nG[0], nG[1], nG[2], fRoll, fPitch); 
			break;
		}
		
		nLastM[0]=nM[0];
		nLastM[1]=nM[1];
		nLastM[2]=nM[2];
		nLastG[0]=nG[0];
		nLastG[1]=nG[1];
		nLastG[2]=nG[2];
		fLastRoll  = fRoll;
		fLastPitch = fPitch;
		fPreviousYaw = yaw;

		if(g_pMain->m_nAccelerometerSel != CISenTekAssistantDlg::G_NONE) {
			g_pMain->SimulationSubDlg.m_fRoll = fRoll = fRoll*180/PI;
			g_pMain->SimulationSubDlg.m_fPitch = fPitch = fPitch*180/PI;
		} else {
			g_pMain->SimulationSubDlg.m_fRoll = 0.0f;
			g_pMain->SimulationSubDlg.m_fPitch = 0.0f;
		}

		fRoll = fRoll*PI/180;	
		fPitch = fPitch*PI/180;		
		yaw = (yaw*180/PI); 
		if(yaw<0)
			yaw += 360;
		
		if(yaw==360)
			yaw = 0;

		g_AngleData = (int)yaw;

		int tmpAngle = g_AngleData;
		if(abs((int)(lastAngle-g_AngleData))>180)
		{
			if(lastAngle<g_AngleData)
				lastAngle +=360;
			else if (g_AngleData<lastAngle)
				g_AngleData +=360;
		}
		g_AngleData = (1-g_pMain->m_fAngleAlpha)*lastAngle+ g_pMain->m_fAngleAlpha*g_AngleData;
		


		if(g_AngleData>360)
			g_AngleData%=360;
		
		
		g_pMain->SimulationSubDlg.m_AngleData = g_AngleData;
		g_pMain->SimulationSubDlg.UpdateData(FALSE);

		fileIndex += sprintf( file_out+fileIndex, "%C, %d, %d, %d, ,,%d\r\n", 'M', nM[0], nM[1], nM[2], g_AngleData/*(int)yaw*/); 

		lastAngle = g_AngleData;

		Sleep(g_pMain->SimulationSubDlg.m_DelayTime);
		g_pMain->m_iSenI2cMaster.IST8301_ClearInterruptSignal();
		

		try {
			file.Write(file_out, fileIndex);
			fileIndex = 0;
		} catch(CFileException* e) {
			e->ReportError();
			e->Delete();
			goto _ERROR;
		}

		if(g_pMain->bStopSimulation) {	
			goto _STOP;
		}
	}	

	goto _EXIT;

_ERROR:
	g_pMain->m_iSenI2cMaster.iSenCloseI2cMaster();
	AfxMessageBox(_T("Error Occurring "));
	goto _EXIT;

_STOP:

_EXIT:

	file.Close();
	g_pMain->SimulationSubDlg.m_SimulatingCtl.SetWindowText(_T("Simulation"));

	ExitThread(0);
}






//do {
					//if(g_AngleData<0)
						//g_AngleData +=360;

					//tmpLastg_AngleData = lastAngle;

	//				if(abs((int)(fPreviousYaw-yaw))>180)
		//			{
			//			if(fPreviousYaw<yaw)
				//			tmpLastg_AngleData +=360;
					//	else if (fPreviousYaw>yaw)
						//	g_AngleData += 360;
				//	}
			//		g_AngleData = (1-g_pMain->m_fAngleAlpha)*tmpLastg_AngleData+ g_pMain->m_fAngleAlpha*g_AngleData;
				
		//			if(g_AngleData>360)
						//g_AngleData -= 360;

					//tmpLastg_AngleData = fPreviousYaw;

					//if(abs((int)(fPreviousYaw-yaw))>180)
					//{
				//		if(fPreviousYaw<yaw)
			//				tmpLastg_AngleData +=360;
		//				else if (fPreviousYaw>yaw)
	//						g_AngleData += 360;
//					}

					//g_pMain->SimulationSubDlg.m_nABC = abs((int)(g_AngleData-tmpLastg_AngleData));
					//g_pMain->SimulationSubDlg.UpdateData(FALSE);
				//} while(abs((int)(yaw-tmpLastg_AngleData))>(50));