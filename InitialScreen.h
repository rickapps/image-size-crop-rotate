#pragma once


// CReminderDlg dialog

class CReminderDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CReminderDlg)

public:
	CReminderDlg(CWnd* pParent = NULL);   // standard constructor
    CReminderDlg(UINT nTemplate, CWnd* pParent = NULL);
	virtual ~CReminderDlg();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum {IDD = IDD_REMINDER, IDH = IDR_STARTUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
