// SubDlgSimulation.cpp : 實作檔
//

#include "stdafx.h"
#include "ISenTekAssistant.h"
#include "SubDlgSimulation.h"
#include "afxdialogex.h"
#include "isenteklib.h"

extern DWORD WINAPI SimulationProc(LPVOID lpParam);

DWORD WINAPI CompassGuiProc(LPVOID lpParam);
DWORD WINAPI LoadCompassPicProc(LPVOID lpParam);

extern float g_fOptimX;
extern float g_fOptimY;
extern float g_fOptimZ;
extern int g_AngleData;
extern int ndirection;
extern int g_LastAngleData;
extern int lastg_AngleData;


#define UPDATE_REGION_TIMER 3
#define RESTART_SIM_TIMER 4

static CSubDlgSimulation *g_pSubDlgSim;

// CSubDlgSimulation 對話方塊

IMPLEMENT_DYNAMIC(CSubDlgSimulation, CSAPrefsSubDlg)

CSubDlgSimulation::CSubDlgSimulation(CWnd* pParent /*=NULL*/)
	: CSAPrefsSubDlg(CSubDlgSimulation::IDD, pParent)
	, m_unit(FALSE)
	, m_nFunSel(SIMULATION_MODE)
	, m_AngleData(0)
	, m_fRoll(0.0)
	, m_fPitch(0.0)
	, CompassW(300) 
	, CompassH(300)
	, CompassX(14)
	, CompassY(1)
	, BgW(328)
	, BgH(282)
	, ImageX(100)
	, ImageY(0)
	, m_nCurrentIndex(0)
	, bImageReady(false)
	
	, m_nU(0)
	, m_nV11(0)
	, m_nV21(0)
	, m_nV12(0)
	, m_nV22(0)
	
	, m_Sc11(1)
	, m_Sc12(0)
	, m_Sc13(0)
	, m_Sc21(0)
	, m_Sc22(1)
	, m_Sc23(0)
	, m_Sc31(0)
	, m_Sc32(0)
	, m_Sc33(1)
	
	
	, m_CalibrationIndication(_T(""))
{
	for(int i=0; i<3; i++){
		m_nCalibratedM[i]=0;
		m_nG[i]=0;
		m_ng[i]=0;
		m_nM[i]=0;
		m_fOptim[i]=0.0f;
	}
}

CSubDlgSimulation::~CSubDlgSimulation()
{
	CString btnStr;
	
	CloseHandle(hCompassGuiThread);

	m_SimulatingCtl.GetWindowText(btnStr);
	if( btnStr==_T("Stop") ) 
		OnBnClickedButtonSimulation();

	mdc->DeleteDC(); 
	mdc1->DeleteDC();
	bgbmp->DeleteObject(); 
	tempbmp->DeleteObject(); 
	maskBmp->DeleteObject();
	for(int i=0; i<360; i++)
		bitmap[i]->DeleteObject();
}

