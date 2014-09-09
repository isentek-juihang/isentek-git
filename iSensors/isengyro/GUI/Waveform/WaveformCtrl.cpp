// CWaveformCtrl.cpp : implementation file
//

#include "../stdafx.h"
#include "WaveformCtrl.h"


// CWaveformCtrl

IMPLEMENT_DYNAMIC(CWaveformCtrl, CStatic)

CWaveformCtrl::CWaveformCtrl() : m_uOffset(0)
	, m_StepX(0)
	, m_StepY(0)
	, m_TextAreaW(0)
	, m_ZeroPos(0)
	, m_MaxPos(0)
	, m_MinPos(0)
{
	m_pMemDC = NULL;
	m_pWaveformMemDC = NULL;
	
	for(int i=0; i<3; i++) {
		m_Data[i] = 0.0f;
		m_bEnableDraw[i] = true;
	}

	m_crBackGround = RGB(10, 10, 10);
	m_crGrids = RGB(100, 100, 100);

	COLORREF color[3] = {RGB(255, 0, 0),RGB(0, 255, 0),RGB(0, 0, 255)};
	SetPen(2, color);
	m_uOffset = 30;
}

CWaveformCtrl::~CWaveformCtrl()
{
	if(m_pMemDC)
		delete m_pMemDC;

	if(m_pWaveformMemDC)
		delete m_pWaveformMemDC;
}


