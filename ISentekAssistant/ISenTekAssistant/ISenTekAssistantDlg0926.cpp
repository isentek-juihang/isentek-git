
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
#include <float.h> 
#include <stdio.h> 
#include <conio.h>
#include <stdlib.h> 
#include <time.h> 
 


#define INT_PROCESS

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI SimulationProc(LPVOID lpParam);

int g_AngleData = 0;
int ndirection=0;
int g_LastAngleData = 0;
//float g_fOptimX(0.0f);
//float g_fOptimY(0.0f);
//float g_fOptimZ(0.0f);

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
	, m_fMsenAlpha(0.25f)
	, m_fUAlpha(0.25F)
	, Gz(1000)
	, GzTh(10000.0f)
	, nDirection(0)
//	, GzTh2(10000.0f)
	, m_GchangeTh(30)
	, m_MradiusTh(500)
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



/******************FUNCTION TO FIND THE DETERMINANT OF THE MATRIX************************/
double CISenTekAssistantDlg::detrm(double a[10][10],int k) 
{ 
double s=1,det=0;
double b[10][10]; 
int m,n,c; 
	if(k==1) 
	{ 
	return(a[0][0]); 
	} 
	else 
	{ 
	det=0; 
		for(int c=0;c<k;c++) 
		{ 
		m=0; 
		n=0; 
			for(int i=0;i<k;i++) 
			{ 
				for(int j=0;j<k;j++) 
				{ 
				b[i][j]=0; 
					if(i!=0&&j!=c) 
					{ 
					b[m][n]=a[i][j]; 
						if(n<(k-2)) 
						n++; 
						else 
						{ 
						n=0; 
						m++; 
						} 
					} 
				} 
			} 
		det=det+s*(a[0][c]*g_pMain->detrm(b,k-1)); 
		s=-1*s; 
		} 
	} 
return(det); 
} 



/*************FUNCTION TO FIND INVERSE OF A MATRIX**************************/ 

void CISenTekAssistantDlg::inv(double inv1[10][10],double num[10][10],int f) 
{ 
double b[10][10], fac[10][10]; 
int p,q,m,n; 
double d;

	for(int q=0;q<f;q++) 
	{ 
		for(int p=0;p<f;p++) 
		{ 
		m=0; 
		n=0; 
			for(int i=0;i<f;i++) 
			{ 
				for(int j=0;j<f;j++) 
				{ 
				b[i][j]=0; 
					if(i!=q&&j!=p) 
					{ 
					b[m][n]=num[i][j]; 
						if(n<(f-2)) 
						n++; 
						else 
						{ 
						n=0; 
						m++; 
						} 
					} 
				}			 
			} 
			fac[q][p]=pow(-1,float(q+p))*detrm(b,f-1); 
		} 
	} 
	for(int i=0;i<f;i++) 
	{ 
		for(int j=0;j<f;j++) 
		{ 
		b[i][j]=fac[j][i]; 
		} 
	} 

	d=g_pMain->detrm(num,f); 
	for(int i=0;i<f;i++) 
	{ 
		for(int j=0;j<f;j++) 
		{ 
		inv1[i][j]=b[i][j]/d; 
		} 
	} 
} 