void CSubDlgSimulation::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_LSB, m_unit);
	DDX_Control(pDX, IDC_BTN_SIMULATION, m_SimulatingCtl);
	DDX_Control(pDX, IDC_BTN_AUTO, m_Auto);
	
	DDX_Text(pDX, IDC_STATIC_MXDATA, m_nM[0]);
	DDX_Text(pDX, IDC_STATIC_MYDATA, m_nM[1]);
	DDX_Text(pDX, IDC_STATIC_MZDATA, m_nM[2]);
	DDX_Text(pDX, IDC_STATIC_GXDATA, m_nG[0]);
	DDX_Text(pDX, IDC_STATIC_GYDATA, m_nG[1]);
	DDX_Text(pDX, IDC_STATIC_GZDATA, m_nG[2]);
	DDX_Text(pDX, IDC_STATIC_GXDATA2, m_ng[0]);
	DDX_Text(pDX, IDC_STATIC_GYDATA2, m_ng[1]);
	DDX_Text(pDX, IDC_STATIC_GZDATA2, m_ng[2]);

	DDX_Text(pDX, IDC_STATIC_CMXDATA, m_nCalibratedM[0]);
	DDX_Text(pDX, IDC_STATIC_CMYDATA, m_nCalibratedM[1]);
	DDX_Text(pDX, IDC_STATIC_CMZDATA, m_nCalibratedM[2]);
	DDX_Text(pDX, IDC_STATIC_CA_XDATA, m_fOptim[0]);
	DDX_Text(pDX, IDC_STATIC_CA_YDATA, m_fOptim[1]);
	DDX_Text(pDX, IDC_STATIC_CA_ZDATA, m_fOptim[2]);
	DDX_Text(pDX, IDC_STATIC_ANGLE_DATA, m_AngleData);
	DDX_Text(pDX, IDC_STATIC_ROLL_DATA, m_fRoll);
	DDX_Text(pDX, IDC_STATIC_PITCH_DATA, m_fPitch);
	DDX_Text(pDX, IDC_STATIC_RADIUS, m_radius);
	DDX_Text(pDX, IDC_STATIC_U, m_nU);
	DDX_Text(pDX, IDC_STATIC_V11, m_nV11);
	DDX_Text(pDX, IDC_STATIC_V21, m_nV21);
	DDX_Text(pDX, IDC_STATIC_V12, m_nV12);
	DDX_Text(pDX, IDC_STATIC_V22, m_nV22);
	
	
	DDX_Text(pDX, IDC_STATIC_SC11, m_Sc11);
	DDX_Text(pDX, IDC_STATIC_SC12, m_Sc12);
	DDX_Text(pDX, IDC_STATIC_SC13, m_Sc13);
	DDX_Text(pDX, IDC_STATIC_SC21, m_Sc21);
	DDX_Text(pDX, IDC_STATIC_SC22, m_Sc22);
	DDX_Text(pDX, IDC_STATIC_SC23, m_Sc23);
	DDX_Text(pDX, IDC_STATIC_SC31, m_Sc31);
	DDX_Text(pDX, IDC_STATIC_SC32, m_Sc32);
	DDX_Text(pDX, IDC_STATIC_SC33, m_Sc33);

	DDX_Text(pDX, IDC_STATIC_CALIBRATION_INDICATION, m_CalibrationIndication);
}


BEGIN_MESSAGE_MAP(CSubDlgSimulation, CSAPrefsSubDlg)
	ON_BN_CLICKED(IDC_BTN_SIMULATION, &CSubDlgSimulation::OnBnClickedButtonSimulation)
	ON_BN_CLICKED(IDC_BTN_AUTO, &CSubDlgSimulation::OnBnClickedBtnAuto)
	
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_SMPG_SPEED, &CSubDlgSimulation::OnCbnSelchangeComboSmpgSpeed)

	ON_CBN_SELCHANGE(IDC_COMBO_SMPG_SOFTORHARD, &CSubDlgSimulation::OnCbnSelchangeComboSmpgSoftorhard)
	ON_WM_CTLCOLOR()
	
	
	ON_STN_CLICKED(IDC_STATIC_MYDATA, &CSubDlgSimulation::OnStnClickedStaticMydata)
	ON_BN_CLICKED(IDC_STATIC_ANGLES_GROUP, &CSubDlgSimulation::OnBnClickedStaticAnglesGroup)
	
	//ON_BN_CLICKED(IDC_CHECK_UNIT, &CSubDlgSimulation::OnBnClickedCheckUnit)
	
	ON_BN_CLICKED(IDC_RADIO_LSB, &CSubDlgSimulation::OnBnClickedRadioLsb)
	ON_BN_CLICKED(IDC_RADIO_MT, &CSubDlgSimulation::OnBnClickedRadioMt)
END_MESSAGE_MAP()


