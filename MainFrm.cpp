// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Cropper.h"
#include "Utils.h"
#include "MainFrm.h"
#include "LeftView.h"
#include "CropperView.h"
#include "CropperDoc.h"
#include "ProcessDlg.h"
#include "ReminderDlg.h"
#include "RegisterDlg.h"
#include "Registry.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
    ON_WM_DESTROY()
    ON_COMMAND(ID_VIEW_NEXT, OnViewNext)
    ON_COMMAND(ID_VIEW_PREVIOUS, OnViewPrevious)
    ON_COMMAND(ID_POSITION_TOP, OnPositionTop)
    ON_COMMAND(ID_UPDATE_DLGBAR, OnUpdateDlgBar)
    ON_WM_SIZE()
    ON_COMMAND(ID_VIEW_ORIGINALS, OnViewOriginals)
    ON_COMMAND(ID_VIEW_PROCESSED, OnViewProcessed)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
    ON_UPDATE_COMMAND_UI(ID_EDIT_TRUE, OnUpdateTrueSize)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SIZE, OnUpdateClientSize)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_POS, OnUpdateStatusBarPos)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_CROP, OnUpdateStatusBarCrop)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateToolBar)
    ON_UPDATE_COMMAND_UI(ID_EDIT_ROTATERIGHT, OnUpdateToolBar)
    ON_UPDATE_COMMAND_UI(ID_EDIT_ROTATELEFT, OnUpdateToolBar)
    ON_UPDATE_COMMAND_UI(ID_EDIT_ROTATEADJUST, OnUpdateToolBar)
    ON_UPDATE_COMMAND_UI(ID_EDIT_FLIPVERTICAL, OnUpdateToolBar)
    ON_UPDATE_COMMAND_UI(ID_EDIT_FLIPHORIZONTAL, OnUpdateToolBar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT, OnUpdateNextPrev)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PREVIOUS, OnUpdateNextPrev)
    ON_WM_DROPFILES()
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CROP,
    ID_INDICATOR_POS
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

    if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}
	CRect oSize;
    CFont myFont;
    CStatic* pEdit = (CStatic*) m_wndDlgBar.GetDlgItem(IDC_IMAGE_NAME);
	pEdit->GetWindowRect(oSize);
	myFont.CreateFont(oSize.Height(), 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_SCRIPT, "MS Sans Serif");
	pEdit->SetFont(&myFont);
    CStatic* pStatic = (CStatic*) m_wndDlgBar.GetDlgItem(IDC_ORIGINAL_CNT);
    pStatic->SetFont(&myFont);
    pStatic = (CStatic*) m_wndDlgBar.GetDlgItem(IDC_TARGET_CNT);
    pStatic->SetFont(&myFont);

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
    TEXTMETRIC tm;
    CClientDC dc(&m_wndStatusBar);
    dc.GetTextMetrics(&tm);
    m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_CROP, SBPS_NORMAL, 16*tm.tmAveCharWidth);
    m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_POS, SBPS_NORMAL, 16*tm.tmAveCharWidth);

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
    m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
    m_wndToolBar.SetButtonStyle(18, TBBS_DROPDOWN);  // View Processed, Originals

    if (Initialize())
	    return 0;
    else
    {
        AfxMessageBox("Your license key is invalid. Enter again or contact support@rajeware.com.");
        return -1;
    }
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CSelectionView), CSize(100, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CCropperView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
    // Set the splitter window position
    int nSize = ((CCropperApp*) AfxGetApp())->m_nSplitterPos;
    if (nSize > 0) {
	    m_wndSplitter.SetColumnInfo(0, nSize, 0);
	    m_wndSplitter.RecalcLayout();  
    }

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers


void CMainFrame::OnDestroy()
{
    // Save the position of our splitter window
	int nMin;
	int nSize;
	m_wndSplitter.GetColumnInfo(0, nSize, nMin);
    ((CCropperApp*) AfxGetApp())->m_nSplitterPos = nSize;
    CFrameWnd::OnDestroy();
}

void CMainFrame::OnViewNext()
{
    // Check Registration and go to next picture
    CSelectionView* pView = (CSelectionView*) m_wndSplitter.GetPane(0, 0);
    pView->OnViewNext();
}

void CMainFrame::OnViewPrevious()
{
    // Check registration and go to previous picture
    CSelectionView* pView = (CSelectionView*) m_wndSplitter.GetPane(0, 0);
    pView->OnViewPrevious();
}

void CMainFrame::OnPositionTop()
{
    // Go to first picture
    CSelectionView* pView = (CSelectionView*) m_wndSplitter.GetPane(0, 0);
    pView->OnPositionTop();
}

void CMainFrame::OnUpdateStatusBarPos(CCmdUI *pCmdUI)
{
    CCropperView* pView = (CCropperView*) m_wndSplitter.GetPane(0, 1);
    pView->OnUpdateStatusBarPos(pCmdUI);
}

void CMainFrame::OnUpdateStatusBarCrop(CCmdUI *pCmdUI)
{
    CCropperView* pView = (CCropperView*) m_wndSplitter.GetPane(0, 1);
    pView->OnUpdateStatusBarCrop(pCmdUI);
}

void CMainFrame::OnUpdateDlgBar()
{
    CString sText;
    CSelectionView* pView = (CSelectionView*) m_wndSplitter.GetPane(0, 0);
    CStatic* pEdit = (CStatic*) m_wndDlgBar.GetDlgItem(IDC_ORIGINAL_PATH);
    sText = "Originals Stored In: " + pView->GetDocument()->m_sSourceDir;
    pEdit->SetWindowText(sText);
    pEdit = (CStatic*) m_wndDlgBar.GetDlgItem(IDC_TARGET_PATH);
    sText = "Processed Copied To: " + pView->GetDocument()->GetDisplayDir();
    pEdit->SetWindowText(sText);
    pEdit = (CStatic*) m_wndDlgBar.GetDlgItem(IDC_IMAGE_NAME);
    pEdit->SetWindowText(pView->GetDocument()->DisplayName());
    pEdit = (CStatic*) m_wndDlgBar.GetDlgItem(IDC_IMAGE_INFO);
    CString sInfo;
    long nTime = (long) (pView->GetDocument()->GetImageSize() * 0.000141343 + 0.5);
    sInfo.Format("File Size (bytes): %d  Download (56K): %d seconds", 
        pView->GetDocument()->GetImageSize(), nTime);
    pEdit->SetWindowText(sInfo);

    CStatic* pStatic = (CStatic*) m_wndDlgBar.GetDlgItem(IDC_ORIGINAL_CNT);
    sInfo.Format("%d", pView->m_nOriginalCount);
    pStatic->SetWindowText(sInfo);

    pStatic = (CStatic*) m_wndDlgBar.GetDlgItem(IDC_TARGET_CNT);
    sInfo.Format("%d", pView->m_nProcessedCount);
    pStatic->SetWindowText(sInfo);

    // Update our tree view list
    pView->UpdateCounts();
    return;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);

    // Resize the two edit boxes containing our source and processed folders
    CEdit* pEdit;
    CRect oRect;
    int nLength;

    if (::IsWindow(m_wndDlgBar.m_hWnd)) {
        pEdit = (CEdit*) m_wndDlgBar.GetDlgItem(IDC_ORIGINAL_PATH);
        pEdit->GetWindowRect(&oRect);
        m_wndDlgBar.ScreenToClient(&oRect);
        nLength = cx - oRect.TopLeft().x - 30;
        pEdit->MoveWindow(oRect.TopLeft().x, oRect.TopLeft().y,
            nLength, oRect.Height(), 0);

        CEdit* pEdit = (CEdit*) m_wndDlgBar.GetDlgItem(IDC_TARGET_PATH);
        pEdit->GetWindowRect(&oRect);
        m_wndDlgBar.ScreenToClient(&oRect);
        pEdit->MoveWindow(oRect.TopLeft().x, oRect.TopLeft().y,
            nLength, oRect.Height(), 0);
    }
}

