// CropperView.cpp : implementation of the CCropperView class
//

#include "stdafx.h"
#include "Cropper.h"
#include "Utils.h"
#include "CropperDoc.h"
#include "CropperView.h"
#include ".\cropperview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCropperView

IMPLEMENT_DYNCREATE(CCropperView, CScrollView)

BEGIN_MESSAGE_MAP(CCropperView, CScrollView)
    ON_WM_SETCURSOR()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_SIZE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(ID_POPUP_NEXT, OnViewNext)
    ON_COMMAND(ID_POPUP_PREVIOUS, OnViewPrevious)
    ON_COMMAND(ID_RESTORE_PICTURE, OnRestorePicture)
END_MESSAGE_MAP()

// CCropperView construction/destruction

CCropperView::CCropperView()
{
	// TODO: add construction code here
    m_nOperation = nNONE;

}

CCropperView::~CCropperView()
{
}

// Get ready to display either a picture or a logo
bool CCropperView::InitViewArea(CDC* pDC, int nState)
{
    CBitmap oLogo;
    DIBSECTION ds;
    CDC memDC;
    CRect oSize;
    bool bShowImage;

    if (nState != nORIGINAL && nState != nPROCESSED)
    {
        // Display our logo
        if (nState == nERROR)
            oLogo.LoadBitmap(IDB_ERROR);
        else if (nState == nEMPTY)
            oLogo.LoadBitmap(IDB_DRAGDROP);
        else if (nState == nDRAGDROP)
            oLogo.LoadBitmap(IDB_DRAGDROP);
        else
            oLogo.LoadBitmap(IDB_SPLASH);
        oLogo.GetObject(sizeof(DIBSECTION), &ds);

        GetClientRect(&oSize);
        memDC.CreateCompatibleDC(pDC);
        CBitmap* pLogo = memDC.SelectObject(&oLogo);
        // Print the background
        pDC->StretchBlt(0,0,oSize.Width(), oSize.Height(), &memDC, 0, 0, 1, 1, SRCCOPY);
        // Display our image
        pDC->BitBlt(100, 50, ds.dsBm.bmWidth, ds.dsBm.bmHeight, &memDC, 0, 0, SRCCOPY);
        bShowImage = false;
    }
    else
        bShowImage = true;

    return bShowImage;
}

// CCropperView drawing
void CCropperView::OnDraw(CDC* pDC)
{
    CCropperDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

    // Do we display the logo?
    if (!InitViewArea(pDC, pDoc->CurrentState()))
        return;

    // Get the size of the image so we can set our scrolling region
    CPoint org(0, 0);
    CRect myRect(org, pDoc->GetSize());
    SetScrollSizes(MM_TEXT, pDoc->GetSize());
    //pDoc->GetImage()->drawEx(pDC->m_hDC, myRect);
    pDoc->m_WinImage.drawEx(pDC->m_hDC, myRect);
    
    if (IsCropping()) {
        int nOldMode = pDC->SetROP2(R2_MERGEPENNOT);
        pDC->SelectStockObject(NULL_BRUSH);
        myRect = GetDocument()->m_CropArea;
        pDC->Rectangle(myRect);
        if (myRect.Height() > 1 && myRect.Width() > 1) // Rect is always normalized here
        {
            myRect.InflateRect(1,1);
            pDC->Rectangle(myRect);
            myRect.DeflateRect(2,2);
            pDC->Rectangle(myRect);
        }
        pDC->SetROP2(nOldMode);
    }
    else  // Turn off the cropping area 
        pDoc->m_CropArea.SetRectEmpty();

    
}

void CCropperView::InvertCropArea(CRect oRect)
{
    CClientDC dc(this);
    OnPrepareDC(&dc);
    int nOldMode = dc.SetROP2(R2_MERGEPENNOT);
    //int nOldMode = dc.SetROP2(R2_NOT);
    dc.SelectStockObject(NULL_BRUSH);
    oRect.NormalizeRect();   // Height and Width methods only work for normalized rects
    dc.Rectangle(oRect);
    if (oRect.Height() > 1 && oRect.Width() > 1) 
    {
        oRect.InflateRect(1,1);
        dc.Rectangle(oRect);
        oRect.DeflateRect(2,2);
        dc.Rectangle(oRect);
    }
    dc.SetROP2(nOldMode);
    return;
}

void CCropperView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
    // Initialize our bounding frame
	CCropperDoc* pDoc = GetDocument();
    pDoc->m_SaveArea =
        pDoc->CalcFrame(pDoc->m_nSaveHeight, pDoc->m_nSaveWidth, pDoc->m_nSaveUnits);
}


// CCropperView diagnostics

