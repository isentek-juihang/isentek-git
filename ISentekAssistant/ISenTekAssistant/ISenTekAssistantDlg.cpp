
// ISenTekAssistantDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ISenTekAssistant.h"
#include "ISenTekAssistantDlg.h"

//#include "afxdialogex.h"
#include "Total.h"
#include <math.h>

#include "IST8301Def.h"
#include "IST8303Def.h"
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
float g_fOptimX(0.0f);
float g_fOptimY(0.0f);
float g_fOptimZ(0.0f);

int IST8301_I2CSLAVEADD = 0x0E;
int IST8303_I2CSLAVEADD = 0x0C;
int DMARD08_I2CSLAVEADD = 0x1C;

float exponential=2.718281828; 
#define FILE_BUFFER_SIZE 0xA0000
#define CURRENT_PATH _T(".\\")

static CISenTekAssistantDlg *g_pMain;

// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
    , m_FileVersion(_T(""))
    , m_iSenInterfaceVer(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_FILEVER, m_FileVersion);
    DDX_Text(pDX, IDC_STATIC_ISENINTERFACE_VER, m_iSenInterfaceVer);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    // TODO: Add extra initialization here
    CISenTekAssistantDlg *pParent = (CISenTekAssistantDlg *)GetParent();
    m_iSenInterfaceVer = pParent->m_iSenI2c.GetLibVer();
    UpdateData(FALSE);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

// CISenTekAssistantDlg dialog

CISenTekAssistantDlg::CISenTekAssistantDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CISenTekAssistantDlg::IDD, pParent) 
    , bStopSimulation(FALSE)
    , bAuto(FALSE)
    
    , m_bSwDebugMode(true)
    , m_nOffsetX(128)
    , m_nOffsetY(128)
    , m_nOffsetZ(128)
    , m_nOffsetDriftX(0)
    , m_nOffsetDriftY(0)
    , m_nOffsetDriftZ(0)
    , m_nInterruptThresholdValue(0)
    , m_nAccelerometerSel(G_KXTF9)
    , m_nMagnetometerSel(M_IST8303)

    , m_Q11(1)
    , m_Q12(0)
    , m_Q13(0)
    , m_Q21(0)
    , m_Q22(1)
    , m_Q23(0)
    , m_Q31(0)
    , m_Q32(0)
    , m_Q33(1)

    , m_R11(1)
    , m_R12(0)
    , m_R13(0)
    , m_R21(0)
    , m_R22(1)
    , m_R23(0)
    , m_R31(0)
    , m_R32(0)
    , m_R33(1)

    , m_G11(3)
    , m_G12(0)
    , m_G13(255)
    , m_G21(-1)
    , m_G22(-4)
    , m_G23(-263)
    , m_G31(-2)
    , m_G32(248)
    , m_G33(-7)
    , m_G41(5)
    , m_G42(-253)
    , m_G43(-4)
    , m_G51(265)
    , m_G52(0)
    , m_G53(-11)
    , m_G61(-256)
    , m_G62(-3)
    , m_G63(9)
    , m_bGsensorCalibrationSW(FALSE)
    , m_moving(1)
    , m_datanumber(25)
    , m_datarange(25)
    , m_hardstd(6.5)
    , m_softstd(5.5)
    , m_pitchlimit(90)
    , m_fUAlpha((int)400)
    , Gz(1000)
    , GzTh(10000.0f)
    , nDirection(0)
    , m_GchangeTh(20)
    , m_MradiusTh(1)
    , m_bFirG(FALSE)
    , m_bFirM(FALSE)
    
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_brush.CreateSolidBrush(RGB(50, 50, 80));

    m_iCurPage = -1;
    m_pages.RemoveAll();
    
    m_pStartPage = NULL;
    
    m_csTitle = "Untitled";

    m_bValidateBeforeChangingPage = false;

    AddPage(SimulationSubDlg,       "Demo");
    
    for(int i=0; i<3; i++)
        m_nMdTh[i] = 0;

//  SetValidateBeforeChangingPages(true);

    for(int i=0; i<nSensorsTotalAxisNum; i++ ) {
        pSMV[i] = NULL; 
    }
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
//  ON_WM_TIMER()
    ON_COMMAND(ID_USB_SETTING_AARDVARK, &CISenTekAssistantDlg::OnSettingUsbAardvark)
    ON_UPDATE_COMMAND_UI(ID_USB_SETTING_AARDVARK, &CISenTekAssistantDlg::OnUpdateSettingUsbAardvark)
    ON_COMMAND(ID_USB_SETTING_IOW, &CISenTekAssistantDlg::OnSettingUsbIow)
    ON_UPDATE_COMMAND_UI(ID_USB_SETTING_IOW, &CISenTekAssistantDlg::OnUpdateSettingUsbIow)  
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
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here
    g_pMain = this;

    m_iSenI2c.iSenOpenI2cAccess(CISenI2cAccess::nDefaultI2C);

    InitializeGUI();    

    //LoadCalibrationFile(&g_fOptimX, &g_fOptimY, &g_fOptimZ);
    if(m_bGsensorCalibrationSW)
        LoadCalibrationDataForG();

    for(int i=0; i<nSensorsTotalAxisNum; i++)
        pSMV[i] = new CSimpleMovingAverage(nMovingAverageOrder);

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

