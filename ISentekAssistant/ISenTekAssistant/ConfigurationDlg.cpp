// ConfigurationDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "ISenTekAssistant.h"
#include "ConfigurationDlg.h"
#include "afxdialogex.h"


// CConfigurationDlg 對話方塊

IMPLEMENT_DYNAMIC(CConfigurationDlg, CDialogEx)

CConfigurationDlg::CConfigurationDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CConfigurationDlg::IDD, pParent)
    , m_bMSensorI2CSlaveAdd(FALSE)
    , m_nGSelection(0)
    , m_nMSelection(0)
    , m_Q11(1)
    , m_Q12(0)
    , m_Q13(0)
    , m_Q21(0)
    , m_Q22(1)
    , m_Q23(0)
    , m_Q31(0)
    , m_Q32(0)
    , m_Q33(1)

    , m_G11(0)
    , m_G12(0)
    , m_G13(256)
    , m_G21(0)
    , m_G22(0)
    , m_G23(-256)
    , m_G31(0)
    , m_G32(256)
    , m_G33(0)
    , m_G41(0)
    , m_G42(-256)
    , m_G43(0)
    , m_G51(256)
    , m_G52(0)
    , m_G53(0)
    , m_G61(-256)
    , m_G62(0)
    , m_G63(1)
    , m_pitchlimit(90)
    ,m_moving(1)
    ,m_datanumber(25)
    ,m_datarange(25)
    ,m_hardstd(6.5)
    ,m_softstd(5.5)
    //, m_GzTh2(0)
    , m_nMdThX(0)
    , m_nMdThY(0)
    , m_nMdThZ(0)
    , m_GchangeTh(0)
    , m_MradiusTh(0)
    , m_fUAlpha(400)
    , m_bGsensorCalibrationSW(FALSE)
    , m_bFirG(FALSE)
    , m_bFirM(FALSE)
{

}

CConfigurationDlg::~CConfigurationDlg()
{
}

void CConfigurationDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Radio(pDX, IDC_RADIO_MSENSOR_I2CADDR1, m_bMSensorI2CSlaveAdd);
    DDX_Radio(pDX, IDC_RADIO_G_NONE, m_nGSelection);
    DDX_Radio(pDX, IDC_RADIO_IST8301, m_nMSelection);
    DDX_Text(pDX, IDC_EDIT_Q11, m_Q11);
    DDX_Text(pDX, IDC_EDIT_Q12, m_Q12);
    DDX_Text(pDX, IDC_EDIT_Q13, m_Q13);
    DDX_Text(pDX, IDC_EDIT_Q21, m_Q21);
    DDX_Text(pDX, IDC_EDIT_Q22, m_Q22);
    DDX_Text(pDX, IDC_EDIT_Q23, m_Q23);
    DDX_Text(pDX, IDC_EDIT_Q31, m_Q31);
    DDX_Text(pDX, IDC_EDIT_Q32, m_Q32);
    DDX_Text(pDX, IDC_EDIT_Q33, m_Q33);

    DDX_Text(pDX, IDC_EDIT_R11, m_R11);
    DDX_Text(pDX, IDC_EDIT_R12, m_R12);
    DDX_Text(pDX, IDC_EDIT_R13, m_R13);
    DDX_Text(pDX, IDC_EDIT_R21, m_R21);
    DDX_Text(pDX, IDC_EDIT_R22, m_R22);
    DDX_Text(pDX, IDC_EDIT_R23, m_R23);
    DDX_Text(pDX, IDC_EDIT_R31, m_R31);
    DDX_Text(pDX, IDC_EDIT_R32, m_R32);
    DDX_Text(pDX, IDC_EDIT_R33, m_R33);

    DDX_Text(pDX, IDC_EDIT_G11, m_G11);
    DDX_Text(pDX, IDC_EDIT_G12, m_G12);
    DDX_Text(pDX, IDC_EDIT_G13, m_G13);
    DDX_Text(pDX, IDC_EDIT_G21, m_G21);
    DDX_Text(pDX, IDC_EDIT_G22, m_G22);
    DDX_Text(pDX, IDC_EDIT_G23, m_G23);
    DDX_Text(pDX, IDC_EDIT_G31, m_G31);
    DDX_Text(pDX, IDC_EDIT_G32, m_G32);
    DDX_Text(pDX, IDC_EDIT_G33, m_G33);
    DDX_Text(pDX, IDC_EDIT_G41, m_G41);
    DDX_Text(pDX, IDC_EDIT_G42, m_G42);
    DDX_Text(pDX, IDC_EDIT_G43, m_G43);
    DDX_Text(pDX, IDC_EDIT_G51, m_G51);
    DDX_Text(pDX, IDC_EDIT_G52, m_G52);
    DDX_Text(pDX, IDC_EDIT_G53, m_G53);
    DDX_Text(pDX, IDC_EDIT_G61, m_G61);
    DDX_Text(pDX, IDC_EDIT_G62, m_G62);
    DDX_Text(pDX, IDC_EDIT_G63, m_G63);

    DDX_Text(pDX, IDC_EDIT_MOVING, m_moving);
    DDX_Text(pDX, IDC_EDIT_DATANUMBER, m_datanumber);
    DDX_Text(pDX, IDC_EDIT_DATARANGE, m_datarange);
    DDX_Text(pDX, IDC_EDIT_HARDSTD, m_hardstd);
    DDX_Text(pDX, IDC_EDIT_SOFTSTD, m_softstd);
    DDX_Text(pDX, IDC_EDIT_PITCHLIMIT, m_pitchlimit);


    //  DDX_Text(pDX, IDC_EDIT3, m_GzTh2);
    DDX_Text(pDX, IDC_EDIT_MX_TH, m_nMdThX);
    DDX_Text(pDX, IDC_EDIT_MY_TH, m_nMdThY);
    DDX_Text(pDX, IDC_EDIT_MZ_TH, m_nMdThZ);
    DDX_Text(pDX, IDC_EDIT_U_FILTER_ALPHA, m_fUAlpha);
    DDX_Text(pDX, IDC_EDIT_G_CHANGE_TH, m_GchangeTh);
    DDX_Text(pDX, IDC_EDIT_M_RADIUS_TH, m_MradiusTh);
    DDX_Check(pDX, IDC_CHECK_G_CALIBRATION_SW, m_bGsensorCalibrationSW);
    DDX_Check(pDX, IDC_CHECK_FIR_G, m_bFirG);
    DDX_Check(pDX, IDC_CHECK_FIR_M, m_bFirM);
}