#ifdef _DEBUG
void CCropperView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CCropperView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CCropperDoc* CCropperView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCropperDoc)));
	return (CCropperDoc*)m_pDocument;
}
#endif //_DEBUG


// CCropperView message handlers
void CCropperView::OnUpdateStatusBarPos(CCmdUI *pCmdUI)
{
    // Report the cursor position provided it is in the image area.
    CPoint curPos(0,0);
    CString sPosition;
    CRect oRect(curPos, GetDocument()->GetSize());
    ::GetCursorPos(&curPos);
    ScreenToClient(&curPos);
    if (oRect.PtInRect(curPos))
    {
        curPos = curPos + GetScrollPosition();
        sPosition.Format("Pos: (%d, %d)", curPos.x, curPos.y);
    }
    else
        sPosition.Empty();
    pCmdUI->Enable();
    pCmdUI->SetText(sPosition);
}

void CCropperView::OnUpdateStatusBarCrop(CCmdUI *pCmdUI)
{
    // Report the cropping area size if it is active
    CString sCropSize = "No Cropping";
    if (IsCropping()) 
    {
        CSize oCrop = GetDocument()->m_CropArea.Size();
        if (oCrop.cx < 0)
            oCrop.cx = -oCrop.cx; 
        if (oCrop.cy < 0)
            oCrop.cy = -oCrop.cy;
        sCropSize.Format("CROP: (%d, %d)", oCrop.cx, oCrop.cy);
    }
    pCmdUI->Enable();
    pCmdUI->SetText(sCropSize);
}

BOOL CCropperView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    BOOL nInd = TRUE;
    LPCSTR sCursor;

    switch (CursorType()) {
        case nTOP:
        case nBOTTOM:
            sCursor = IDC_SIZENS;
            break;
        case nRIGHT:
        case nLEFT:
            sCursor = IDC_SIZEWE;
            break;
        case nTOPLEFT:
        case nBOTTOMRIGHT:
            sCursor = IDC_SIZENWSE;
            break;
        case nTOPRIGHT:
        case nBOTTOMLEFT:
            sCursor = IDC_SIZENESW;
            break;
        case nMOVE:
            sCursor = IDC_SIZEALL;
            break;
        default:
            sCursor = NULL;
    }

    // Pop-up menus seem to be HTCAPTION areas
    if (nHitTest == HTCAPTION || nHitTest == HTMENU ||
        nHitTest == HTHSCROLL || nHitTest == HTVSCROLL)
        sCursor = NULL;

    if (sCursor == NULL)
        nInd = CScrollView::OnSetCursor(pWnd, nHitTest, message);
    else
        ::SetCursor(::LoadCursor(NULL, sCursor));
    return nInd;
}

// Compare the current cursor position with the current cropping area. Return the
// appropriate cursor for the position. If the cursor is not within the cropping area,
// return NULL.
int CCropperView::CursorType()
{
    int nType = nDRAW;
    CRect oBigger;
    CRect oSmaller;
    CRect oCropArea = GetDocument()->m_CropArea;

    // If the cropping area is very small, do not change the cursor
    if (!IsCropping()) 
        return nType;

    oBigger = oCropArea;
    oSmaller = oCropArea;
    oBigger.InflateRect(4, 4);   // Increase/Decrease size by four pixels to allow user
    oSmaller.DeflateRect(4, 4);    // to be a little sloppier with mouse postion.
    CPoint curPos;
    ::GetCursorPos(&curPos);
    ScreenToClient(&curPos);
    curPos = curPos + GetScrollPosition();
    if (oBigger.PtInRect(curPos)) {
        if (oSmaller.PtInRect(curPos))
            nType = nMOVE;         // Cursor is inside area - display move cursor
        else if (curPos.y >= oCropArea.top-4 && curPos.y <= oCropArea.top+4)
        {
            // Are we at a top corner?
            if (curPos.x >= oBigger.left && curPos.x <= oSmaller.left)
                nType = nTOPLEFT;
            else if (curPos.x >= oSmaller.right && curPos.x <= oBigger.right)
                nType = nTOPRIGHT;
            else
                nType = nTOP;       // We are on the edge
        }
        else if (curPos.y >= oCropArea.bottom-4 && curPos.y <= oCropArea.bottom+4)
        {
            // Are we on a bottom corner?
            if (curPos.x >= oBigger.left && curPos.x <= oSmaller.left)
                nType = nBOTTOMLEFT;
            else if (curPos.x >= oSmaller.right && curPos.x <= oBigger.right)
                nType = nBOTTOMRIGHT;
            else
                nType = nBOTTOM;
        }
        else if (curPos.x >= oCropArea.right-4 && curPos.x <= oCropArea.right+4)
        {
            // Are we on a top or bottom right corner?
            if (curPos.y >= oBigger.top && curPos.y <= oSmaller.top)
                nType = nTOPRIGHT;
            else if (curPos.y >= oSmaller.bottom && curPos.y <= oBigger.bottom)
                nType = nBOTTOMRIGHT;
            else
                nType = nRIGHT;
        }
        else if (curPos.x >= oCropArea.left-4 && curPos.x <= oCropArea.left+4)
        {
            // Are we on a top or bottom left corner?
            // Are we on a top or bottom right corner?
            if (curPos.y >= oBigger.top && curPos.y <= oSmaller.top)
                nType = nTOPLEFT;
            else if (curPos.y >= oSmaller.bottom && curPos.y <= oBigger.bottom)
                nType = nBOTTOMLEFT;
            else
                nType = nLEFT;
        }
    }

    return nType;
}