BEGIN_MESSAGE_MAP(CWaveformCtrl, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CWaveformCtrl message handlers

BOOL CWaveformCtrl::Create()
{
	//Postcondition:	
	//	Returns TRUE if the function creates the control successfully
	//	or FALSE if it fails.
	
	m_pMemDC = new CDC;
	if(!m_pMemDC)
		return FALSE;

	m_pWaveformMemDC = new CDC;
	if(!m_pWaveformMemDC)
		return FALSE;

	// get our client rectangle
	GetClientRect(m_rcClient);

	m_StepX = (INT)(m_rcClient.Width() / 11);
	m_TextAreaW = m_StepX + (m_rcClient.Width() % 11);
	m_StepY = (UINT)(m_rcClient.Height() / 8);
	m_StepY1 = (UINT)(m_rcClient.Height() % 8);
	m_nFirstLinePos = m_StepX;

	m_rcWaveform = m_rcClient;
	m_rcWaveform.right = m_StepX*10;

	if(!InvalidateCtrl())
		return FALSE;
	
	return TRUE;
}

void CWaveformCtrl::SetRange(UINT offset)
{
	m_uOffset = offset;
	
	UpdateTextArea();
}

void CWaveformCtrl::UpdateTextArea(void)
{
	//Set the background color of the control
	CBrush bkBrush;
	if(!bkBrush.CreateSolidBrush(m_crBackGround))
		return;
	
	m_pMemDC->FillRect(m_rcClient, &bkBrush);

	CFont myFont;
	CString tmpStr;
	myFont.CreateFontW(m_StepY/2, m_StepX/5, 50, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
							CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_SWISS, _T("Times New Roman"));
	m_pMemDC->SelectObject(myFont);
	m_pMemDC->SetTextColor(RGB(255, 255, 255));
	m_pMemDC->SetBkMode(TRANSPARENT);

	tmpStr.Format(_T("%d"), 0);
	m_pMemDC->TextOutW((m_rcClient.left+10), (m_ZeroPos-((m_StepY/2)/2)), _T("0"));
	tmpStr.Format(_T("+%d"), m_uOffset);
	m_pMemDC->TextOutW(m_rcClient.left+10, m_MaxPos-((m_StepY/2)/2), tmpStr);
	tmpStr.Format(_T("-%d"), m_uOffset);
	m_pMemDC->TextOutW(m_rcClient.left+10, m_MinPos-((m_StepY/2)/2), tmpStr);

	m_pMemDC->BitBlt(m_TextAreaW, 0, m_rcWaveform.Width(), m_rcWaveform.Height(), m_pWaveformMemDC, 0, 0, SRCCOPY);
	
	InvalidateRect(m_rcClient);
}

BOOL CWaveformCtrl::InvalidateCtrl()
{
	//Postcondition:
	//	Paints the entire client area of the control
	//	Returns TRUE if it's done successfuly or FALSE if it fails

	CClientDC dc(this);		

	if(m_pMemDC->GetSafeHdc())
		return FALSE;

	if(m_pWaveformMemDC->GetSafeHdc())
		return FALSE;

	if(!m_pMemDC->CreateCompatibleDC(&dc))
		return FALSE;

	if(!m_pWaveformMemDC->CreateCompatibleDC(&dc))
		return FALSE;

	CBitmap bmp;
	if(!bmp.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height()))
		return FALSE;

	CBitmap bmp1;
	if(!bmp1.CreateCompatibleBitmap(&dc, m_rcWaveform.Width(), m_rcWaveform.Height()))
		return FALSE;
		
	if(!m_pMemDC->SelectObject(bmp))
		return FALSE;

	if(!m_pWaveformMemDC->SelectObject(bmp1))
		return FALSE;

	//Set the background color of the control
	CBrush bkBrush;
	if(!bkBrush.CreateSolidBrush(m_crBackGround))
		return FALSE;
	
	m_pMemDC->FillRect(m_rcClient, &bkBrush);
	m_pWaveformMemDC->FillRect(m_rcWaveform, &bkBrush);

	//Select a specified pen to the device context to draw background lines
	CPen bkLinesPen;
	if(!bkLinesPen.CreatePen(PS_SOLID, 1, m_crGrids))
		return FALSE;

	if(!m_pWaveformMemDC->SelectObject(bkLinesPen))
		return FALSE;	

	//Draw background lines	
#if 0
	register int i = m_rcClient.left - 1;
	for(int g = 0; g<11; g++)
	{
		m_pWaveformMemDC->MoveTo(i, m_rcClient.top);
		m_pWaveformMemDC->LineTo(i, m_rcClient.bottom);
		i += m_StepX;
	}
#endif
	register int j = m_rcClient.top - 1;	
	for(int i = 0; i<8; i++)
	{
		m_pWaveformMemDC->MoveTo(m_rcWaveform.left, j);
		m_pWaveformMemDC->LineTo(m_rcWaveform.right, j);
		j += m_StepY;
		if(i==0) {
			m_MaxPos = j;
		}
		if( i == 3 ) {
			m_ZeroPos = j;
		}
		if(i==6) {
			m_MinPos = j;
		}
	}
	
	CFont myFont;
	CString tmpStr;
	int ChatW;
	if((UINT)(m_StepX/5)>(m_StepY/2))
		ChatW = m_StepY/4;
	else
		ChatW = m_StepX/6;
	myFont.CreateFontW(m_StepY/2, ChatW/*m_StepX/5*/, 50, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
							CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_SWISS, _T("Consolas")/*_T("Times New Roman")*/);
	m_pMemDC->SelectObject(myFont);
	m_pMemDC->SetTextColor(RGB(255, 255, 255));
	m_pMemDC->SetBkMode(TRANSPARENT);

	tmpStr.Format(_T("%d"), 0);
	m_pMemDC->TextOutW((m_rcClient.left+10), (m_ZeroPos-((m_StepY/2)/2)), _T("0"));
	tmpStr.Format(_T("+%d"), m_uOffset);
	m_pMemDC->TextOutW(m_rcClient.left+10, m_MaxPos-((m_StepY/2)/2), tmpStr);
	tmpStr.Format(_T("-%d"), m_uOffset);
	m_pMemDC->TextOutW(m_rcClient.left+10, m_MinPos-((m_StepY/2)/2), tmpStr);

	for(int i = 0; i<3; i++) {
		m_yPreviousPos[i] = m_yPos[i] = m_ZeroPos;
	}
	m_pMemDC->BitBlt(m_TextAreaW, 0, m_rcWaveform.Width(), m_rcWaveform.Height(), m_pWaveformMemDC, 0, 0, SRCCOPY);
	
	InvalidateRect(m_rcClient);

	return TRUE;
}