void CMainFrame::OnViewOriginals()
{
    CCropperView* pView = (CCropperView*) m_wndSplitter.GetPane(0, 1);
 	ShellExecute(NULL, "open", pView->GetDocument()->m_sSourceDir, 
        NULL, NULL, SW_SHOWNORMAL | SW_RESTORE);
}

void CMainFrame::OnViewProcessed()
{
    CCropperView* pView = (CCropperView*) m_wndSplitter.GetPane(0, 1);
 	ShellExecute(NULL, "open", pView->GetDocument()->m_sProcessDir,
        NULL, NULL, SW_SHOWNORMAL);
}

// Set check marks on the main menu as needed
void CMainFrame::OnUpdateTrueSize(CCmdUI *pCmdUI)
{
    CCropperView* pView = (CCropperView*) m_wndSplitter.GetPane(0, 1);
    int nState = pView->GetDocument()->CurrentState();
    if (nState == nORIGINAL || nState == nPROCESSED)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);

    if (pView->GetDocument()->m_bClientResize)
        pCmdUI->SetCheck(0);
    else
        pCmdUI->SetCheck(1);
}

void CMainFrame::OnUpdateClientSize(CCmdUI *pCmdUI)
{
    CCropperView* pView = (CCropperView*) m_wndSplitter.GetPane(0, 1);
    int nState = pView->GetDocument()->CurrentState();
    if (nState == nORIGINAL || nState == nPROCESSED)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);

    if (pView->GetDocument()->m_bClientResize)
        pCmdUI->SetCheck(1);
    else
        pCmdUI->SetCheck(0);
}