void CCropperView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CPoint offPoint;
    // We need to do something. Let's figure our what
    if (InPicture(point))
    {
        SetCapture();
        m_nOperation = CursorType();
    }
    else
        m_nOperation = nNONE;

    switch (m_nOperation) {
        case nNONE:
            break;
        case nDRAW:
            InvertCropArea(GetDocument()->m_CropArea);
            offPoint = point + GetScrollPosition();
            GetDocument()->m_CropArea.SetRect(offPoint, offPoint);
            break;
        case nMOVE:
            m_oMoveTo = point + GetScrollPosition();
           break;
        default:
            // Size the rectangle
            break;
    }
    CScrollView::OnLButtonDown(nFlags, point);
}

void CCropperView::OnLButtonUp(UINT nFlags, CPoint point)
{

    if (GetCapture() == this) {
        GetDocument()->m_CropArea.NormalizeRect();
        ReleaseCapture();
        m_nOperation = nNONE;
    }
    // If the cropping area is too small, make it go away
    if (!IsCropping()) {
        InvertCropArea(*(GetDocument()->m_CropArea));
        GetDocument()->m_CropArea.SetRectEmpty();
    }

    CScrollView::OnLButtonUp(nFlags, point);
}

void CCropperView::OnMouseMove(UINT nFlags, CPoint point)
{
    // Get a pointer to our current cropping area
    CRect* pCropArea = &(GetDocument()->m_CropArea);
    // Make a copy of it
    CRect oMove(pCropArea);
    CPoint theOffset;
    CRect oPicture(0, 0, GetDocument()->GetSize().cx, GetDocument()->GetSize().cy);
    if (GetCapture() == this) {
        point = point + GetScrollPosition();
        InvertCropArea(*pCropArea);
        switch (m_nOperation) {
            case nDRAW:
                // Do not draw outside picture area
                InPicture(point);
                pCropArea->SetRect(pCropArea->TopLeft(), point);
                InvertCropArea(*pCropArea);
            break;
            case nMOVE:
                theOffset = point - m_oMoveTo;
                oMove.OffsetRect(theOffset);
                // Make adjustments if the cropping area is outside the picture
                if (oMove.BottomRight().x > oPicture.BottomRight().x)
                    theOffset.x = theOffset.x + (oPicture.BottomRight().x - oMove.BottomRight().x);
                if (oMove.BottomRight().y > oPicture.BottomRight().y)
                    theOffset.y = theOffset.y + (oPicture.BottomRight().y - oMove.BottomRight().y);
                if (oMove.TopLeft().x < oPicture.TopLeft().x)
                    theOffset.x = theOffset.x + (oPicture.TopLeft().x - oMove.TopLeft().x);
                if (oMove.TopLeft().y < oPicture.TopLeft().y)
                    theOffset.y = theOffset.y + (oPicture.TopLeft().y - oMove.TopLeft().y);
                // Move the cropping area for real
                pCropArea->OffsetRect(theOffset);
                m_oMoveTo = point;
                InvertCropArea(*pCropArea);
                break;
            case nRIGHT:
                InPicture(point);  // Adjust our point to be within picture area
                pCropArea->SetRect(pCropArea->TopLeft().x,
                    pCropArea->TopLeft().y,
                    point.x, pCropArea->BottomRight().y);
                InvertCropArea(*pCropArea);
                break;
            case nLEFT:
                InPicture(point);  // Adjust our point to be within picture area
                pCropArea->SetRect(point.x, pCropArea->TopLeft().y,
                    pCropArea->BottomRight().x,
                    pCropArea->BottomRight().y);
                InvertCropArea(*pCropArea);
                break;
            case nTOP:
               InPicture(point);  // Adjust our point to be within picture area
               pCropArea->SetRect(pCropArea->TopLeft().x, point.y,
                    pCropArea->BottomRight().x,
                    pCropArea->BottomRight().y);
                InvertCropArea(*pCropArea);
                break;
            case nBOTTOM:
                InPicture(point);  // Adjust our point to be within picture area
                pCropArea->SetRect(pCropArea->TopLeft().x,
                    pCropArea->TopLeft().y,
                    pCropArea->BottomRight().x, point.y);
                InvertCropArea(*pCropArea);
                break;
            case nTOPLEFT:
                InPicture(point);  // Adjust our point to be within picture area
                pCropArea->SetRect(point, pCropArea->BottomRight());
                InvertCropArea(*pCropArea);
                break;
            case nTOPRIGHT:
                InPicture(point);  // Adjust our point to be within picture area
                pCropArea->SetRect(pCropArea->TopLeft().x, point.y,
                    point.x, pCropArea->BottomRight().y);
                InvertCropArea(*pCropArea);
                break;
            case nBOTTOMRIGHT:
                InPicture(point);  // Adjust our point to be within picture area
                pCropArea->SetRect(pCropArea->TopLeft(), point);
                InvertCropArea(*pCropArea);
                break;
            case nBOTTOMLEFT:
                InPicture(point);  // Adjust our point to be within picture area
                pCropArea->SetRect(point.x, pCropArea->TopLeft().y,
                    pCropArea->BottomRight().x, point.y);
                InvertCropArea(*pCropArea);
                break;
            default:
                break;
        }
    }
    CScrollView::OnMouseMove(nFlags, point);
}