/*************FUNCTION TO FIND EIGENVALUE AND EIGENVECTOR**************************/ 
void  CISenTekAssistantDlg::Jacobi_Cyclic_Method(double eigenvalues[], double *eigenvectors, double *A, int n)
{
  int row, i, j, k, m;
  double *pAk, *pAm, *p_r, *p_e;
  double threshold_norm;
  double threshold; 
  double tan_phi, sin_phi, cos_phi, tan2_phi, sin2_phi, cos2_phi;
  double sin_2phi, cos_2phi, cot_2phi;
  double dum1;
  double dum2;
  double dum3;
  double r;
  double max;

                  // Take care of trivial cases

   if ( n < 1) return;
   if ( n == 1) {
      eigenvalues[0] = *A;
      *eigenvectors = 1.0;
      return;
   }

          // Initialize the eigenvalues to the identity matrix.

   for (p_e = eigenvectors, i = 0; i < n; i++)
      for (j = 0; j < n; p_e++, j++)
         if (i == j) *p_e = 1.0; else *p_e = 0.0;
  
            // Calculate the threshold and threshold_norm.
 
   for (threshold = 0.0, pAk = A, i = 0; i < ( n - 1 ); pAk += n, i++) 
      for (j = i + 1; j < n; j++) threshold += *(pAk + j) * *(pAk + j);
   threshold = sqrt(threshold + threshold);
   threshold_norm = threshold * DBL_EPSILON;
   max = threshold + 1.0;
   while (threshold > threshold_norm) {
      threshold /= 10.0;
      if (max < threshold) continue;
      max = 0.0;
      for (pAk = A, k = 0; k < (n-1); pAk += n, k++) {
         for (pAm = pAk + n, m = k + 1; m < n; pAm += n, m++) {
            if ( fabs(*(pAk + m)) < threshold ) continue;

                 // Calculate the sin and cos of the rotation angle which
                 // annihilates A[k][m].

            cot_2phi = 0.5 * ( *(pAk + k) - *(pAm + m) ) / *(pAk + m);
            dum1 = sqrt( cot_2phi * cot_2phi + 1.0);
            if (cot_2phi < 0.0) dum1 = -dum1;
            tan_phi = -cot_2phi + dum1;
            tan2_phi = tan_phi * tan_phi;
            sin2_phi = tan2_phi / (1.0 + tan2_phi);
            cos2_phi = 1.0 - sin2_phi;
            sin_phi = sqrt(sin2_phi);
            if (tan_phi < 0.0) sin_phi = - sin_phi;
            cos_phi = sqrt(cos2_phi); 
            sin_2phi = 2.0 * sin_phi * cos_phi;
            cos_2phi = cos2_phi - sin2_phi;

                     // Rotate columns k and m for both the matrix A 
                     //     and the matrix of eigenvectors.

            p_r = A;
            dum1 = *(pAk + k);
            dum2 = *(pAm + m);
            dum3 = *(pAk + m);
            *(pAk + k) = dum1 * cos2_phi + dum2 * sin2_phi + dum3 * sin_2phi;
            *(pAm + m) = dum1 * sin2_phi + dum2 * cos2_phi - dum3 * sin_2phi;
            *(pAk + m) = 0.0;
            *(pAm + k) = 0.0;
            for (i = 0; i < n; p_r += n, i++) {
               if ( (i == k) || (i == m) ) continue;
               if ( i < k ) dum1 = *(p_r + k); else dum1 = *(pAk + i);
               if ( i < m ) dum2 = *(p_r + m); else dum2 = *(pAm + i);
               dum3 = dum1 * cos_phi + dum2 * sin_phi;
               if ( i < k ) *(p_r + k) = dum3; else *(pAk + i) = dum3;
               dum3 = - dum1 * sin_phi + dum2 * cos_phi;
               if ( i < m ) *(p_r + m) = dum3; else *(pAm + i) = dum3;
            }
            for (p_e = eigenvectors, i = 0; i < n; p_e += n, i++) {
               dum1 = *(p_e + k);
               dum2 = *(p_e + m);
               *(p_e + k) = dum1 * cos_phi + dum2 * sin_phi;
               *(p_e + m) = - dum1 * sin_phi + dum2 * cos_phi;
            }
         }
         for (i = 0; i < n; i++)
            if ( i == k ) continue;
            else if ( max < fabs(*(pAk + i))) max = fabs(*(pAk + i));
      }
   }
   for (pAk = A, k = 0; k < n; pAk += n, k++) eigenvalues[k] = *(pAk + k); 
}

