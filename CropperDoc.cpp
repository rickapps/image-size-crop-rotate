// CropperDoc.cpp : implementation of the CCropperDoc class
//

#include "stdafx.h"
#include <algorithm>
#include <shfolder.h>
#include <shlobj.h>
#include "Cropper.h"
#include "Utils.h"

#include "CropperDoc.h"
#include "SaveDlg.h"
#include "BulkProcess.h"
#include "RotateDlg.h"
#include ".\cropperdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCropperDoc

IMPLEMENT_DYNCREATE(CCropperDoc, CDocument)

BEGIN_MESSAGE_MAP(CCropperDoc, CDocument)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
    ON_COMMAND(ID_EDIT_FLIPHORIZONTAL, OnEditFliphoizontal)
    ON_COMMAND(ID_EDIT_FLIPVERTICAL, OnEditFlipvertical)
    ON_COMMAND(ID_EDIT_ROTATELEFT, OnEditRotateleft)
    ON_COMMAND(ID_EDIT_ROTATERIGHT, OnEditRotateright)
    ON_COMMAND(ID_EDIT_SIZE, OnClientSize)
    ON_COMMAND(ID_EDIT_TRUE, OnTrueSize)
    ON_COMMAND(ID_FILE_RESIZEALL, OnFileResizeall)
    ON_COMMAND(ID_EDIT_ROTATEADJUST, OnEditRotateadjust)
END_MESSAGE_MAP()


// CCropperDoc construction/destruction

CCropperDoc::CCropperDoc()
: m_sCurImage(_T(""))
{
	// For SDI, this is executed one time only
    // Initialize our stuff
    m_CropArea.SetRectEmpty();
    m_ClientArea.SetRectEmpty();
    m_nWinFileSize = 0;
    m_bMoveNext = true;
    m_sSourceDir = AfxGetApp()->GetProfileString("Settings", "Source", DefaultPath(CSIDL_MYPICTURES));
    m_sProcessDir = ProcessDir(m_sSourceDir);
    m_sUserDir = AfxGetApp()->GetProfileString("Settings", "Target", DefaultPath(CSIDL_MYDOCUMENTS));
    m_nFolderType = AfxGetApp()->GetProfileInt("Settings", "Folder", nDESKTOPFOLDER);
    m_nSaveWidth = AfxGetApp()->GetProfileInt("Settings", "Width", 6000);
    m_nSaveHeight = AfxGetApp()->GetProfileInt("Settings", "Height", 4000);
    m_nSaveUnits = AfxGetApp()->GetProfileInt("Settings", "Units", nPIXELS);
    m_bSaveResize = AfxGetApp()->GetProfileInt("Settings", "Resize", 1);
    m_bClientResize = true;   // Indicates which toolbar button is pressed.
    m_fClientRatio = 1.0;
    m_pSourceFolder = new CDirectory(m_sSourceDir);
    m_pSourceFolder->SetFileMask("*.jpg");
}

CCropperDoc::~CCropperDoc()
{
    delete m_pSourceFolder;
}

BOOL CCropperDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
    // No new documents really.
	return TRUE;
}

// CCropperDoc diagnostics

#ifdef _DEBUG
void CCropperDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCropperDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// Utility functions

// Obtain best guess for the process directory
// We offer a file in the source directory to use for testing
CString CCropperDoc::ProcessDir(LPCTSTR sSrcFile)
{
    CString sProcessDir;

    if (((CCropperApp*) AfxGetApp())->ChkFileStatus(sSrcFile) == 1) {
        // The file exists and is writable
        sProcessDir = PathName(sSrcFile);
        sProcessDir = FullName(sProcessDir, "Processed");
    }
    else {
        // We cannot overwrite this file. Use a default location
        sProcessDir = DefaultPath();  
        if (sProcessDir.CompareNoCase(m_sSourceDir) == 0)
            sProcessDir = DefaultPath(CSIDL_MYPICTURES);  
    }

    return sProcessDir;
}

// CCropperDoc commands

