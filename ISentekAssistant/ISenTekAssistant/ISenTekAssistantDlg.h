
// ISenTekAssistantDlg.h : header file
//

#pragma once

#include "SAPrefsStatic.h"

#include "afxwin.h"


#include "SubDlgSimulation.h"

//#include "./interface/aardvark/aardvark.h"
//#include "./interface/aardvark/AardvarkClass.h"
#include "ISenI2cAccess.h"
#include "SimpleMovingAverage.h"

class pageStruct
{
   public:
   CSAPrefsSubDlg *pDlg;
   UINT id;
   CSAPrefsSubDlg *pDlgParent;
   CString csCaption;
};

#define WM_CHANGE_PAGE (WM_APP + 100)
#define WM_SET_FOCUS_WND (WM_APP + 101)



// CISenTekAssistantDlg dialog
class CISenTekAssistantDlg : public CDialogEx
{
// Construction
public:
    CISenTekAssistantDlg(CWnd* pParent = NULL); // standard constructor
    ~CISenTekAssistantDlg();
    
    enum G_SENSOR{ G_NONE, G_KXTF9, G_DMARD08 };
    enum M_SENSOR{ M_IST8301, M_IST8303 };
    enum UNIT{ LSB, MT};
// Dialog Data
    enum { IDD = IDD_ISENTEKASSISTANT_DIALOG };
    CStatic m_boundingFrame;

    void SetTitle(CString t)   {m_csTitle = t;}

    void SetConstantText(CString t)   {m_csConstantText = t;}

    void SetValidateBeforeChangingPages(bool b) {m_bValidateBeforeChangingPage = b;}

    bool AddPage(CSAPrefsSubDlg &page, const char *pCaption, CSAPrefsSubDlg *pDlgParent = NULL);
    
    bool ShowPage(int iPage);
    bool ShowPage(CSAPrefsSubDlg * pPage);
    
    void EndSpecial(UINT res, bool bOk = true);
    
    void SetStartPage(CSAPrefsSubDlg *pPage = NULL) {m_pStartPage = pPage;}

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
protected:
    HICON m_hIcon;

    bool EndOK();

    CSAPrefsStatic     m_captionBar;
    CTreeCtrl         m_pageTree;

    HTREEITEM FindHTREEItemForDlg(CSAPrefsSubDlg *pParent);
    
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTvnSelchangedPageTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnGetdispinfoPageTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnSelchangingPageTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg long OnChangePage(UINT, LONG);
    afx_msg long OnSetFocusWnd(UINT, LONG);

    CPtrArray   m_pages;
    int         m_iCurPage;
    CRect       m_frameRect;
    CString     m_csTitle, m_csConstantText;

    bool m_bValidateBeforeChangingPage;

    CSAPrefsSubDlg  *m_pStartPage;

    CMap< CSAPrefsSubDlg *, CSAPrefsSubDlg *, DWORD, DWORD&  > m_dlgMap;

public: 

    CISenI2cAccess m_iSenI2c;

    static const unsigned int nSensorsTotalAxisNum = 6;
    static const unsigned int nMovingAverageOrder = 4;
    CSimpleMovingAverage   *pSMV[nSensorsTotalAxisNum];

    float GetYawAngle(float fMX, float fMY, float fMZ, float fRoll, float fPitch);
    double detrm(double a[10][10],int k);
    void inv(double inv1[10][10],double num[10][10],int f);
    void Jacobi_Cyclic_Method(double eigenvalues[], double *eigenvectors, double *A, int n);
    void HardCalibration(double center[3],float Y[90],double MM[90][11],double MMtran[11][90],int Stdnumber,int kk);
    void SoftCalibration(double center[3],double Sc[3][3],double MM[90][11],double MMtran[11][90],int Stdnumber,int kk);
    void GsensorCalibration(double W[6][4],double Y[6][3], double X[4][3]);
    void Sort(double MM[90][11],double MMtran[11][90],float Y[90],int Stdnumber,int kk);
    void rot(float q[4], int data[3],float rotresult[3]);
    void quaternionmutiply(float q2[4], float q1[4], float qresults[4]);
    void kalmangain(float P[3][3], float H[3][3], float R[3][3], double L[3][3]);
    void quaternionconjugate(float q[4], float qc[4]);
    void quaternionunit(float q[4]);
    void observationmatrix(float rotref[3], float H[3][3]);
    void pseudoinverse(float H[3][3], double Hinverse[3][3]);
    void pmatrixpriori(float P[3][3],float Q[3][3],float timeinterval);
    void pmatrixupdate(float P[3][3],double L[3][3], float H[3][3], float I[3][3]);
public:
    
public:
    bool LoadCalibrationFile(float *OptimX, float *OptimY, float *OptimZ);
    bool LoadCalibrationDataForG(void);
    G_SENSOR m_nAccelerometerSel;
    M_SENSOR m_nMagnetometerSel;
    int Gz;
    float GzTh;
    //float GzTh2;
    int m_GchangeTh;
    int m_MradiusTh;
    
    int m_nOffsetX;
    int m_nOffsetY;
    int m_nOffsetZ;
    int m_nOffsetDriftX;
    int m_nOffsetDriftY;
    int m_nOffsetDriftZ;
    UINT m_nInterruptThresholdValue;    
    void InitializeGUI(void);
    void InitializePrettyPage(void);
    bool m_bSwDebugMode;
    void ShowTestLog(bool bShow);

    
    CBrush m_brush; 
    
    CSubDlgSimulation       SimulationSubDlg;

    int nDirection;

    float m_Q11;
    float m_Q12;
    float m_Q13;
    float m_Q21;
    float m_Q22;
    float m_Q23;
    float m_Q31;
    float m_Q32;
    float m_Q33;
    float m_R11;
    float m_R12;
    float m_R13;
    float m_R21;
    float m_R22;
    float m_R23;
    float m_R31;
    float m_R32;
    float m_R33;

    float m_G11;
    float m_G12;
    float m_G13;
    float m_G21;
    float m_G22;
    float m_G23;
    float m_G31;
    float m_G32;
    float m_G33;
    float m_G41;
    float m_G42;
    float m_G43;
    float m_G51;
    float m_G52;
    float m_G53;
    float m_G61;
    float m_G62;
    float m_G63;
    bool m_bGsensorCalibrationSW;

    float m_pitchlimit;
    float m_moving;
    float m_datanumber;
    float m_datarange;
    float m_hardstd;
    float m_softstd;
    float m_fUAlpha;
    float m_nMdTh[3];

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg void OnBnClickedButton1();
    
    afx_msg void OnClose();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    
    CEdit m_strMsgCtl;

    bool bStopSimulation;   
    bool bAuto;
    
    BOOL m_bFirG;
    BOOL m_bFirM;

    afx_msg void OnSettingUsbAardvark();
    afx_msg void OnUpdateSettingUsbAardvark(CCmdUI *pCmdUI);
    afx_msg void OnSettingUsbIow();
    afx_msg void OnUpdateSettingUsbIow(CCmdUI *pCmdUI);
    afx_msg void OnSettingConfiguration();  
    afx_msg void OnHelpAbout(); 
    
};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// Dialog Data
    enum { IDD = IDD_ABOUTBOX };
    CString m_FileVersion;

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
public:
    CString m_iSenInterfaceVer;
    virtual BOOL OnInitDialog();
};
