#include "stdafx.h"
#include "iSensors.h"
#include "iSensorsDlg.h"


#define WINDOWS_WIDTH 1100
#define WINDOWS_HEIGHT 680

void CiSensorsDlg::PreCreatePage(int iPage)
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
		}
	}
}

void CiSensorsDlg::InitializeGUI(void)
{
	CString tmpStr;

	//Size of Dialog	
	HDC dcTemp =::GetDC(NULL);
	int nScreenHeight =::GetDeviceCaps(dcTemp, VERTRES);
	int nScreenWidth =::GetDeviceCaps(dcTemp, HORZRES);
	::ReleaseDC(NULL, dcTemp);
	int AxilX = (nScreenWidth-WINDOWS_WIDTH)/2;
	int AxilY = (nScreenHeight-WINDOWS_HEIGHT)/2;
	SetWindowPos(&wndNoTopMost, AxilX, AxilY, WINDOWS_WIDTH, WINDOWS_HEIGHT, SWP_SHOWWINDOW);
	
	InitializePosition();
	//
	InitializePrettyPage();
	PreCreatePage(ACCELEROMETER);
	PreCreatePage(MAGNETOMETER);
	PreCreatePage(GYROSCOPE);

	CreateStatusBar();

	UpdateData(FALSE);
}

void CiSensorsDlg::MyExpandTree(HTREEITEM hTreeItem)
{
    if(!m_pageTree.ItemHasChildren(hTreeItem))
    {
        return;
    }
    HTREEITEM hNextItem = m_pageTree.GetChildItem(hTreeItem);
    while (hNextItem != NULL)
    {
        MyExpandTree(hNextItem);
        hNextItem = m_pageTree.GetNextItem(hNextItem, TVGN_NEXT);
    }
    m_pageTree.Expand(hTreeItem,TVE_EXPAND);
}

void CiSensorsDlg::InitializePrettyPage(void)
{
	/////////////////////////////////////////////////
	TreeCtrlImages.Create(IDB_TREE_BITMAP, 17, 3, RGB(255, 255, 255));//建立image list物件
	m_pageTree.SetImageList(&TreeCtrlImages, TVSIL_NORMAL);//將image list設定給IDC_DIR_TREE控制項

	long l = GetWindowLong(m_pageTree.m_hWnd, GWL_STYLE);
#if (_MSC_VER > 1100)
	l = l | TVS_TRACKSELECT ;
#else
	//#define TVS_TRACKSELECT         0x0200
	l = l | 0x0200;
#endif
	SetWindowLong(m_pageTree.m_hWnd, GWL_STYLE, l);
	
	// where will the dlgs live?
	m_boundingFrame.GetWindowRect(m_frameRect);
	ScreenToClient(m_frameRect);
	m_frameRect.DeflateRect(2,2);
	
	SetWindowText(m_csTitle);
	
	// set some styles for the pretty page indicator bar
	m_captionBar.m_textClr     = ::GetSysColor(COLOR_3DFACE);
	m_captionBar.m_fontWeight  = FW_BOLD;
	m_captionBar.m_fontSize    = 14;
	m_captionBar.m_csFontName  = "Verdana";
	USES_CONVERSION; 
	m_captionBar.SetConstantText(W2A(m_csConstantText));
	 
	// fill the tree. we'll create the pages as we need them
	for (int i=0;i<m_pages.GetSize();i++)
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
		ASSERT(pPS);
		ASSERT(pPS->pDlg);
		if (pPS)
		{
			TV_INSERTSTRUCT tvi;
			
			// find this node's parent...
			tvi.hParent = FindHTREEItemForDlg(pPS->pDlgParent);
			
			tvi.hInsertAfter = TVI_LAST;
			tvi.item.cchTextMax = 0;
			tvi.item.pszText = LPSTR_TEXTCALLBACK;
			tvi.item.lParam = (long)pPS;
			tvi.item.mask = TVIF_PARAM | TVIF_TEXT;
			
			HTREEITEM hTree = m_pageTree.InsertItem(&tvi);
			
			// keep track of the dlg's we've added (for parent selection)
			if (hTree)
			{
				DWORD dwTree = (DWORD)hTree;
				m_dlgMap.SetAt(pPS->pDlg, dwTree);
			}
		}
	}
	
	// start with page 0
	if (m_pStartPage==NULL)
	{
		if (ShowPage(0))
		{
			m_iCurPage = 0;   
		}

	} else {
		// find start page
		for (int i=0;i<m_pages.GetSize();i++)
		{
			pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
			ASSERT(pPS);
			if (pPS)
			{
				ASSERT(pPS->pDlg);
				if (pPS->pDlg == m_pStartPage)
				{
					ShowPage(i);
					m_iCurPage = i;
					break;
				}
			}
		}
	}

	//MyExpandTree(m_pageTree.GetRootItem());
	MyExpandTree(m_pageTree.GetPrevSiblingItem(m_pageTree.GetRootItem()));
}

void CiSensorsDlg::CreateStatusBar(void)
{
	//Status Bar
	CRect rect;
    GetClientRect(&rect);
    rect.top = rect.bottom - 16;
	int rc = m_StatusBar.Create(WS_CHILD | WS_BORDER | WS_VISIBLE /*| SBT_OWNERDRAW*/, rect, this, NULL);
	int strPartDim[] = { 180, 260, 340, 420, 800, 900 -1 };
	m_StatusBar.SetSimple(FALSE);
    m_StatusBar.SetMinHeight(16);
    m_StatusBar.SetParts(7, strPartDim);
	
	if(m_iSenI2c->GetI2cDevSel()==CISenI2cMaster::I2CDEV_NONE) {
		m_StatusBar.SetText(_T("I2C Master: None"), 0, 0);
	} else if(m_iSenI2c->GetI2cDevSel()==CISenI2cMaster::I2CDEV_AARDVARK) {
		m_StatusBar.SetText(_T("I2C Master: AARDVARK"), 0, 0);
	} else if(m_iSenI2c->GetI2cDevSel()==CISenI2cMaster::I2CDEV_IOW) {
		m_StatusBar.SetText(_T("I2C Master: IOW"), 0, 0);
	}
}

void CiSensorsDlg::InitializePosition(void)
{
	if(!m_SingleDlg) {
		GetDlgItem(IDC_TREE_PAGE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CAPTION_BAR)->SetWindowPos(NULL, 194, 0, 780, 26, SWP_SHOWWINDOW);
		GetDlgItem(IDC_STATIC_DLG_FRAME)->SetWindowPos(NULL, 194, 28, 888, 380, SWP_SHOWWINDOW);
		
		GetDlgItem(IDC_LIST_MSG)->ShowWindow(SW_SHOW);
	} else {
		GetDlgItem(IDC_TREE_PAGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CAPTION_BAR)->SetWindowPos(NULL, 2, 0, 640, 26, SWP_SHOWWINDOW);
		GetDlgItem(IDC_STATIC_DLG_FRAME)->SetWindowPos(NULL, 2, 28, 1080, 568, SWP_SHOWWINDOW);		
		GetDlgItem(IDC_LIST_MSG)->ShowWindow(SW_HIDE);
	}

}