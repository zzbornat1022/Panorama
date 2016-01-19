#include "StdAfx.h"
#include "Mosaic.h"


CMosaic::CMosaic(void)
{
	m_iPanoramaPreWidth = 0;
	m_iPanoramaPreHeight = 0;
	m_ptFirstFramePosition.x = 0;
	m_ptFirstFramePosition.y = 0;
}

CMosaic::~CMosaic(void)
{
}

IplImage* CMosaic::Mosaic( IplImage** pImages, int iImageAmount, int iFrameWidth, int iFrameHeight, int iPanoramaWidth, int iPanoramaHeight )
{
	// Create Panorama IplImage
	m_iPanoramaPreWidth = iPanoramaWidth * 2;
	m_iPanoramaPreHeight = iPanoramaHeight * 2;
	if ( CreatePanorama( &m_pPanorama, m_iPanoramaPreWidth, m_iPanoramaPreHeight ) == false )
	{
		AfxMessageBox( _T("Creating Background Failed") );
		return NULL;
	}

	// Set panorama background color to black
	SetBackgroundColor( m_pPanorama, 0 );

	// Stick First Frame To Panorama; img->origin = 1 means origin point is at left bottom
	m_ptFirstFramePosition.x = (m_iPanoramaPreWidth - iFrameWidth) / 2;
	m_ptFirstFramePosition.y = (m_iPanoramaPreHeight - iFrameHeight) / 2;
	StickFirstFrame( pImages[0], m_ptFirstFramePosition, m_pPanorama );

	return m_pPanorama;
}

bool CMosaic::CreatePanorama( IplImage** pPanorama, int iWidth, int iHeight )
{
	if ( *pPanorama = cvCreateImage( cvSize( iWidth, iHeight ), IPL_DEPTH_8U, 3 ) )
	{
		(*pPanorama)->origin = 1;
		return true;
	}
	return false;
}

void CMosaic::SetBackgroundColor( IplImage* pImg, int iColor )
{
	memset( pImg->imageData, iColor, pImg->widthStep*pImg->height );
}

void CMosaic::StickFirstFrame ( IplImage* pFirstFrame, point ptPosition, IplImage* pPanorama )
{
	CvRect rectPanoramaROI = cvRect( ptPosition.x, ptPosition.y, pFirstFrame->width, pFirstFrame->height ); 
	cvSetImageROI( pPanorama, rectPanoramaROI );
	cvCopy( pFirstFrame, pPanorama );
	cvResetImageROI(pPanorama); 
}