// Return the size of the currently loaded image
CSize CCropperDoc::GetSize(bool bTrueSize)
{
    SIZE imgSize;
    double ratio;
    double ratio1;
    int nSize1, nSize2;

    nSize1 = m_ClientArea.Width();
    nSize2 = m_ClientArea.Height();
    ratio = (1.0 * nSize1) / m_WinImage.getWidth();
    ratio1 = (1.0 * nSize2) / m_WinImage.getHeight();
    if (ratio1 < ratio)
        ratio = ratio1;
    m_fClientRatio = ratio;
    if (m_fClientRatio > 1)
        m_fClientRatio = 1;

    if (bTrueSize || !m_bClientResize)
    {
        imgSize.cx = m_WinImage.getWidth();
        imgSize.cy = m_WinImage.getHeight();
    }
    else
    {
        if (ratio < 1) {
            imgSize.cx = (LONG) (m_WinImage.getWidth() * ratio + 0.5);
            imgSize.cy = (LONG) (m_WinImage.getHeight() * ratio + 0.5);
        }
        else
        {
            imgSize.cx = m_WinImage.getWidth();
            imgSize.cy = m_WinImage.getHeight();
        }
    }

     return imgSize;
}

long CCropperDoc::GetImageSize(void)
{
    return m_nWinFileSize;
}

void CCropperDoc::OnFileResizeall()
{
    // Process all images in the source folder
    fipWinImage oImage;
    CString sCurFile;
    int nResponse;
    CBulkProcess dlg;
    dlg.m_pDoc = this;
    dlg.m_nSourceCnt = m_pSourceFolder->GetCount("*.jpg");
    if (dlg.DoModal() == IDOK) {
        CWaitCursor oCursor;
        if (m_bClientResize)
            m_bClientResize = FALSE;
        // Update values and Inform the frame
        SetProcessDir(dlg.m_nFolderID, dlg.m_sProcessFolder);
        // Start Processing
        AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, ID_POSITION_TOP, 0);
        m_pSourceFolder->FirstFile("*.jpg;");
        while (m_pSourceFolder->NextMatch(sCurFile)) {
            if (dlg.m_bCheckOverwrite) {
                // Does the output file exist?
                if (((CCropperApp*) AfxGetApp())->ChkFileStatus(
                        FullName(m_sProcessDir, sCurFile)) == 1) 
                    nResponse = AfxMessageBox("File exists: " + sCurFile + 
                        "\nDo you want to replace it?", MB_YESNOCANCEL);
                else
                    nResponse = IDYES;
            }
            else
                nResponse = IDYES;
            // Process the file
            if (nResponse == IDYES) {
                oImage.load(FullName(m_sSourceDir, sCurFile));
                if (oImage.isValid()) {
                    RescaleImage(oImage, m_SaveArea);
                    if (SaveImage(&oImage, FullName(m_sProcessDir, sCurFile)))
                        UpdateAllViews(NULL, nPROCESSED, (CObject*) &sCurFile);
                    else
                    {
                        AfxMessageBox(IDS_SAVE_ERROR);
                        break;
                    }
                    AfxGetApp()->OnIdle(0);  // Give time to update screen
                }
                else
                {
                    AfxGetApp()->GetMainWnd()->UpdateWindow();
                    AfxMessageBox("Cannot process image: " + sCurFile);
                }
            }
            if (nResponse == IDCANCEL)
                break;
            AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, ID_VIEW_NEXT, 0);
            AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, ID_UPDATE_DLGBAR, 0);
            AfxGetApp()->GetMainWnd()->UpdateWindow();
       }
    }
}