// // Is the current position inside the image?
BOOL CCropperView::InPicture(CPoint& point)
{
    BOOL bInd;
    CPoint oPoint(0, 0);
    CRect oRect(oPoint, GetDocument()->GetSize());
    bInd = oRect.PtInRect(point);
    if (!bInd) {
        // Adjust the point so it is inside the image area
        if (point.x > oRect.BottomRight().x)
            point.x = oRect.BottomRight().x;
        if (point.x < oRect.TopLeft().x)
            point.x = oRect.TopLeft().x;
        if (point.y > oRect.BottomRight().y)
            point.y = oRect.BottomRight().y;
        if (point.y < oRect.TopLeft().y)
            point.y = oRect.TopLeft().y;
    }
    return bInd;
}

// Return true if we have a valid cropping area within the picture.
bool CCropperView::IsCropping(void)
{
    bool bInd;
    // Make a copy of the cropping area so we can normalize it.
    // Do not normalize the actual cropping area - that causes drawing problems.
    CRect oCrop(GetDocument()->m_CropArea);
    oCrop.NormalizeRect();
    CSize oArea = oCrop.Size();
    bInd = (oArea.cx * oArea.cy > 150);
    if (bInd) {
        // Make sure cropping area is inside picture
        CRect oPicture(0, 0, GetDocument()->GetSize().cx, GetDocument()->GetSize().cy);
        CRect oRect;
        oRect.IntersectRect(&oPicture, &oCrop);
        if (oRect.Size() != oArea) 
            bInd = false;
    }
    return bInd;
}

void CCropperView::OnSize(UINT nType, int cx, int cy)
{
    CScrollView::OnSize(nType, cx, cy);

    // Tell the document the size of our client area
    // We use this to size the image to fit the screen
    this->GetClientRect(&(GetDocument()->m_ClientArea));
}

// Quick way to process an image
void CCropperView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    GetDocument()->OnFileSaveAs();
}

void CCropperView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    // If we are changing pictures, reset the scroll region
    SetScrollPos(SB_HORZ, 0);
    SetScrollPos(SB_VERT, 0);
    CScrollView::OnUpdate(pSender, lHint, pHint);
}


void CCropperView::OnRButtonDown(UINT nFlags, CPoint point)
{
    if (GetDocument()->CurrentState() == nORIGINAL ||
        GetDocument()->CurrentState() == nPROCESSED)
    {
	    CMenu menu;
        ClientToScreen(&point);
	    VERIFY(menu.LoadMenu(IDR_PICTURE_MENU));
	    CMenu* pPopup = menu.GetSubMenu(0);
	    ASSERT(pPopup != NULL);
	    ::SetMenuDefaultItem(pPopup->m_hMenu, ID_FILE_SAVE_AS, false);
	    pPopup->TrackPopupMenu(TPM_LEFTALIGN
						        | TPM_RIGHTBUTTON, 
						    point.x, point.y, this); 
    }
    CScrollView::OnRButtonDown(nFlags, point);
}

void CCropperView::OnViewNext()
{
    // From the pop-up menu
    AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, ID_VIEW_NEXT, 0);
}

void CCropperView::OnViewPrevious()
{
    // From the pop-up menu
    AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, ID_VIEW_PREVIOUS, 0);
}

void CCropperView::OnRestorePicture()
{
    // Read the picture from disk
    GetDocument()->SetImage(GetDocument()->CurrentState(), 
        FileName(GetDocument()->m_sCurImage));

}
