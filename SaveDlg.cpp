// SaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "SaveDlg.h"
#include "CropperDoc.h"
#include ".\savedlg.h"

// CSaveDlg dialog

IMPLEMENT_DYNAMIC(CSaveDlg, CDialog)
CSaveDlg::CSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveDlg::IDD, pParent)
    , m_bMoveNext(FALSE)
    , m_bReduceSize(FALSE)
    , m_sUserName(_T(""))
    , m_nDisposition(1)
{
    m_bCropping = false;
}

CSaveDlg::~CSaveDlg()
{
}

void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHK_NEXT, m_bMoveNext);
    DDX_Check(pDX, IDC_CHK_REDUCE, m_bReduceSize);
    DDX_Text(pDX, IDC_NAME, m_sUserName);
    DDV_MaxChars(pDX, m_sUserName, 50);
    DDX_CBIndex(pDX, IDC_CBO_FOLDER, m_nDisposition);
}


BEGIN_MESSAGE_MAP(CSaveDlg, CDialog)
END_MESSAGE_MAP()

void CSaveDlg::ChkExtension(CString& sFileName)
{
    int nLen = sFileName.GetLength() - 1;
    int nPos = sFileName.ReverseFind('.');
    if (nPos == 0)    // File name is a dot
        sFileName.Empty();
    else if (nPos == nLen)      // File name ends with a dot
        sFileName+="jpg";
    else if (nPos < 0)     // File name contains no dot
        // Append .jpg
        sFileName+=".jpg";
    else {                 // File has an extension
        sFileName.MakeLower();
        if (sFileName.Find(".jpg", nPos) != nPos) {
            sFileName = sFileName.Left(nPos);
            sFileName+=".jpg";
        }
    }
    
    return;
}

// CSaveDlg message handlers

void CSaveDlg::OnOK()
{
    CString sFileName;
    int nResponse = IDNO;
    int nStatus;

    // Did they give us a name?
    if (!UpdateData(TRUE))
        return;
    // Make sure it has a .jpg extension
    ChkExtension(m_sUserName);
    if (m_sUserName.IsEmpty()) {
        AfxMessageBox("Please enter a valid file name");
        return;
    }

    // Does the file exist?
    if (m_nDisposition > 0)
    {
        m_sFullName = m_sProcessDir + "\\" + m_sUserName;
        m_nFolder = nPROCESSED;
    }
    else
    {
        m_sFullName = m_sSourceDir + "\\" + m_sUserName;
        m_nFolder = nORIGINAL;
    }
    nStatus = ((CCropperApp*) AfxGetApp())->ChkFileStatus(m_sFullName); 
    if (nStatus == 1)
        nResponse = AfxMessageBox(IDS_OVERWRITE, MB_YESNO);
    else if (nStatus == -1)
        AfxMessageBox(IDS_READONLY);
    else
        nResponse = IDYES;

    if (nResponse == IDYES)
        CDialog::OnOK();
}

BOOL CSaveDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    if (m_bCropping)
        SetWindowText("Save Cropped Area");

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