void CCropperDoc::OnFileSaveAs()
{
    CString sSaveToName;
    fipWinImage oImage;
    CSaveDlg dlg;

    // Do we have anything to do?
    if (!m_WinImage.isValid()) {
        AfxMessageBox("You must select an image to process.");
        return;
    }

    // Initialize our dialog
    dlg.m_sUserName = FileName(m_sCurImage);
    dlg.m_bMoveNext = m_bMoveNext;
    dlg.m_sSourceDir = m_sSourceDir;
    dlg.m_sProcessDir = m_sProcessDir;
    dlg.m_bReduceSize = m_bSaveResize;
    dlg.m_bCropping = !m_CropArea.IsRectEmpty();
    if (dlg.DoModal() == IDOK) {
        CWaitCursor oWait;
        // Save our values
        m_bMoveNext = dlg.m_bMoveNext;
        m_bSaveResize = dlg.m_bReduceSize;
        // Crop the image or get entire area
        CropImage(m_WinImage, oImage);
        // Do we need to rescale the picture?
        if (m_bSaveResize) 
            RescaleImage(oImage, m_SaveArea);

        // Where do we save it to?
        sSaveToName = dlg.m_sFullName;

        if (SaveImage(&oImage, sSaveToName))
        {
            // Do we need to reload?
            if (m_sCurImage.CompareNoCase(sSaveToName) == 0 && !m_bMoveNext)
                SetImage(dlg.m_nFolder, FileName(sSaveToName));
            UpdateAllViews(NULL, dlg.m_nFolder, (CObject*) &sSaveToName);

            if (m_bMoveNext)
                AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_VIEW_NEXT, 0);
            else
                // Update our dialog bar
                AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_UPDATE_DLGBAR, 0);
        }
        else
            AfxMessageBox(IDS_SAVE_ERROR);
    }
}

void CCropperDoc::OnEditFliphoizontal()
{
    CWaitCursor oWait;
    m_WinImage.flipHorizontal();
    UpdateAllViews(NULL);
}

void CCropperDoc::OnEditFlipvertical()
{
    CWaitCursor oWait;
    m_WinImage.flipVertical();
    UpdateAllViews(NULL);
}

void CCropperDoc::OnEditRotateleft()
{
    CWaitCursor oWait;
    m_WinImage.rotate(90.0);
    SaveImage(&m_WinImage, m_sCurImage);
    UpdateAllViews(NULL);
}

void CCropperDoc::OnEditRotateright()
{
    CWaitCursor oWait;
    m_WinImage.rotate(-90.0);
    SaveImage(&m_WinImage, m_sCurImage);
    UpdateAllViews(NULL);
}

// Display the requested image
BOOL CCropperDoc::SetImage(int nView, LPCSTR sImage)
{
    CWaitCursor oWait;

    BOOL bInd;
    CString sFile;

    switch (nView) {
        case nORIGINAL:
        case nPROCESSED:
            if (nView == nORIGINAL)
                sFile = FullName(m_sSourceDir, sImage);
            if (nView == nPROCESSED)
                sFile = FullName(m_sProcessDir, sImage);

            bInd = (((CCropperApp*) AfxGetApp())->ChkFileStatus(sFile, &m_nWinFileSize) != 0);
            if (bInd) {
                // Load the image
                m_sCurImage = sFile;
                m_WinImage.load(sFile);
                bInd = m_WinImage.isValid();
            }

            // Do we need a resized client image?
            if (bInd && m_bClientResize)
                GetSize();
 
            // Get stats for the full size image
            if (bInd) {
                SetTitle(DisplayName());
                m_nAngle = 0;
            }
            else {
                nView = nERROR;
                m_nWinFileSize = 0L;
                SetTitle("Invalid Image");
            }
            if (nView != nERROR)
                break;
        case nLOGO:
        case nEMPTY:
        case nDRAGDROP:
            SetTitle("");
            m_CropArea.SetRectEmpty();
            m_WinImage.setSize(FIT_BITMAP, 0, 0, 0);
            if (nView != nERROR)
            {
                m_sCurImage.Empty();
                m_nWinFileSize = 0;
                if (m_pSourceFolder->GetCount("*.jpg") <= 0)
                    nView = nEMPTY;
            }
            bInd = TRUE;
            break;
        default:
            nView = nERROR;
            VERIFY(false);
    }
    m_nState = nView;
    UpdateAllViews(NULL);    
    // Inform the frame
    AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_UPDATE_DLGBAR, 0);

    return bInd;
}

