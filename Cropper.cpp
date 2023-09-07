
// Cropper.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Cropper.h"
#include "MainFrm.h"
#include "Utils.h"

#include "CropperDoc.h"
#include "LeftView.h"
#include "SpecDlg.h"
#include "ReminderDlg.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CLinkURL - helper class used on about box

CLinkURL::CLinkURL()
{
	m_Font.CreateFont(0, 0, 0, 0, FW_BOLD, 0, TRUE, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_SCRIPT, "MS Sans Serif");
}

CLinkURL::~CLinkURL()
{
	m_Font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CLinkURL, CStatic)
	//{{AFX_MSG_MAP(CLinkURL)
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
    ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkURL message handlers

void CLinkURL::OnLButtonUp(UINT nFlags, CPoint point) 
{
	HINSTANCE hResult;
	// Go to the web site. If m_sURL is empty, use the control's text
	if (m_sURL.GetLength() <= 0)
		GetWindowText(m_sURL);
	if (m_sURL.GetLength() > 0)
		hResult = ShellExecute(NULL, "open", m_sURL, NULL, NULL, SW_SHOWNORMAL);
	if (hResult <= (HINSTANCE) 32)
		AfxMessageBox("Start your web browser and visit http://rajeware.com", IDOK, 0);
	CStatic::OnLButtonUp(nFlags, point);
}

BOOL CLinkURL::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// Change the cursor to a hand
	HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_LINK);
	ASSERT(hCursor);
	::SetCursor(hCursor);
	
	return TRUE;
}


void CLinkURL::PreSubclassWindow() 
{
	SetFont(&m_Font);	
	CStatic::PreSubclassWindow();
}
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for our fancy about box

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HBRUSH m_hBackBrush;
	CLinkURL m_Mail;
	CLinkURL m_Link;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_hBackBrush = CreateSolidBrush(RGB(255, 255, 0));
	// make our hyperlink
	m_Link.SubclassDlgItem(IDC_URL, this);
	m_Link.SetWindowText("http://www.rajeware.com");
	m_Link.m_sURL.LoadString(IDS_WEBSITE_URL);

	// Set our version
	CString sVersion;
    CString sBuild;
	sVersion.LoadString(IDS_VERSION);
    // Set the build
    sBuild = "  Build: TM";
#ifdef PROTECTED
    sBuild.Format("  Build: ST%d", TRIALTIME);
#endif
#ifdef REMINDER
    sBuild.Format("  Build: TR%d", TRIALTIME);
#endif
    sVersion+=sBuild;
	CStatic* pStatic = ((CStatic*) GetDlgItem(IDC_VERSION));
	pStatic->SetWindowText(sVersion);

#ifdef PROTECTED
    // Are we registered?
    CString sLabel;
    CString sValue;
    CRegistry oReg(CRegistry::Machine, "RAJE Software", "FitToList");
    sValue = oReg.GetString("License", "User");
    if (sValue.GetLength() > 0)
    {
        sLabel = "Licensed to: " + sValue;
        sValue = oReg.GetString("License", "Company");
        sLabel += "  " + sValue;
        pStatic = (CStatic*) GetDlgItem(IDC_LICENSE);
        pStatic->SetWindowText(sLabel);
    }
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr;
	
	hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_URL)
		pDC->SetTextColor(RGB(0, 128, 0));

	return hbr;
}

/////////////////////////////////////////////////////////////////////////////
// CCropperApp

