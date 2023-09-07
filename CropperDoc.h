// CropperDoc.h : interface of the CCropperDoc class
// Copyright (C) 2004 Rick A. Eichhorn

#pragma once
#include "FreeImagePlus.h"
#include "Directory.h"

// Constants to designate the current view
const int nLOGO = 0;         // Photos Folder
const int nORIGINAL = 1;     // Original picture
const int nPROCESSED = 2;    // Processed picture
const int nDIRECTORY = 3;    // Source or Target Directory Change
const int nEMPTY = 4;        // No pictures in current directory
const int nERROR = 5;        // Picture cannot be displayed
const int nVIEWORIG = 9;     // View Originals
const int nVIEWPROC = 7;     // View Processed
const int nDRAGDROP = 8;     // Drag and drop
// Target folders for processed pictures
const int nDESKTOPFOLDER = 0;   // Folder on the desktop
const int nAUTOSETFOLDER = 1;   // Folder within processed folder
const int nUSERDEFFOLDER = 2;   // User defined folder 

class CCropperDoc : public CDocument
{
protected: // create from serialization only
	CCropperDoc();
	DECLARE_DYNCREATE(CCropperDoc)

// Attributes
public:
    fipWinImage m_WinImage;     // Original picture
    CString m_sSourceDir;       // Folder for original pictures
    CString m_sProcessDir;      // Folder for cropped pictures
    CString m_sUserDir;         // Folder that user picked
    long m_nWinFileSize;        // Size of image file on disk
    CRect m_CropArea;           // Cropping dimensions
    CRect m_ClientArea;         // Client area for display Resizing
    CRect m_SaveArea;           // Pixel dimensions for m_nSaveWidth and m_nSaveHeight
    int m_nSaveWidth;           // Width of save area in tenths m_nSaveUnits
    int m_nSaveHeight;          // Height of save area in tenths m_nSaveUnits
    BOOL  m_bClientResize;      // Indicator to Show full or resized image
    BOOL  m_bSaveResize;        // Indicator to Save full or resized image
    CString m_sCurImage;        // Full path name of the current image
    int m_nSaveUnits;           // Units (in, cm, pixels) for saving images
    BOOL m_bMoveNext;           // Move to the next picture after processing
    double m_fClientRatio;      // Resizing factor to convert client/full size
    int m_nFolderType;         // Are we using Desktop, Processed, or user defined?
    int m_nAngle;               // Picture is slightly rotated

// Operations
public:
    CDirectory* m_pSourceFolder;
    // Display the requested image
    BOOL SetImage(int nView, LPCSTR sImage);
    bool SetSourceDir(LPCSTR sDirectory);
    int SetProcessDir(int nType, LPCSTR sDirectory);

// Overrides
	virtual BOOL OnNewDocument();

// Implementation
public:
	virtual ~CCropperDoc();
    CSize GetSize(bool bTrueSize = false);
    long GetImageSize(void);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
        int m_nState;               // nLOGO, nORIGINAL, nEMPTY, etc.

protected:
    CString DefaultPath(int nFolder = CSIDL_DESKTOPDIRECTORY);
    CString ProcessDir(LPCTSTR sSrcDir);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditFliphoizontal();
    afx_msg void OnEditFlipvertical();
    afx_msg void OnEditRotateleft();
    afx_msg void OnEditRotateright();
    afx_msg void OnFileResizeall();
    afx_msg void OnEditRotateadjust();
    afx_msg void OnClientSize();
    afx_msg void OnTrueSize();
    CString DisplayName(void);
    virtual void OnCloseDocument();
    void RescaleRect(CRect& oCrop, BOOL bTrueSize = TRUE);
    double RescaleImage(fipWinImage& oImage, CRect oFrame, bool bForceFit = false);
    BOOL CropImage(fipWinImage& oOriginal, fipWinImage& oCropped);
    BOOL SaveImage(fipWinImage* oImage, LPCSTR sFullName);
    CRect CalcFrame(int nHeight, int nWidth, int nUnits);
    int CurrentState(void);
    // Return folder name in format to display to the user
    CString GetDisplayDir(void);
};