// Change source and target directories. Update the dialog bar
bool CCropperDoc::SetSourceDir(LPCSTR sDirectory)
{
    CString sSrc;
    bool bInd = false;
    
    // Does this directory have images?
    CDirectory oSrcDir(sDirectory);
    if (oSrcDir.GetCount("*.jpg") > 0) {
        delete m_pSourceFolder;
        m_pSourceFolder = new CDirectory(sDirectory);
        m_pSourceFolder->SetFileMask("*.jpg");
        m_pSourceFolder->SetFirst(sSrc);
        m_sSourceDir = sDirectory;
        bInd = true;
        // Do we need to change the processed folder? We do if they match.
        if (m_sSourceDir.CompareNoCase(m_sProcessDir) == 0)
        {
            m_sProcessDir = ProcessDir(FullName(m_sSourceDir, sSrc));
            m_nFolderType = nAUTOSETFOLDER;
        }
        UpdateAllViews(NULL, nDIRECTORY, 0);
        // Inform the frame
        AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_UPDATE_DLGBAR, 0);
    }
    return bInd;
}

// Set the process directory as instructed, but override if there
// is a problem.
int CCropperDoc::SetProcessDir(int nType, LPCSTR sDirectory)
{
    switch (nType) {
        case nAUTOSETFOLDER:
            m_sProcessDir = ProcessDir(m_sSourceDir);
            break;
        case nDESKTOPFOLDER:
            m_sProcessDir = DefaultPath();
            break;
        default:
            m_sProcessDir = sDirectory;
            break;  // Accept the directory given
    }
    // Do not allow the source and target directories to be the same
    if (m_sSourceDir.CompareNoCase(m_sProcessDir) == 0)
        m_sProcessDir = DefaultPath();
    if (m_sSourceDir.CompareNoCase(m_sProcessDir) == 0)
        m_sProcessDir = DefaultPath(CSIDL_MYPICTURES);

    // Now they are different.
    // What is our folder type?
    if (m_sProcessDir.CompareNoCase(DefaultPath()) == 0)
        m_nFolderType = nDESKTOPFOLDER;
    else if (m_sProcessDir.CompareNoCase(DefaultPath(CSIDL_MYPICTURES)) == 0)
        m_nFolderType = nDESKTOPFOLDER;
    else if (m_sProcessDir.CompareNoCase(ProcessDir(m_sSourceDir)) == 0)
        m_nFolderType = nAUTOSETFOLDER;
    else
    {
        m_nFolderType = nUSERDEFFOLDER;
        m_sUserDir = sDirectory;      // Save the user directory for the registry
    }
    // Update the UI
    UpdateAllViews(NULL, nDIRECTORY, 0);
    // Inform the frame
    AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_UPDATE_DLGBAR, 0);

    return m_nFolderType;
}

void CCropperDoc::OnCloseDocument()
{
    // Save our source and target directories
    AfxGetApp()->WriteProfileString("Settings", "Source", m_sSourceDir);
    AfxGetApp()->WriteProfileString("Settings", "Target", m_sUserDir);
    AfxGetApp()->WriteProfileInt("Settings", "Folder", m_nFolderType);
    AfxGetApp()->WriteProfileInt("Settings", "Width", m_nSaveWidth);
    AfxGetApp()->WriteProfileInt("Settings", "Height", m_nSaveHeight);
    AfxGetApp()->WriteProfileInt("Settings", "Units", m_nSaveUnits);
    AfxGetApp()->WriteProfileInt("Settings", "Resize", m_bSaveResize);

    CDocument::OnCloseDocument();
}

void CCropperDoc::OnClientSize()
{
    CWaitCursor oWait;
    // Display the image to fit the resize area
    m_bClientResize = true;
    GetSize();
    // Resize our cropping area
    RescaleRect(m_CropArea, FALSE);
    UpdateAllViews(NULL);
}

void CCropperDoc::OnTrueSize()
{
    CWaitCursor oWait;
    // Display the image using its true dimensions
    m_bClientResize = false;
    // Resize our cropping area
    RescaleRect(m_CropArea, TRUE);
    UpdateAllViews(NULL);
}

