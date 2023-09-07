// CropperDoc.cpp : implementation of the CCropperDoc class
//

#include "stdafx.h"
#include <direct.h>   //For mkdir function
#include <errno.h>    //For mkdir function
#include "Cropper.h"

#include "CropperDoc.h"
#include "ProcessDlg.h"
#include ".\cropperdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCropperDoc

IMPLEMENT_DYNCREATE(CCropperDoc, CDocument)

BEGIN_MESSAGE_MAP(CCropperDoc, CDocument)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
    ON_COMMAND(ID_EDIT_FLIPHOIZONTAL, OnEditFliphoizontal)
    ON_COMMAND(ID_EDIT_FLIPVERTICAL, OnEditFlipvertical)
    ON_COMMAND(ID_EDIT_ROTATELEFT, OnEditRotateleft)
    ON_COMMAND(ID_EDIT_ROTATERIGHT, OnEditRotateright)
    ON_COMMAND(ID_EDIT_SIZE, OnEditSize)
    ON_COMMAND(ID_EDIT_TRUE, OnEditTrue)
    ON_COMMAND(ID_EDIT_SET, OnEditSet)
    ON_UPDATE_COMMAND_UI(ID_EDIT_TRUE, OnUpdateEditTrue)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SIZE, OnUpdateEditSize)
END_MESSAGE_MAP()


// CCropperDoc construction/destruction

CCropperDoc::CCropperDoc()
: m_sCurImage(_T(""))
{
	// For SDI, this is executed one time only
    // Initialize our stuff
    m_CropArea.SetRectEmpty();
    m_nWinFileSize = 0;
    m_bMoveNext = true;
    //TODO: Set to the MyPictures directory
    m_sProcessDir = AfxGetApp()->GetProfileString("Settngs", "Target", "C:\\");
    m_sSourceDir = AfxGetApp()->GetProfileString("Settings", "Source", "C:\\");
    int nWidth = AfxGetApp()->GetProfileInt("Settings", "Width", 150);
    int nHeight = AfxGetApp()->GetProfileInt("Settings", "Height", 100);
    m_ResizeArea.SetRect(0, 0, nWidth, nHeight);
    m_bShowResize = false;

}

CCropperDoc::~CCropperDoc()
{
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
CString CCropperDoc::FullName(LPCTSTR sDir, LPCTSTR sName)
{
	// Combine sName with sDir to generate a full file name
	CString sFullName;

	// Check the directory to see if it ends with a slash
	CString oDir = sDir;
	int nLoc = oDir.ReverseFind('\\') + 1;
	if (nLoc != oDir.GetLength())
		oDir = oDir + '\\';

	sFullName = oDir + sName;

	return sFullName;
}

CString CCropperDoc::PathName(LPCTSTR sFullName)
{
	// Retrieve the directory of the given file or dir
	CString sPathName;

	CString oFullName = sFullName;
	int nLoc = oFullName.ReverseFind('\\');
	if (nLoc < 0)
	{
		nLoc = oFullName.ReverseFind(':');
		// If the colon is the last char, ignore it
		if (nLoc == oFullName.GetLength() - 1)
			nLoc = -1;
	}

	if (nLoc < 0)
		sPathName = "";
	else
		sPathName = oFullName.Left(nLoc);

	return sPathName;
}

CString CCropperDoc::FileName(LPCTSTR sFullName)
{
	// Retrieve the file name of the given file or dir
	CString sFileName;

	CString oFullName = sFullName;
	int nLoc = oFullName.ReverseFind('\\');
	if (nLoc < 0)
		nLoc = oFullName.ReverseFind(':');

	if (nLoc < 0)
		sFileName = "";
	else
		sFileName = oFullName.Right(oFullName.GetLength() - nLoc - 1);

	return sFileName;
}

BOOL CCropperDoc::MkDir(CString sPath)
{
	// Create the directory named by sPath.  Return TRUE
	// if the directory name is valid.
	BOOL bInd;

	if (sPath == "")
		return FALSE;

	if (_mkdir((LPCTSTR) sPath) == 0)
		return TRUE;

	int nErr = errno;
	if (nErr == EEXIST)
		return TRUE;

	// The directory could not be created.
	// Try moving down the tree
	CString sSubPath = PathName(sPath);
	bInd = MkDir(sSubPath);
	if (bInd)
		bInd = MkDir(sPath);

	return bInd;
}

// CCropperDoc commands

// Return the size of the currently loaded image
CSize CCropperDoc::GetSize(void)
{
    SIZE imgSize;
    imgSize.cx = m_WinImage.getWidth();
    imgSize.cy = m_WinImage.getHeight();
    return imgSize;
}

long CCropperDoc::GetImageSize(void)
{
    return m_nWinFileSize;
}

void CCropperDoc::OnFileSaveAs()
{
    CString sOrigName;
    fipImage oImage;
    CProcessDlg dlg;

    // Do we have anything to do?
    if (!m_WinImage.isValid()) {
        AfxMessageBox("You must select an image to process.");
        return;
    }

    // Save either the full picture or the cropping area
    CRect oArea(0, 0, GetSize().cx, GetSize().cy);
    if (!m_CropArea.IsRectEmpty())
        oArea = m_CropArea;

    sOrigName = m_sCurImage;
    // Initialize our dialog
    dlg.m_sProcessName = DisplayName();
    dlg.m_bMoveNext = m_bMoveNext;
    dlg.m_sSourceDir = m_sSourceDir;
    dlg.m_sProcessDir = m_sProcessDir;
    if (dlg.DoModal() == IDOK) {
        m_WinImage.copySubImage(oImage, oArea.TopLeft().x,
            oArea.TopLeft().y, oArea.BottomRight().x,
            oArea.BottomRight().y);
        m_bMoveNext = dlg.m_bMoveNext;
        if (dlg.m_nFolder > 0)
            m_sCurImage = FullName(m_sProcessDir, dlg.m_sProcessName);
        else
            m_sCurImage = FullName(m_sSourceDir, dlg.m_sProcessName);
        oImage.save(m_sCurImage);
        // Do we need to reload?
        if (m_sCurImage.CompareNoCase(sOrigName) == 0 && !m_bMoveNext)
            SetImage(dlg.m_nFolder+1, DisplayName());

        UpdateAllViews(NULL, dlg.m_nFolder+1, (CObject*) &dlg.m_sProcessName);

        if (m_bMoveNext)
            AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_VIEW_NEXT, 0);
    }
}

