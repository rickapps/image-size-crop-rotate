// ProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "ProcessDlg.h"

// CProcessDlg dialog

IMPLEMENT_DYNAMIC(CProcessDlg, CDialog)
CProcessDlg::CProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessDlg::IDD, pParent)
    , m_sProcessName(_T(""))
    , m_bMoveNext(FALSE)
    , m_nFolder(1)
    , m_bReduceSize(FALSE)
    , m_nUnits(nPIXELS)
    , m_fMaxHeight(0)
    , m_fMaxWidth(0)
{
}

CProcessDlg::~CProcessDlg()
{
}

void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_NAME, m_sProcessName);
    DDV_MaxChars(pDX, m_sProcessName, 30);
    DDX_Check(pDX, IDC_CHK_NEXT, m_bMoveNext);
    DDX_CBIndex(pDX, IDC_CBO_FOLDER, m_nFolder);
    DDX_Check(pDX, IDC_CHK_REDUCE, m_bReduceSize);
    DDX_CBIndex(pDX, IDC_CBO_UNITS, m_nUnits);
    DDX_Text(pDX, IDC_MAX_HEIGHT, m_fMaxHeight);
    DDV_MinMaxDouble(pDX, m_fMaxHeight, 0.1, 2000);
    DDX_Text(pDX, IDC_MAX_WIDTH, m_fMaxWidth);
	DDV_MinMaxDouble(pDX, m_fMaxWidth, 0.1, 2000);
}


BEGIN_MESSAGE_MAP(CProcessDlg, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDC_CHK_REDUCE, OnBnClickedChkReduce)
    ON_CBN_SELCHANGE(IDC_CBO_UNITS, OnCbnSelchangeCboUnits)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEIGHT, OnDeltaposSpinHeight)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WIDTH, OnDeltaposSpinWidth)
END_MESSAGE_MAP()


// CProcessDlg message handlers

BOOL CProcessDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    OnCbnSelchangeCboUnits();
    m_fMaxWidth = m_nWidth * 0.1;
    m_fMaxHeight = m_nHeight * 0.1;
    // OnBnClick toggles the value of our flag...
    m_bReduceSize = !m_bReduceSize;
    OnBnClickedChkReduce();
    UpdateData(FALSE);  // To update m_fMaxWidth and m_fMaxHeight

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcessDlg::OnBnClickedOk()
{
    CString sFileName;
    CFileStatus oStatus;
    int nResponse = IDNO;

    // Did they give us a name?
    if (!UpdateData(TRUE))
        return;
    // Make sure it has a .jpg extension
    ChkExtension(m_sProcessName);
    if (m_sProcessName.IsEmpty()) {
        AfxMessageBox("Please enter a valid file name");
        return;
    }

    // Does the file exist?
    if (m_nFolder > 0)
        sFileName = m_sProcessDir + "\\" + m_sProcessName;
    else
        sFileName = m_sSourceDir + "\\" + m_sProcessName;
    if (CFile::GetStatus(sFileName, oStatus)) 
        nResponse = AfxMessageBox("File exists. Do you want to replace it?", MB_YESNO);
    else
        nResponse = IDYES;

    if (nResponse == IDYES)
    {
        // Calculate our frame area for picture resizing
        if (m_bReduceSize) {
            m_nWidth = (int)(m_fMaxWidth * 10);
            m_nHeight = (int)(m_fMaxHeight * 10);
        }
        OnOK();
    }
}

void CProcessDlg::OnBnClickedChkReduce()
{
    m_bReduceSize = !m_bReduceSize;
    // Disable/Enable the measurement controls
    CEdit* pEdit = (CEdit*) GetDlgItem(IDC_MAX_HEIGHT);
    pEdit->EnableWindow(m_bReduceSize);
    pEdit = (CEdit*) GetDlgItem(IDC_MAX_WIDTH);
    pEdit->EnableWindow(m_bReduceSize);
    CComboBox* pCBO = (CComboBox*) GetDlgItem(IDC_CBO_UNITS);
    pCBO->EnableWindow(m_bReduceSize);
    CButton* pButton = (CButton*) GetDlgItem(IDC_CHK_REDUCE);
    pButton->SetCheck(m_bReduceSize);
}