// Append the image size to the display name
CString CCropperDoc::DisplayName(void)
{
    CString sTitle;
    
    sTitle = FileName(m_sCurImage);
    if (sTitle.GetLength() > 0)
        sTitle.Format("%d X %d  %s",
            GetSize(true).cx, GetSize(true).cy, 
            FileName(m_sCurImage));
    else
        sTitle.LoadString(AFX_IDS_APP_TITLE);

    return sTitle;
}

BOOL CCropperDoc::SaveImage(fipWinImage* oImage, LPCSTR sFullName)
{
    BOOL bInd = FALSE;
    int nStatus;

    nStatus = ((CCropperApp*) AfxGetApp())->ChkFileStatus(sFullName);
    if (nStatus == 0)
    {
        CString sDirName = PathName(sFullName);
        // Does the directory exist?
        if (MkDir(sDirName))
            bInd = true;
    }
    else
        bInd = true;

    if (bInd)
    {
        bInd = oImage->save(sFullName);
    }

    return bInd;
}

BOOL CCropperDoc::CropImage(fipWinImage& oOriginal, fipWinImage& oCropped)
{
    BOOL bInd = FALSE;
    // Save either the full picture or the cropping area
    CRect oArea(0, 0, oOriginal.getWidth(), oOriginal.getHeight());  // Full Image
    if (!m_CropArea.IsRectEmpty()) {
        oArea = m_CropArea;                         // Cropping Area
        // Do we need to scale the cropping area to true size?
        if (m_bClientResize)
            RescaleRect(oArea, TRUE);  
    }

    // Crop the image or get entire area
    bInd = oOriginal.copySubImage(oCropped, oArea.TopLeft().x,
        oArea.TopLeft().y, oArea.BottomRight().x,
        oArea.BottomRight().y);

    return bInd;
}

// If necesary, resize the image to fit within the frame. If the
// image already fits, do nothing. If bForceFit is true, do not reorient
// oFrame. Make the image fit the specified oFrame orientation.
double CCropperDoc::RescaleImage(fipWinImage& oImage, CRect oFrame, bool bForceFit)
{
    double ratio;
    double ratio1;

    if (!m_WinImage.isValid())
        return 0;

    // Figure out the new dimensions of our picture
    // We want to preserve the aspect ratio.
    // We need to allow for both vertically and horozontally
    // orientated pictures.
    int nSize1, nSize2;
    if (bForceFit) {
        // Make it fit
        nSize1 = oFrame.Width();
        nSize2 = oFrame.Height();
    }
    else if (oImage.getWidth() > oImage.getHeight()) {
        // Horizontal
        nSize1 = max(oFrame.Width(), oFrame.Height());
        nSize2 = min(oFrame.Width(), oFrame.Height());
    } else {
        // Vertical
        nSize1 = min(oFrame.Width(), oFrame.Height());
        nSize2 = max(oFrame.Width(), oFrame.Height());
    }
    ratio = (1.0 * nSize1) / oImage.getWidth();
    ratio1 = (1.0 * nSize2) / oImage.getHeight();
    if (ratio1 < ratio)
        ratio = ratio1;
    if (ratio < 1) {
        oImage.rescale((int) (oImage.getWidth() * ratio + 0.5),
                            (int) (oImage.getHeight() * ratio + 0.5),
                    FILTER_BILINEAR);  // FILTER_BSPLINE, FILTER_BILINEAR, FILTER_BOX
    }
    else
        ratio = 1.0;

    return ratio;
}

// Toggle cropping area between client sized and true sized
void CCropperDoc::RescaleRect(CRect& oCrop, BOOL bTrueSize)
{
    if (bTrueSize) {
        // Crop area is client sized. Resize it to true size
        m_CropArea.NormalizeRect();
        oCrop.SetRect((int)(m_CropArea.TopLeft().x / m_fClientRatio + 0.5),
            (int)(m_CropArea.TopLeft().y / m_fClientRatio + 0.5),
            (int)(m_CropArea.BottomRight().x / m_fClientRatio + 0.5),
            (int)(m_CropArea.BottomRight().y / m_fClientRatio + 0.5));
        // Make sure these borders are within our picture boarders.
        // May have rounding problems
        CRect oTrueSize(0, 0, m_WinImage.getWidth(), m_WinImage.getHeight());
        oCrop.IntersectRect(oTrueSize, oCrop);

    } else {
        // Crop area is true sized. Resize it to client size
        oCrop.SetRect((int)(m_CropArea.TopLeft().x * m_fClientRatio + 0.5),
            (int)(m_CropArea.TopLeft().y * m_fClientRatio + 0.5),
            (int)(m_CropArea.BottomRight().x * m_fClientRatio + 0.5),
            (int)(m_CropArea.BottomRight().y * m_fClientRatio + 0.5));
    }
    return;
}

