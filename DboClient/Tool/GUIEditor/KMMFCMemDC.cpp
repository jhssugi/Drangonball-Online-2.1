// KMMFCMemDC.cpp: implementation of the CKMMemDC class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"		// <-- Precomplied Header



#include "KMMFCMemDC.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//@@---------------------------------------------------------------------------
// ????/?Ҹ???
//
//-----------------------------------------------------------------------------
CKMMemDC::CKMMemDC()
{
	InitMember();
}

CKMMemDC::CKMMemDC( CWnd* pSrcWnd, int width, int height, COLORREF clr )
{
	InitMember();
	Create( pSrcWnd, width, height, clr );
}

CKMMemDC::CKMMemDC( CWnd* pSrcWnd, UINT nIDBitmap, BITMAP* pBMInfo )
{
	InitMember();
	CreateFromID( pSrcWnd, nIDBitmap, pBMInfo );
}

CKMMemDC::CKMMemDC( CWnd* pSrcWnd, const char* szFileName, BITMAP* pBMInfo )
{
	InitMember();
	CreateFromFile( pSrcWnd, szFileName, pBMInfo );
}

CKMMemDC::CKMMemDC( CWnd* pSrcWnd, HBITMAP hBitmap, BITMAP* pBMInfo )
{
	InitMember();
	CreateFromHBM( pSrcWnd, hBitmap, pBMInfo );
}

CKMMemDC::~CKMMemDC()
{
	Delete();
}


//@@---------------------------------------------------------------------------
// InitMember()
// - ?ܼ? ???????? ?ʱ?ȭ
//-----------------------------------------------------------------------------
void CKMMemDC::InitMember()
{
	m_nWidth  = 0;
	m_nHeight = 0;
	m_pMDC = NULL;

	m_crColorkey = RGB(0,0,0);
}// end of InitMember()


//@@---------------------------------------------------------------------------
// Delete()
// - ?ش? MemDC?? ?ı?
//-----------------------------------------------------------------------------
void CKMMemDC::Delete()
{

	if( m_pMDC != NULL ) 
	{
		m_pMDC->DeleteDC();
		delete( m_pMDC );
		
		InitMember();
	}

}// end of Delete()


//@@---------------------------------------------------------------------------
// Create()
// - ?ѻ????θ? ä???? ???????ִ? Memory DC ??????
//-----------------------------------------------------------------------------
int CKMMemDC::Create( CWnd* pSrcWnd, int width, int height, COLORREF clr )
{

	//// ????üũ
	if( pSrcWnd == NULL )
	{
		AfxMessageBox( "CKMMemDC::CreateEmpty()\n\n ?????????? pSrcWnd" );
		return FALSE;
	}

	if( m_pMDC != NULL )
	{
		AfxMessageBox( "CKMMemDC::CreateEmpty()\n\n m_pMDC?? ?̹? ????????" );
		return FALSE;
	}


	//// ?ҽ??? ?????? CDC ?ϳ? ??????
	CClientDC srcDC( pSrcWnd );


	//// MemoryDC ????

	//(1) ?ʿ??? ?? ?޾Ƴ??? 
	m_nWidth  = width;
	m_nHeight = height;

	//(2) ȣȯ?? MemDC ??????
	m_pMDC = new CDC;
	m_pMDC->CreateCompatibleDC( &srcDC );

	//(3) ȣȯ?? Bitmap ??????
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &srcDC, width, height );

	//(4) MemDC?? Bitmap ???????ֱ?
	m_pMDC->SelectObject( bitmap );

	//(5) ???̻? ?ʿ????? Bitmap ?ı?
	bitmap.DeleteObject();
	

	//// ?? ä????
	CRect rc(0,0,width,height);
	CBrush brush( clr );
	m_pMDC->FillRect( &rc, &brush );

	return TRUE;
}// end of Create()



