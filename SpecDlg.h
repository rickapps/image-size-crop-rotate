#pragma once


// CSpecDlg dialog

class CSpecDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpecDlg)

public:
	CSpecDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecDlg();

// Dialog Data
	enum { IDD = IDD_SETSPECS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CString m_sProcessFolder;
    CString m_sSourceFolder;
    int m_nHeight;
    int m_nWidth;
    int m_nUnits;
    afx_msg void OnBnClickedBrowse();
    afx_msg void OnBnClickedSetsize();
protected:
    virtual void OnOK();
public:
    virtual BOOL OnInitDialog();
private:
    void SetLabel(void);
public:
    // Populate the listbox with three choices for the Processed Folder
    int PopulateFolderList(int nDefault);
    int m_nProcessFolder; //(nDESKTOPFOLDER, nAUTOSETFOLDER, nUSERDEFFOLDER)
};
