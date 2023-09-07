#pragma once


// CSizeDlg dialog

class CSizeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSizeDlg)

public:
	CSizeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSizeDlg();
    int m_nWidth;           // Divide by ten to get m_fMaxWidth
    int m_nHeight;          // Divide by ten to get m_fMaxHeight
    int m_nUnits;           // Centimeters, Inches, or pixels

// Dialog Data
	enum { IDD = IDD_RESIZE };

private:
    double m_fMaxHeight;    // If units are pixels, these match
    double m_fMaxWidth;     // m_FrameSize

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnCbnSelchangeCboUnits();
    afx_msg void OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult);
protected:
    virtual void OnOK();
};