void CProcessDlg::OnCbnSelchangeCboUnits()
{
    CSpinButtonCtrl* pHeight = (CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_HEIGHT);
    CSpinButtonCtrl* pWidth = (CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_WIDTH);
    CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_CBO_UNITS);
    CString sWidth;
    CString sHeight;
    int nMinW, nMinH;
    int nMaxW, nMaxH;
    int nDefaultW, nDefaultH;
    // Reset the spin button controls
    switch(pCombo->GetCurSel()) {
        case nINCHES:  // Inches
            nMinW = 5;
            nMaxW = 100;
            nMinH = 5;
            nMaxH = 100;
            nDefaultW = 60;
            nDefaultH = 40;
            sHeight.Format("%4.1f", nDefaultH * 0.1);
            sWidth.Format("%4.1f", nDefaultW * 0.1);
            break;
        case nCENTIMETERS:  // Centimeters
            nMinW = 15;
            nMaxW = 250;
            nMinH = 15;
            nMaxH = 250;
            nDefaultW = 160;
            nDefaultH = 100;
            sHeight.Format("%4.1f", nDefaultH * 0.1);
            sWidth.Format("%4.1f", nDefaultW * 0.1);
            break;
        case nPIXELS:  // Pixels
            nMinW = 30;
            nMaxW = 1000;
            nMinH = 30;
            nMaxH = 1000;
            nDefaultW = 600;
            nDefaultH = 400;
            sHeight.Format("%d", nDefaultH);
            sWidth.Format("%d", nDefaultW);
            break;
        default:
            ASSERT(false);
    }
    pHeight->SetRange(nMinH, nMaxH);
    pHeight->SetPos(nDefaultH);
    pWidth->SetRange(nMinW, nMaxW);
    pWidth->SetPos(nDefaultW);
    pHeight->GetBuddy()->SetWindowText(sHeight);
    pWidth->GetBuddy()->SetWindowText(sWidth);
}

// Update the text for the height - spin button is about to change
void CProcessDlg::OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
    CString sAmount;
    int nPos;
    int nMax;
    int nMin;

    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    CSpinButtonCtrl* pHeight = (CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_HEIGHT);
    CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_CBO_UNITS);
    pHeight->GetRange(nMin, nMax);
    nPos = pNMUpDown->iPos + pNMUpDown->iDelta;
    if (nPos < nMin)
        nPos = nMin;
    if (nPos > nMax)
        nPos = nMax;
    if (pCombo->GetCurSel() == nPIXELS)
        sAmount.Format("%d", nPos);
    else
        sAmount.Format("%4.1f", nPos * 0.1);
    pHeight->GetBuddy()->SetWindowText(sAmount);
    *pResult = 0;
}

// Update the text for the Width - spin button is about to change
void CProcessDlg::OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
    CString sAmount;
    int nPos;
    int nMax;
    int nMin;

    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    CSpinButtonCtrl* pHeight = (CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_WIDTH);
    CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_CBO_UNITS);
    pHeight->GetRange(nMin, nMax);
    nPos = pNMUpDown->iPos + pNMUpDown->iDelta;
    if (nPos < nMin)
        nPos = nMin;
    if (nPos > nMax)
        nPos = nMax;
    if (pCombo->GetCurSel() == nPIXELS)
        sAmount.Format("%d", nPos);
    else
        sAmount.Format("%4.1f", nPos * 0.1);
    pHeight->GetBuddy()->SetWindowText(sAmount);
    *pResult = 0;
}

void CProcessDlg::ChkExtension(CString& sFileName)
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