// CSubDlgSimulation 訊息處理常式
BOOL CSubDlgSimulation::OnInitDialog()
{
	// TODO: Add extra initialization here
	g_pSubDlgSim = this;
	
	GetClientRect(&ClientRect); 

	LoadImageThread();

	m_OdrCtl = (CComboBox *)GetDlgItem(IDC_COMBO_SMPG_SPEED);
	m_OdrCtl->InsertString(0, _T("Slower"));
	m_OdrCtl->InsertString(1, _T("Normal"));
	m_OdrCtl->InsertString(2, _T("Faster"));
	m_OdrCtl->InsertString(3, _T("Fastest"));
	m_OdrCtl->SetCurSel(1) ;
	
	m_OdrCtl1 = (CComboBox *)GetDlgItem(IDC_COMBO_SMPG_SOFTORHARD);
	m_OdrCtl1->InsertString(0, _T("4 elements"));
	m_OdrCtl1->InsertString(1, _T("10 elements"));
	m_OdrCtl1->SetCurSel(0) ;

	ShowGyroInfo((bool)SW_SHOW);
	ShowMagneticSensorInfo((bool)SW_SHOW);
	ShowAccelerometerInfo((bool)SW_SHOW);
	ShowAnglesInfo((bool)SW_SHOW);
	ShowCalibrationInfo((bool)SW_SHOW);
	GetDlgItem(IDC_BTN_SIMULATION)->ShowWindow(SW_SHOW);
	
	//CreateCompassGuiThread();

	m_fOptim[0] = g_fOptimX; 
	m_fOptim[1] = g_fOptimY;
	m_fOptim[2] = g_fOptimZ;

	//bruce 20121219
	 LOGFONT lf;                        // Used to create the CFont.

   CDialog::OnInitDialog();           // Call default ::OnInitDialog

   memset(&lf, 0, sizeof(LOGFONT));   // Clear out structure.
   lf.lfHeight = 18;                  // Request a 20-pixel-high font
   m_font.CreateFontIndirect(&lf);    // Create the font.

   // Use the font to paint a control. This code assumes
   // a control named IDC_TEXT1 in the dialog box.
   GetDlgItem(IDC_STATIC_V11)->SetFont(&m_font);
   GetDlgItem(IDC_STATIC_V21)->SetFont(&m_font);
   GetDlgItem(IDC_STATIC_U)->SetFont(&m_font);
  
   /*********/


	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}





void CSubDlgSimulation::OnBnClickedBtnAuto()
{
	// TODO: Add your control notification handler code here
	CISenTekAssistantDlg *pParent = (CISenTekAssistantDlg *)GetParent();
	HANDLE hThread;
	DWORD dwData;
	DWORD dwThreadId;
	CString btnStr;

	UpdateData(TRUE);
	m_Auto.GetWindowText(btnStr);
	
	if(btnStr==_T("File"))
	{
		pParent->bAuto = FALSE;
		m_Auto.SetWindowText(_T("Auto"));
	}
	else if( btnStr==_T("Auto") ) 
	{
		pParent->bAuto = TRUE;

		m_Auto.SetWindowText(_T("File"));
	}
	UpdateData(FALSE);
}

void CSubDlgSimulation::OnBnClickedButtonSimulation()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CISenTekAssistantDlg *pParent = (CISenTekAssistantDlg *)GetParent();
	HANDLE hThread;
	DWORD dwData;
	DWORD dwThreadId;
	CString btnStr;

	UpdateData(TRUE);	

	if(!pParent->m_iSenI2c.CheckI2c()) {
		AfxMessageBox(_T("I2C device is disabled. You need to enable the I2C option to access the Sensor"));
		return;
	}
	
	m_SimulatingCtl.GetWindowText(btnStr);
	
	if(btnStr==_T("Simulation")) {
		pParent->bStopSimulation = FALSE;
		m_SimulatingCtl.SetWindowText(_T("Stop"));

		pParent->LoadCalibrationFile(&g_fOptimX, &g_fOptimY, &g_fOptimZ);
		m_fOptim[0] = g_fOptimX;
		m_fOptim[1] = g_fOptimY;
		m_fOptim[2] = g_fOptimZ;

		if(pParent->m_bGsensorCalibrationSW)
			pParent->LoadCalibrationDataForG();


		hThread = CreateThread(
					NULL, 0,
					SimulationProc,
					&dwData,
					0,
					&dwThreadId);

		if(hThread == NULL)	{
			AfxMessageBox(_T("Create Thread Error"));
			ExitProcess(0);
		}

	} else if( btnStr==_T("Stop") ) {
		pParent->bStopSimulation = TRUE;

		m_SimulatingCtl.SetWindowText(_T("Simulation"));
	}

	UpdateData(FALSE);
}