BEGIN_MESSAGE_MAP(CConfigurationDlg, CDialogEx)
    ON_EN_CHANGE(IDC_EDIT_MX_TH, &CConfigurationDlg::OnEnChangeEditMxTh)
    ON_EN_CHANGE(IDC_EDIT_MY_TH, &CConfigurationDlg::OnEnChangeEditMyTh)
    ON_EN_CHANGE(IDC_EDIT_MZ_TH, &CConfigurationDlg::OnEnChangeEditMzTh)
    ON_BN_SETFOCUS(IDC_RADIO_IST8301, &CConfigurationDlg::OnBnSetfocusRadioIst8301)
    ON_BN_SETFOCUS(IDC_RADIO_IST8303, &CConfigurationDlg::OnBnSetfocusRadioIst8303)
END_MESSAGE_MAP()


// CConfigurationDlg 訊息處理常式


void CConfigurationDlg::OnEnChangeEditMxTh()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}


void CConfigurationDlg::OnEnChangeEditMyTh()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}


void CConfigurationDlg::OnEnChangeEditMzTh()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}



BOOL CConfigurationDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    OnMSensorSelectionChanged();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX 屬性頁應傳回 FALSE
}


void CConfigurationDlg::OnBnSetfocusRadioIst8301()
{
    m_nMSelection = 0;
    OnMSensorSelectionChanged();
}


void CConfigurationDlg::OnBnSetfocusRadioIst8303()
{
    m_nMSelection = 1;
    OnMSensorSelectionChanged();
}


void CConfigurationDlg::OnMSensorSelectionChanged(void)
{
    if (m_nMSelection == 0)
    {
        this->SetDlgItemTextW(IDC_RADIO_MSENSOR_I2CADDR1, _T("0x0E"));
        this->SetDlgItemTextW(IDC_RADIO_MSENSOR_I2CADDR2, _T("0x0F"));
    }
    else
    {
        this->SetDlgItemTextW(IDC_RADIO_MSENSOR_I2CADDR1, _T("0x0C"));
        this->SetDlgItemTextW(IDC_RADIO_MSENSOR_I2CADDR2, _T("0x0E"));
    }
}
