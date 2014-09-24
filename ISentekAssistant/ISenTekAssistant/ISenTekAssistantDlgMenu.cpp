#include "Total.h"

#include "afxdialogex.h"

#include "ConfigurationDlg.h"

extern DWORD WINAPI DumpAllRegisterProc(LPVOID lpParam);
extern DWORD WINAPI OneTimeProgrammingProc(LPVOID lpParam);
extern DWORD WINAPI DumpOtpProc(LPVOID lpParam);
extern DWORD WINAPI BatchFileProc(LPVOID lpParam);

extern int IST8301_I2CSLAVEADD;
extern int IST8303_I2CSLAVEADD;
extern int DMARD08_I2CSLAVEADD;

void CISenTekAssistantDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    ASSERT(pPopupMenu != NULL);

    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pParentMenu == NULL);

    HMENU hParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        state.m_pParentMenu = pPopupMenu;   
    else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
    {
        CWnd* pParent = this;
        if (pParent != NULL &&
           (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
        {
           int nIndexMax = ::GetMenuItemCount(hParentMenu);
           for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
           {
            if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
            {
                state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
                break;
            }
           }
        }
    }
    
    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (state.m_nIndex=0; state.m_nIndex<state.m_nIndexMax; state.m_nIndex++)
    {   
        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
        if (state.m_nID == 0)
           continue; 

        ASSERT(state.m_pOther == NULL);
        ASSERT(state.m_pMenu != NULL);
        if (state.m_nID == (UINT)-1)
        {
           state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
           if (state.m_pSubMenu == NULL ||
            (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
            state.m_nID == (UINT)-1)
           {
            continue;      
           }
           state.DoUpdate(this, TRUE);   
        }
        else
        {
           state.m_pSubMenu = NULL;
           state.DoUpdate(this, FALSE);
        }

        UINT nCount = pPopupMenu->GetMenuItemCount();
        if (nCount < state.m_nIndexMax)
        {   
            state.m_nIndex -= (state.m_nIndexMax - nCount);
            while (state.m_nIndex < nCount &&
                pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
            {
                state.m_nIndex++;
            }
        }
        state.m_nIndexMax = nCount;
    }
}

void CISenTekAssistantDlg::OnSettingUsbAardvark()
{
    if(m_iSenI2c.GetI2cDevSel()!=CISenI2cAccess::AARDVARK) {
        m_iSenI2c.iSenCloseI2cAccess();
        m_iSenI2c.iSenOpenI2cAccess(CISenI2cAccess::AARDVARK);
    } else
        m_iSenI2c.iSenCloseI2cAccess();
}

void CISenTekAssistantDlg::OnUpdateSettingUsbAardvark(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_iSenI2c.GetI2cDevSel()==CISenI2cAccess::AARDVARK);
}

void CISenTekAssistantDlg::OnSettingUsbIow()
{
    if(m_iSenI2c.GetI2cDevSel()!=CISenI2cAccess::IOW) {
        m_iSenI2c.iSenCloseI2cAccess();
        m_iSenI2c.iSenOpenI2cAccess(CISenI2cAccess::IOW);
    } else
        m_iSenI2c.iSenCloseI2cAccess();
}

void CISenTekAssistantDlg::OnUpdateSettingUsbIow(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_iSenI2c.GetI2cDevSel()==CISenI2cAccess::IOW);
}

void CISenTekAssistantDlg::OnSettingConfiguration()
{
    BeginWaitCursor();  

    CConfigurationDlg ConfigurationDlg;

    if (m_nMagnetometerSel == 0)
    {
        if (IST8301_I2CSLAVEADD==0x0E)
            ConfigurationDlg.m_bMSensorI2CSlaveAdd = 0;
        else 
            ConfigurationDlg.m_bMSensorI2CSlaveAdd = 1;
    }
    else
    {
        if (IST8303_I2CSLAVEADD==0x0C)
            ConfigurationDlg.m_bMSensorI2CSlaveAdd = 0;
        else 
            ConfigurationDlg.m_bMSensorI2CSlaveAdd = 1;
    }
    //ConfigurationDlg.m_strDMARD08_I2CSLAVEADD.Format(_T("%02X"), DMARD08_I2CSLAVEADD );

    ConfigurationDlg.m_Q11 = m_Q11;
    ConfigurationDlg.m_Q12 = m_Q12;
    ConfigurationDlg.m_Q13 = m_Q13;
    ConfigurationDlg.m_Q21 = m_Q21;
    ConfigurationDlg.m_Q22 = m_Q22;
    ConfigurationDlg.m_Q23 = m_Q23;
    ConfigurationDlg.m_Q31 = m_Q31;
    ConfigurationDlg.m_Q32 = m_Q32;
    ConfigurationDlg.m_Q33 = m_Q33;

    ConfigurationDlg.m_R11 = m_R11;
    ConfigurationDlg.m_R12 = m_R12;
    ConfigurationDlg.m_R13 = m_R13;
    ConfigurationDlg.m_R21 = m_R21;
    ConfigurationDlg.m_R22 = m_R22;
    ConfigurationDlg.m_R23 = m_R23;
    ConfigurationDlg.m_R31 = m_R31;
    ConfigurationDlg.m_R32 = m_R32;
    ConfigurationDlg.m_R33 = m_R33;

    ConfigurationDlg.m_G11 = m_G11;
    ConfigurationDlg.m_G12 = m_G12;
    ConfigurationDlg.m_G13 = m_G13;
    ConfigurationDlg.m_G21 = m_G21;
    ConfigurationDlg.m_G22 = m_G22;
    ConfigurationDlg.m_G23 = m_G23;
    ConfigurationDlg.m_G31 = m_G31;
    ConfigurationDlg.m_G32 = m_G32;
    ConfigurationDlg.m_G33 = m_G33;
    ConfigurationDlg.m_G41 = m_G41;
    ConfigurationDlg.m_G42 = m_G42;
    ConfigurationDlg.m_G43 = m_G43;
    ConfigurationDlg.m_G51 = m_G51;
    ConfigurationDlg.m_G52 = m_G52;
    ConfigurationDlg.m_G53 = m_G53;
    ConfigurationDlg.m_G61 = m_G61;
    ConfigurationDlg.m_G62 = m_G62;
    ConfigurationDlg.m_G63 = m_G63;
    ConfigurationDlg.m_bGsensorCalibrationSW = m_bGsensorCalibrationSW;

    ConfigurationDlg.m_bFirG = m_bFirG;
    ConfigurationDlg.m_bFirM = m_bFirM;

    ConfigurationDlg.m_moving = m_moving;
    ConfigurationDlg.m_datanumber = m_datanumber;
    ConfigurationDlg.m_datarange = m_datarange;
    ConfigurationDlg.m_hardstd = m_hardstd;
    ConfigurationDlg.m_softstd = m_softstd;
    ConfigurationDlg.m_pitchlimit = m_pitchlimit;

    ConfigurationDlg.m_fUAlpha = m_fUAlpha;
    ConfigurationDlg.m_nGSelection = (int)m_nAccelerometerSel;
    ConfigurationDlg.m_nMSelection = (int)m_nMagnetometerSel;
    
    ConfigurationDlg.m_GchangeTh = m_GchangeTh;
    ConfigurationDlg.m_MradiusTh = m_MradiusTh;

    ConfigurationDlg.m_nMdThX = m_nMdTh[0];
    ConfigurationDlg.m_nMdThY = m_nMdTh[1];
    ConfigurationDlg.m_nMdThZ = m_nMdTh[2];

    INT_PTR nResponse = ConfigurationDlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is dismissed with OK
        if (ConfigurationDlg.m_nMSelection == M_IST8301)
        {
            if (ConfigurationDlg.m_bMSensorI2CSlaveAdd)
                IST8301_I2CSLAVEADD = 0x0F;
            else
                IST8301_I2CSLAVEADD = 0x0E;
        }
        else
        {
            if (ConfigurationDlg.m_bMSensorI2CSlaveAdd)
                IST8303_I2CSLAVEADD = 0x0E;
            else
                IST8303_I2CSLAVEADD = 0x0C;
        }
//      DMARD08_I2CSLAVEADD = HexadecimalToDecimal(ConfigurationDlg.m_strDMARD08_I2CSLAVEADD);

        m_Q11 = ConfigurationDlg.m_Q11;
        m_Q12 = ConfigurationDlg.m_Q12;
        m_Q13 = ConfigurationDlg.m_Q13;
        m_Q21 = ConfigurationDlg.m_Q21;
        m_Q22 = ConfigurationDlg.m_Q22;
        m_Q23 = ConfigurationDlg.m_Q23;
        m_Q31 = ConfigurationDlg.m_Q31;
        m_Q32 = ConfigurationDlg.m_Q32;
        m_Q33 = ConfigurationDlg.m_Q33;

        m_R11 = ConfigurationDlg.m_R11;
        m_R12 = ConfigurationDlg.m_R12;
        m_R13 = ConfigurationDlg.m_R13;
        m_R21 = ConfigurationDlg.m_R21;
        m_R22 = ConfigurationDlg.m_R22;
        m_R23 = ConfigurationDlg.m_R23;
        m_R31 = ConfigurationDlg.m_R31;
        m_R32 = ConfigurationDlg.m_R32;
        m_R33 = ConfigurationDlg.m_R33;

        m_G11 = ConfigurationDlg.m_G11;
        m_G12 = ConfigurationDlg.m_G12;
        m_G13 = ConfigurationDlg.m_G13;
        m_G21 = ConfigurationDlg.m_G21;
        m_G22 = ConfigurationDlg.m_G22;
        m_G23 = ConfigurationDlg.m_G23;
        m_G31 = ConfigurationDlg.m_G31;
        m_G32 = ConfigurationDlg.m_G32;
        m_G33 = ConfigurationDlg.m_G33;
        m_G41 = ConfigurationDlg.m_G41;
        m_G42 = ConfigurationDlg.m_G42;
        m_G43 = ConfigurationDlg.m_G43;
        m_G51 = ConfigurationDlg.m_G51;
        m_G52 = ConfigurationDlg.m_G52;
        m_G53 = ConfigurationDlg.m_G53;
        m_G61 = ConfigurationDlg.m_G61;
        m_G62 = ConfigurationDlg.m_G62;
        m_G63 = ConfigurationDlg.m_G63;
        m_bGsensorCalibrationSW = ConfigurationDlg.m_bGsensorCalibrationSW;

        m_bFirG = ConfigurationDlg.m_bFirG;
        m_bFirM = ConfigurationDlg.m_bFirM;


        m_fUAlpha = ConfigurationDlg.m_fUAlpha;
        m_nAccelerometerSel = (G_SENSOR)ConfigurationDlg.m_nGSelection;
        m_nMagnetometerSel = (M_SENSOR)ConfigurationDlg.m_nMSelection;

        m_moving=ConfigurationDlg.m_moving;
        m_datanumber=ConfigurationDlg.m_datanumber;
        m_datarange=ConfigurationDlg.m_datarange;
        m_hardstd=ConfigurationDlg.m_hardstd;
        m_softstd=ConfigurationDlg.m_softstd;
        m_pitchlimit=ConfigurationDlg.m_pitchlimit;

        m_GchangeTh = ConfigurationDlg.m_GchangeTh;
        m_MradiusTh = ConfigurationDlg.m_MradiusTh;

        m_nMdTh[0] = ConfigurationDlg.m_nMdThX;
        m_nMdTh[1] = ConfigurationDlg.m_nMdThY;
        m_nMdTh[2] = ConfigurationDlg.m_nMdThZ;
    }

    EndWaitCursor();
}

void CISenTekAssistantDlg::OnHelpAbout()
{
    // TODO: 在此加入您的命令處理常式程式碼
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}