bool CISenTekAssistantDlg::LoadCalibrationDataForG(void)
{
    FILE *file;
    if ((file=fopen("goffset.csv","r"))==NULL) {
        m_G11 = m_G12 = m_G13 = 0.0f;
        m_G21 = m_G22 = m_G23 = 0.0f;
        m_G31 = m_G32 = m_G33 = 0.0f;
        m_G41 = m_G42 = m_G43 = 0.0f;
        m_G51 = m_G52 = m_G53 = 0.0f;
        m_G61 = m_G62 = m_G63 = 0.0f;
        return false;
    } else {
        fscanf(file, "%f,%f,%f", &m_G11, &m_G12, &m_G13);
        fscanf(file, "%f,%f,%f", &m_G21, &m_G22, &m_G23);
        fscanf(file, "%f,%f,%f", &m_G31, &m_G32, &m_G33);
        fscanf(file, "%f,%f,%f", &m_G41, &m_G42, &m_G43);
        fscanf(file, "%f,%f,%f", &m_G51, &m_G52, &m_G53);
        fscanf(file, "%f,%f,%f", &m_G61, &m_G62, &m_G63);
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
double CISenTekAssistantDlg::detrm(double a[10][10], int k) 
{ 
    double s=1, det=0;
    double b[10][10]; 
    int m, n; 
    
    if(k==1) { //
        return(a[0][0]); 
    } else { 
        det=0; 
        for(int c=0; c<k; c++) 
        { 
            m=0; 
            n=0; 
            for(int i=0; i<k; i++) 
            { 
                for(int j=0; j<k; j++) 
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
            det=det+s*(a[0][c]*g_pMain->detrm(b, k-1)); 
            s=-1*s; 
        } 
    } 
    return(det); 
} 



/*************FUNCTION TO FIND INVERSE OF A MATRIX**************************/ 

void CISenTekAssistantDlg::inv(double inv1[10][10], double num[10][10], int f) 
{ 
    double b[10][10], fac[10][10]; 
    int m,n; 
    double d;

    for(int q=0; q<f; q++) 
    { 
        for(int p=0; p<f; p++) 
        { 
            m=0; 
            n=0; 
            for(int i=0; i<f; i++) { 
                for(int j=0; j<f; j++) 
                { 
                    b[i][j]=0; 
                    if(i!=q&&j!=p) 
                    { 
                        b[m][n]=num[i][j]; 
                        if(n<(f-2)) 
                            n++; 
                        else { 
                            n=0; 
                            m++; 
                        } 
                    } 
                }            
            } 
            fac[q][p] = pow(-1, float(q+p)) * detrm(b,f-1); 
        } 
    } 
    for(int i=0; i<f; i++) 
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
  int i, j, k, m;
  double *pAk, *pAm, *p_r, *p_e;
  double threshold_norm;
  double threshold; 
  double tan_phi, sin_phi, cos_phi, tan2_phi, sin2_phi, cos2_phi;
  double sin_2phi, cos_2phi, cot_2phi;
  double dum1;
  double dum2;
  double dum3;
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
/*************FUNCTION FOR HARD IRON CALIBRATION**************************/ 
void  CISenTekAssistantDlg::HardCalibration(double center[3],float Y[90],double MM[90][11],double MMtran[11][90],int Stdnumber,int kk)
{
    double D[10][10];
    double invD[10][10];
    double invDXT[4][90];
    double invDXTY[4];
    for(int i=0;i<4;i++) 
    { 
        for(int j=0;j<4;j++) 
        {   
            D[i][j]=0; 
            for(int k=0;k<Stdnumber-kk;k++)
            {
                D[i][j]+=MMtran[i+6][k]*MM[k][j+6];
            }
        } 
    }

    g_pMain->inv(invD,D,(int) 4);   

    for(int i=0;i<4;i++) 
    { 
        for(int j=0;j<Stdnumber-kk;j++) 
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
        for(int k=0;k<Stdnumber-kk;k++)
        {
            invDXTY[i]+=invDXT[i][k]*Y[k];
        }    
    }

    center[0]=((float)0.5*invDXTY[0]);
    center[1]=((float)0.5*invDXTY[1]);
    center[2]=((float)0.5*invDXTY[2]);
}

/*************FUNCTION FOR SOFT IRON CALIBRATION**************************/
void  CISenTekAssistantDlg::SoftCalibration(double center[3],double Sc[3][3],double MM[90][11],double MMtran[11][90],int Stdnumber,int kk)
{
    double D[10][10];
    double invD[10][10];
    double D410410[10][10];
    double D13410[3][7];
    double D13410T[7][3];
    double invDD13410T[7][3];
    double D13410invDD13410T[3][3];
    double D1313minusD13410invDD13410T[3][3];
    double invc1D1313minusD13410invDD13410T[3][3];
    double C1[3][3]={{0,0,2},{0,-1,0},{2,0,0}};
    double C1inv[3][3]={{0,0,0.5},{0,-1,0},{0.5,0,0}};
    double eigenvectorsTT[1][3];
    double eigenvectorsTC1[1][3];
    double ratiok;
    double eigenvalues[3];
    double eigenvectors[3][3];
    double v2[7];
    double eigenvectorsT[3][3];
    for(int i=0;i<10;i++) 
    { 
        for(int j=0;j<10;j++) 
        {   
        D[i][j]=0; 
            for(int k=0;k<Stdnumber-kk;k++)
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
            for(int j=0;j<3;j++)
            {
                eigenvectors[j][2-i-1]=(eigenvectors[j][2-i]);
            }
        }
    }
            
    
    for (int i=0;i<3;i++)//20121119
    {
        eigenvectorsTT[0][i]=eigenvectors[i][0];
    }

    
    for (int i=0;i<3;i++)
    {
        eigenvectorsTC1[0][i]=0;
        for (int j=0;j<3;j++)
        {
            eigenvectorsTC1[0][i]+=eigenvectorsTT[0][j]*C1[j][i];
        }
    }
    
    ratiok=0;
    for (int j=0;j<3;j++)
    {
        ratiok+=eigenvectorsTC1[0][j]*eigenvectors[j][0];
    }
    ratiok=pow(ratiok,0.5);
    ratiok=1/ratiok;

    for (int i=0;i<3;i++)
    {
        eigenvectors[i][0]=ratiok*eigenvectors[i][0];
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
}


/*************FUNCTION FOR G-SENSOR CALIBRATION**************************/ 
void  CISenTekAssistantDlg::GsensorCalibration(double W[6][4], double Y[6][3], double X[4][3])
{
    double WT[4][6];
    double D[10][10];
    double invD[10][10];
    double invDWT[4][6];

    for (int i=0;i<6;i++)
    {
        for (int j=0;j<4;j++)
        {
        WT[j][i]=W[i][j];
        }
    }


    for(int i=0;i<4;i++) 
    { 
        for(int j=0;j<4;j++) 
        {   
            D[i][j]=0; 
            for(int k=0;k<6;k++)
            {
                D[i][j]+=WT[i][k]*W[k][j];
            }
        } 
    }

    g_pMain->inv(invD, D, (int) 4); 

    for(int i=0;i<4;i++) 
    { 
        for(int j=0;j<6;j++) 
        {   
            invDWT[i][j]=0;
            for(int k=0;k<4;k++)
            {
                invDWT[i][j]+=invD[i][k]*WT[k][j];
            }
        } 
    }

    for (int i=0;i<4;i++)
    {
        for(int j=0;j<3;j++) 
        {       
            X[i][j]=0;
            for(int k=0;k<6;k++)
            {
                X[i][j]+=invDWT[i][k]*Y[k][j];
            }    
        }
    }

}

/**********************SORT**********************************/
void CISenTekAssistantDlg::Sort(double MM[90][11],double MMtran[11][90],float Y[90],int Stdnumber,int kk)
{
    double MMtmp;
    float Ytmp;
    for(int i=0;i<Stdnumber-1-kk;i++)//sort
    {
        if (MM[i][10]>MM[(i+1)][10])
        {
            for(int j=0;j<11;j++)
            {
                Ytmp=Y[i+1];
                Y[i+1]=Y[i];
                Y[i]=Ytmp;
                MMtmp=MM[i+1][j];
                MMtran[j][i+1]=MM[i+1][j]=MM[i][j];
                MMtran[j][i]=MM[i][j]=MMtmp;
            }
        }
    }   
}

/*************QUATERNION MULTIPLY*************************/
void CISenTekAssistantDlg::quaternionmutiply(float q2[4], float q1[4], float qresults[4])
{
    qresults[0]=q2[0]*q1[0]-q2[1]*q1[1]-q2[2]*q1[2]-q2[3]*q1[3];
    qresults[1]=q2[0]*q1[1]+q2[1]*q1[0]+q2[2]*q1[3]-q2[3]*q1[2];
    qresults[2]=q2[0]*q1[2]-q2[1]*q1[3]+q2[2]*q1[0]+q2[3]*q1[1];
    qresults[3]=q2[0]*q1[3]+q2[1]*q1[2]-q2[2]*q1[1]+q2[3]*q1[0];
}

/*************ROTATION MATRIX**************************/ 
void CISenTekAssistantDlg::rot(float q[4], int data[3], float rotresult[3])
{

    float R[3][3];

    R[0][0]=pow(q[0],2)+pow(q[1],2)-pow(q[2],2)-pow(q[3],2);
    R[0][1]=2*(q[1]*q[2]+q[0]*q[3]);
    R[0][2]=2*(q[1]*q[3]-q[0]*q[2]);
    R[1][0]=2*(q[1]*q[2]-q[0]*q[3]);
    R[1][1]=pow(q[0],2)-pow(q[1],2)+pow(q[2],2)-pow(q[3],2);
    R[1][2]=2*(q[2]*q[3]+q[0]*q[1]);
    R[2][0]=2*(q[1]*q[3]+q[0]*q[2]);
    R[2][1]=2*(q[2]*q[3]-q[0]*q[1]);
    R[2][2]=pow(q[0],2)-pow(q[1],2)-pow(q[2],2)+pow(q[3],2);

    for(int i=0;i<3;i++)
    {
        rotresult[i]=0;
        for(int j=0;j<3;j++)
        {
            rotresult[i]+=R[i][j]*data[j];
        }
    }

}



void CISenTekAssistantDlg::kalmangain(float P[3][3], float H[3][3], float R[3][3], double L[3][3])
{
    float HT[3][3];
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
            HT[i][j]=H[j][i];
        }
    }

    float HTP[3][3];
    for (int i=0; i<3; i++)
    {
        for (int j=0;j<3;j++)
        {
            HTP[i][j]=0;
            for (int k=0;k<3;k++)
            {
                HTP[i][j]+=HT[i][k]*P[k][j];
            }
        }
    }

    float HTPH[3][3];
    for (int i=0; i<3; i++)
    {
        for (int j=0;j<3;j++)
        {
            HTPH[i][j]=0;
            for (int k=0;k<3;k++)
            {
                HTPH[i][j]+=HTP[i][k]*H[k][j];
            }
        }
    }

    double  HTPHaddR[10][10];
    for (int i=0; i<3; i++)
    {
        for (int j=0;j<3;j++)
        {
            HTPHaddR[i][j]=HTPH[i][j]+R[i][j];
        }
    }

    double invHTPHaddR[10][10];
    inv(invHTPHaddR,HTPHaddR,3);
    
    double HTinvHTPHaddR[3][3];
    for (int i=0; i<3;i++)
    {
        for (int j=0; j<3;j++)
        {
            HTinvHTPHaddR[i][j]=0;
            for (int k=0; k<3;k++)
            {
                HTinvHTPHaddR[i][j]+=HT[i][k]*invHTPHaddR[k][j];
            }
        }
    }

    for (int i=0; i<3;i++)
    {
        for (int j=0; j<3;j++)
        {
            L[i][j]=0;
            for (int k=0; k<3;k++)
            {
                L[i][j]+=P[i][k]*HTinvHTPHaddR[k][j];
            }
        }
    }
}


void CISenTekAssistantDlg::quaternionconjugate(float q[4], float qc[4])
{
    qc[0]=q[0];
    qc[1]=-q[1];
    qc[2]=-q[2];
    qc[3]=-q[3];
}

void CISenTekAssistantDlg::quaternionunit(float qdelta[4])
{
    float normq;
    normq=pow((pow(qdelta[0],(int)2)+pow(qdelta[1],(int)2)+pow(qdelta[2],(int)2)+pow(qdelta[3],(int)2)),(float)0.5);
    
    for (int i=0;i<4;i++)
    {
        qdelta[i]=qdelta[i]/normq;
    }
}

void CISenTekAssistantDlg::observationmatrix(float rotref[3], float H[3][3])
{
    H[0][0]=0;
    H[0][1]=2*rotref[2];
    H[0][2]=-2*rotref[1];
    H[1][0]=-2*rotref[2];
    H[1][1]=0;
    H[1][2]=2*rotref[0];
    H[2][0]=2*rotref[1];
    H[2][1]=-2*rotref[0];
    H[2][2]=0;
}

void CISenTekAssistantDlg::pseudoinverse(float H[3][3], double Hinverse[3][3])
{
    float HT[3][3];
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
        HT[i][j]=H[j][i];
        }
    }
    double HTH[10][10];
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
        HTH[i][j]=0;
            for (int k=0;k<3;k++)
            {
            HTH[i][j]+=HT[i][k]*H[k][j];
            }
        }
    }