void CCropperDoc::OnEditFliphoizontal()
{
    m_WinImage.flipHorizontal();
    UpdateAllViews(NULL);
}

void CCropperDoc::OnEditFlipvertical()
{
    m_WinImage.flipVertical();
    UpdateAllViews(NULL);
}

void CCropperDoc::OnEditRotateleft()
{
    m_WinImage.rotate(90.0);
    UpdateAllViews(NULL);
}

void CCropperDoc::OnEditRotateright()
{
    m_WinImage.rotate(-90.0);
    UpdateAllViews(NULL);
}

// Display the requested image
bool CCropperDoc::SetImage(int nView, LPCSTR sImage)
{
    bool bInd;
    CString sFile;
    if (nView == nORIGINAL)
        sFile = FullName(m_sSourceDir, sImage);
    if (nView == nPROCESSED)
        sFile = FullName(m_sProcessDir, sImage);

    CFileStatus oStatus;
    bInd = CFile::GetStatus(sFile, oStatus);
    if (bInd) {
        m_sCurImage = sFile;
        bInd = LoadImage();
    }
    if (bInd) {
        m_nWinFileSize = (long) oStatus.m_size;
        SetTitle(DisplayName());
    }
    else {
        m_nWinFileSize = 0L;
        SetTitle("Invalid Image");
    }
    UpdateAllViews(NULL);    
    // Inform the frame
    AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_UPDATE_DLGBAR, 0);

    return bInd;
}

bool CCropperDoc::SetSourceDir(LPCSTR sDirectory)
{
    m_sSourceDir = sDirectory;
    UpdateAllViews(NULL, 3, 0);
    // Inform the frame
    AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_UPDATE_DLGBAR, 0);
    return false;
}

bool CCropperDoc::SetProcessDir(LPCSTR sDirectory)
{
    m_sProcessDir = sDirectory;
    UpdateAllViews(NULL, 3, 0);
    // Inform the frame
    AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_UPDATE_DLGBAR, 0);
    return false;
}

void CCropperDoc::OnCloseDocument()
{
    // Save our source and target directories
    AfxGetApp()->WriteProfileString("Settings", "Source", m_sSourceDir);
    AfxGetApp()->WriteProfileString("Settings", "Target", m_sProcessDir);
    AfxGetApp()->WriteProfileInt("Settings", "Width", m_ResizeArea.Width());
    AfxGetApp()->WriteProfileInt("Settings", "Height", m_ResizeArea.Height());

    CDocument::OnCloseDocument();
}

void CCropperDoc::OnEditSize()
{
    // Display the image to fit the resize area
    m_bShowResize = true;
    LoadImage();
    UpdateAllViews(NULL);
}

void CCropperDoc::OnEditTrue()
{
    // Display the image using its true dimensions
    m_bShowResize = false;
    LoadImage();
    UpdateAllViews(NULL);
}

void CCropperDoc::OnEditSet()
{
    // Allow the user to select the resize area
}


void CCropperDoc::OnUpdateEditTrue(CCmdUI *pCmdUI)
{
    if (m_bShowResize)
        pCmdUI->SetCheck(0);
    else
        pCmdUI->SetCheck(1);
}

void CCropperDoc::OnUpdateEditSize(CCmdUI *pCmdUI)
{
    if (m_bShowResize)
        pCmdUI->SetCheck(1);
    else
        pCmdUI->SetCheck(0);
}

// Reload the current document from disk
bool CCropperDoc::LoadImage(void)
{
    bool bInd;
    CWaitCursor oCursor;  // Could take a bit of time to rescale

    // We know the file name is valid
    bInd = (m_WinImage.load(m_sCurImage) && m_WinImage.isValid());
    if (bInd && m_bShowResize)
    {
        double ratio;
        // Figure out the new dimensions of our picture
        // We want to preserve the aspect ratio
        if (GetSize().cx > GetSize().cy)
            ratio = (1.0 * m_ResizeArea.Width()) / GetSize().cx;
        else
            ratio = (1.0 * m_ResizeArea.Height()) / GetSize().cy;
        if (ratio < 1)
            m_WinImage.rescale((int) (GetSize().cx * ratio),
                               (int) (GetSize().cy * ratio),
                     FILTER_BSPLINE);
    }

    return bInd;
}

CString CCropperDoc::DisplayName(void)
{
    return FileName(m_sCurImage);
}
