// SizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "SizeDlg.h"
#include ".\sizedlg.h"


// CSizeDlg dialog

IMPLEMENT_DYNAMIC(CSizeDlg, CDialog)
CSizeDlg::CSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSizeDlg::IDD, pParent)
{
    m_nUnits = nPIXELS;
    m_fMaxHeight = 0;
    m_fMaxWidth = 0;
}

CSizeDlg::~CSizeDlg()
{
}

void CSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_CBIndex(pDX, IDC_CBO_UNITS, m_nUnits);
    DDX_Text(pDX, IDC_MAX_HEIGHT, m_fMaxHeight);
    DDV_MinMaxDouble(pDX, m_fMaxHeight, 0.1, 2000);
    DDX_Text(pDX, IDC_MAX_WIDTH, m_fMaxWidth);
	DDV_MinMaxDouble(pDX, m_fMaxWidth, 0.1, 2000);
}


BEGIN_MESSAGE_MAP(CSizeDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_CBO_UNITS, OnCbnSelchangeCboUnits)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEIGHT, OnDeltaposSpinHeight)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WIDTH, OnDeltaposSpinWidth)
END_MESSAGE_MAP()


// CSizeDlg message handlers

BOOL CSizeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    OnCbnSelchangeCboUnits();
    m_fMaxWidth = m_nWidth * 0.1;
    m_fMaxHeight = m_nHeight * 0.1;
    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CSizeDlg::OnCbnSelchangeCboUnits()
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

void CSizeDlg::OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult)
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

void CSizeDlg::OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult)
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

void CSizeDlg::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    if (UpdateData(TRUE))
    {
        m_nHeight = (int) (m_fMaxHeight * 10);
        m_nWidth = (int) (m_fMaxWidth * 10);
        CDialog::OnOK();
    }
}
