#pragma once

#include <opencv2/opencv.hpp>

struct point
{
	int x;
	int y;
};

class CMosaic
{
public:
	CMosaic(void);
	~CMosaic(void);

	IplImage* Mosaic( IplImage** pImages, int iImageAmount, int iFrameWidth, int iFrameHeight, int iPanoramaWidth, int iPanoramaHeight );

private:

	// TODO: release
	IplImage* m_pPanorama;

	int m_iPanoramaPreWidth;
	int m_iPanoramaPreHeight;

	point m_ptFirstFramePosition;

private:
	bool CreatePanorama( IplImage** pBackground, int iWidth, int iHeight );
	void SetBackgroundColor( IplImage* pImg, int iColor );
	void StickFirstFrame ( IplImage* pFirstFrame, point ptPosition, IplImage* pPanorama );
};