void CSubDlgSimulation::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值	
	if(nIDEvent == UPDATE_REGION_TIMER) {
		CRect ViewRect(ImageX, ImageY, (int)(ImageX+BgW), (int)(ImageY+BgH*1.07));
		ViewRect.NormalizeRect();		
		InvalidateRect(&ViewRect);

		KillTimer(UPDATE_REGION_TIMER);
	} else if(nIDEvent == RESTART_SIM_TIMER) {
		OnBnClickedButtonSimulation();
		KillTimer(RESTART_SIM_TIMER);
	}
	CSAPrefsSubDlg::OnTimer(nIDEvent);
}


void CSubDlgSimulation::ShowAccelerometerInfo(bool bShow)
{
	GetDlgItem(IDC_STATIC_G_SENINFO_GROUP)->ShowWindow(bShow);	
	GetDlgItem(IDC_STATIC_GX)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GXDATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GY)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GYDATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GZ)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GZDATA)->ShowWindow(bShow);
}

void CSubDlgSimulation::ShowGyroInfo(bool bShow)
{
	GetDlgItem(IDC_STATIC_G_SENINFO_GROUP2)->ShowWindow(bShow);	
	GetDlgItem(IDC_STATIC_GX2)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GXDATA2)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GY2)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GYDATA2)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GZ2)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_GZDATA2)->ShowWindow(bShow);
}

void CSubDlgSimulation::ShowMagneticSensorInfo(bool bShow)
{
	GetDlgItem(IDC_STATIC_M_SENINFO_GROUP)->ShowWindow(bShow);	
	GetDlgItem(IDC_STATIC_MX)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_MXDATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_MY)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_MYDATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_MZ)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_MZDATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_CALIBRATED)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_CMXDATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_CMYDATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_CMZDATA)->ShowWindow(bShow);
}



void CSubDlgSimulation::ShowAnglesInfo(bool bShow)
{
	GetDlgItem(IDC_STATIC_ANGLES_GROUP)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_ANGLE)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_ANGLE_DATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_ROLL)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_ROLL_DATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_PITCH)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_PITCH_DATA)->ShowWindow(bShow);
}

void CSubDlgSimulation::ShowCalibrationInfo(bool bShow)
{
	GetDlgItem(IDC_STATIC_CALIBRATION_GROUP)->ShowWindow(bShow);	
	GetDlgItem(IDC_STATIC_CAX)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_CA_XDATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_CAY)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_CA_YDATA)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_CAZ)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_CA_ZDATA)->ShowWindow(bShow);
}

void CSubDlgSimulation::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	CSAPrefsSubDlg::OnClose();
}

void CSubDlgSimulation::LoadImage(void)
{
	CClientDC dc(this);
	
	mdc = new CDC;         
	mdc1 = new CDC;		   
	
	bgbmp = new CBitmap;  
	tempbmp = new CBitmap;
	maskBmp = new CBitmap;
	
	mdc->CreateCompatibleDC(&dc);  	
	mdc1->CreateCompatibleDC(&dc); 
	tempbmp->CreateCompatibleBitmap(&dc, ClientRect.right, ClientRect.bottom); 
	
	bgbmp->m_hObject = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_BK), IMAGE_BITMAP, ClientRect.right, ClientRect.bottom, LR_SHARED); 

	maskBmp->m_hObject = GetCompassResource(360);
	for(int i=0; i<360; i++) {
		bitmap[i] = new CBitmap;
		bitmap[i]->m_hObject = GetCompassResource(i); 
	}
	
	mdc->SelectObject(tempbmp); 

	bImageReady = true;
}


