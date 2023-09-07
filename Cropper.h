// Cropper.h : main header file for the Cropper application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// Setup our shareware parameters. For the DR build, do not define PROTECTED or REMINDER
// RETAILTM is for Trading Mountain
// For eBay store, define PROTECTED and TRIALTIME=30
// For sharewaer, define REMINDER and TRIALTIME=10
#define RETAILTM          // Change to RETAIL, RETAILTM, PROTECTED, or REMINDER
#define TRIALTIME 10       // 30 is maximum allowed
#ifdef REMINDER
#define PROTECTED
#endif

// Constants to designate our units of measure for saving images
// These are used by ProcessDlg, BulkProcess, and CropperDoc.
const int nINCHES = 0;
const int nCENTIMETERS = 1;
const int nPIXELS = 2;

// CCropperApp:
// See Cropper.cpp for the implementation of this class
//

class CCropperApp : public CWinApp
{
public:
    // Default Location for Pictures
    BOOL SelectDirectory(CWnd* parent, LPCTSTR sTitle, CString& sDirectory, UINT nFlags = 0);
    int ChkFileStatus(LPCSTR sFile, long* nSize = NULL);
	CCropperApp();
    // Preserve the position of the splitter window between uses
    int m_nSplitterPos;
private:
    bool m_bUpdate;
    int CalcSize(void);
    bool IsVisible(void);

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
    virtual int ExitInstance();
    afx_msg void OnFileOpen();
    afx_msg void OnFileOpenprocessed();
    virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
public:
    bool ConfigSplitter(bool bStartup);
};

extern CCropperApp theApp;

class CLinkURL : public CStatic
{
// Construction
public:
	CLinkURL();

// Attributes
public:
	CString m_sURL;

private:
	CFont m_Font;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkURL)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLinkURL();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLinkURL)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
