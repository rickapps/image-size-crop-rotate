// SpecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "SpecDlg.h"
#include "SizeDlg.h"
#include ".\specdlg.h"

// CSpecDlg dialog

IMPLEMENT_DYNAMIC(CSpecDlg, CDialog)
CSpecDlg::CSpecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecDlg::IDD, pParent)
    , m_nProcessFolder(0)
    , m_sProcessFolder(_T(""))
{
}

CSpecDlg::~CSpecDlg()
{
}

void CSpecDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_CBIndex(pDX, IDC_CBO_FOLDER, m_nProcessFolder);
}


BEGIN_MESSAGE_MAP(CSpecDlg, CDialog)
    ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
    ON_BN_CLICKED(IDC_SETSIZE, OnBnClickedSetsize)
END_MESSAGE_MAP()

// CSpecDlg message handlers

void CSpecDlg::OnBnClickedBrowse()
{
    // Select a new Processed Folder
    BOOL bInd = FALSE;
    CString sDir;
    CCropperApp* theApp = (CCropperApp*) AfxGetApp();
    bInd = theApp->SelectDirectory(
        this, "Select a folder to store your processed pictures", sDir);
    if (bInd)
    {
        bInd = (m_sSourceFolder.CompareNoCase(sDir) != 0);
        if (bInd)
        {
            m_sProcessFolder = sDir;
            PopulateFolderList(2);
        }
        else
            AfxMessageBox(IDS_SAMEFOLDER);
    }
}

void CSpecDlg::OnBnClickedSetsize()
{
    // Let them change the size of our bounding frame
    CSizeDlg dlg;
    dlg.m_nHeight = m_nHeight;
    dlg.m_nWidth = m_nWidth;
    dlg.m_nUnits = m_nUnits;
    if (dlg.DoModal() == IDOK)
    {
        m_nHeight = dlg.m_nHeight;
        m_nWidth = dlg.m_nWidth;
        m_nUnits = dlg.m_nUnits;
        SetLabel();
    }
}

void CSpecDlg::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    CDialog::OnOK();
}

BOOL CSpecDlg::OnInitDialog()
{
	CRect oSize;
    CFont myFont;

    CStatic* pStatic = (CStatic*) GetDlgItem(IDC_MAX_HEIGHT);
	pStatic->GetWindowRect(oSize);
	myFont.CreateFont(oSize.Height(), 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_SCRIPT, "MS Sans Serif");
    pStatic->SetFont(&myFont);

    pStatic = (CStatic*) GetDlgItem(IDC_MAX_WIDTH);
    pStatic->SetFont(&myFont);

    SetLabel();
    PopulateFolderList(m_nProcessFolder);
    CDialog::OnInitDialog();
    
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecDlg::SetLabel(void)
{
    CString sCaption;
    CString sHeight;
    CString sWidth;
    CString sUnits;
    sCaption.LoadString(IDS_SIZING);
    if (m_nUnits == nPIXELS)
    {
        sHeight.Format("%5.0f", m_nHeight * 0.1);
        sWidth.Format("%5.0f", m_nWidth * 0.1);
        sUnits = "pixels";
    }
    else if (m_nUnits == nINCHES)
    {
        sHeight.Format("%.1f", m_nHeight * 0.1);
        sWidth.Format("%.1f", m_nWidth * 0.1);
        sUnits = "inches";
    }
    else
    {
        sHeight.Format("%.1f", m_nHeight * 0.1);
        sWidth.Format("%.1f", m_nWidth * 0.1);
        sUnits = "centimeters";
    }

    sCaption.Replace("$HEIGHT$", sHeight.Trim());
    sCaption.Replace("$WIDTH$", sWidth.Trim());
    sCaption.Replace("$UNITS$", sUnits.Trim());
    CStatic* pStatic = (CStatic*) GetDlgItem(IDC_SIZING);
    pStatic->SetWindowText(sCaption);

    pStatic = (CStatic*) GetDlgItem(IDC_MAX_WIDTH);
    sCaption.Format("%s X", sWidth);
    pStatic->SetWindowText(sCaption);

    pStatic = (CStatic*) GetDlgItem(IDC_MAX_HEIGHT);
    pStatic->SetWindowText(sHeight);

    pStatic = (CStatic*) GetDlgItem(IDC_UNITS);
    pStatic->SetWindowText(sUnits);
}

// Populate the listbox with three choices for the Processed Folder
int CSpecDlg::PopulateFolderList(int nDefault)
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
    return pCombo->SetCurSel(nDefault);
}
