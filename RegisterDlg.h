#pragma once


// CRegisterDlg dialog

class CRegisterDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRegisterDlg();

// Dialog Data
	enum { IDD = IDD_REGISTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CString m_sName;
    CString m_sCompany;
    CString m_sRegKey;
protected:
    virtual void OnOK();
public:
    bool WriteLicense(void);
};