CRect CCropperDoc::CalcFrame(int nHeight, int nWidth, int nUnits)
{
    CRect oFrame;
    CDC* pDC = AfxGetApp()->GetMainWnd()->GetDC();
    int nValueX;
    int nValueY;

    switch (nUnits) {
        case nINCHES:           // Inches
            nValueX = (int)(pDC->GetDeviceCaps(LOGPIXELSX) * nWidth * 0.1 + 0.5);
            nValueY = (int)(pDC->GetDeviceCaps(LOGPIXELSY) * nHeight * 0.1 + 0.5);
            break;
        case nCENTIMETERS:           // Centimeters
            nValueX = (int)(pDC->GetDeviceCaps(LOGPIXELSX) * nWidth * 0.1 * 0.3937 + 0.5);
            nValueY = (int)(pDC->GetDeviceCaps(LOGPIXELSY) * nHeight * 0.1 * 0.3937 + 0.5);
            break;
        case nPIXELS:           // Pixels
            nValueX = (int)(nWidth * 0.1);
            nValueY = (int)(nHeight * 0.1);
            break;
        default:
            ASSERT(false);
    }
    oFrame.SetRect(0, 0, nValueX, nValueY);

    return oFrame;
}

// Default Location for Pictures
CString CCropperDoc::DefaultPath(int nFolder)
{
    CString szPath;
    bool bInd = false;
    // Try for the Desktop folder. Not supported by all Windows versions
    try {
    bInd = SUCCEEDED(SHGetFolderPath(NULL, 
                                nFolder|CSIDL_FLAG_CREATE, 
                                NULL, 
                                SHGFP_TYPE_CURRENT, 
                                szPath.GetBuffer(MAX_PATH)));
    }
    catch (CNotSupportedException* e)
    {
        e->Delete();
    }
    szPath.ReleaseBuffer();

    if (szPath.GetLength() > 0)
    {
        // Create a FitToList folder on the desktop
        if (nFolder == CSIDL_DESKTOPDIRECTORY)
            szPath = FullName(szPath, "FitToList Photos");
    }
    else  // Use the app folder if we cant get a directory
        szPath = PathName(AfxGetApp()->m_pszAppName);
 
    return szPath;
}

void CCropperDoc::OnEditRotateadjust()
{
    // TODO: Add your command handler code here
    CRotateDlg dlg;
    dlg.m_nAngle = m_nAngle;
    if (dlg.DoModal() == IDOK) {
        m_nAngle = dlg.m_nAngle;
        CWaitCursor oWait;
        m_WinImage.load(m_sCurImage);
        if (m_nAngle != 0)
            m_WinImage.rotate(m_nAngle);
        UpdateAllViews(NULL);
    }
}

int CCropperDoc::CurrentState(void)
{
     return m_nState;
}

// Return folder name in format to display to the user
CString CCropperDoc::GetDisplayDir(void)
{
    CString sDir = m_sProcessDir;
    switch(m_nFolderType) {
        case nAUTOSETFOLDER:
            sDir = ProcessDir(m_sSourceDir);
            break;
        case nDESKTOPFOLDER:
            if (m_sProcessDir.CompareNoCase(DefaultPath()) == 0)
                sDir = "Your Desktop - 'FitToList Photos' folder";
            else
                sDir = "Your 'My Pictures' Folder";
            break;
        case nUSERDEFFOLDER:
            sDir = m_sProcessDir;
            break;
        default:
            ASSERT(true);
    }
    return sDir;
}