void CSubDlgSimulation::ChangeImage(int Angle,int ndirection)
{
	//UpdateData();
	Angle = 360 - Angle;
	
	if(Angle>360)
		Angle%=360;
	if(Angle==360)	
		Angle = 0;	


	int m_nJump=1;
	while(abs((int)(m_nCurrentIndex-Angle))>m_nJump) {
		
	if(ndirection=0,abs((int)(m_nCurrentIndex-Angle))>m_nJump)
	{
		if(m_nCurrentIndex>Angle) {
			if((m_nCurrentIndex-Angle)>180)
				m_nCurrentIndex+=m_nJump;
			else
				m_nCurrentIndex-=m_nJump;
		} else if(m_nCurrentIndex<Angle) {
			if(abs((int)(m_nCurrentIndex-Angle))>180)
				m_nCurrentIndex-=m_nJump;
			else
				m_nCurrentIndex+=m_nJump;
		} 
	}
	
	if(m_nCurrentIndex>360)
		m_nCurrentIndex -= 360;
	if(m_nCurrentIndex<0)
		m_nCurrentIndex += 360;

	UpdateData(FALSE);	
		
	CClientDC dc(this);
	
	mdc1->SelectObject(bgbmp);   
	mdc->BitBlt(0, 0, BgW, (int)(BgH*1.07), mdc1, 0, 0, SRCCOPY);
	
	mdc1->SelectObject(maskBmp); 
	mdc->BitBlt(CompassX, CompassY, CompassW, CompassH, mdc1, 0, 0, SRCAND);
	
	if(m_nCurrentIndex!=360)
		mdc1->SelectObject(bitmap[m_nCurrentIndex]); 
	else
		mdc1->SelectObject(bitmap[0]); 
	mdc->BitBlt(CompassX, CompassY, CompassW, CompassH, mdc1, 0, 0, SRCPAINT);
	
	dc.BitBlt(ImageX, ImageY, BgW, (int)(BgH*1.07), mdc, 0, 0, SRCCOPY); 
}
}






void CSubDlgSimulation::LoadImageThread(void)
{
	HANDLE hThread;
	DWORD dwData;
	DWORD dwThreadId;

	hThread = CreateThread(
				NULL, 0,
				LoadCompassPicProc,
				&dwData,
				0,
				&dwThreadId);

	if(hThread == NULL)	{
		AfxMessageBox(_T("Create Thread Error"));
		ExitProcess(0);
	}
}

void CSubDlgSimulation::CreateCompassGuiThread(void)
{
	// TODO: 在此加入您的命令處理常式程式碼
	DWORD dwData;
	DWORD dwThreadId;

	hCompassGuiThread = CreateThread(
				NULL, 0,
				CompassGuiProc,
				&dwData,
				0,
				&dwThreadId);

	if(hCompassGuiThread == NULL)	{
		AfxMessageBox(_T("Create Thread Error"));
		ExitProcess(0);
	}
}



void CSubDlgSimulation::OnBnClickedRadioLsb()
{
	// TODO: Add your control notification handler code here
	if (IDC_RADIO_LSB==FALSE)
	{
    m_unit=TRUE;
	}
	else
	{
	m_unit=FALSE;
	}
}

void CSubDlgSimulation::OnBnClickedRadioMt()
{
	// TODO: Add your control notification handler code here
	if (IDC_RADIO_MT==FALSE)
	{
    m_unit=FALSE;
	}
	else
	{
	m_unit=TRUE;
	}
}


void CSubDlgSimulation::OnCbnSelchangeComboSmpgSpeed()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData();
	if(m_nFunSel==SIMULATION_MODE) {
		CString btnStr;
		m_SimulatingCtl.GetWindowText(btnStr);
		if( btnStr==_T("Simulation") ) 
			OnBnClickedButtonSimulation();
		else {
			OnBnClickedButtonSimulation();
			SetTimer(RESTART_SIM_TIMER, 1500, NULL);
		}
	}
	UpdateData(FALSE);
}

