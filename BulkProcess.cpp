// BulkProcess.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "SizeDlg.h"
#include ".\bulkprocess.h"

// CBulkProcess dialog

IMPLEMENT_DYNAMIC(CBulkProcess, CDialog)
CBulkProcess::CBulkProcess(CWnd* pParent /*=NULL*/)
	: CDialog(CBulkProcess::IDD, pParent)
    , m_bCheckOverwrite(TRUE)
    , m_nSourceCnt(0)
    , m_nFolderID(0)
    , m_sProcessFolder(_T(""))
{
}

CBulkProcess::~CBulkProcess()
{
}

void CBulkProcess::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHK_OVERWRITE, m_bCheckOverwrite);
    DDX_Text(pDX, IDC_COUNT, m_nSourceCnt);
    DDX_CBIndex(pDX, IDC_CBO_FOLDER, m_nFolderID);
}


BEGIN_MESSAGE_MAP(CBulkProcess, CDialog)
    ON_BN_CLICKED(IDC_SETSIZE, OnBnClickedSetsize)
    ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
END_MESSAGE_MAP()


// CBulkProcess message handlers
BOOL CBulkProcess::OnInitDialog()
{
    SetLabel();
    m_sProcessFolder = m_pDoc->m_sUserDir;
    PopulateFolderList(m_pDoc->m_nFolderType);  // Select Processed Folder
    CDialog::OnInitDialog();
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CBulkProcess::OnOK()
{
    CDialog::OnOK();
}

void CBulkProcess::OnBnClickedSetsize()
{
    // Let them change the size of our bounding frame
    CSizeDlg dlg;
    dlg.m_nHeight = m_pDoc->m_nSaveHeight;
    dlg.m_nWidth = m_pDoc->m_nSaveWidth;
    dlg.m_nUnits = m_pDoc->m_nSaveUnits;
    if (dlg.DoModal() == IDOK)
    {
        m_pDoc->m_nSaveHeight = dlg.m_nHeight;
        m_pDoc->m_nSaveWidth = dlg.m_nWidth;
        m_pDoc->m_nSaveUnits = dlg.m_nUnits;
        SetLabel();
    }
}

void CBulkProcess::SetLabel(void)
{
    CString sCaption;
    CString sHeight;
    CString sWidth;
    CString sUnits;
    sCaption.LoadString(IDS_MAX_SIZE);
    if (m_pDoc->m_nSaveUnits == nPIXELS)
    {
        sHeight.Format("%5.0f", m_pDoc->m_nSaveHeight * 0.1);
        sWidth.Format("%5.0f", m_pDoc->m_nSaveWidth * 0.1);
        sUnits = "pixels";
    }
    else if (m_pDoc->m_nSaveUnits == nINCHES)
    {
        sHeight.Format("%.1f", m_pDoc->m_nSaveHeight * 0.1);
        sWidth.Format("%.1f", m_pDoc->m_nSaveWidth * 0.1);
        sUnits = "inches";
    }
    else
    {
        sHeight.Format("%.1f", m_pDoc->m_nSaveHeight * 0.1);
        sWidth.Format("%.1f", m_pDoc->m_nSaveWidth * 0.1);
        sUnits = "centimeters";
    }

    sCaption.Replace("$HEIGHT$", sHeight.Trim());
    sCaption.Replace("$WIDTH$", sWidth.Trim());
    sCaption.Replace("$UNITS$", sUnits.Trim());
    CStatic* pStatic = (CStatic*) GetDlgItem(IDC_MAX_SIZE);
    pStatic->SetWindowText(sCaption);
}

void CBulkProcess::OnBnClickedBrowse()
{
    // Select a new Processed Folder
    BOOL bInd = FALSE;
    CString sDir;
    CCropperApp* theApp = (CCropperApp*) AfxGetApp();
    bInd = theApp->SelectDirectory(
        this, "Select a folder to store your processed pictures", sDir);
    if (bInd)
    {
        bInd = (m_pDoc->m_sSourceDir.CompareNoCase(sDir) != 0);
        if (bInd)
        {
            m_sProcessFolder = sDir;
            PopulateFolderList(2);
            UpdateData(FALSE);
        }
        else
            AfxMessageBox(IDS_SAMEFOLDER);
    }
}

// Populate the listbox with three choices for the Processed Folder
int CBulkProcess::PopulateFolderList(int nDefault)
{
    // The listbox contains three choices of processed folder - Desktop,
    // Originals Folder, or a user defined folder. The user defined folder
    // is from a registry setting.
    CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_CBO_FOLDER);
    pCombo->ResetContent();
    pCombo->AddString("My Desktop - FitToList Photos Folder");
    pCombo->AddString("Processed Folder in same folder as Originals");
    pCombo->AddString(m_sProcessFolder);
    // Set the combo to the default value requested
    ASSERT(nDefault >= 0 && nDefault < 3);
    m_nFolderID =  pCombo->SetCurSel(nDefault);
    return m_nFolderID;
}
