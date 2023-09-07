#pragma once


// CProcessDlg dialog

class CProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessDlg)

public:
	CProcessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcessDlg();

// Dialog Data
	enum { IDD = IDD_SAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedChkReduce();
    afx_msg void OnCbnSelchangeCboUnits();
    afx_msg void OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult);

public:
    CString m_sProcessName;
    CString m_sSourceDir;
    CString m_sProcessDir;
    BOOL m_bMoveNext;       // Indicates move to next picture
    BOOL m_bReduceSize;     // Do we alter the picture size?
    int m_nWidth;           // Divide by ten to get m_fMaxWidth
    int m_nHeight;          // Divide by ten to get m_fMaxHeight
    int m_nUnits;           // Centimeters, Inches, or pixels

    int m_nFolder;

private:
    double m_fMaxHeight;    // If units are pixels, these match
    double m_fMaxWidth;     // m_FrameSize
    void ChkExtension(CString& sFileName);
};
