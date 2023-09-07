// ReminderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "ReminderDlg.h"
#include "RegisterDlg.h"
#include ".\reminderdlg.h"


// CReminderDlg dialog

IMPLEMENT_DYNAMIC(CReminderDlg, CDialog)
CReminderDlg::CReminderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReminderDlg::IDD, pParent)
{
    m_nConfiguration = Startup;
    m_nDaysRemaining = 0;
}

CReminderDlg::~CReminderDlg()
{
    DeleteObject(m_Bitmap);
    DeleteObject(m_Background);
	DeleteObject(m_hBackBrush);
    DeleteObject(m_hLogoBrush);
    m_oFont.DeleteObject();
    m_oAltFont.DeleteObject();
}

void CReminderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CReminderDlg, CDialog)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_SHOWKEY, OnBnClickedShowkey)
    ON_BN_CLICKED(IDC_PURCHASE, OnBnClickedPurchase)
END_MESSAGE_MAP()


// CReminderDlg message handlers

BOOL CReminderDlg::OnInitDialog()
{
	// Change some fonts and make our hyperlink
	m_Link.SubclassDlgItem(IDC_LINK, this);
	m_Link.SetWindowText("Alternate Purchase Method");
	m_Link.m_sURL.LoadString(IDS_ALTPURCHASE_URL);

  	m_MailTo.SubclassDlgItem(IDC_MAILTO, this);
	m_MailTo.SetWindowText("Customer Support");
	m_MailTo.m_sURL.LoadString(IDS_MAILTO);

    m_Price.SubclassDlgItem(IDC_PRICE, this);
    m_Price.SetWindowText("$14.95");
    m_Price.m_sURL.LoadString(IDS_PURCHASE_URL);

    CDialog::OnInitDialog();

	m_hLogoBrush = CreateSolidBrush(RGB(0,0,128));
    m_Bitmap.LoadBitmap(IDB_BUYNOW);
    CButton* pButton = (CButton*) GetDlgItem(IDC_PURCHASE);
    pButton->SetBitmap(m_Bitmap);
    pButton = (CButton*) GetDlgItem(IDCANCEL);

    // Setup our font for the price
	CRect oSize;
	CStatic* pStatic = (CStatic*) GetDlgItem(IDC_PRICE);
	pStatic->GetWindowRect(oSize);
	m_oFont.CreateFont(oSize.Height(), oSize.Width()/7, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_SCRIPT, "MS Sans Serif");
	pStatic->SetFont(&m_oFont);
    CEdit* pEdit = (CEdit*) GetDlgItem(IDC_MESSAGE);
	m_oAltFont.CreateFont(0, 0, 0, 0, FW_BOLD, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_SCRIPT, "MS Sans Serif");

    // Setup the logo area and buttons
    if (m_nDaysRemaining > 3)
    {
        m_Background.LoadBitmap(IDB_BLACK);  
        m_hBackBrush = CreateSolidBrush(RGB(0,0,255));
        m_BackColor = RGB(0,0,255);
        m_TextColor = RGB(255,255,255);
    }
    else if (m_nDaysRemaining >= 0)  // Allow them to use at zero days
    {
        m_Background.LoadBitmap(IDB_RED);
        m_hBackBrush = CreateSolidBrush(RGB(255,0,0));
        m_BackColor = RGB(255,0,0);
        m_TextColor = RGB(255,255,255);
    }
    else
        m_nConfiguration = Expired;


    CString sMessage;
    switch (m_nConfiguration) {
        case Expired:
            m_Background.LoadBitmap(IDB_WHITE);
            m_hBackBrush = CreateSolidBrush(RGB(255,255,0));
            m_BackColor = RGB(255,255,0);
            m_TextColor = RGB(0,0,0);
            pButton->SetWindowText("Exit Program");
#ifdef REMINDER
            sMessage.Format(IDS_REMIND_OVER, TRIALTIME);
#else
            sMessage.LoadString(IDS_EBAY_OVER);
#endif
            break;
        case Startup:
            pButton->SetWindowText("Continue Trial");
            sMessage.Format(IDS_REMIND_START, TRIALTIME, m_nDaysRemaining);
            break;
        case Shutdown:
            pButton->SetWindowText("Exit");
            sMessage.Format(IDS_REMIND_END, m_nDaysRemaining);
            break;
    }
	pStatic = (CStatic*) GetDlgItem(IDC_BACK);
	pStatic->SetBitmap((HBITMAP) m_Background);
    pEdit->SetFont(&m_oAltFont);
    pEdit->SetWindowText(sMessage);




    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CReminderDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr;

    // TODO:  Change any attributes of the DC here
	// Set the drawing properties of our controls
    switch (pWnd->GetDlgCtrlID()) {
        case IDC_MESSAGE:
        case IDC_LINK:
        case IDC_MAILTO:
            pDC->SetBkColor(m_BackColor);
            pDC->SetTextColor(m_TextColor);
            hbr = m_hBackBrush;
            break;
        default:
	        pDC->SetBkColor(RGB(0,0,128));
            pDC->SetTextColor(RGB(255,255,128));
            if (pWnd->GetDlgCtrlID() == IDC_COPYRIGHT)
                pDC->SetTextColor(RGB(0,255,0));  // make it green

            if (nCtlColor == CTLCOLOR_STATIC)
		        hbr= m_hLogoBrush;
	        else
		        hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    }
    return hbr;
}

void CReminderDlg::SetState(config nType, int nRemain)
{
    m_nConfiguration = nType;
    m_nDaysRemaining = nRemain;
}

void CReminderDlg::OnBnClickedShowkey()
{
    // Let them register
    CRegisterDlg dlg;
    dlg.DoModal();
}

void CReminderDlg::OnBnClickedPurchase()
{
	HINSTANCE hResult;
    CString sURL;
    sURL.LoadString(IDS_PURCHASE_URL);
    // Take us to our purchase page
	hResult = ShellExecute(NULL, "open", sURL, NULL, NULL, SW_SHOWNORMAL);
	if (hResult <= (HINSTANCE) 32)
		AfxMessageBox("Start your web browser and visit http://rajeware.com", IDOK, 0);
}