//@@---------------------------------------------------------------------------
// CreateFromID()
// - Resource?? Bitmap?? ?̿??? MemoryDC ??????
//-----------------------------------------------------------------------------
int CKMMemDC::CreateFromID( CWnd* pSrcWnd, 
							   UINT nIDBitmap, 
							   BITMAP* pBMInfo )
{

	//// ????üũ
	if( pSrcWnd == NULL )
	{
		AfxMessageBox( "CKMMemDC::CreateFromID()\n\n ?????????? pSrcWnd" );
		return FALSE;
	}

	if( m_pMDC != NULL )
	{
		AfxMessageBox( "CKMMemDC::CreateFromID()\n\n m_pMDC?? ?̹? ????????" );
		return FALSE;
	}


	//// ?ҽ??? ?????? CDC ?ϳ? ??????
	CClientDC srcDC( pSrcWnd );


	//// MemoryDC ????

	//(1) ȣȯ?? MemDC ??????
	m_pMDC = new CDC;
	m_pMDC->CreateCompatibleDC( &srcDC );


	//(2) ȣȯ?? Bitmap ??????
	CBitmap bitmap;
	CBitmap* pBitmapOld = NULL;

	//(3) Resource Bitmap ID???? ?̿??Ͽ? Bitmap ?б?
	bitmap.LoadBitmap( nIDBitmap );

	//(4) MemDC?? Bitmap ???????ֱ?
	pBitmapOld = (CBitmap*)m_pMDC->SelectObject( bitmap );

		//// ?ʿ??? ?????? ????
		BITMAP bm;	
		bitmap.GetBitmap( &bm );

		m_nWidth  = bm.bmWidth;
		m_nHeight = bm.bmHeight;

		// ?????ڰ? ??û?? ???쿡?? Bitmap ?????? ??????.
		if( pBMInfo != NULL )
			bitmap.GetBitmap( pBMInfo );

	//(5) ???̻? ?ʿ????? Bitmap ?ı?
	bitmap.DeleteObject();

	return TRUE;
}// end of CreateFromID()


//@@---------------------------------------------------------------------------
// CreateFromFile()
// - ?ܺο? ?ִ? Bitmap ?????? ?о MemoryDC?? ????.
//-----------------------------------------------------------------------------
int CKMMemDC::CreateFromFile( CWnd* pSrcWnd, 
							     const char* szFileName, 
							     BITMAP* pBMInfo )
{

	//// ????üũ
	if( pSrcWnd == NULL )
	{
		AfxMessageBox( "CKMMemDC::CreateFromFile()\n\n ?????????? pSrcWnd" );
		return FALSE;
	}

	if( m_pMDC != NULL )
	{
		AfxMessageBox( "CKMMemDC::CreateFromFile()\n\n m_pMDC?? ?̹? ????????" );
		return FALSE;
	}


	//// ?ҽ??? ?????? CDC ?ϳ? ??????
	CClientDC srcDC( pSrcWnd );


	//// MemoryDC ????

	//(1) ȣȯ?? MemDC ??????
	m_pMDC = new CDC;
	m_pMDC->CreateCompatibleDC( &srcDC );


	//(2) ?ܺ? ???Ϸ? ???? HBITMAP???? ?б?
	//    - MFC?????? ?Լ??? ??ã?? API/GDI ???? ?Լ? ????????.
	HBITMAP hBitmap;
	hBitmap = (HBITMAP) ::LoadImage( NULL, szFileName, IMAGE_BITMAP,
									 0, 0, LR_LOADFROMFILE );
	if( hBitmap == NULL )
		return 0;

	//(3) MemDC?? HBITMAP ???????ֱ?
	::SelectObject( m_pMDC->m_hDC, hBitmap );

		//// ?ʿ??? ?????? ????
 		BITMAP bm;
		::GetObject( hBitmap, sizeof(bm), &bm );

		m_nWidth  = bm.bmWidth;
		m_nHeight = bm.bmHeight;

		// ?????ڰ? ??û?? ???쿡?? Bitmap ?????? ??????.
		if( pBMInfo != NULL )
			::GetObject( hBitmap, sizeof(bm), &pBMInfo );

	//(4) ?? ?̻? ?ʿ????? HBITMAP ?ı? 
	::DeleteObject( hBitmap );

	return TRUE;
}// end of CreateFromFile()