DWORD WINAPI CompassGuiProc(LPVOID lpParam)
{
	while(1) {
	
		if(g_pSubDlgSim->m_nFunSel == CSubDlgSimulation::SIMULATION_MODE && g_pSubDlgSim->bImageReady==true) {
			//Sleep(4);
			//Sleep(g_pSubDlgSim->m_DelayTime);
			//ChangeImage(int Angle, int lastAngle1, bool rolling, bool pitching, bool yawing)
			g_pSubDlgSim->ChangeImage(g_AngleData,ndirection);
			//g_pSubDlgSim->ChangeImage(g_AngleData, g_LastAngleData,rolling,pitching, yawing);
		} else
			Sleep(10);
	}

	ExitThread(0);
}

DWORD WINAPI LoadCompassPicProc(LPVOID lpParam)
{
	g_pSubDlgSim->LoadImage();

	ExitThread(0);
}

HBRUSH CSubDlgSimulation::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSAPrefsSubDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if(pWnd->GetDlgCtrlID() == (IDC_STATIC_V11))   
   {      
      //set the static text color to red      
      pDC->SetTextColor(RGB(255, 0, 0));   
   }   
	if(pWnd->GetDlgCtrlID() == (IDC_STATIC_V21))   
   {      
      //set the static text color to red      
      pDC->SetTextColor(RGB(255, 0, 0));   
   }   
	if(pWnd->GetDlgCtrlID() == (IDC_STATIC_U))   
   {      
      //set the static text color to red      
      pDC->SetTextColor(RGB(255, 0, 0));   
   }   

	if (nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC) {
		CISenTekAssistantDlg *pParent = (CISenTekAssistantDlg *)GetParent();
				
		pDC->SetBkMode(TRANSPARENT);

		pParent->m_brush.DeleteObject();
		pParent->m_brush.CreateSolidBrush(RGB(255, 255, 255));
		hbr = pParent->m_brush;
	}
	// TODO:  如果預設值並非想要的，則傳回不同的筆刷
	return hbr;
}



void CSubDlgSimulation::OnCbnSelchangeComboSmpgSoftorhard()
{
	// TODO:  如果這是 RICHEDIT 控制項，控制項將不會
	// 傳送此告知，除非您覆寫 CSAPrefsSubDlg::OnInitDialog()
	// 函式和呼叫 CRichEditCtrl().SetEventMask()
	// 讓具有 ENM_CHANGE 旗標 ORed 加入遮罩。

	// TODO:  在此加入控制項告知處理常式程式碼
	UpdateData();
	UpdateData(FALSE);
}

/*
void CSubDlgSimulation::OnEnChangeEditBbbb()
{
	// TODO:  如果這是 RICHEDIT 控制項，控制項將不會
	// 傳送此告知，除非您覆寫 CSAPrefsSubDlg::OnInitDialog()
	// 函式和呼叫 CRichEditCtrl().SetEventMask()
	// 讓具有 ENM_CHANGE 旗標 ORed 加入遮罩。

	// TODO:  在此加入控制項告知處理常式程式碼
	UpdateData();
	UpdateData(FALSE);
}


void CSubDlgSimulation::OnEnChangeEditDfTh()
{
	// TODO:  如果這是 RICHEDIT 控制項，控制項將不會
	// 傳送此告知，除非您覆寫 CSAPrefsSubDlg::OnInitDialog()
	// 函式和呼叫 CRichEditCtrl().SetEventMask()
	// 讓具有 ENM_CHANGE 旗標 ORed 加入遮罩。

	// TODO:  在此加入控制項告知處理常式程式碼
	UpdateData();
	UpdateData(FALSE);
}


void CSubDlgSimulation::OnEnChangeEditDirTh()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSAPrefsSubDlg::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	UpdateData(FALSE);
}
*/

void CSubDlgSimulation::OnBnClickedStaticGSeninfoGroup2()
{
	// TODO: Add your control notification handler code here
}


void CSubDlgSimulation::OnStnClickedStaticMydata()
{
	// TODO: Add your control notification handler code here
}


void CSubDlgSimulation::OnBnClickedStaticAnglesGroup()
{
	// TODO: Add your control notification handler code here
}








