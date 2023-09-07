// RegisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "Utils.h"
#include "RegisterDlg.h"
#include "Registry.h"


// CRegisterDlg dialog

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialog)
CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterDlg::IDD, pParent)
    , m_sName(_T(""))
    , m_sCompany(_T(""))
    , m_sRegKey(_T(""))
{
}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_REGNAME, m_sName);
    DDV_MaxChars(pDX, m_sName, 40);
    DDX_Text(pDX, IDC_REGCOMPANY, m_sCompany);
    DDV_MaxChars(pDX, m_sCompany, 40);
    DDX_Text(pDX, IDC_REGKEY, m_sRegKey);
	DDV_MaxChars(pDX, m_sRegKey, 37);
}

BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
END_MESSAGE_MAP()


// CRegisterDlg message handlers

void CRegisterDlg::OnOK()
{
    int n = 0;
    int m = 0;
    CString sWord;

    // Do they have access rights?
    CRegistry oReg(CRegistry::Machine, "RAJE Software", "FitToList");
    if (!oReg.WriteInt("CropSet", "Check", 1))
    {
        AfxMessageBox(IDS_RIGHTS_ERROR);
        return;
    }
    if (UpdateData(TRUE))
    {
        m_sName.Trim();
        m_sCompany.Trim();
        m_sRegKey.Trim();
        m_sRegKey.MakeUpper();
        sWord = m_sName + m_sCompany;
        if (sWord.GetLength() > 23)
        {
            n = (sWord.GetLength() + sWord[6]) % 26;
            if (m_sRegKey.GetLength() > 13)
                m = m_sRegKey[13];
        }
        else if (m_sCompany.GetLength() > 7)
        {
            // Does not go in here
            sWord = m_sCompany;
            n = (sWord.GetLength() + sWord[2]) %26;
            if (m_sRegKey.GetLength() > 10)
                m = m_sRegKey[10];
        }
        if (n + 65 == m)
            n = GenMod(m_sRegKey);
        else
            n = 0;
    }

    if (n < 3 || n == 7 || n == 8)
    {
        UpdateData(FALSE);
        AfxMessageBox(IDS_KEY_ERROR);
    }
    else 
    {
        if (WriteLicense())
            CDialog::OnOK();
        else
            AfxMessageBox(IDS_WRITE_ERROR);
    }
}

// Only admin user can do this
bool CRegisterDlg::WriteLicense(void)
{
    bool bInd = false;
    char sName[MAX_COMPUTERNAME_LENGTH+1];
    BYTE sConvert[50];
    DWORD nSize = MAX_COMPUTERNAME_LENGTH+1;;
    if (::GetComputerName(sName, &nSize) == 0)
    {
        strcpy_s(sName, "Unknown");
        nSize = 7;
    }
    int nLen = m_sRegKey.GetLength();
    for (int i = 0; i<nLen; i++)
        sConvert[i] = (m_sRegKey[i] ^ sName[i%nSize]);
    CRegistry oReg(CRegistry::Machine, "RAJE Software", "FitToList");
    bInd = (oReg.WriteString("License", "User", m_sName) == TRUE);
    if (bInd)
        bInd = (oReg.WriteString("License", "Company", m_sCompany) == TRUE);
    if (bInd)
        bInd = (oReg.WriteBinary("License", "Key", sConvert, nLen) == TRUE);
    if (bInd)
        bInd = LeaveDropping("FitToList", true);

    return bInd;
}
