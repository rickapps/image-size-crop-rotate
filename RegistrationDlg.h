#pragma once


// CRegistrationDlg dialog

class CRegistrationDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegistrationDlg)

public:
	CRegistrationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRegistrationDlg();

public:
    afx_msg void OnStnClickedBuynow();
    virtual BOOL OnInitDialog();
};
