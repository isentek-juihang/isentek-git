
// iSensorsDlg.h : header file
//

#pragma once

//#include "./GUI/SA/SAPrefsStatic.h"
#include "SAPrefsStatic.h"
//#include "./GUI/SA/SAPrefsSubDlg.h"

#include "./Interface/iSenInterfaceLinker.h"
#include "./Interface/I2C/ISenI2cMaster.h"
#include "I2cMasterOperation.h"
#include "afxwin.h"

#include "./SubDlgs/SubDlgsHeader.h"

#define FILE_BUFFER_SIZE 0x1000

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

// CiSensorsDlg dialog
class CiSensorsDlg : public CDialogEx
{
// Construction
public:
	CiSensorsDlg(CWnd* pParent = NULL);	// standard constructor
	~CiSensorsDlg();

// Dialog Data
	enum { IDD = IDD_ISENSORS_DIALOG };

	enum  SUBPAGE{ ACCELEROMETER, ACCELEROMETER_DATA, ACCELEROMETER_APP,
				   MAGNETOMETER, MAGNETOMETER_DATA, MAGNETOMETER_APP,
				   GYROSCOPE, GYROSCOPE_DATA, GYROSCOPE_APP,
				   COMPASS, AIRMOUSE, IMAGE	};

	CStatic	m_boundingFrame;

	// dialog title
	void SetTitle(CString t)   {m_csTitle = t;}

	// used in the pretty shaded static control
	void SetConstantText(CString t)   {m_csConstantText = t;}

	// if you set this, UpdateData will be called on the current page before
	// switching to the new page. if UpdateData fails, the page is not changed.
	void SetValidateBeforeChangingPages(bool b)	{m_bValidateBeforeChangingPage = b;}

	// add a page (page, page title, optional parent)
	bool AddPage(CSAPrefsSubDlg &page, const char *pCaption, CSAPrefsSubDlg *pDlgParent = NULL);
	
	// show a page
	bool ShowPage(int iPage);
	bool ShowPage(CSAPrefsSubDlg * pPage);
	
	// end the dialog with a special return code
	void EndSpecial(UINT res, bool bOk = true);
	
	// set the first page
	void SetStartPage(CSAPrefsSubDlg *pPage = NULL) {m_pStartPage = pPage;}

	void PreCreatePage(int iPage); //Added by David

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	

// Implementation
protected:
	HICON m_hIcon;

	bool EndOK();

	// if you don't like this, you can replace it with a static
	CSAPrefsStatic	m_captionBar;
	CTreeCtrl	    m_pageTree;
	CImageList		TreeCtrlImages;

	// check to see if this dlg has already been added to the tree
	HTREEITEM FindHTREEItemForDlg(CSAPrefsSubDlg *pParent);

	//virtual void OnOK();
	//virtual void OnCancel();
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

	CSAPrefsSubDlg	*m_pStartPage;

	// store info about *pDlgs that have been added to 
	// the tree - used for quick lookup of parent nodes
	// DWORDs are used because HTREEITEMs can't be... blame Microsoft
	CMap< CSAPrefsSubDlg *, CSAPrefsSubDlg *, DWORD, DWORD&  > m_dlgMap;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	///////////////Menu/////////////////////////
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	//Setting	
	afx_msg void OnSettingI2cUsb();
	afx_msg void OnSettingSingledialog();
	afx_msg void OnUpdateSettingSingledialog(CCmdUI *pCmdUI);
	//Help
	afx_msg void OnHelpAboutIsensors();
	
	//////////////End of Menu///////////////////
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();	
	DECLARE_MESSAGE_MAP()
public:

	inline int GetCurrentPage(void) { return m_iCurPage;}
	
	CButton m_StartingCtl;
	bool bStopSignal;
	HANDLE m_hThread;

	/**********GUI Function**********/	
	void InitializeGUI(void);
	void InitializePrettyPage(void);
	void InitializePosition(void);
	void CreateStatusBar(void);
	CStatusBarCtrl  m_StatusBar;
	BOOL m_SingleDlg;

	CListBox m_MsgListCtl;
	void __cdecl wPrintMsg( _In_z_ _Printf_format_string_ const wchar_t *format, ...);
	void __cdecl wPrintMsg( _In_z_ _Printf_format_string_ const wchar_t *owner, _In_z_ _Printf_format_string_ const wchar_t *format, ...);
	
	//IIC Master
	CISenI2cMaster *m_iSenI2c;
		
	//SubDlg
	CSubDlgAccelerometer	m_SubDlgAccelerometer;
	CSubDlgMagnetometer		m_SubDlgMagnetometer;
	CSubDlgGyroscope		m_SubDlgGyroscope;
	CSubDlgCompass			m_SubDlgCompass;

	CSubDlgAccelerometerData m_SubDlgAccelerometerData;
	CSubDlgAccelerometerApp m_SubDlgAccelerometerApp;

	CSubDlgMagnetometerData	m_SubDlgMagnetometerData;
	CSubDlgMagnetometerApp  m_SubDlgMagnetometerApp;

	CSubDlgGyroscopeData m_SubDlgGyroscopeData;
	CSubDlgGyroscopeApp m_SubDlgGyroscopeApp;

	void MyExpandTree(HTREEITEM hTreeItem);
	
};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

extern CiSensorsDlg *g_pMain;