//@@---------------------------------------------------------------------------
// CreateFromHBM()
// - HBITMAP???? MemoryDC?? ????.
//-----------------------------------------------------------------------------
int CKMMemDC::CreateFromHBM( CWnd* pSrcWnd, 
							    HBITMAP hBitmap, 
							    BITMAP* pBMInfo )
{

	//// ????üũ
	if( pSrcWnd == NULL )
	{
		AfxMessageBox( "CKMMemDC::CreateFromHBM()\n\n ?????????? pSrcWnd" );
		return FALSE;
	}

	if( m_pMDC != NULL )
	{
		AfxMessageBox( "CKMMemDC::CreateFromHBM()\n\n m_pMDC?? ?̹? ????????" );
		return FALSE;
	}


	//// ?ҽ??? ?????? CDC ?ϳ? ??????
	CClientDC srcDC( pSrcWnd );


	//// MemoryDC ????

	//(1) ȣȯ?? MemDC ??????
	m_pMDC = new CDC;
	m_pMDC->CreateCompatibleDC( &srcDC );


	//(2) MemDC?? HBITMAP ???????ֱ?
	::SelectObject( m_pMDC->m_hDC, hBitmap );

		//// ?ʿ??? ?????? ????
 		BITMAP bm;
		::GetObject( hBitmap, sizeof(bm), &bm );

		m_nWidth  = bm.bmWidth;
		m_nHeight = bm.bmHeight;

		// ?????ڰ? ??û?? ???쿡?? Bitmap ?????? ??????.
		if( pBMInfo != NULL )
			::GetObject( hBitmap, sizeof(bm), &pBMInfo );

	return TRUE;
}// end of CreateFromHBM()





//@@---------------------------------------------------------------------------
// 
//
//-----------------------------------------------------------------------------
int CKMMemDC::BltTo( CDC* pDestDC, 
					 int dx, int dy, 
					 DWORD dwROP )
{
	return ::BitBlt( pDestDC->m_hDC, 
					 dx, dy, 
					 m_nWidth, m_nHeight, 

					 m_pMDC->m_hDC, 
					 0, 0, 

					 dwROP );
}//

int CKMMemDC::BltTo( int sx, int sy,
					 CDC* pDestDC, 
					 int dx, int dy,
					 DWORD dwROP )
{
	return ::BitBlt( pDestDC->m_hDC,   
					 dx, dy, 
					 m_nWidth, m_nHeight, 

					 m_pMDC->m_hDC, 
					 sx, sy, 

					 dwROP );
}//

int CKMMemDC::BltTo( int sx, int sy, int sw, int sh,
					 CDC* pDestDC, 
					 int dx, int dy,
					 DWORD dwROP )
{
	return ::BitBlt( pDestDC->m_hDC,   
					 dx, dy, 
					 sw, sh, 

					 m_pMDC->m_hDC, 
					 sx, sy, 

					 dwROP );
}//



//-----------------------------------------------------------------------------
// StretchBltTo()
// - Ȯ??/???? ???????? ???? 
//-----------------------------------------------------------------------------
int CKMMemDC::StretchBltTo( CDC* pDestDC, 
							int dx, int dy, int dw, int dh,
							DWORD dwROP )
{
	pDestDC->SetStretchBltMode( HALFTONE );

	return ::StretchBlt( pDestDC->m_hDC,   
						 dx, dy, dw, dh,
		
						 m_pMDC->m_hDC, 
						 0, 0, m_nWidth, m_nHeight,
						 dwROP );
	
}// end of StretchBltTo()

int CKMMemDC::StretchBltTo( int sx, int sy,
							CDC* pDestDC, 
							int dx, int dy, int dw, int dh,
							DWORD dwROP )
{
	pDestDC->SetStretchBltMode( HALFTONE );

	return ::StretchBlt( pDestDC->m_hDC,   
						 dx, dy, dw, dh,
		
						 m_pMDC->m_hDC, 
						 sx, sy, m_nWidth, m_nHeight,
						 dwROP );
	
}// end of StretchBltTo()

