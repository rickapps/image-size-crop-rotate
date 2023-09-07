#pragma once


// CResizeDlg dialog

class CResizeDlg : public CDialog
{
	DECLARE_DYNAMIC(CResizeDlg)

public:
	CResizeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CResizeDlg();
    CRect m_Resize;

// Dialog Data
	enum { IDD = IDD_RESIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    virtual void OnOK();
};