void CWaveformCtrl::OnPaint()
{
	CPaintDC dc(this);	

	if(m_pMemDC->GetSafeHdc())
		dc.BitBlt(0, 0, m_rcClient.Width(), m_rcClient.Height(), m_pMemDC, 0, 0, SRCCOPY);
}

void CWaveformCtrl::DrawFunction()
{
	if(!m_pMemDC->GetSafeHdc())
		return;
	if(!m_pWaveformMemDC->GetSafeHdc())
		return;

	CRect bkRect(m_rcWaveform.right - MOVE_PIXEL, m_rcWaveform.top, m_rcWaveform.right, m_rcWaveform.bottom);

	CBrush bkBrush;
	bkBrush.CreateSolidBrush(m_crBackGround);
	m_pWaveformMemDC->FillRect(bkRect, &bkBrush);

	m_pWaveformMemDC->BitBlt(0, 0, m_rcWaveform.Width(), m_rcWaveform.Height(), m_pWaveformMemDC, MOVE_PIXEL, 0, SRCCOPY);

	CPen myPen;
	myPen.CreatePen(PS_SOLID, 1, m_crGrids);
	m_pWaveformMemDC->SelectObject(myPen);

#if 0
	m_nFirstLinePos -= MOVE_PIXEL;
	if(m_nFirstLinePos < 0)
		m_nFirstLinePos += m_StepX;

	int nX = m_rcClient.right - ((m_rcClient.right - m_nFirstLinePos) % m_StepX) - 1;
	m_pWaveformMemDC->MoveTo(nX, m_rcClient.top);
	m_pWaveformMemDC->LineTo(nX, m_rcClient.bottom);	
#endif

	register int j = m_rcClient.top - 1;
	for(int i = 0; i<8; i++)
	{
		m_pWaveformMemDC->MoveTo(m_rcWaveform.left, j);
		m_pWaveformMemDC->LineTo(m_rcWaveform.right, j);
		j += m_StepY;
	}
#if 1
	for(int i=0; i<3; i++) {
		if(m_bEnableDraw[i]) {
			m_yPos[i] = ((int)m_ZeroPos - (int)(m_Data[i] * (float)(m_ZeroPos-m_MaxPos)/ (float)m_uOffset));
	
			m_pWaveformMemDC->SelectObject(m_colorPen[i]);
			m_pWaveformMemDC->MoveTo(m_rcWaveform.right - 11 - MOVE_PIXEL, m_yPreviousPos[i]);
			m_pWaveformMemDC->LineTo(m_rcWaveform.right - 11, m_yPos[i]);
	
			m_yPreviousPos[i] = m_yPos[i];
		}
	}
#endif

	m_pMemDC->BitBlt(m_TextAreaW, 0, m_rcWaveform.Width(), m_rcWaveform.Height(), m_pWaveformMemDC, 0, 0, SRCCOPY);
	
	Invalidate();
}

void CWaveformCtrl::SetData(float fX, float fY, float fZ)
{
	m_Data[X] = fX;
	m_Data[Y] = fY;
	m_Data[Z] = fZ;
	DrawFunction();
}

void CWaveformCtrl::SetPen(int nWidth, COLORREF crColor[3])
{
	for(int i=0; i<3; i++) {
		m_colorPen[i].DeleteObject();
		m_colorPen[i].CreatePen(PS_SOLID, nWidth, crColor[i]);
	}
}

void CWaveformCtrl::SetEnable(bool bX, bool bY, bool bZ)
{
	m_bEnableDraw[CWaveformCtrl::X] = bX;
	m_bEnableDraw[CWaveformCtrl::Y] = bY;
	m_bEnableDraw[CWaveformCtrl::Z] = bZ;

}