int CKMMemDC::StretchBltTo( int sx, int sy, int sw, int sh,
							CDC* pDestDC, 
							int dx, int dy, int dw, int dh,
							DWORD dwROP )
{
	pDestDC->SetStretchBltMode( HALFTONE );
	
	return ::StretchBlt( pDestDC->m_hDC,   
						 dx, dy, dw, dh,
		
						 m_pMDC->m_hDC, 
						 sx, sy, sw, sh,
						 dwROP );
	
}// end of StretchBltTo(x,y,w,h, dc,x,y,w,h rop)



//-----------------------------------------------------------------------------
// TransBltTo()
// - ?÷?Ű ?????ؼ? ???? / ?߰? library ?ʿ??? ( msimg32.lib )
//-----------------------------------------------------------------------------
int CKMMemDC::TransBltTo( CDC* pDestDC, 
					      int dx, int dy )
{
	return ::TransparentBlt( pDestDC->m_hDC,   
							 dx, dy, m_nWidth, m_nHeight,
					   
							 m_pMDC->m_hDC, 
							 0, 0, m_nWidth, m_nHeight,
							 m_crColorkey );
	
}// end of TransparentBltTo()

int CKMMemDC::TransBltTo( int sx, int sy,
						  CDC* pDestDC, 
					      int dx, int dy )
{
	return ::TransparentBlt( pDestDC->m_hDC,   
							 dx, dy, m_nWidth, m_nHeight,
					   
							 m_pMDC->m_hDC, 
							 sx, sy, m_nWidth, m_nHeight,
							 m_crColorkey );
	
}// end of TransparentBltTo()

int CKMMemDC::TransBltTo( int sx, int sy, int sw, int sh,
						   CDC* pDestDC, 
					       int dx, int dy, int dw, int dh )
{
	return ::TransparentBlt( pDestDC->m_hDC,   
							 dx, dy, dw, dh,
					   
							 m_pMDC->m_hDC, 
							 sx, sy, sw, sh,
							 m_crColorkey );
	
}// end of TransparentBltTo(x,y,w,h, dc,x,y,w,h, clr)

////
int CKMMemDC::TransBltTo( CDC* pDestDC, 
					      int dx, int dy,
						  COLORREF clrKey )
{
	return ::TransparentBlt( pDestDC->m_hDC,   
							 dx, dy, m_nWidth, m_nHeight,
					   
							 m_pMDC->m_hDC, 
							 0, 0, m_nWidth, m_nHeight,
							 clrKey );
	
}// end of TransparentBltTo()

int CKMMemDC::TransBltTo( int sx, int sy,
						  CDC* pDestDC, 
					      int dx, int dy,
						  COLORREF clrKey )
{
	return ::TransparentBlt( pDestDC->m_hDC,   
							 dx, dy, m_nWidth, m_nHeight,
					   
							 m_pMDC->m_hDC, 
							 sx, sy, m_nWidth, m_nHeight,
							 clrKey );
	
}// end of TransparentBltTo()

int CKMMemDC::TransBltTo( int sx, int sy, int sw, int sh,
						   CDC* pDestDC, 
					       int dx, int dy, int dw, int dh,
						   COLORREF clrKey )
{
	return ::TransparentBlt( pDestDC->m_hDC,   
							 dx, dy, dw, dh,
					   
							 m_pMDC->m_hDC, 
							sx, sy, sw, sh,
							clrKey );
	
}// end of TransparentBltTo(x,y,w,h, dc,x,y,w,h, clr)