/************************MAIN**************************/ 
DWORD WINAPI SimulationProc(LPVOID lpParam)
{
	CString tmpStr;

	int nODR = 2;
	int nPresetTime = 0;

	float yaw=0.0;
	static unsigned int lastAngle=0;
	int nLastG[3] = {0};
	//int nLastM[3] = {0};
	int nG[3] = {0};
	int nM[3] = {0};
	bool btimes=false;
	int nMRaw[3] = {0};
	int nLastMRaw[3] = {0};
	int nTrueCenter[3]={0};
	int nDelta[3]={0};
	float fRoll  =0.0f;
	float fPitch =0.0f;
	float fLastRoll  =0.0f;
	float fLastPitch =0.0f;
	float fPreviousYaw=0.0;
	char file_out[FILE_BUFFER_SIZE]={0};
	int fileIndex=0;	

	
	CFile file(_T("Simulation.csv"), CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite);

	fileIndex = sprintf( file_out, "MX Data, MY Data, MZ Data, GX Data, GY Data, GZ Data, Roll, Picth, yaw angle\r\n"); 

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

		//g_pMain->SimulationSubDlg.UpdateData(FALSE);		//Debug
		nMRaw[0]=nM[0];  //(without subtract center)
		nMRaw[1]=nM[1];
		nMRaw[2]=nM[2];

		nM[0] =nMRaw[0]- (int)g_fOptimX;
		nM[1] =nMRaw[1]- (int)g_fOptimY;
		nM[2] =nMRaw[2]- (int)g_fOptimZ;
		
		

#if 1

	
	g_pMain->SimulationSubDlg.m_CalibrationIndication = _T("");
  //bruce
	int counttimes;
	int Filterorder=6;
	float M[6][3];
	float G[6][3];
	float GG[6][3];
	int hardorsoft;////hard=0,soft=1
	int hardorsoft1;
	int hardorsoft2;
	int hardorsoft3;
	bool candidate=true;
	int counttimes1;
	int Stdnumber=40;
	int threholdnumber=20;
	float r2;
	float rdif;
	double MM[40][10];
	double MMtran[10][40];
	float Y[40];
	float MMM[40];
	float MMMtot;
	float MMMtot1;
	float MMMavg;
	float MMMstd;
	double center[3];
	double centerold[3];
	double D[10][10];
	double invD[10][10];
	double invDXT[4][30];
	double invDXTY[4];
	double D410410[10][10];
	double D13410[3][7];
	double D13410T[7][3];
	double invDD13410T[7][3];
	double D13410invDD13410T[3][3];
	double D1313minusD13410invDD13410T[3][3];
	double invc1D1313minusD13410invDD13410T[3][3];
	double C1inv[3][3]={{0,0,0.5},{0,-1,0},{0.5,0,0}};
	double eigenvalues[3];
	double eigenvectors[3][3];
	double v2[7];
	double eigenvectorsT[3][3];
	double Sc[3][3];  //soft coefficient
	double Scold[3][3];
	int nmraw[3];
	double FIRcoef[6]={0.14649366759134141,0.15089551483644295,0.18201244868413599,0.18201244868413599,0.15089551483644295,0.14649366759134141};///FS20FP1FS3WP1

		
	/*************************INITIALIZATION*************************/ 
	
	if (btimes==false)
	{
	hardorsoft2=1;
		for (int i=0;i<Filterorder;i++)
		{
			for(int j=0;j<3;j++)
			{
			M[i][j]=0;
			}
		}
		for (int i=0;i<Stdnumber;i++)
		{
		MMM[i]=0;	
		Y[i]=0;
			for (int j=0;j<10;j++)
			{
		    MM[i][j]=0;
			MMtran[j][i]=0;
			}
		}
			
	MMMtot=0;
	counttimes=0;
	counttimes1=0;
		for(int i=0;i<3;i++)
		{
		center[i]=0;
		}
    Sc[0][0]=Sc[1][1]=Sc[2][2]=1;
    Sc[0][1]=Sc[0][2]=Sc[1][0]=Sc[1][2]=Sc[2][0]=Sc[2][1]=0;
	}

	/*************************Soft or hard iron calibration****************************************/
	hardorsoft = g_pMain->SimulationSubDlg.m_OdrCtl1->GetCurSel();
	if (hardorsoft==0)
	{
		hardorsoft1=1;
	}
	else
	{
		hardorsoft1=-1;
	}
	hardorsoft3=hardorsoft1*hardorsoft2;
	/*************************CALIBRATION DETECTION*************************/ 


	r2=pow((float)(pow((nMRaw[0]-center[0]),2)+pow((nMRaw[1]-center[1]),2)+pow((nMRaw[2]-center[2]),2)),(float)0.5); 
	
		if ((r2>170)&&(counttimes1>(threholdnumber-1)))  //////////////Choosing Candidates
		{
			if ((r2>250)&&(counttimes1>(threholdnumber-1)))
			{
			g_pMain->SimulationSubDlg.m_CalibrationIndication = _T("Magnetic field interruption");
			}
		}
		else if((r2>500)&&(counttimes1<=(threholdnumber-1)))
		{
		g_pMain->SimulationSubDlg.m_CalibrationIndication = _T("Magnetic field interruption");
		}
		else
		{
			for (int i=0;i<Stdnumber;i++)
			{
			float rn=pow((float)(pow((float)(nMRaw[0]-MM[i][6]),2)+pow((float)(nMRaw[1]-MM[i][7]),2)+pow((float)(nMRaw[2]-MM[i][8]),2)),(float).5);
	        
				if(rn<30)
				{
				candidate=false;
				break;
				}		
			}
			
		    if (candidate==true)//Candidates
			{
				if ((counttimes1<Stdnumber)) //(是否前Stdnumber-1次)  
				{
				MM[counttimes1][0]=MMtran[0][counttimes1]=pow((nMRaw[0]),(float)2);
				MM[counttimes1][1]=MMtran[1][counttimes1]=(nMRaw[0]*nMRaw[1]);
				MM[counttimes1][2]=MMtran[2][counttimes1]=pow((nMRaw[1]),(float)2);
				MM[counttimes1][3]=MMtran[3][counttimes1]=(nMRaw[0]*nMRaw[2]);
				MM[counttimes1][4]=MMtran[4][counttimes1]=(nMRaw[1]*nMRaw[2]);
				MM[counttimes1][5]=MMtran[5][counttimes1]=pow((nMRaw[2]),(float)2);
					for(int i=0;i<3;i++)
					{
					MM[counttimes1][i+6]=MMtran[i+6][counttimes1]=nMRaw[i];
					}
				MM[counttimes1][9]=MMtran[9][counttimes1]=MMtran[9][counttimes1]=1;
				Y[counttimes1]=MM[counttimes1][0]+MM[counttimes1][2]+MM[counttimes1][5];
				}				
				else
				{
					for(int i=1;i<Stdnumber;i++)
					{
					MMM[i-1]=MMM[i];
					Y[i-1]=Y[i];
						for(int j=0;j<10;j++)
						{
						MM[i-1][j]=MM[i][j];
						MMtran[j][i-1]=MMtran[j][i];
						}				
					}
				MM[Stdnumber-1][0]=MMtran[0][Stdnumber-1]=pow((nMRaw[0]),(float)2);
				MM[Stdnumber-1][1]=MMtran[1][Stdnumber-1]=(nMRaw[0]*nMRaw[1]);
				MM[Stdnumber-1][2]=MMtran[2][Stdnumber-1]=pow((nMRaw[1]),(float)2);
				MM[Stdnumber-1][3]=MMtran[3][Stdnumber-1]=(nMRaw[0]*nMRaw[2]);
				MM[Stdnumber-1][4]=MMtran[4][Stdnumber-1]=(nMRaw[1]*nMRaw[2]);
				MM[Stdnumber-1][5]=MMtran[5][Stdnumber-1]=pow((nMRaw[2]),(float)2);
					for(int i=0;i<3;i++)
					{
					MM[Stdnumber-1][i+6]=MMtran[i+6][Stdnumber-1]=nMRaw[i];
					}
				MM[Stdnumber-1][9]=MMtran[9][Stdnumber-1]=1;
				Y[Stdnumber-1]=MM[Stdnumber-1][0]+MM[Stdnumber-1][2]+MM[Stdnumber-1][5];
				}
			counttimes1++;

 /***********************HARD IRON CALIBRATION**************************/
		        if ((hardorsoft==0)&&(((counttimes1>threholdnumber)&&((MMMstd>10)||(hardorsoft3<0)))||(counttimes1==threholdnumber)))
		        {			
					for(int i=0;i<4;i++) 
					{ 
						for(int j=0;j<4;j++) 
						{ 	
				         D[i][j]=0; 
							for(int k=0;k<Stdnumber;k++)
							{
							D[i][j]+=MMtran[i+6][k]*MM[k][j+6];
							}
						} 
					}
				g_pMain->inv(invD,D,(int) 4);		
					for(int i=0;i<4;i++) 
					{ 
						for(int j=0;j<Stdnumber;j++) 
						{ 	
						invDXT[i][j]=0;
							for(int k=0;k<4;k++)
							{
							invDXT[i][j]+=invD[i][k]*MMtran[k+6][j];
							}
						} 
					}
		
					for(int i=0;i<4;i++) 
					{ 		
					invDXTY[i]=0;
						for(int k=0;k<Stdnumber;k++)
						{
						invDXTY[i]+=invDXT[i][k]*Y[k];
						}	 
					}
					center[0]=((float)0.5*invDXTY[0]);
					center[1]=((float)0.5*invDXTY[1]);
					center[2]=((float)0.5*invDXTY[2]);	
	
					if (counttimes1<Stdnumber)/////standard deviation
					{
					MMMtot=0;
						for (int i=0;i<counttimes1;i++)
						{
						MMM[i]=pow((float)(pow((MM[i][6]-center[0]),2)+pow((MM[i][7]-center[1]),2)+pow((MM[i][8]-center[2]),2)),(float)0.5);    
						MMMtot+=MMM[i];
						}
					MMMavg=MMMtot/(counttimes1);
					MMMtot1=0;
						for(int i=0;i<(counttimes1);i++)
						{
						MMMtot1+=pow((float)(MMM[i]-MMMavg),2);
						}
					MMMstd=pow(MMMtot1/(counttimes1),(float).5);
					}
					else
					{
					MMMtot=0;
						for (int i=0;i<Stdnumber;i++)
						{
						MMM[i]=pow((float)(pow((MM[i][6]-center[0]),2)+pow((MM[i][7]-center[1]),2)+pow((MM[i][8]-center[2]),2)),(float)0.5);    
						MMMtot+=MMM[i];
						}
					MMMavg=MMMtot/(Stdnumber);
					MMMtot1=0;
						for(int i=0;i<(Stdnumber);i++)
						{
						MMMtot1+=pow((float)(MMM[i]-MMMavg),2);
						}
					MMMstd=pow(MMMtot1/(Stdnumber),(float).5);
					}
					
				Sc[0][0]=Sc[1][1]=Sc[2][2]=1;
				Sc[0][1]=Sc[0][2]=Sc[1][0]=Sc[1][2]=Sc[2][0]=Sc[2][1]=0;
				}
/*******************SOFT IRON CALIBRATION*******************************/
				else if ((hardorsoft==1)&&(((counttimes1>threholdnumber)&&((MMMstd>10)||(hardorsoft3<0)))||(counttimes1==threholdnumber)))
				{
					for(int i=0;i<10;i++) 
					{ 
						for(int j=0;j<10;j++) 
						{ 	
					       D[i][j]=0; 
							for(int k=0;k<Stdnumber;k++)
							{
							D[i][j]+=MMtran[i][k]*MM[k][j];
							}
						} 
					}
					
					for(int i=0;i<7;i++)
					{
						for(int j=0;j<7;j++)
						{
						D410410[i][j]=D[i+3][j+3];
						}
					}
				 
					g_pMain->inv(invD,D410410,(int) 7);
			
				
					for(int i=0;i<3;i++)
					{
						for(int j=0;j<7;j++)
						{
						D13410T[j][i]=D13410[i][j]=D[i][j+3];
						}
					}
			       
					for(int i=0;i<7;i++)
					{
						for (int j=0;j<3;j++)
						{
						invDD13410T[i][j]=0;
							for (int k=0;k<7;k++)
							{
							invDD13410T[i][j]+=invD[i][k]*D13410T[k][j];
							}
						}
					}
                     

					for(int i=0;i<3;i++)
					{
						for (int j=0;j<3;j++)
						{
						D13410invDD13410T[i][j]=0;
							for (int k=0;k<7;k++)
							{
							D13410invDD13410T[i][j]+=D13410[i][k]*invDD13410T[k][j];
							}
						}
					}
                    
					for(int i=0;i<3;i++)
					{
						for (int j=0;j<3;j++)
						{
						D1313minusD13410invDD13410T[i][j]=D[i][j]-D13410invDD13410T[i][j];
						}
					}
                    
				
					for(int i=0;i<3;i++)
					{
						for (int j=0;j<3;j++)
						{
						invc1D1313minusD13410invDD13410T[i][j]=0;
							for (int k=0;k<3;k++)
							{
							invc1D1313minusD13410invDD13410T[i][j]+=C1inv[i][k]*D1313minusD13410invDD13410T[k][j];
							}
						}
					}

					
				g_pMain->Jacobi_Cyclic_Method(eigenvalues, (double*)eigenvectors,(double*) invc1D1313minusD13410invDD13410T, (int)3);
					

					for (int i=0;i<2;i++)//sort
					{
						if (eigenvalues[2-i]>eigenvalues[2-i-1])
						{
						eigenvalues[2-i-1]=eigenvalues[2-i];
							for(int j=0;j<2;j++)
							{
							eigenvectors[j][2-i-1]=eigenvectors[j][2-i];
							}
						}
					}

					
			
					for (int i=0;i<7;i++)
					{
					v2[i]=0;
						for (int j=0;j<3;j++)
						{
						v2[i]+=-invDD13410T[i][j]*eigenvectors[j][0];
						}
					}

					if (v2[6]>0)
					{
						for (int i=0;i<3;i++)
						{
						eigenvectors[i][0]=-eigenvectors[i][0];
						}
						for (int i=0; i<7;i++)
						{
						v2[i]=-v2[i];
						}
					}

	
				D[0][0]=2*eigenvectors[0][0]; //AA
				D[0][1]=eigenvectors[1][0];
				D[0][2]=v2[0];
				D[1][0]=eigenvectors[1][0];
				D[1][1]=2*eigenvectors[2][0];
				D[1][2]=v2[1];
				D[2][0]=v2[0];
				D[2][1]=v2[1];
				D[2][2]=2*v2[2];


				g_pMain->inv(invD,D,(int) 3); //invAA


					for (int i=0;i<3;i++)
					{
						for (int j=0;j<3;j++)
						{
						center[i]=0;
							for (int k=0;k<3;k++)
							{
							center[i]+=(invD[i][k]*-v2[k+3]);
							}
						}
					}
					
					for (int i=0;i<3;i++)
					{
						for (int j=0;j<3;j++)
						{
						D13410invDD13410T[i][j]=D[i][j];
						}
					}
                
				g_pMain->Jacobi_Cyclic_Method(eigenvalues, (double*)eigenvectors,(double*)D13410invDD13410T, 3);
						
					for (int i=0;i<3;i++)
					{
						if (eigenvalues[i]<0)
						{
						eigenvalues[i]=-eigenvalues[i];
						}
						eigenvalues[i]=pow(eigenvalues[i],(double).5);
					}

					
					for (int i=0;i<3;i++)
					{
						for (int j=0;j<3;j++)
						{
						eigenvectorsT[i][j]=eigenvectors[j][i];
						}
					}
					
		

				/*
				eigvecAA*[eigvalAA(1,1)^.5 0 0;
                        0 eigvalAA(2,2)^.5 0;
                         0 0 eigvalAA(3,3)^.5]
				*/
					for (int i=0;i<3;i++)
					{
						for (int j=0;j<3;j++)
						{
					     D13410invDD13410T[i][j]=eigenvectors[i][j]*eigenvalues[j];
						}
					}
			/***********************************************************************/
					for(int i=0;i<3;i++)
					{
						for (int j=0;j<3;j++)
						{
						Sc[i][j]=0;
							for (int k=0;k<3;k++)
							{
							 Sc[i][j]+=D13410invDD13410T[i][k]*eigenvectorsT[k][j];
							}
						}
					}
					if (counttimes1<Stdnumber)/////standard deviation
					{
					MMMtot=0;
						for (int i=0;i<counttimes1;i++)
						{
							for(int j=0; j<3;j++)
							{
							nmraw[j]=0;
								for (int k=0;k<3;k++)
								{
								nmraw[j]+=Sc[j][k]*MM[i][k+6];
								}
							}
						MMM[i]=pow((float)(pow((nmraw[0]-center[0]),2)+pow((nmraw[1]-center[1]),2)+pow((nmraw[2]-center[2]),2)),(float)0.5);    
						MMMtot+=MMM[i];
						}
					MMMavg=MMMtot/(counttimes1);
					MMMtot1=0;
						for(int i=0;i<(counttimes1);i++)
						{
						MMMtot1+=pow((float)(MMM[i]-MMMavg),2);
						}
					MMMstd=pow(MMMtot1/(counttimes1),(float).5);
					}
					else
					{
					MMMtot=0;
						for (int i=0;i<Stdnumber;i++)
						{
							for(int j=0; j<3;j++)
							{
							nmraw[j]=0;
								for (int k=0;k<3;k++)
								{
								nmraw[j]+=Sc[j][k]*MM[i][k+6];
								}
							}
						MMM[i]=pow((float)(pow((nmraw[0]-center[0]),2)+pow((nmraw[1]-center[1]),2)+pow((nmraw[2]-center[2]),2)),(float)0.5);    
						MMMtot+=MMM[i];
						}
					MMMavg=MMMtot/Stdnumber;
					MMMtot1=0;
						for(int i=0;i<(Stdnumber);i++)
						{
						MMMtot1+=pow((float)(MMM[i]-MMMavg),2);
						}
					MMMstd=pow(MMMtot1/(Stdnumber),(float).5);
					}
 				}
				else  //<29
				{
					if (counttimes1<Stdnumber)/////standard deviation
					{
					MMMtot=0;
						for (int i=0;i<counttimes1;i++)
						{
							for(int j=0; j<3;j++)
							{
							nmraw[j]=0;
								for (int k=0;k<3;k++)
								{
								nmraw[j]+=Sc[j][k]*MM[i][k+6];
								}
							}
						MMM[i]=pow((float)(pow((nmraw[0]-center[0]),2)+pow((nmraw[1]-center[1]),2)+pow((nmraw[2]-center[2]),2)),(float)0.5);    
						MMMtot+=MMM[i];
						}
					MMMavg=MMMtot/(counttimes1);
					MMMtot1=0;
						for(int i=0;i<(counttimes1);i++)
						{
						MMMtot1+=pow((float)(MMM[i]-MMMavg),2);
						}
					MMMstd=pow(MMMtot1/(counttimes1),(float).5);
					}
					else
					{
					MMMtot=0;
						for (int i=0;i<Stdnumber;i++)
						{
							for(int j=0; j<3;j++)
							{
							nmraw[j]=0;
								for (int k=0;k<3;k++)
								{
								nmraw[j]+=Sc[j][k]*MM[i][k+6];
								}
							}
						MMM[i]=pow((float)(pow((nmraw[0]-center[0]),2)+pow((nmraw[1]-center[1]),2)+pow((nmraw[2]-center[2]),2)),(float)0.5);    
						MMMtot+=MMM[i];
						}
					MMMavg=MMMtot/Stdnumber;
					MMMtot1=0;
						for(int i=0;i<(Stdnumber);i++)
						{
						MMMtot1+=pow((float)(MMM[i]-MMMavg),2);
						}
					MMMstd=pow(MMMtot1/(Stdnumber),(float).5);
					}
				}
			}					
		}



		/*********************FIR Filter**************************/ 

		
		if (counttimes<Filterorder-1) //(是否前Filterorder次)  
		{	
			for(int i=0;i<3;i++)
			{
			M[counttimes][i]=nMRaw[i];
			G[counttimes][i]=nG[i];			
			}
			counttimes++;
		}
		
		else //(第Filterorder次之後)
		{
			for (int i=0; i<3;i++)
			{
			M[Filterorder-1][i]=nMRaw[i];
			G[Filterorder-1][i]=nG[i];
			}		
		
		float Mtot[3]={0};
		float Gtot[3]={0};
			for(int i=0; i<Filterorder; i++)
			{
				for(int j=0; j<3;j++)
				{
				Mtot[j]+=FIRcoef[i]*M[i][j];
				Gtot[j]+=FIRcoef[i]*G[i][j];
				}
			}
			
			//judge results
			
			if(Sc[0][0]>1.5||Sc[0][0]<0.5||Sc[1][1]>1.5||Sc[1][1]<0.5||Sc[2][2]>1.5||Sc[2][2]<0.5)
			{
				for(int i=0; i<3;i++)
				{
				center[i]=centerold[i];
					for (int j=0;j<3;j++)
					{
					Sc[i][j]=Scold[i][j];
					}
				}
			}
			
			float Mtotsc[3]={0};//soft iron calibration
			for(int i=0; i<3;i++)
			{
				for (int j=0;j<3;j++)
				{
				Mtotsc[i]+=Sc[i][j]*Mtot[j];
				Scold[i][j]=Sc[i][j];
				}
			}


			for(int i=0;i<3;i++)//hard iron calibration
			{	
				nM[i]=Mtotsc[i]-center[i];
				nG[i]=Gtot[i];
				centerold[i]=center[i];
			}


			for (int i=0; i<Filterorder-1;i++)
			{
				for(int j=0; j<3;j++)
				{
				M[i][j]=M[i+1][j];			
				G[i][j]=G[i+1][j];
				}
			}	
		}
		
		

#endif

		/**********Heading**************/
		fRoll = atan2((float)nG[1], (float)nG[2]);
		fPitch = atan2(-( nG[0]), nG[1]*sin(fRoll)+nG[2]*cos(fRoll));
		yaw = g_pMain->GetYawAngle((float)nM[0], (float)nM[1], (float)nM[2], fRoll, fPitch);		
		


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

		/*******************moving filter***********************/
		if(yaw<0)
			yaw += 360;
		
		if(yaw==360)
			yaw = 0;

		g_AngleData = (int)yaw;
		int lastAngle1=lastAngle;
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
		if(g_AngleData==360)
			g_AngleData=0;   
		
		/****************indicator stop moving trigger***************************/
		if ((abs((int)(nG[0]-nLastG[0]))<2)&&(abs((int)(nG[1]-nLastG[1]))<2)&&(abs((int)(nG[2]-nLastG[2]))<2)&&(nG[0]+nG[1]+nG[2]!=0))
		{
        g_AngleData=lastAngle;
		}
		/*************************Simulation**********************/
		g_pMain->SimulationSubDlg.ChangeImage(g_AngleData,ndirection);
		g_pMain->SimulationSubDlg.m_AngleData = g_AngleData;
		g_pMain->SimulationSubDlg.UpdateData(FALSE);

		g_pMain->SimulationSubDlg.m_nG[0] = nG[0];
		g_pMain->SimulationSubDlg.m_nG[1] = nG[1];
		g_pMain->SimulationSubDlg.m_nG[2] = nG[2];		

		g_pMain->SimulationSubDlg.m_nCalibratedM[0] = nM[0];
		g_pMain->SimulationSubDlg.m_nCalibratedM[1] = nM[1];
		g_pMain->SimulationSubDlg.m_nCalibratedM[2] = nM[2];

		g_pMain->SimulationSubDlg.m_nV11=center[0];
		g_pMain->SimulationSubDlg.m_nV21=center[1];
		g_pMain->SimulationSubDlg.m_nU=center[2];
		g_pMain->SimulationSubDlg.m_nV12=counttimes1;
		g_pMain->SimulationSubDlg.m_nV22=MMMstd;	

		g_pMain->SimulationSubDlg.m_Sc11 = Sc[0][0];
		g_pMain->SimulationSubDlg.m_Sc12 = Sc[0][1];
		g_pMain->SimulationSubDlg.m_Sc13 = Sc[0][2];
		g_pMain->SimulationSubDlg.m_Sc21 = Sc[1][0];
		g_pMain->SimulationSubDlg.m_Sc22 = Sc[1][1];
		g_pMain->SimulationSubDlg.m_Sc23 = Sc[1][2];
		g_pMain->SimulationSubDlg.m_Sc31 = Sc[2][0];
		g_pMain->SimulationSubDlg.m_Sc32 = Sc[2][1];
		g_pMain->SimulationSubDlg.m_Sc33 = Sc[2][2];

		


/************************Update************************************/
		nLastMRaw[0]=nMRaw[0];
		nLastMRaw[1]=nMRaw[1];
		nLastMRaw[2]=nMRaw[2];
		hardorsoft2=hardorsoft1;
		nLastG[0]=nG[0];
		nLastG[1]=nG[1];
		nLastG[2]=nG[2];
		lastAngle=g_AngleData;
		btimes=true;
		fLastRoll  = fRoll;
		fLastPitch = fPitch;
		fPreviousYaw = yaw;
		ndirection=0;



		
#if 0
		switch (g_pMain->m_nAccelerometerSel)
		{
		case CISenTekAssistantDlg::G_NONE:
			break;
		case CISenTekAssistantDlg::G_KXTF9:
			fileIndex += sprintf( file_out+fileIndex, "%C, %d, %d, %d, %f, %f\r\n", 'G', nG[0], nG[1], nG[2], fRoll*180/PI, fPitch*180/PI); 
			break;	
		case CISenTekAssistantDlg::G_DMARD08:
			fileIndex += sprintf( file_out+fileIndex, "%C, %d, %d, %d, %f, %f\r\n", 'G', nG[0], nG[1], nG[2], fRoll*180/PI, fPitch*180/PI); 
			break;
		}
		fileIndex += sprintf( file_out+fileIndex, "%C, %d, %d, %d, ,,%d\r\n", 'M', nM[0], nM[1], nM[2], g_AngleData/*(int)yaw*/); 
#else
		fileIndex += sprintf( file_out+fileIndex, "%d, %d, %d, %d, %d, %d, %f, %f, %d\r\n", nM[0], nM[1], nM[2],  nG[0], nG[1], nG[2], fRoll*180/PI, fPitch*180/PI, g_AngleData); 
#endif
		//g_pMain->SimulationSubDlg.m_fLastAngle = g_LastAngleData = lastAngle = g_AngleData;
		g_pMain->SimulationSubDlg.UpdateData(FALSE);
		//Sleep(g_pMain->SimulationSubDlg.m_DelayTime);
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



