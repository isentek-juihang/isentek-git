#pragma once


// CWaveformCtrl

class CWaveformCtrl : public CStatic
{
	DECLARE_DYNAMIC(CWaveformCtrl)

public:
	CWaveformCtrl();
	virtual ~CWaveformCtrl();	
	
	typedef enum AXIS_NAME{
		X = 0,	
		Y = 1,	
		Z = 2
	};

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	float m_Data[3];
	BOOL m_bEnableDraw[3];
	static const int MOVE_PIXEL = 1;

	INT m_yPos[3];
	INT m_yPreviousPos[3];
	INT m_nFirstLinePos;

	INT m_ZeroPos;
	INT m_MaxPos;
	INT m_MinPos;

	INT m_StepX;
	INT m_TextAreaW;
	UINT m_StepY;
	UINT m_StepY1;
	
	UINT m_uOffset;

	CRect m_rcClient;
	CRect m_rcWaveform;

	CPen m_colorPen[3];
	CDC *m_pMemDC;
	CDC *m_pWaveformMemDC;	

	COLORREF m_crBackGround;
	COLORREF m_crGrids;
	
	BOOL InvalidateCtrl();
	void DrawFunction();
	void UpdateTextArea(void);

public:
	BOOL Create();
	void SetData(float fX, float fY, float fZ);	
	void SetEnable(bool bX, bool bY, bool bZ);	
	void SetRange(UINT offset);
	void SetPen(int nWidth, COLORREF crColor[3]);
};