//@@---------------------------------------------------------------------------
// AlphaBltTo()
// - ???????? ?????ؼ? ????
//-----------------------------------------------------------------------------
int CKMMemDC::AlphaBltTo( CDC* pDestDC, 
						  int dx, int dy,
						  BYTE alpha )
{
	static BLENDFUNCTION bf;

	bf.BlendOp				= AC_SRC_OVER;
	bf.BlendFlags			= 0;
	bf.SourceConstantAlpha	= alpha;
	bf.AlphaFormat			= 0; //AC_SRC_ALPHA; 
	
	int res = ::AlphaBlend( pDestDC->m_hDC,
							dx, dy, m_nWidth, m_nHeight,
						   
							m_pMDC->m_hDC,
							0, 0, m_nWidth, m_nHeight,
							bf );
	return res;
	
}// end of AlpahBltTo()

int CKMMemDC::AlphaBltTo( int sx, int sy,
						  CDC* pDestDC, 
						  int dx, int dy,
						  BYTE alpha )
{
	static BLENDFUNCTION bf;

	bf.BlendOp				= AC_SRC_OVER;
	bf.BlendFlags			= 0;
	bf.SourceConstantAlpha	= alpha;
	bf.AlphaFormat			= 0; //AC_SRC_ALPHA; 
	
	int res = ::AlphaBlend( pDestDC->m_hDC,
							 dx, dy, m_nWidth, m_nHeight,
						   
							 m_pMDC->m_hDC,
							 sx, sy, m_nWidth, m_nHeight,
							 bf );
	return res;
	
}// end of AlpahBltTo()


int CKMMemDC::AlphaBltTo( int sx, int sy, int sw, int sh,
						   CDC* pDestDC, 
						   int dx, int dy, int dw, int dh,
						   BYTE alpha )
{
	static BLENDFUNCTION bf;

	bf.BlendOp				= AC_SRC_OVER;
	bf.BlendFlags			= 0;
	bf.SourceConstantAlpha	= alpha;
	bf.AlphaFormat			= 0; //AC_SRC_ALPHA; 
	
	int res = ::AlphaBlend( pDestDC->m_hDC,
							 dx, dy, dw, dh,
					   
							 m_pMDC->m_hDC,
							 sx, sy, sw, sh,
							 bf );

	return res;
	
}// end of AlpahBltTo(x,y,w,h, dc,x,y,w,h, a)


//@@---------------------------------------------------------------------------
// FillColor()
// - ?ܼ? ?? ä????.
//-----------------------------------------------------------------------------
int CKMMemDC::FillColor( COLORREF clr )
{
	CRect rc(0,0,m_nWidth,m_nHeight);
	CBrush brush( clr );
	m_pMDC->FillRect( &rc, &brush );

	return 1;
}// end of FillColor()


int CKMMemDC::FillColor( COLORREF clr, CRect rc )
{
	CBrush brush( clr );
	m_pMDC->FillRect( &rc, &brush );

	return 1;
}// end of FillColor( clr, rc )


int CKMMemDC::FillColor( COLORREF clr, int x, int y, int w, int h )
{
	CRect rc(x,y,x+w,y+h);
	CBrush brush( clr );
	m_pMDC->FillRect( &rc, &brush );

	return 1;
}// end of FillColor( clr, xywh )



//@@---------------------------------------------------------------------------
// ???? ?׸???.
//-----------------------------------------------------------------------------
int CKMMemDC::DrawGrid	( int gridX, int gridY, COLORREF clr, int penWidth )
{

	if( m_pMDC == NULL ) return FALSE;

	CPen  penNew( PS_SOLID, penWidth, clr );
	CPen* pPenOld = NULL;

	pPenOld = (CPen*)m_pMDC->SelectObject( &penNew ); 

		int x1,y1, x2,y2;

		// ???η? ?׸???.
		y1 = 0;	y2 = m_nHeight;
		for( x1=0; x1<m_nWidth;  x1+=gridX )
		{
			x2 = x1;
			m_pMDC->MoveTo( x1,y1 );
			m_pMDC->LineTo( x2,y2 );
		}

		// ???η? ?׸???.
		x1 = 0;	x2 = m_nWidth;
		for( y1=0; y1<m_nHeight; y1+=gridY )
		{
			y2 = y1;
			m_pMDC->MoveTo( x1,y1 );
			m_pMDC->LineTo( x2,y2 );
		}

	m_pMDC->SelectObject( pPenOld ); 
	penNew.DeleteObject();

	return 1;
}// end of ()