void CMainFrame::OnUpdateToolBar(CCmdUI* pCmdUI)
{
    CCropperView* pView = (CCropperView*) m_wndSplitter.GetPane(0, 1);
    int nState = pView->GetDocument()->CurrentState();
    if (nState == nORIGINAL || nState == nPROCESSED)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
    return;
}

void CMainFrame::OnUpdateNextPrev(CCmdUI* pCmdUI)
{
    CCropperView* pView = (CCropperView*) m_wndSplitter.GetPane(0, 1);
    int nState = pView->GetDocument()->CurrentState();
    if (nState == nEMPTY)
        pCmdUI->Enable(FALSE);
    else
        pCmdUI->Enable(TRUE);
    return;
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
    // Do not let them drag multiple targets
    CCropperView* pView = (CCropperView*) m_wndSplitter.GetPane(0, 1);
    char sFile[MAX_PATH];
    int nNumDropped;

    nNumDropped = DragQueryFile(hDropInfo, 0xFFFFFFFF, sFile, MAX_PATH);
    if (nNumDropped > 1)
    {
        AfxMessageBox(IDS_DRAGDROP);
        return;
    }
    else 
        DragQueryFile(hDropInfo, 0, sFile, MAX_PATH);

    if (((CCropperApp*) AfxGetApp())->ChkFileStatus(sFile) == 0)
        CFrameWnd::OnDropFiles(hDropInfo);   // They dropped a single folder
    else
    {
        // If they dropped a file, get the file's folder
        CString sDir = PathName(sFile);
        if (!pView->GetDocument()->SetSourceDir(sDir))
            AfxMessageBox(IDS_NOPICTURES);
    }
}

void CMainFrame::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	// this function handles the dropdown menus from the toolbar
	NMTOOLBAR* pNMToolBar = (NMTOOLBAR*)pNotifyStruct;
	CRect rect;
	CMenu menu;
	CMenu* pPopup;

	// translate the current toolbar item rectangle into screen coordinates
	// so that we'll know where to pop up the menu
	m_wndToolBar.GetToolBarCtrl().GetRect(pNMToolBar->iItem, &rect);
	rect.top = rect.bottom;
	::ClientToScreen(pNMToolBar->hdr.hwndFrom, &rect.TopLeft());
    switch(pNMToolBar->iItem) {
        case ID_VIEW_PROCESSED:
		    menu.LoadMenu(IDR_VIEW_POPUP);
		    pPopup = menu.GetSubMenu(0);
            // Set the first item as a default
            ::SetMenuDefaultItem(pPopup->m_hMenu, ID_VIEW_PROCESSED, false);
            break;
    }
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.top + 1, AfxGetMainWnd());
	*pResult = TBDDRET_DEFAULT;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    // Do not bugger up the title bar
    cs.style &= ~FWS_ADDTOTITLE;
    return CFrameWnd::PreCreateWindow(cs);
}


BOOL CMainFrame::DestroyWindow()
{
    ((CCropperApp*) AfxGetApp())->ConfigSplitter(false);

    return CFrameWnd::DestroyWindow();
}

// Check registration key
bool CMainFrame::Initialize(void)
{
    bool bInd = true;
#ifdef PROTECTED
    CString sC;
    CString sU;
    CString sK;

    // Read our keys
    GetKey(&sU, &sC, &sK);
    if (sK.IsEmpty()) 
        return bInd;
    
    sU += sC;
    // Do remaining checks. We already passed preliminary checks
    // Check key positions 0,1,2,32
    int n = (sU[0] + 3) %26;
    if (n != sK[0] - 65)
        bInd = false;
    n = sU[1] %26;
    if (n != sK[1] - 65)
        bInd = false;
    n = (sU[23] - 5) %10;
    if (n != sK[2] - 48)
        bInd = false;
    // Check end chars
    n = ((sU[10] + 27) % 26) + 65;
    if (sU[10] >= 48 && sU[10] <= 57)
        n = sU[10];
    if (n != sK[32])
        bInd = false;

    if (!bInd)
        // Screw up the registration
        LeaveDropping("FitToList", false);

#endif
    return bInd;
}

void CMainFrame::OnFileNew()
{
    // TODO: Add your command handler code here
    CRegisterDlg dlg;
    dlg.DoModal();
}
