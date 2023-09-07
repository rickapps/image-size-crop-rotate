#pragma once


// CSaveDlg dialog

class CSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CSaveDlg)

public:
	CSaveDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSaveDlg();

// Dialog Data
	enum { IDD = IDD_SAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    BOOL m_bMoveNext;
    BOOL m_bReduceSize;
    bool m_bCropping;
    int m_nFolder;
    CString m_sFullName;
    CString m_sSourceDir;
    CString m_sProcessDir;
    CString m_sUserName;
private:
    int m_nDisposition;
protected:
    virtual void OnOK();
    void ChkExtension(CString& sFileName);
public:
    virtual BOOL OnInitDialog();
};
