#include "Total.h"

#define WINDOWS_WIDTH 1040
#define WINDOWS_HEIGHT 420

void CISenTekAssistantDlg::InitializeGUI(void)
{
	CString tmpStr;

	HDC dcTemp =::GetDC(NULL);
	int nScreenHeight =::GetDeviceCaps(dcTemp, VERTRES);
	int nScreenWidth =::GetDeviceCaps(dcTemp, HORZRES);
	::ReleaseDC(NULL, dcTemp);
	int AxilX = (nScreenWidth-WINDOWS_WIDTH)/2;
	int AxilY = (nScreenHeight-WINDOWS_HEIGHT)/2;
	SetWindowPos(&wndNoTopMost, AxilX, AxilY, WINDOWS_WIDTH, WINDOWS_HEIGHT, SWP_SHOWWINDOW);
	
	InitializePrettyPage();

	UpdateData(FALSE);
}

void CISenTekAssistantDlg::InitializePrettyPage(void)
{
	/////////////////////////////////////////////////
	long l = GetWindowLong(m_pageTree.m_hWnd, GWL_STYLE);
#if (_MSC_VER > 1100)
	l = l | TVS_TRACKSELECT ;
#else
	//#define TVS_TRACKSELECT         0x0200
	l = l | 0x0200;
#endif
	SetWindowLong(m_pageTree.m_hWnd, GWL_STYLE, l);
	
	m_boundingFrame.GetWindowRect(m_frameRect);
	ScreenToClient(m_frameRect);
	m_frameRect.DeflateRect(2,2);
	
	SetWindowText(m_csTitle);
	
	m_captionBar.m_textClr     = ::GetSysColor(COLOR_3DFACE);
	m_captionBar.m_fontWeight  = FW_BOLD;
	m_captionBar.m_fontSize    = 14;
	m_captionBar.m_csFontName  = "Verdana";
	USES_CONVERSION; 
	m_captionBar.SetConstantText(W2A(m_csConstantText));
	
	for (int i=0;i<m_pages.GetSize();i++)
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
		ASSERT(pPS);
		ASSERT(pPS->pDlg);
		if (pPS)
		{
			TV_INSERTSTRUCT tvi;
			
			tvi.hParent = FindHTREEItemForDlg(pPS->pDlgParent);
			
			tvi.hInsertAfter = TVI_LAST;
			tvi.item.cchTextMax = 0;
			tvi.item.pszText = LPSTR_TEXTCALLBACK;
			tvi.item.lParam = (long)pPS;
			tvi.item.mask = TVIF_PARAM | TVIF_TEXT;
			
			HTREEITEM hTree = m_pageTree.InsertItem(&tvi);
			
			if (hTree)
			{
				DWORD dwTree = (DWORD)hTree;
				m_dlgMap.SetAt(pPS->pDlg, dwTree);
			}
		}
	}
	
	if (m_pStartPage==NULL)
	{
		if (ShowPage(0))
		{
			m_iCurPage = 0;   
		}
	}
	else
	{
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
}

HBRUSH CISenTekAssistantDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if( pWnd->GetDlgCtrlID() == IDC_EDIT_MSG)
	{	
		pDC->SetTextColor(RGB(105, 255, 100));

		pDC->SetBkMode(TRANSPARENT);

		m_brush.DeleteObject();
		m_brush.CreateSolidBrush(RGB(0, 0, 0));

		hbr = m_brush;
	} 
	if (nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC) {
		pDC->SetBkMode(TRANSPARENT);

		m_brush.DeleteObject();
		m_brush.CreateSolidBrush(RGB(255, 255, 255));

		hbr = m_brush;
	}

	return hbr;
}

void CISenTekAssistantDlg::ShowTestLog(bool bShow)
{
	GetDlgItem(IDC_STATIC_LOG)->ShowWindow(bShow);
	GetDlgItem(IDC_LIST_MSG)->ShowWindow(bShow);
}