//@@---------------------------------------------------------------------------
// Colorkey ????.
//-----------------------------------------------------------------------------
void CKMMemDC::SetColorkey( BYTE r, BYTE g, BYTE b )
{
	m_crColorkey = RGB(r,g,b);
}// end of()

void CKMMemDC::SetColorkey( COLORREF colorkey )
{
	m_crColorkey = colorkey;
}// end of ()

void CKMMemDC::SetColorkey( int pixelX, int pixelY )
{
	m_crColorkey = m_pMDC->GetPixel( pixelX, pixelY );
}// end of ()



//@@---------------------------------------------------------------------------
// Colorkey?? ????.
//-----------------------------------------------------------------------------
BOOL CKMMemDC::IsColorkey ( BYTE r, BYTE g, BYTE b )
{
	COLORREF color = RGB(r,g,b);
	if( m_crColorkey == color ) return TRUE;
	else                        return FALSE;

}// end of()

BOOL CKMMemDC::IsColorkey ( COLORREF color )
{
	if( m_crColorkey == color ) return TRUE;
	else                        return FALSE;
}// end of()


//@@---------------------------------------------------------------------------
// Colorkey?? ?????Ͽ? ????ȭ.
//-----------------------------------------------------------------------------
BOOL CKMMemDC::SelectFix_Optimize ( CRect& rcSelect )
{

	// ???ÿ????? ?????? ??????.
	if( rcSelect.IsRectEmpty() ) {
		return FALSE;
	}
	

	int left,    right,    top,    bottom;
	int leftNew, rightNew, topNew, bottomNew;

	COLORREF color;
	int  curX,curY;

	BOOL bOptDone = FALSE;

	////
	leftNew	 = left	  = rcSelect.left;
	rightNew = right  = rcSelect.right  -1;
	rightNew = top	  = rcSelect.top;
	rightNew = bottom = rcSelect.bottom -1;


	//// (1/4) ?????? ?Ʒ??? 
	bOptDone = FALSE;
	for( curY=top; curY<bottom; curY++ ) 
	{//y
		for( curX=left; curX<right; curX++ ) 
		{//x
			color = m_pMDC->GetPixel( curX,curY );

			if( !IsColorkey( color ) )
			{
				topNew = curY;
				bOptDone = TRUE;
				break;
			}
		}//x
		if( bOptDone == TRUE )	break;
	}//y


	//// (2/4) ?Ʒ????? ???? 
	bOptDone = FALSE;
	for( curY=bottom; curY>top; curY-- )
	{//y
		for( curX=left; curX<right; curX++ )
		{//x
			color = m_pMDC->GetPixel( curX,curY );

			if( !IsColorkey( color ) )
			{
				bottomNew = curY+1;
				bOptDone = TRUE;
				break;
			}
		}//x
		if( bOptDone == TRUE )	break;
	}//y


	//// (3/4) ???ʿ??? ??????????
	bOptDone = FALSE;
	for( curX=left; curX<right; curX++ )
	{//y
		for( curY=top; curY<bottom; curY++ )
		{//x
			color = m_pMDC->GetPixel( curX,curY );

			if( !IsColorkey( color ) )
			{
				leftNew = curX;
				bOptDone = TRUE;
				break;
			}
		}//x
		if( bOptDone == TRUE )	break;
	}//y


	//// (4/4) ?????ʿ??? ????????
	bOptDone = FALSE;
	for( curX=right; curX>left; curX-- )
	{//y
		for( curY=top; curY<bottom; curY++ )
		{//x
			color = m_pMDC->GetPixel( curX,curY );

			if( !IsColorkey( color ) )
			{
				rightNew = curX+1;
				bOptDone = TRUE;
				break;
			}
		}//x
		if( bOptDone == TRUE )	break;
	}//y
	

	//// done~!
	CRect rcFixed( leftNew,topNew, rightNew,bottomNew );

	if( rcFixed.IsRectEmpty() )
	{
		return FALSE;
	}
	else
	{
		rcSelect = rcFixed;
		return TRUE;
	}

	return TRUE;
}// end of()