double invHTH[10][10];
inv(invHTH,HTH,3);
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
        Hinverse[i][j]=0;
            for (int k=0;k<3;k++)
            {
            Hinverse[i][j]+=invHTH[i][k]*HT[k][j];
            }
        }
    }
}

void CISenTekAssistantDlg::pmatrixpriori(float P[3][3],float Q[3][3],float timeinterval)
{
float e2T=pow(exponential ,2*timeinterval); 

    for (int i=0; i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
        P[i][j]=P[i][j]+Q[i][j];
        }
    }



}

void CISenTekAssistantDlg::pmatrixupdate(float P[3][3],double L[3][3], float H[3][3], float I[3][3])
{
float LH[3][3];

    for (int i=0;i<3;i++)
    {
        for (int j=0; j<3;j++)
        {
        LH[i][j]=0;
            for (int k=0; k<3;k++)
            {
            LH[i][j]+=(float)L[i][k]*H[k][j];
            }
        }
    }


float IminusLH[3][3];
    for (int i=0;i<3; i++)
    {
        for (int j=0;j<3;j++)
        {
        IminusLH[i][j]=I[i][j]-LH[i][j];
        }
    }


float a[3][3];
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
        a[i][j]=0;
            for (int k=0;k<3;k++)
            {
            a[i][j]+=IminusLH[i][k]*P[k][j];
            }
        }
    }

    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
        P[i][j]=a[i][j];
        }
    }

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
    int nG[3] = {0};
    float ng[3] = {0},ngperdegree[3]={0};
    int ngg[3]={0};
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

    //bruce
    bool btimes1=false;
    float Radius=0;
    double Wg[6][4];
    double Yg[6][3]={{0,0,256},{0,0,-256},{0,256,0},{0,-256,0},{256,0,0},{-256,0,0}};
    double Xg[4][3];
    double nGc[3];
    float rn;
    float rth;
    int counttimes;
    int Filterorder=11;
    float M[11][3]={0};
    float G[11][3]={0};

    double FIRcoef[11]={0.067289146684595721,0.064280841406849323,0.085748954255491658,0.10402842120662813,0.11627053139181061 ,0.12057930773688116 ,0.11627053139181061 ,0.10402842120662813 ,0.085748954255491658,0.064280841406849323,0.067289146684595721}; //FS20FP.1FS2WP10;
    int hardorsoft;////hard=0,soft=1
    int hardorsoft1;
    int hardorsoft2;
    int hardorsoft3;
    int counttimes1;
    int kk;
    int threholdnumber=g_pMain-> m_datanumber;
    float r2;
    int Stdnumber=90;
    double MM[90][11]={0};
    double MMtran[11][90]={0};
    
    float Y[90];
    float MMM[90];
    float MMMtot;
    float MMMtot1;
    float MMMavg;
    float MMMstd;   
    double Sc[3][3];  //soft coefficient
    double Scold[3][3];
    int nmraw[3];
    double center[3];
    double centerold[3];
    float Mtotsc[3]={0};//soft iron calibration
    float Mtot[3];
    float Gtot[3];
    float Unit;

    float timeinterval;
    float I[3][3]={{1,0,0},{0,1,0},{0,0,1}};
    float Q[3][3],R[3][3];
    bool bRet = FALSE;

    Q[0][0]=g_pMain->m_Q11;
    Q[0][1]=g_pMain->m_Q12;
    Q[0][2]=g_pMain->m_Q13;
    Q[1][0]=g_pMain->m_Q21;
    Q[1][1]=g_pMain->m_Q22;
    Q[1][2]=g_pMain->m_Q23;
    Q[2][0]=g_pMain->m_Q31;
    Q[2][1]=g_pMain->m_Q32;
    Q[2][2]=g_pMain->m_Q33;

    R[0][0]=g_pMain->m_R11;
    R[0][1]=g_pMain->m_R12;
    R[0][2]=g_pMain->m_R13;
    R[1][0]=g_pMain->m_R21;
    R[1][1]=g_pMain->m_R22;
    R[1][2]=g_pMain->m_R23;
    R[2][0]=g_pMain->m_R31;
    R[2][1]=g_pMain->m_R32;
    R[2][2]=g_pMain->m_R33;

    if(g_pMain->m_bGsensorCalibrationSW) {
        Wg[0][0]=g_pMain->m_G11;
        Wg[0][1]=g_pMain->m_G12;
        Wg[0][2]=g_pMain->m_G13;
        Wg[0][3]=1;

        Wg[1][0]=g_pMain->m_G21;
        Wg[1][1]=g_pMain->m_G22;
        Wg[1][2]=g_pMain->m_G23;
        Wg[1][3]=1;

        Wg[2][0]=g_pMain->m_G31;
        Wg[2][1]=g_pMain->m_G32;
        Wg[2][2]=g_pMain->m_G33;
        Wg[2][3]=1;

        Wg[3][0]=g_pMain->m_G41;
        Wg[3][1]=g_pMain->m_G42;
        Wg[3][2]=g_pMain->m_G43;
        Wg[3][3]=1;

        Wg[4][0]=g_pMain->m_G51;
        Wg[4][1]=g_pMain->m_G52;
        Wg[4][2]=g_pMain->m_G53;
        Wg[4][3]=1;

        Wg[5][0]=g_pMain->m_G61;
        Wg[5][1]=g_pMain->m_G62;
        Wg[5][2]=g_pMain->m_G63;
        Wg[5][3]=1;
    }

    g_pMain->GsensorCalibration(Wg,Yg, Xg);

    CFile file(_T("Simulation.csv"), CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite);
    fileIndex = sprintf( file_out, "Mx, My, Mz, Gx, Gy, Gz, Roll, Pitch, Yaw\r\n"); 
    nODR = g_pMain->SimulationSubDlg.m_OdrCtl->GetCurSel();
    nPresetTime = 0;

    //Initialize M-Sensor
    switch (g_pMain->m_nMagnetometerSel)
    {
    case CISenTekAssistantDlg::M_IST8301:
        bRet = g_pMain->m_iSenI2c.IST8301_ChipsetInit(g_pMain->m_nOffsetX,
            g_pMain->m_nOffsetY,
            g_pMain->m_nOffsetZ,
            g_pMain->m_nOffsetDriftX,
            g_pMain->m_nOffsetDriftY,
            g_pMain->m_nOffsetDriftZ,
            nODR,
            nPresetTime,
            g_pMain->m_nInterruptThresholdValue );
        if (!bRet) {
            AfxMessageBox(_T("Initialize IST8301 fail"));
        }
        break;

    case CISenTekAssistantDlg::M_IST8303:
        bRet = g_pMain->m_iSenI2c.IST8303_ChipsetInit(6);
        if (!bRet) {
            AfxMessageBox(_T("Initialize IST8303 fail"));
        }
        break;
    }

    /*
    if(!g_pMain->m_iSenI2c.iSenSetI2cBitrate(g_pMain->m_fUAlpha))
    {
    goto _STOP;
    }
    */
    /*
    if((!g_pMain->m_iSenI2c.ITG3205_DLPF(g_pMain->m_MradiusTh))||(!g_pMain->m_iSenI2cMaster.ITG3205_Rate(g_pMain->m_GchangeTh ))||(!g_pMain->m_iSenI2cMaster.ITG3205_INTCON())) 
    {
    goto _STOP;
    }*/
    
    //Initialize G-Sensor
    switch (g_pMain->m_nAccelerometerSel)
    {
    case CISenTekAssistantDlg::G_NONE:
        break;

    case CISenTekAssistantDlg::G_KXTF9:
        if(!g_pMain->m_iSenI2c.KXTF9_Chipset_Init()) {
            AfxMessageBox(_T("Initialize KXTF9 fail"));
        }
        break;
    
    case CISenTekAssistantDlg::G_DMARD08:
        break;
    }
    timeinterval = (float)1/(1000/(g_pMain->m_GchangeTh+1));

    while(1)
    {
        bool candidate=true;
        bool Radiuseshow=true;
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
            if(g_pMain->m_iSenI2c.KXTF9_ReadSensorData(&nG[1], &nG[0], &nG[2])<0) {
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
            if(!g_pMain->m_iSenI2c.DMARD08_ReadSensorData(&nG[1], &nG[0], &nG[2])) {
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
        
        nG[0] = g_pMain->pSMV[3]->AddData(nG[0]);
        nG[1] = g_pMain->pSMV[4]->AddData(nG[1]);
        nG[2] = g_pMain->pSMV[5]->AddData(nG[2]);

        switch (g_pMain->m_nMagnetometerSel)
        {
            case CISenTekAssistantDlg::M_IST8301:
                while( !g_pMain->m_iSenI2c.IST8301_CheckDataReady() && !g_pMain->bStopSimulation)
                {
                    Sleep(1);
                }       
                
                nM[0] = g_pMain->m_iSenI2c.IST8301_GetXYZ(IST8301_DATAY)*-1;
                nM[1] = g_pMain->m_iSenI2c.IST8301_GetXYZ(IST8301_DATAX)*-1;                
                nM[2] = g_pMain->m_iSenI2c.IST8301_GetXYZ(IST8301_DATAZ)*-1;
                break;
            
            case CISenTekAssistantDlg::M_IST8303:
                while( !g_pMain->m_iSenI2c.IST8303_CheckDataReady() && !g_pMain->bStopSimulation)
                {
                    Sleep(1);
                }       
                
                nM[0] = g_pMain->m_iSenI2c.IST8303_GetXYZ(IST8303_DATAX);
                nM[1] = g_pMain->m_iSenI2c.IST8303_GetXYZ(IST8303_DATAY);              
                nM[2] = g_pMain->m_iSenI2c.IST8303_GetXYZ(IST8303_DATAZ)*-1;
                break;
        }

        nM[0] = g_pMain->pSMV[0]->AddData(nM[0]);
        nM[1] = g_pMain->pSMV[1]->AddData(nM[1]);
        nM[2] = g_pMain->pSMV[2]->AddData(nM[2]);

        // Auto calibration code by Bruce//
        // 2012/11/08 Release//
        //1. g-sensor calibration 
        //2. calibration detection
        //3. hard iron calibration
        //4. soft iron calibration (20121119)
        //5. tilt compensation
        //6. FIR filter
        //7. pitch limit
        //8. indicator limit 
        //9. auto calibration/read file 
        //10. sort (20121122)
        //11. magnetic field strength (20121122)


        nMRaw[0]=nM[0];  //raw data
        nMRaw[1]=nM[1];
        nMRaw[2]=nM[2];


         //g-sensor offset ignorance
        if(g_pMain->m_bGsensorCalibrationSW) {
            nGc[0] = nG[0]*Xg[0][0]+nG[1]*Xg[1][0]+nG[2]*Xg[2][0]+Xg[3][0];
            nGc[1] = nG[0]*Xg[0][1]+nG[1]*Xg[1][1]+nG[2]*Xg[2][1]+Xg[3][1];
            nGc[2] = nG[0]*Xg[0][2]+nG[1]*Xg[1][2]+nG[2]*Xg[2][2]+Xg[3][2];

            for (int i=0;i<3;i++)
            {
                nG[i]=(int)nGc[i];
            }
        }
        
        if (abs(nG[0])<8)
        {
            nG[0]=0;
        }
        if (abs(nG[1])<8)
        {
            nG[1]=0;
        }
        if (abs(nG[2])<8)
        {
            nG[2]=0;
        }
        
    
    
        if (g_pMain->bAuto)
        {
            btimes=false;
            /*************************INITIALIZATION*************************/ 
            if (btimes1==false)
            {
                hardorsoft2=1;
    
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
                kk=0;
                btimes1=true;
            }
            /*************************Soft or hard iron calibration****************************************/
            hardorsoft = g_pMain->SimulationSubDlg.m_OdrCtl1->GetCurSel();
            if (hardorsoft==0)
            {
                hardorsoft1=1;
            } else {
                hardorsoft1=-1;
            }
            hardorsoft3=hardorsoft1*hardorsoft2;
            /*************************CALIBRATION DETECTION*************************/ 
            if (hardorsoft3<1)
            {
                goto _HARDORSOFT;
            }
    
            r2=pow((pow((nMRaw[0]),(float)2)+pow((nMRaw[1]),(float)2)+pow((nMRaw[2]),(float)2)),(float)0.5); 
    
            if ((((abs((int)(nG[0]-nLastG[0]))<20)&&(abs((int)(nG[1]-nLastG[1]))<20)&&(abs((int)(nG[2]-nLastG[2]))<20))&&(nG[0]+nG[1]+nG[2]!=0)&&counttimes1!=0))  //////////////Choosing Candidates
            {
                if ((r2>500||r2<50)&&(counttimes1>(threholdnumber-1)))
                {
                    for (int i=0;i<3;i++)
                    {
                        center[i]=centerold[i];
                        for (int j=0;j<3;j++)
                        {
                            Sc[i][j]=Scold[i][j];
                        }
                    }
                    g_pMain->SimulationSubDlg.m_CalibrationIndication = _T("Magnetic field interruption");
                }
            } else if((r2>500||r2<50)&&((int)(nG[0]+nG[1]+nG[2])==0)&&counttimes1!=0) {
            
                for (int i=0;i<3;i++) {
                    center[i]=centerold[i];
                    for (int j=0;j<3;j++) {
                        Sc[i][j]=Scold[i][j];
                    }
                }
            
                g_pMain->SimulationSubDlg.m_CalibrationIndication = _T("Magnetic field interruption");
            } else {
            
                if(counttimes1>50)
                {
                    rth=g_pMain->m_datarange+20;
                } else {
                    rth=g_pMain->m_datarange;
                }


                for (int i=0;i<Stdnumber;i++)
                {
                    rn=pow((float)(pow((float)(nMRaw[0]-MM[i][6]),2)+pow((float)(nMRaw[1]-MM[i][7]),2)+pow((float)(nMRaw[2]-MM[i][8]),2)),(float).5);
            
                    if(rn<rth)
                    {
                        candidate=false;
                        break;
                    }       
                }
            
                if (candidate==true)//Candidates
                {
                    if (kk<1)
                    {
                        for(int i=1;i<Stdnumber;i++)
                        {
                            MMM[i-1]=MMM[i];
                            Y[i-1]=Y[i];
                            for(int j=0;j<10;j++)
                            {
                                MM[i-1][j] = MM[i][j];
                                MMtran[j][i-1] = MMtran[j][i];
                            }               
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
                    MM[Stdnumber-1][9] = MMtran[9][Stdnumber-1] = 1;
                    Y[Stdnumber-1] = MM[Stdnumber-1][0]+MM[Stdnumber-1][2]+MM[Stdnumber-1][5];
            
                    counttimes1++;
                    if(kk>0)
                    {
                        counttimes1-=1;
                    }
                    kk=0;

_HARDORSOFT: 

     /***********************HARD IRON CALIBRATION**************************/
                    if ((hardorsoft==0)&&((counttimes1==threholdnumber)||((counttimes1>threholdnumber)&&MMMstd>g_pMain->m_hardstd)||(hardorsoft3<0&&(counttimes1>threholdnumber))))
                    {           
                        g_pMain->HardCalibration(center,Y,MM,MMtran,Stdnumber,kk);
                
                        Sc[0][0]=Sc[1][1]=Sc[2][2]=1;
                        Sc[0][1]=Sc[0][2]=Sc[1][0]=Sc[1][2]=Sc[2][0]=Sc[2][1]=0;
                        Radiuseshow=false;
                    }
                
    /*******************SOFT IRON CALIBRATION*******************************/
                    if ((hardorsoft==1)&&((counttimes1==threholdnumber)||((counttimes1>threholdnumber)&&MMMstd>g_pMain->m_softstd)||(hardorsoft3<0&&(counttimes1>threholdnumber))))
                    {
                        g_pMain->SoftCalibration(center, Sc, MM, MMtran, Stdnumber, kk);
                        Radiuseshow=false;
                    }

    /*****************************************Standard deviation********************************************************/
                    if (counttimes1<Stdnumber)
                    {
                        MMMtot=0;
                        for (int i=0;i<counttimes1-kk;i++)
                        {
                            for(int j=0; j<3;j++)
                            {
                                nmraw[j]=0;
                                for (int k=0;k<3;k++)
                                {
                                    nmraw[j]+=Sc[j][k]*MM[Stdnumber-1-kk-i][k+6];
                                }
                            }
                            MMM[i]=pow((float)(pow((nmraw[0]-center[0]),2)+pow((nmraw[1]-center[1]),2)+pow((nmraw[2]-center[2]),2)),(float)0.5);    
                            MMMtot+=MMM[i];
                        }
                        MMMavg=MMMtot/(counttimes1-kk);
                        MMMtot1=0;
                        for(int i=0;i<(counttimes1-kk);i++)
                        {
                            MMtran[10][Stdnumber-1-kk-i]=MM[Stdnumber-1-kk-i][10]=pow((float)(MMM[i]-MMMavg),2); //(r[i]-ravg)^2
                            MMMtot1+=MM[Stdnumber-1-kk-i][10];
                        }
                        MMMstd=pow(MMMtot1/(counttimes1-kk),(float).5);
                
                        if(((MMMstd>g_pMain->m_hardstd&&hardorsoft==0)||(MMMstd>g_pMain->m_softstd&&hardorsoft==1))&&counttimes1>threholdnumber-1&&kk<1)
                        {
                            g_pMain->Sort(MM,MMtran,Y,Stdnumber,kk);
                            
                            kk++;
                            goto _HARDORSOFT;
                        }
                    /*
                        if (kk>0&&((MMMstd>g_pMain->m_hardstd&&hardorsoft==0)||(MMMstd>g_pMain->m_softstd&&hardorsoft==1))&&counttimes1>threholdnumber&&hardorsoft3>0)
                        {
                            for (int i=0;i<3;i++)
                            {
                            center[i]=centerold[i];
                                for (int j=0;j<3;j++)
                                {
                                Sc[i][j]=Scold[i][j];
                                }
                            }
                        }*/
                    } else  {
                        MMMtot=0;
                        for (int i=0;i<Stdnumber-kk;i++)
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
                        MMMavg=MMMtot/(Stdnumber-kk);
                        MMMtot1=0;

                        for(int i=0;i<(Stdnumber-kk);i++)
                        {
                            MMtran[10][i]=MM[i][10]=pow((float)(MMM[i]-MMMavg),2); //(r[i]-ravg)^2
                            MMMtot1+=MM[i][10];
                        }
                        MMMstd=pow(MMMtot1/(Stdnumber-kk),(float).5);
                

                        if(((MMMstd>g_pMain->m_hardstd&&hardorsoft==0)||(MMMstd>g_pMain->m_softstd&&hardorsoft==1))&&counttimes1>Stdnumber&&kk<1)
                        {
                        g_pMain->Sort(MM,MMtran,Y,Stdnumber,kk);
                        kk++;
                        goto _HARDORSOFT;
                        }
                    
                        /*if (kk>0&&((MMMstd>g_pMain->m_fGsenAlpha&&hardorsoft==0)||(MMMstd>g_pMain->m_fMsenAlpha&&hardorsoft==1))&&hardorsoft3>0)
                        {
                            for (int i=0;i<3;i++)
                            {
                            center[i]=centerold[i];
                                for (int j=0;j<3;j++)
                                {
                                Sc[i][j]=Scold[i][j];
                                }
                            }
                        }*/
                    }
                }                   
            }
        }

        else
        {
            btimes1=false;
        /*************************INITIALIZATION*************************/ 
            if (btimes==false)
            {
                MMMstd=0;
                MMMavg=0;
                center[0]=g_fOptimX;
                center[1]=g_fOptimY;
                center[2]=g_fOptimZ;

                hardorsoft2=1;
            
                for (int i=0;i<Stdnumber;i++)
                {
                    MMM[i]=0;   
                    Y[i]=0;
                    for (int j=0;j<11;j++)
                    {
                        MM[i][j]=0;
                        MMtran[j][i]=0;
                    }
                }
            
                MMMtot=0;
                counttimes=0;
                counttimes1=0;
                kk=0;
                Sc[0][0]=Sc[1][1]=Sc[2][2]=1;
                Sc[0][1]=Sc[0][2]=Sc[1][0]=Sc[1][2]=Sc[2][0]=Sc[2][1]=0;
            }
            btimes=true;
        }

        /********Magnetic field strength***********/
        if(Radiuseshow==false||g_pMain->bAuto==false)
        {
            Radius=MMMavg;
        }

        /*********************FIR Filter**************************/ 
        for(int i=0; i<3;i++)
        {
            Mtotsc[i]=0;
            for (int j=0;j<3;j++)
            {
                Mtotsc[i]+=Sc[i][j]*nMRaw[j];
                Scold[i][j]=Sc[i][j];
            }
        }

            
        for(int i=0;i<3;i++)//hard iron calibration
        {   
            nM[i]=Mtotsc[i]-center[i];
        }

        if (counttimes<Filterorder-1) //(是否前Filterorder次)  
        {   
            for(int i=0;i<3;i++)
            {
                M[counttimes][i] = nM[i];
                G[counttimes][i] = nG[i];            
            }
            counttimes++;
        } else {//(第Filterorder次之後)
            for (int i=0; i<3;i++)
            {
                M[Filterorder-1][i] = nM[i];
                G[Filterorder-1][i] = nG[i];
            }       
        }

        for(int i=0;i<3;i++)
        {
            Mtot[i]=0;
            Gtot[i]=0;
        }

        for(int i=0; i<Filterorder; i++)
        {           
            for(int j=0; j<3;j++)
            {
                if(g_pMain->m_bFirM)
                    Mtot[j]+=FIRcoef[i]*M[i][j];
                else
                    Mtot[j] = nM[j];

                if(g_pMain->m_bFirG)
                    Gtot[j]+=FIRcoef[i]*G[i][j];
                else
                    Gtot[j] = nG[j];
            }
        }
        
        for (int i=0; i<Filterorder-1; i++)
        {
            for(int j=0; j<3;j++)
            {
                if(g_pMain->m_bFirM)
                    M[i][j]=M[i+1][j];  
                if(g_pMain->m_bFirG)
                    G[i][j]=G[i+1][j];
            }
        }   

        /**********Heading**************/
        fRoll = atan2((float)Gtot[1], (float)Gtot[2]);
        fPitch = atan2(-( Gtot[0]), Gtot[1]*sin(fRoll)+Gtot[2]*cos(fRoll));
        yaw = g_pMain->GetYawAngle((float)Mtot[0], (float)Mtot[1], (float)Mtot[2], fRoll, fPitch);      
        


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
      /****************Pitch limit***************************/
        if (abs(fPitch/PI*180)>g_pMain->m_pitchlimit&&nG[2]<0)
        {
        yaw=lastAngle;
        }

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
        g_AngleData = (1-g_pMain->m_moving)*lastAngle+ g_pMain->m_moving*g_AngleData;

        if(g_AngleData>360)
            g_AngleData%=360;
        if(g_AngleData==360)
            g_AngleData=0;   
        
        /****************indicator stop moving trigger***************************/
        if ((abs((int)(nG[0]-nLastG[0]))<1)&&(abs((int)(nG[1]-nLastG[1]))<1)&&(abs((int)(nG[2]-nLastG[2]))<1)&&(nG[0]+nG[1]+nG[2]!=0))
        {
        g_AngleData=lastAngle;
        }

        /*******************UNIT****************************/
        if (g_pMain->SimulationSubDlg.m_unit)
        {
        Unit=(float)1/3;
        }
        else
        {
        Unit=1;
        }
        /*************************Simulation**********************/
        g_pMain->SimulationSubDlg.ChangeImage(g_AngleData,ndirection);
        g_pMain->SimulationSubDlg.m_AngleData = g_AngleData;
        

        g_pMain->SimulationSubDlg.m_nG[0] =(int) Gtot[0];
        g_pMain->SimulationSubDlg.m_nG[1] = (int)Gtot[1];
        g_pMain->SimulationSubDlg.m_nG[2] = (int)Gtot[2];       

        
        g_pMain->SimulationSubDlg.m_nM[0] = (int)(nMRaw[0]*Unit);
        g_pMain->SimulationSubDlg.m_nM[1] = (int)(nMRaw[1]*Unit);
        g_pMain->SimulationSubDlg.m_nM[2] = (int)(nMRaw[2]*Unit);

        g_pMain->SimulationSubDlg.m_nCalibratedM[0] =(int) (Mtot[0]*Unit);
        g_pMain->SimulationSubDlg.m_nCalibratedM[1] =(int) (Mtot[1]*Unit);
        g_pMain->SimulationSubDlg.m_nCalibratedM[2] =(int) (Mtot[2]*Unit);

        g_pMain->SimulationSubDlg.m_nV11=center[0]*Unit;
        g_pMain->SimulationSubDlg.m_nV21=center[1]*Unit;
        g_pMain->SimulationSubDlg.m_nU=center[2]*Unit;
        g_pMain->SimulationSubDlg.m_nV12=counttimes1;
        g_pMain->SimulationSubDlg.m_nV22=MMMstd*Unit;   
        g_pMain->SimulationSubDlg.m_radius=Radius*Unit;


        g_pMain->SimulationSubDlg.m_Sc11 = Sc[0][0];
        g_pMain->SimulationSubDlg.m_Sc12 = Sc[0][1];
        g_pMain->SimulationSubDlg.m_Sc13 = Sc[0][2];
        g_pMain->SimulationSubDlg.m_Sc21 = Sc[1][0];
        g_pMain->SimulationSubDlg.m_Sc22 = Sc[1][1];
        g_pMain->SimulationSubDlg.m_Sc23 = Sc[1][2];
        g_pMain->SimulationSubDlg.m_Sc31 = Sc[2][0];
        g_pMain->SimulationSubDlg.m_Sc32 = Sc[2][1];
        g_pMain->SimulationSubDlg.m_Sc33 = Sc[2][2];

        

        g_pMain->SimulationSubDlg.UpdateData(FALSE);
/************************Update************************************/
        nLastMRaw[0]=nMRaw[0];
        nLastMRaw[1]=nMRaw[1];
        nLastMRaw[2]=nMRaw[2];
        hardorsoft2=hardorsoft1;
        nLastG[0]=nG[0];
        nLastG[1]=nG[1];
        nLastG[2]=nG[2];
        lastAngle=g_AngleData;
        
        fLastRoll  = fRoll;
        fLastPitch = fPitch;
        fPreviousYaw = yaw;
        ndirection=0;
        
        for (int i=0;i<3;i++)
        {
        centerold[i]=center[i];
            for (int j=0;j<3;j++)
            {
            Scold[i][j]=Sc[i][j];
            }
        }
        
        fileIndex += sprintf( file_out+fileIndex, "%f, %f, %f, %f, %f, %f, %f, %f, %d\r\n", Mtot[0], Mtot[1], Mtot[2],  Gtot[0], Gtot[1], Gtot[2], fRoll*180/PI, fPitch*180/PI, g_AngleData);
        g_pMain->SimulationSubDlg.UpdateData(FALSE);

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
    g_pMain->m_iSenI2c.iSenCloseI2cAccess();
    AfxMessageBox(_T("Error Occurring "));
    goto _EXIT;

_STOP:

_EXIT:

    file.Close();
    g_pMain->SimulationSubDlg.m_SimulatingCtl.SetWindowText(_T("Simulation"));

    ExitThread(0);


}

        
