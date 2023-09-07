#pragma once


// CReminderDlg dialog

class CReminderDlg : public CDialog
{
	DECLARE_DYNAMIC(CReminderDlg)

public:
	CReminderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReminderDlg();

// Dialog Data
	enum { IDD = IDD_REMINDER };
    enum config { Expired = 0, Startup = 1, Shutdown = 2};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
private:
    int m_nDaysRemaining;
    config m_nConfiguration;
    CLinkURL m_Link;
    CLinkURL m_MailTo;
    CLinkURL m_Price;
    CBitmap m_Bitmap;
    CBitmap m_Background;
    CFont m_oFont;
    CFont m_oAltFont;
	HBRUSH m_hBackBrush;
    HBRUSH m_hLogoBrush;
    COLORREF m_BackColor;
    COLORREF m_TextColor;
    
public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    void SetState(config nType, int nRemain);
    afx_msg void OnBnClickedShowkey();
    afx_msg void OnBnClickedPurchase();
};