//@@---------------------------------------------------------------------------
// MemDC?? ??/???̿? ?????Ͽ? ????.
//-----------------------------------------------------------------------------
BOOL CKMMemDC::SelectFix_Bound ( CRect& rcSelect )
{
	// ???ÿ????? ?????? ??????.
	if( rcSelect.IsRectEmpty() ) {
		return FALSE;
	}
	


	CRect rcImage(0,0,m_nWidth,m_nHeight);

	CRect rcFix;

	// ??ġ?? ?????? ?ִ? ????.
	if( rcFix.IntersectRect( &rcSelect, &rcImage ) )
	{
		rcSelect = rcFix;
		return TRUE;
	}

	// ??ġ?? ?????? ???? ????.
	else
	{
		// ???ÿ??? ??ȿȭ.
		rcSelect.SetRectEmpty();
		return FALSE;
	}

	return TRUE;
}// end of()


//@@---------------------------------------------------------------------------
// Grid???????? ????.
//-----------------------------------------------------------------------------
BOOL CKMMemDC::SelectFix_Grid ( int gridX, int gridY, CRect& rcSelect )
{
	int left,    right,    top,    bottom;
	int leftNew, rightNew, topNew, bottomNew;

	////
	leftNew	 = left	  = rcSelect.left;
	rightNew = right  = rcSelect.right  -1;
	rightNew = top	  = rcSelect.top;
	rightNew = bottom = rcSelect.bottom -1;

	////
	leftNew   = (left/gridX) * gridX;
	topNew    = (top /gridY) * gridY;

	rightNew  = ( (right /gridX)+1 ) * gridX;
	bottomNew = ( (bottom/gridY)+1 ) * gridY;


	//// done~!
	CRect rcFixed( leftNew,topNew, rightNew,bottomNew );
	SelectFix_Bound( rcFixed ); // ???????? ?ǹ??????? ?????Ƿ?.

	if( rcFixed.IsRectEmpty() )
	{
		return FALSE;
	}
	else
	{
		rcSelect = rcFixed;
		return TRUE;
	}

	return TRUE;
}// end of()



//@@---------------------------------------------------------------------------
// RECT?????ȿ? MDC?? ?????ϴ? ?? ?˻??Ѵ?.
// ---------------------------------------------------------------------------
// 1. CPoint pt - MemDC?? TopLeft ??ġ
//    CRect* pRect - ?˻??? ????.
//
// 2. RECT?????? ??ġ?? FALSE?? ??ȯ.
// 
// 3. ??ġ?? ?????? ???ϰ? ?ʹٸ? IntersectRect()?? ?????Ͽ? ?ٲܰ?.
//
//-----------------------------------------------------------------------------
BOOL CKMMemDC::IsInRect ( CPoint pt, CRect* pRect )
{
	if( pRect->left   > pt.x               ) return FALSE;
	if( pRect->top    > pt.y               ) return FALSE;
	if( pRect->right  < pt.x + GetWidth()  ) return FALSE;
	if( pRect->bottom < pt.y + GetHeight() ) return FALSE;

	return TRUE;
}// IsInRect()


