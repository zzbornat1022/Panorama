#include "StdAfx.h"
#include "Mosaic.h"


CMosaic::CMosaic(void)
{
}


CMosaic::~CMosaic(void)
{
}

IplImage* CMosaic::Mosaic( IplImage** pImages, int iImageAmount, int iFrameWidth, int iFrameHeight, int iPanoramaWidth, int iPanoramaHeight )
{
	m_iPanoramaPreWidth = iPanoramaWidth * 2;
	m_iPanoramaPreHeight = iPanoramaHeight * 2;
	if ( CreatePanorama( &m_pPanorama, m_iPanoramaPreWidth, m_iPanoramaPreHeight ) == false )
	{
		AfxMessageBox( _T("Creating Background Failed") );
		return NULL;
	}

	// Set panorama background color to black
	SetBackgroundColor( m_pPanorama, 0 );
	return m_pPanorama;
}

bool CMosaic::CreatePanorama( IplImage** pPanorama, int iWidth, int iHeight )
{
	if ( *pPanorama = cvCreateImage( cvSize( iWidth, iHeight ), IPL_DEPTH_8U, 3 ) )
		return true;
	return false;
}

void CMosaic::SetBackgroundColor( IplImage* pImg, int iColor )
{
	memset( pImg->imageData, iColor, pImg->widthStep*pImg->height );
}