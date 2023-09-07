// RotateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Cropper.h"
#include "RotateDlg.h"
#include ".\rotatedlg.h"


// CRotateDlg dialog

IMPLEMENT_DYNAMIC(CRotateDlg, CDialog)
CRotateDlg::CRotateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRotateDlg::IDD, pParent)
    , m_nAngle(0)
{
}

CRotateDlg::~CRotateDlg()
{
}

void CRotateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRotateDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CRotateDlg message handlers

BOOL CRotateDlg::OnInitDialog()
{

    // Set our slider control
    CStatic* pStatic = (CStatic*) GetDlgItem(IDC_FRAME);
    pStatic->GetWindowRect(&m_rectBoundary);
    ScreenToClient(&m_rectBoundary);
    CSliderCtrl* pSlider = (CSliderCtrl*) GetDlgItem(IDC_ANGLE);
    m_nSliderPos = 10 - m_nAngle;   // Slider pos is 0 at top, 20 at bottom
    pSlider->SetRange(0, 20);
    pSlider->SetTicFreq(5);
	pSlider->SetPos(m_nSliderPos);
    CDialog::OnInitDialog();
    

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CRotateDlg::OnPaint()
{
    CPoint origin;
    CPoint endPoint;
    origin.x = m_rectBoundary.TopLeft().x;
    origin.y = m_rectBoundary.TopLeft().y +
        (m_rectBoundary.BottomRight().y - m_rectBoundary.TopLeft().y) / 2;

    endPoint.x = m_rectBoundary.BottomRight().x;
    endPoint.y = origin.y + (m_nSliderPos - 10) * 
        (m_rectBoundary.BottomRight().y - m_rectBoundary.TopLeft().y) / 20;

    CPen oRed(PS_SOLID, 1, RGB(255,0,0));
    CPaintDC dc(this); // device context for painting
    dc.SelectObject(&oRed);
    dc.MoveTo(origin);
    dc.LineTo(endPoint);

}

void CRotateDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // They are moving the slider control
    CSliderCtrl* pSlider = (CSliderCtrl*) GetDlgItem(IDC_ANGLE);
    // Cannot use nPos. Only accurate if nSBCode is TB_THUMBTRACK
    m_nSliderPos = pSlider->GetPos();
    TRACE("Value is: %d\n", m_nSliderPos);
    InvalidateRect(m_rectBoundary, TRUE);
    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CRotateDlg::OnOK()
{
    // Return the position of the angle
    m_nAngle = 10 - m_nSliderPos;  // Slider pos is 0 at top, 20 at bottom
    CDialog::OnOK();
}