//@@---------------------------------------------------------------------------
BOOL CKMMemDC::GetThumbNail	( CWnd* pSrcWnd, CBitmap *pBMOut, 
								int sX, int sY, int sW, int sH, 
								int thW, int thH )
{
	int offX,offY, scaleW,scaleH;

	// Thumbnail ?? MDC??????
	CKMMemDC mdcThumb( pSrcWnd, thW, thH, RGB(192,192,192) );
	CDC* pDCThumb = mdcThumb.GetMDC();

	// ???? Image ũ?Ⱑ Thumbnail???? ???? ??Ȳ.
	if( ( thW > sW ) && ( thH > sH ) )
	{
		offX = thW/2 - sW/2; // Thumbnail?? ???߾ӿ? ??ġ??Ų??.
		offY = thH/2 - sH/2; 
		this->BltTo( pDCThumb, offX,offY );
	}

	// ???? Image ũ?Ⱑ Thumbnail???? ū ??Ȳ.
	else
	{
		// ???η? ?? ?? ????.
		if( sW > sH )
		{
			scaleW = thW;
			scaleH = (int)( sH * thH / sW );
			offX = 0;
			offY = thH/2 - scaleH/2;
		}

		// ???η? ?? ?? ????.
		else
		{
			scaleW = (int)( sW * thW / sH );
			scaleH = thH;
			offX = thW/2 - scaleW/2;
			offY = 0;
		}
		this->StretchBltTo( sX,sY,sW,sH, pDCThumb, offX,offY,scaleW,scaleH );
	}


	// Thumbnail ?׵θ? ?ܰ???.
	CPen penNew( PS_SOLID, 1, RGB(212,208,200) );
	CPen* pPenOld = (CPen*)pDCThumb->SelectObject( &penNew );
	CBrush* pBrOld = (CBrush*)pDCThumb->SelectStockObject( NULL_BRUSH );
	pDCThumb->Rectangle( 0,0,thW,thH );
	pDCThumb->SelectObject( pBrOld );
	pDCThumb->SelectObject( pPenOld );
	penNew.DeleteObject();
	




	// CBitmap?? ???? Pixel ???? Ȯ??.
	int thW_4      = (thW*3)+3&~3;
	int pixelsSize = thW_4*thH;
	unsigned char* pixels = (unsigned char*)malloc( pixelsSize );
	memset( pixels, 0, pixelsSize );


	// Thumbnail MDC?? ?̿??Ͽ? Pixels ä????.
	COLORREF color;
	BYTE r,g,b;
	for( int y=0; y<thH; y++ )
	for( int x=0; x<thW*3; x+=3 ) {
		color = pDCThumb->GetPixel( x/3, thH-y-1 );
		r = GetRValue( color );
		g = GetGValue( color );
		b = GetBValue( color );
		pixels[ thW_4*y +x   ] = b;
		pixels[ thW_4*y +x+1 ] = g;
		pixels[ thW_4*y +x+2 ] = r;
	}


	// CBitmap ??ü ??????.
	BITMAPINFO bmI;  memset( &bmI, 0, sizeof(BITMAPINFO) );
	bmI.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER); 
	bmI.bmiHeader.biWidth			= thW; 
	bmI.bmiHeader.biHeight			= thH; 
	bmI.bmiHeader.biPlanes			= 1; 
	bmI.bmiHeader.biBitCount		= 24; 
	bmI.bmiHeader.biCompression		= 0; 
	bmI.bmiHeader.biSizeImage		= pixelsSize;
	bmI.bmiHeader.biXPelsPerMeter	= 0; 
	bmI.bmiHeader.biYPelsPerMeter	= 0; 
	bmI.bmiHeader.biClrUsed			= 0; 
	bmI.bmiHeader.biClrImportant	= 0;

	CClientDC dc(pSrcWnd);
	HBITMAP hBM;
	hBM = CreateDIBitmap( dc.m_hDC, &bmI.bmiHeader, CBM_INIT,
						pixels, &bmI, DIB_RGB_COLORS );
	if( hBM == NULL )
	{
		return FALSE;
	}

	// CBitmap ????.
	pBMOut->DeleteObject();
	pBMOut->Attach( hBM );

	////
	return TRUE;
}// end of ()


//@@---------------------------------------------------------------------------
BOOL CKMMemDC::GetThumbNail	( CWnd* pSrcWnd, CBitmap *pBMOut, 
								int thW, int thH )
{
	return GetThumbNail( pSrcWnd, pBMOut, 0,0,m_nWidth,m_nHeight, thW, thH );
}// end of ()