BEGIN_MESSAGE_MAP(CCropperApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_COMMAND(ID_FILE_OPENPROCESSED, OnFileOpenprocessed)
END_MESSAGE_MAP()


// CCropperApp construction

CCropperApp::CCropperApp()
: m_nSplitterPos(0)
{
	EnableHtmlHelp();
    m_bUpdate = false;
}


// The one and only CCropperApp object

CCropperApp theApp;

// CCropperApp initialization

BOOL CCropperApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	SetRegistryKey(_T("RAJE Software\\Picture Editor"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
    // Read our settings
	m_nSplitterPos = GetProfileInt("Settings", "Splitter", 0);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCropperDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSelectionView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

    // Reminder screen - do not display if we have a key

    if (!ConfigSplitter(true))
        return FALSE;       // Still calls ExitInstance, but m_bUpdate will be false

	// Enable DDE Execute open
	//EnableShellOpen();
	//RegisterShellFileTypes(TRUE);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
 }

// App command to run the dialog
void CCropperApp::OnAppAbout()
{
	//CAboutDlg aboutDlg;
    CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CCropperApp message handlers
int CCropperApp::ExitInstance()
{
    // Save our settings
    WriteProfileInt("Settings", "Splitter", m_nSplitterPos);

    return CWinApp::ExitInstance();
}

// Set a new source directory
void CCropperApp::OnFileOpen()
{
    CString sDir;
    bool bInd;

    POSITION pos = GetFirstDocTemplatePosition();
    CDocTemplate* pTemplate = GetNextDocTemplate(pos);
    pos = pTemplate->GetFirstDocPosition();
    CCropperDoc* pDoc = (CCropperDoc*) pTemplate->GetNextDoc(pos);

    if (SelectDirectory(AfxGetMainWnd(), "Select the folder containing the pictures you want to process",
        sDir, BIF_NONEWFOLDERBUTTON)) {
        bInd = pDoc->SetSourceDir(sDir);
        if (bInd)
            AddToRecentFileList(pDoc->m_sSourceDir);
        else
            AfxMessageBox(IDS_NOPICTURES);
    }
}

// Set a new target directory
void CCropperApp::OnFileOpenprocessed()
{
    CString sDir;
    POSITION pos = GetFirstDocTemplatePosition();
    CDocTemplate* pTemplate = GetNextDocTemplate(pos);
    pos = pTemplate->GetFirstDocPosition();
    CCropperDoc* pDoc = (CCropperDoc*) pTemplate->GetNextDoc(pos);
    // Display our dialog
    CSpecDlg dlg;
    dlg.m_sProcessFolder = pDoc->m_sUserDir;
    dlg.m_sSourceFolder = pDoc->m_sSourceDir;
    dlg.m_nProcessFolder = pDoc->m_nFolderType;
    dlg.m_nHeight = pDoc->m_nSaveHeight;
    dlg.m_nWidth = pDoc->m_nSaveWidth;
    dlg.m_nUnits = pDoc->m_nSaveUnits;
    if (dlg.DoModal() == IDOK)
    {
        // Retain our values
        pDoc->SetProcessDir(dlg.m_nProcessFolder, dlg.m_sProcessFolder);
        pDoc->m_nSaveHeight = dlg.m_nHeight;
        pDoc->m_nSaveWidth = dlg.m_nWidth;
        pDoc->m_nSaveUnits = dlg.m_nUnits;
        pDoc->m_SaveArea =
            pDoc->CalcFrame(dlg.m_nHeight, dlg.m_nWidth, dlg.m_nUnits);
    }
}

CDocument* CCropperApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
    // This could be called by drag and drop
    TRACE("File is: %s\n", lpszFileName);
    POSITION pos = GetFirstDocTemplatePosition();
    CDocTemplate* pTemplate = GetNextDocTemplate(pos);
    pos = pTemplate->GetFirstDocPosition();
    CCropperDoc* pDoc = (CCropperDoc*) pTemplate->GetNextDoc(pos);
    if (!pDoc->SetSourceDir(lpszFileName))
        AfxMessageBox(IDS_NOPICTURES);
    return pDoc;
}

BOOL CCropperApp::SelectDirectory(CWnd* parent, LPCTSTR sTitle, CString& sDirectory, UINT nFlag) 
{
    // Display a dialog to allow the user to change directories.
	// This call is only available for Windows95 and new
	// version of NT.
	BOOL bInd = FALSE;
	BROWSEINFO oDirStruct;
    char sDisplayName[_MAX_PATH];

	// Get a pidlRoot
    IMalloc* pMalloc;
    if (SHGetMalloc(&pMalloc) != NOERROR)
    {
       TRACE(_T("Failed to get pointer to shells task allocator"));
       return FALSE;
    }
	LPITEMIDLIST pidlRoot;
	// Restrict to My Computer
	SHGetSpecialFolderLocation(parent->m_hWnd, CSIDL_DRIVES, &pidlRoot); 

	oDirStruct.hwndOwner = parent->m_hWnd;
	oDirStruct.lpszTitle = sTitle;
	oDirStruct.pszDisplayName = sDisplayName;
 	oDirStruct.pidlRoot = pidlRoot;
	oDirStruct.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_UAHINT;
    if (nFlag > 0)
        oDirStruct.ulFlags |= nFlag;
	oDirStruct.lpfn = NULL;
	oDirStruct.lParam = 0;
	LPITEMIDLIST pItemIDList = SHBrowseForFolder(&oDirStruct);
	// We are done with the root
	if(pidlRoot)
		pMalloc->Free(pidlRoot);

    if (pItemIDList)
    {
      char sPath[_MAX_PATH];
      if (SHGetPathFromIDList(pItemIDList, sPath))
	  {
		  sDirectory = sPath;
		  bInd = TRUE;
	  }

      //avoid memory leaks by deleting the PIDL
      //using the shells task allocator
      pMalloc->Free(pItemIDList);
      if (pMalloc)
        pMalloc->Release();
    }

	return bInd;
}

// Return zero if the file does not exist. Return 1 if file is found
// Return -1 if file exists but is read only.
int CCropperApp::ChkFileStatus(LPCSTR sFile, long* nSize)
{
    int nStatus;
    long nBytes = 0L;
    CFile oFile;
    CFileStatus oStatus;

    if (oFile.Open(sFile, CFile::modeRead | CFile::shareDenyNone, NULL))
    {
        // A bug in GetStatus sometimes causes it to throw an exception
        // Seems to happen on CDROM drive where drive letter has been changed.
        try {
        oFile.GetStatus(oStatus);
		if (oStatus.m_attribute & CFile::readOnly)
            nStatus = -1;  // Read Only
        else
            nStatus = 1;   // Read Write
        }
        catch (CException* e) {
            e->Delete();
            nStatus = -1;   // Assume read only
        }
        nBytes = (long) oStatus.m_size;
        oFile.Close();
    }
    else
        nStatus = 0;       // File does not exist

    if (nSize != NULL)
        *nSize = nBytes;

    return nStatus;
}

// Do we show a reminder screen?
bool CCropperApp::ConfigSplitter(bool bStartup)
{
    bool bInd = true;
#ifdef PROTECTED
    CReminderDlg dlg;
    int nSize;
    CString sCompany;
    CString sUser;
    CString sKey;

    // Read our keys
    CRegistry oReg(CRegistry::Machine, "RAJE Software", "FitToList");
    sCompany = oReg.GetString("License", "Company");
    sUser = oReg.GetString("License", "User");
    sKey = oReg.GetString("License", "Key");

    if (TRIALTIME > 30)
        return false;
    nSize = CalcSize();
    if (nSize <= TRIALTIME)
    {
        if (IsVisible())
            return true;
    }
    if (nSize > TRIALTIME)
        nSize = -1;
    if (bStartup)
        dlg.SetState(CReminderDlg::Startup, nSize);
    else
        dlg.SetState(CReminderDlg::Shutdown, nSize);
#ifndef REMINDER
    if (nSize < 0)
#endif
    dlg.DoModal();
    if (nSize < 0)         // We are expired. Exit the program
        bInd = false;
    if (bInd)
        bInd = LeaveDropping("FitToList", true);
#endif
    return bInd;        
}

// Calculate days remaining
int CCropperApp::CalcSize(void)
{
    int nDays;
    FILETIME ft;
    SYSTEMTIME st;
    CString sKey;
    COleDateTimeSpan oDiff;
    bool bError = false;

    // Read our keys
    CRegistry oReg(CRegistry::Machine, "RAJE Software", "FitToList");
    // Last Run
    COleDateTime oLastRun;
    if (!GetDropping("FitToList", &oLastRun))
        bError = true;
    if (oLastRun.GetStatus() == COleDateTime::invalid)
        bError = true;

    ::GetSystemTime(&st);
    COleDateTime oNow(st);
    
    // Have we been backclocked?
    if (!bError)
    {
        oDiff = oNow - oLastRun;
        if (oDiff.GetTotalHours() < -4)
            bError = true;
    }
    // Get our install date
    sKey = oReg.GetString("CropSet", "PosX");
    if (sscanf_s(sKey, "%x", &(ft.dwHighDateTime)) != 1)
        bError = true;
    sKey = oReg.GetString("CropSet", "PosY");
    if (sscanf_s(sKey, "%x", &(ft.dwLowDateTime)) != 1)
        bError = true;
    // CRC check for install date
    int nSalt = oReg.GetInt("License", "Salt", 0);
    if (nSalt != (ft.dwHighDateTime & ft.dwLowDateTime))
        bError = true;

    COleDateTime oInstall(ft);
    if (oInstall.GetStatus() == COleDateTime::invalid)
        bError = true;

    // Have we been backclocked?
    if (!bError)
    {
        oDiff = oLastRun - oInstall;
        if (oDiff.GetTotalSeconds() < 0)
            bError = true;
    }

    if (!bError)
    {
        // All time are universal, not local
        FileTimeToSystemTime(&ft, &st);
        //SystemTimeToTzSpecificLocalTime(NULL, &st, &local);
        COleDateTime oInstall(st);
        ::GetSystemTime(&st);
        //SystemTimeToTzSpecificLocalTime(NULL, &st, &local);
        COleDateTime oNow(st);
        oNow.SetDate(oNow.GetYear(), oNow.GetMonth(), oNow.GetDay());
        oInstall.SetDate(oInstall.GetYear(), oInstall.GetMonth(), oInstall.GetDay());
        oDiff = oNow - oInstall;
        nDays = (int) (TRIALTIME - oDiff.GetTotalDays());
    }
    else
        nDays = TRIALTIME + 1;   // Error
       
    return nDays;
}

// Are we registered?
bool CCropperApp::IsVisible(void)
{
    bool bInd = true;
#ifdef PROTECTED
    CString sUser;
    CString sCompany;
    CString sKey;

    GetKey(&sUser, &sCompany, &sKey);
    // Do a few checks
    int n = 5;
    int m = 62;
    CString sWord = sUser + sCompany;
    if (sWord.GetLength() > 23)
    {
        n = (sWord.GetLength() + sWord[6]) % 26;
        if (sKey.GetLength() > 13)
            m = sKey[13];
    }
    if (n + 65 == m)
        n = GenMod(sKey);
    else
        n = 6;

    // Check position 0  - 4/15/05
    m = (sUser[0]+3)%26+65;
    if (m == sKey[0])
        n++;
    if (n < 10)
        bInd = false;

    // Check position 1 - 11/26/05
    m = (sUser[1]+0)%26+65;
    if (m == sKey[1])
        n++;
    if (n < 11)
        bInd = false;
#endif
    return bInd;
}
