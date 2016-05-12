#include "StdAfx.h"
#include "Mosaic.h"


CMosaic::CMosaic(void)
{
	m_iPanoramaPreWidth = 0;
	m_iPanoramaPreHeight = 0;
	m_ptFirstFrameLeftBottomVertex.x = 0;
	m_ptFirstFrameLeftBottomVertex.y = 0;
	m_iPartitionNum = 4;
	m_iMosaicFrameNo = 0;
}

CMosaic::~CMosaic(void)
{
}

IplImage* CMosaic::MosaicVideo( IplImage** pImages, int iImageAmount, int iFrameWidth, int iFrameHeight, int iPanoramaWidth, int iPanoramaHeight )
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
	m_ptFirstFrameLeftBottomVertex.x = (m_iPanoramaPreWidth - iFrameWidth) / 2;
	m_ptFirstFrameLeftBottomVertex.y = (m_iPanoramaPreHeight - iFrameHeight) / 2;
	StickFirstFrame( pImages[0], m_ptFirstFrameLeftBottomVertex, m_pPanorama );

	// TODO: change 2 to iImageAmount
	for ( int i = 1; i < iImageAmount; i ++)
	{
		MosaicFrame( pImages[i], m_rectRefMosaicRegion, m_pPanorama, m_rectCurrentPanoramaRegion, m_vcLastFrameRegion, m_iPartitionNum );
	}

	// set panorama roi
	cvSetImageROI( m_pPanorama, m_rectCurrentPanoramaRegion );

	//feature* feat1, * feat2;
	//int iFeature1Num = sift_features( pImages[0], &feat1 );
	//int iFeature2Num = sift_features( pImages[1], &feat2 );
	// TODO: get_matched_features
	//int iMatchedFeaturesNum = FindMatchedFeatures( feat1, iFeature1Num, feat2, iFeature2Num );
	//CvMat* H;
	//IplImage* xformed;
	//feature*** inliners;
	//int* in_n;
	//inliners = (feature***) calloc( 1, sizeof( feature** ) );
	//in_n = (int*) calloc( 1, sizeof(int) );
	//H = ransac_xform( feat1, iFeature1Num, FEATURE_FWD_MATCH, 4, 0.01, 3.0, inliners, in_n );
	//if ( H )
	//{
	//	xformed = cvCreateImage( cvGetSize( pImages[1] ), IPL_DEPTH_8U, 3 );
	//	xformed->origin = 1;
	//	cvWarpPerspective( pImages[0], xformed, H, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll( 0 ) );
	//	cvNamedWindow( "Xformed", 1 );
	//	cvShowImage( "Xformed", xformed );
	//	cvShowImage( " pImages[1]",  pImages[1] );
	//	cvWaitKey( 0 );
	//	cvReleaseImage( &xformed );
	//	cvReleaseMat( &H );
	//}
	//IplImage* pStackedImg = DrawMatchedFeatures( pImages[0], pImages[1], **inliners, *in_n );
	//cvShowImage( "Stacked Image", pStackedImg );
	//draw_features( pImages[0], feat1, iFeature1Num );

	return m_pPanorama;
}

IplImage* CMosaic::MosaicPics( IplImage** pImages, int iImageAmount, int iPanoramaWidth, int iPanoramaHeight )
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
	m_ptFirstFrameLeftBottomVertex.x = (m_iPanoramaPreWidth - pImages[0]->width) / 2;
	m_ptFirstFrameLeftBottomVertex.y = (m_iPanoramaPreHeight - pImages[0]->height) / 2;
	StickFirstFrame( pImages[0], m_ptFirstFrameLeftBottomVertex, m_pPanorama );

	for ( int i = 1; i < iImageAmount; i++ )
		MosaicPic( pImages[i], m_rectRefMosaicRegion, m_pPanorama, m_rectCurrentPanoramaRegion, m_vcLastFrameRegion );

	// set panorama roi
	//cvSetImageROI( m_pPanorama, m_rectCurrentPanoramaRegion );

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
	// TODO: cvZero
	memset( pImg->imageData, iColor, pImg->widthStep*pImg->height );
}

void CMosaic::StickFirstFrame( IplImage* pFirstFrame, CvPoint ptPosition, IplImage* pPanorama )
{
	CvRect rectPanoramaROI = cvRect( ptPosition.x, ptPosition.y, pFirstFrame->width, pFirstFrame->height ); 
	cvSetImageROI( pPanorama, rectPanoramaROI );
	cvCopy( pFirstFrame, pPanorama );
	cvResetImageROI(pPanorama); 

	m_vcLastFrameRegion = (vertex_coord*) malloc ( sizeof( struct vertex_coord ) );
	m_vcLastFrameRegion->left_bottom_vertex.x = m_rectCurrentPanoramaRegion.x = m_ptFirstFrameLeftBottomVertex.x;
	m_vcLastFrameRegion->left_bottom_vertex.y = m_rectCurrentPanoramaRegion.y = m_ptFirstFrameLeftBottomVertex.y;
	m_vcLastFrameRegion->left_top_vertex.x = m_ptFirstFrameLeftBottomVertex.x;
	m_vcLastFrameRegion->left_top_vertex.y = m_ptFirstFrameLeftBottomVertex.y + pFirstFrame->height;
	m_vcLastFrameRegion->right_bottom_vertex.x = m_ptFirstFrameLeftBottomVertex.x + pFirstFrame->width;
	m_vcLastFrameRegion->right_bottom_vertex.y = m_ptFirstFrameLeftBottomVertex.y;
	m_vcLastFrameRegion->right_top_vertex.x = m_ptFirstFrameLeftBottomVertex.x + pFirstFrame->width;
	m_vcLastFrameRegion->right_top_vertex.y = m_ptFirstFrameLeftBottomVertex.y + pFirstFrame->height;
	m_rectCurrentPanoramaRegion.width = pFirstFrame->width;
	m_rectCurrentPanoramaRegion.height = pFirstFrame->height;
	UpdatePanoramaAndRefMosaicRegion( m_vcLastFrameRegion, m_rectRefMosaicRegion, m_rectCurrentPanoramaRegion, m_pPanorama );
}

bool CMosaic::MosaicFrame( IplImage* pFrame, CvRect &rectRefMosaicRegion, IplImage* pPanorama, CvRect &rectCurrentPanoramaRegion, struct vertex_coord* vcLastFrameRegion, int iPartitionNum )
{
	// set panorama roi
	cvSetImageROI( pPanorama, rectRefMosaicRegion );
	// TODO: release
	IplImage* pRefMosaicRegion = cvCreateImage( cvSize( rectRefMosaicRegion.width, rectRefMosaicRegion.height ), IPL_DEPTH_8U, 3 );
	pRefMosaicRegion->origin = 1;
	cvCopy( pPanorama, pRefMosaicRegion, 0 );
	cvResetImageROI( pPanorama );

	// TODO: release
	int* iCornerFlag;
	IplImage** pImagePartitions;
	int iFrameWidth = pFrame->width;
	int iFrameHeight = pFrame->height;

	// divide one frame to partitions
	iCornerFlag = (int *)calloc( 4, sizeof(int) );
	pImagePartitions = DivideImage( pFrame, iPartitionNum, iCornerFlag );

	vector<matched_feature_pair> vMatchedVertexPairs;

	for ( int i = 0; i < iPartitionNum; i++ )
	{
		// TODO: responsive
		int iXOffset = i / 2 * pImagePartitions[i]->width;
		if ( iXOffset != 0 )
			iXOffset -= 1;
		int iYOffset = i % 2 * pImagePartitions[i]->height;
		if ( iYOffset != 0 )
			iYOffset -= 1;

		// TODO: release
		feature* feat1, * feat2;
		CvMat* matTransformation;
		CvMat* matTransformationInvert;
		matTransformationInvert = cvCreateMat(3,3,CV_64FC1);
		feature*** inliners;
		int* in_n;
		vector<matched_feature_pair> vAdjacentMatchedVertexPairs;
		inliners = (feature***) calloc( 1, sizeof( feature** ) );
		in_n = (int*) calloc( 1, sizeof( int ) );
		int iFeature1Num = sift_features( pImagePartitions[i], &feat1 );
		int iFeature2Num = sift_features( pRefMosaicRegion, &feat2 );
		int iMatchedFeaturesNum = FindMatchedFeatures( feat1, iFeature1Num, feat2, iFeature2Num );
		vAdjacentMatchedVertexPairs = FindIncludedVetexPairs( vMatchedVertexPairs, iXOffset, iYOffset, pImagePartitions[i]->width, pImagePartitions[i]->height );
		matTransformation = ransac_xform( feat1, iFeature1Num, FEATURE_FWD_MATCH, 4, 0.01, 3.0, inliners, in_n, vAdjacentMatchedVertexPairs );

		// show matched pairs on partition and ref
		// TODO: release
		IplImage* pStackedImg = stack_imgs( pRefMosaicRegion, pImagePartitions[i] );
		pStackedImg->origin = 1;
		for ( int j = 0; j < vAdjacentMatchedVertexPairs.size(); j++ )
		{
			// TODO: get_matched_features
			cvLine( pStackedImg, cvPoint( (int) ( vAdjacentMatchedVertexPairs[j].ref_coord.x ),  (int) ( vAdjacentMatchedVertexPairs[j].ref_coord.y ) ), cvPoint( ( (int) ( vAdjacentMatchedVertexPairs[j].cur_coord.x ) + pRefMosaicRegion->width),  (int) ( vAdjacentMatchedVertexPairs[j].cur_coord.y ) ), CV_RGB( 255, 255, 0 ), 1, 8, 0 );
		}
		char chTempOutputPath[255];
		sprintf( chTempOutputPath, "output/Pano%d-%d.jpg", m_iMosaicFrameNo, i );
		cvSaveImage( chTempOutputPath, pStackedImg);
		cvReleaseImage( &pStackedImg );

		// stick frame
		if ( matTransformation )
		{
			// calculate vertex in ref and collect matched vertex pair
			double dbMatData[9];
			dbMatData[0] = cvmGet( matTransformation, 0, 0 );
			dbMatData[1] = cvmGet( matTransformation, 0, 1 );
			dbMatData[2] = cvmGet( matTransformation, 0, 2 );
			dbMatData[3] = cvmGet( matTransformation, 1, 0 );
			dbMatData[4] = cvmGet( matTransformation, 1, 1 );
			dbMatData[5] = cvmGet( matTransformation, 1, 2 );
			dbMatData[6] = cvmGet( matTransformation, 2, 0 );
			dbMatData[7] = cvmGet( matTransformation, 2, 1 );
			dbMatData[8] = cvmGet( matTransformation, 2, 2 );
			int iTempXInCur, iTempYInCur;	// x, y in current partition
			int iXInPano, iYInPano;
			vertex_coord vcPartitionVertexInRef;
			CvPoint ptCur, ptRef;	// point in current frame and ref
			// calculate left bottom vertex
			iTempXInCur = 0; iTempYInCur = 0;
			ptCur.x = iTempXInCur + iXOffset;
			ptCur.y = iTempYInCur + iYOffset;
			vcPartitionVertexInRef.left_bottom_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			vcPartitionVertexInRef.left_bottom_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, vcPartitionVertexInRef.left_bottom_vertex );
			// calculate left top vertex
			iTempXInCur = 0; iTempYInCur = pImagePartitions[i]->height - 1;
			ptCur.x = iTempXInCur + iXOffset;
			ptCur.y = iTempYInCur + iYOffset;
			vcPartitionVertexInRef.left_top_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			vcPartitionVertexInRef.left_top_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, vcPartitionVertexInRef.left_top_vertex );
			// calculate right bottom vertex
			iTempXInCur = pImagePartitions[i]->width - 1; iTempYInCur = 0;
			ptCur.x = iTempXInCur + iXOffset;
			ptCur.y = iTempYInCur + iYOffset;
			vcPartitionVertexInRef.right_bottom_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			vcPartitionVertexInRef.right_bottom_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, vcPartitionVertexInRef.right_bottom_vertex );
			// calculate right top vertex
			iTempXInCur = pImagePartitions[i]->width - 1; iTempYInCur = pImagePartitions[i]->height - 1;
			ptCur.x = iTempXInCur + iXOffset;
			ptCur.y = iTempYInCur + iYOffset;
			vcPartitionVertexInRef.right_top_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			vcPartitionVertexInRef.right_top_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, vcPartitionVertexInRef.right_top_vertex );
			// calculate four middle point
			iTempXInCur = pImagePartitions[i]->width / 2; iTempYInCur = 0;
			ptCur.x = iTempXInCur + iXOffset;
			ptCur.y = iTempYInCur + iYOffset;
			ptRef.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			ptRef.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );
			iTempXInCur = 0; iTempYInCur = pImagePartitions[i]->height / 2;
			ptCur.x = iTempXInCur + iXOffset;
			ptCur.y = iTempYInCur + iYOffset;
			ptRef.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			ptRef.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );
			iTempXInCur = pImagePartitions[i]->width / 2; iTempYInCur = iTempYInCur = pImagePartitions[i]->height - 1;
			ptCur.x = iTempXInCur + iXOffset;
			ptCur.y = iTempYInCur + iYOffset;
			ptRef.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			ptRef.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );
			iTempXInCur = pImagePartitions[i]->width - 1; iTempYInCur = pImagePartitions[i]->height / 2;
			ptCur.x = iTempXInCur + iXOffset;
			ptCur.y = iTempYInCur + iYOffset;
			ptRef.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			ptRef.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
			AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );

			// update vcLastFrameRegion
			if ( i == iCornerFlag[0] )
			{
				vcLastFrameRegion->left_bottom_vertex.x = (int)((dbMatData[0] * 0 + dbMatData[1] * 0 + dbMatData[2]) / (dbMatData[6] * 0 + dbMatData[7] * 0 + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.x;
				vcLastFrameRegion->left_bottom_vertex.y = (int)((dbMatData[3] * 0 + dbMatData[4] * 0 + dbMatData[5]) / (dbMatData[6] * 0 + dbMatData[7] * 0 + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.y;
			}
			else if ( i == iCornerFlag[1] )
			{
				vcLastFrameRegion->left_top_vertex.x = (int)((dbMatData[0] * 0 + dbMatData[1] * pImagePartitions[i]->height + dbMatData[2]) / (dbMatData[6] * 0 + dbMatData[7] * pImagePartitions[i]->height + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.x;
				vcLastFrameRegion->left_top_vertex.y = (int)((dbMatData[3] * 0 + dbMatData[4] * pImagePartitions[i]->height + dbMatData[5]) / (dbMatData[6] * 0 + dbMatData[7] * pImagePartitions[i]->height + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.y;
			}
			else if ( i == iCornerFlag[2] )
			{
				vcLastFrameRegion->right_bottom_vertex.x = (int)((dbMatData[0] * pImagePartitions[i]->width + dbMatData[1] * 0 + dbMatData[2]) / (dbMatData[6] * pImagePartitions[i]->width + dbMatData[7] * 0 + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.x;
				vcLastFrameRegion->right_bottom_vertex.y = (int)((dbMatData[3] * pImagePartitions[i]->width + dbMatData[4] * 0 + dbMatData[5]) / (dbMatData[6] * pImagePartitions[i]->width + dbMatData[7] * 0 + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.y;
			}
			else if ( i == iCornerFlag[3] )
			{
				vcLastFrameRegion->right_top_vertex.x = (int)((dbMatData[0] * pImagePartitions[i]->width + dbMatData[1] * pImagePartitions[i]->height + dbMatData[2]) / (dbMatData[6] * pImagePartitions[i]->width + dbMatData[7] * pImagePartitions[i]->height + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.x;
				vcLastFrameRegion->right_top_vertex.y = (int)((dbMatData[3] * pImagePartitions[i]->width + dbMatData[4] * pImagePartitions[i]->height + dbMatData[5]) / (dbMatData[6] * pImagePartitions[i]->width + dbMatData[7] * pImagePartitions[i]->height + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.y;
			}

			// calculate the mosaic rectangle in ref
			CvPoint ptLeftBottomVertexInRef, ptRightTopVertexInRef;
			if ( vcPartitionVertexInRef.left_bottom_vertex.x < vcPartitionVertexInRef.left_top_vertex.x )
				ptLeftBottomVertexInRef.x = vcPartitionVertexInRef.left_bottom_vertex.x;
			else
				ptLeftBottomVertexInRef.x = vcPartitionVertexInRef.left_top_vertex.x;
			if ( vcPartitionVertexInRef.left_bottom_vertex.y < vcPartitionVertexInRef.right_bottom_vertex.y )
				ptLeftBottomVertexInRef.y = vcPartitionVertexInRef.left_bottom_vertex.y;
			else
				ptLeftBottomVertexInRef.y = vcPartitionVertexInRef.right_bottom_vertex.y;
			if ( vcPartitionVertexInRef.right_bottom_vertex.x > vcPartitionVertexInRef.right_top_vertex.x )
				ptRightTopVertexInRef.x = vcPartitionVertexInRef.right_bottom_vertex.x;
			else
				ptRightTopVertexInRef.x = vcPartitionVertexInRef.right_top_vertex.x;
			if ( vcPartitionVertexInRef.left_top_vertex.y > vcPartitionVertexInRef.right_top_vertex.y )
				ptRightTopVertexInRef.y = vcPartitionVertexInRef.left_top_vertex.y;
			else
				ptRightTopVertexInRef.y = vcPartitionVertexInRef.right_top_vertex.y;

			// paste frame to panorama inversely
			CvScalar tmpScalar;
			cvInvert( matTransformation, matTransformationInvert, CV_LU );
			dbMatData[0] = cvmGet( matTransformationInvert, 0, 0 );
			dbMatData[1] = cvmGet( matTransformationInvert, 0, 1 );
			dbMatData[2] = cvmGet( matTransformationInvert, 0, 2 );
			dbMatData[3] = cvmGet( matTransformationInvert, 1, 0 );
			dbMatData[4] = cvmGet( matTransformationInvert, 1, 1 );
			dbMatData[5] = cvmGet( matTransformationInvert, 1, 2 );
			dbMatData[6] = cvmGet( matTransformationInvert, 2, 0 );
			dbMatData[7] = cvmGet( matTransformationInvert, 2, 1 );
			dbMatData[8] = cvmGet( matTransformationInvert, 2, 2 );
			for ( int x = ptLeftBottomVertexInRef.x; x <= ptRightTopVertexInRef.x; x++ )
			{
				for ( int y = ptLeftBottomVertexInRef.y; y < ptRightTopVertexInRef.y; y++ )
				{
					iTempXInCur = (int)((dbMatData[0] * x + dbMatData[1] * y + dbMatData[2]) / (dbMatData[6] * x + dbMatData[7] * y + dbMatData[8]) + 0.5);
					iTempYInCur = (int)((dbMatData[3] * x + dbMatData[4] * y + dbMatData[5]) / (dbMatData[6] * x + dbMatData[7] * y + dbMatData[8]) + 0.5);
					if ( iTempXInCur < 0 || iTempYInCur < 0 || iTempXInCur >= pImagePartitions[i]->width ||iTempYInCur >= pImagePartitions[i]->height )
					{
						continue;
					}
					else
					{
						tmpScalar = cvGet2D( pImagePartitions[i], iTempYInCur, iTempXInCur );
						cvSet2D( pRefMosaicRegion, y, x, tmpScalar );
						iXInPano = x + m_rectRefMosaicRegion.x;
						iYInPano = y + m_rectRefMosaicRegion.y;
						cvSet2D( pPanorama, iYInPano, iXInPano, tmpScalar );
					}
				}
			}

			// release
			cvReleaseImage( &pImagePartitions[i] );
			cvReleaseMat( &matTransformation );
			cvReleaseMat( &matTransformationInvert );
			if ( feat1 != NULL )
			{
				free( feat1 );
				feat1 = NULL;
			}
			if ( feat2 != NULL )
			{
				free( feat2 );
				feat2 = NULL;
			}
			if ( inliners != NULL )
			{
				free( inliners );
				inliners	=	NULL;
			}
			if ( in_n != NULL )
			{
				free( in_n );
				in_n = NULL;
			}
		}
	}

 	//// display the partition vertex on ref
 	//for ( int j = 0; j < vMatchedVertexPairs.size(); j++ )
 	//{
 	//	cvCircle( pRefMosaicRegion, cvPoint( vMatchedVertexPairs[j].ref_coord.x, vMatchedVertexPairs[j].ref_coord.y ), 3, CV_RGB(0,255,0), -1, 8, 0 );
 	//}
 	//cvShowImage( "pRefMosaicRegion", pRefMosaicRegion );
 	//cvWaitKey( 0 );
	
	cvReleaseImage(&pRefMosaicRegion);
	if ( iCornerFlag != NULL )
	{
		free( iCornerFlag );
		iCornerFlag = NULL;
	}
	if ( pImagePartitions != NULL )
	{
		free( pImagePartitions );
		pImagePartitions = NULL;
	}

	UpdatePanoramaAndRefMosaicRegion( vcLastFrameRegion, rectRefMosaicRegion, rectCurrentPanoramaRegion, pPanorama );

	return true;
}

bool CMosaic::MosaicPic( IplImage* pFrame, CvRect &rectRefMosaicRegion, IplImage* pPanorama, CvRect &rectCurrentPanoramaRegion, struct vertex_coord* vcLastFrameRegion )
{
	// set panorama roi
	cvSetImageROI( pPanorama, rectRefMosaicRegion );
	// TODO: release
	IplImage* pRefMosaicRegion = cvCreateImage( cvSize( rectRefMosaicRegion.width, rectRefMosaicRegion.height ), IPL_DEPTH_8U, 3 );
	pRefMosaicRegion->origin = 1;
	cvCopy( pPanorama, pRefMosaicRegion, 0 );
	cvResetImageROI( pPanorama );

	// divide pImages[1] to 3 parts
	IplImage** pImage1Parts = DivideImageTo3Parts( pFrame );

	// Temp way
	int iXOffset, iYOffset;
	vector<matched_feature_pair> vMatchedVertexPairs;

	// mosaic middle part
	iXOffset = 0;
	iYOffset = pImage1Parts[0]->height;
	// TODO: release
	feature* feat1, * feat2;
	CvMat* matTransformation;
	CvMat* matTransformationInvert;
	matTransformationInvert = cvCreateMat(3,3,CV_64FC1);
	feature*** inliners;
	int* in_n;
	vector<matched_feature_pair> vAdjacentMatchedVertexPairs;
	inliners = (feature***) calloc( 1, sizeof( feature** ) );
	in_n = (int*) calloc( 1, sizeof( int ) );
	int iFeature1Num = sift_features( pImage1Parts[1], &feat1 );
	int iFeature2Num = sift_features( pRefMosaicRegion, &feat2 );
	int iMatchedFeaturesNum = FindMatchedFeatures( feat1, iFeature1Num, feat2, iFeature2Num );
	vAdjacentMatchedVertexPairs = FindIncludedVetexPairs( vMatchedVertexPairs, iXOffset, iYOffset, pImage1Parts[1]->width, pImage1Parts[1]->height );
	matTransformation = ransac_xform( feat1, iFeature1Num, FEATURE_FWD_MATCH, 4, 0.01, 3.0, inliners, in_n, vAdjacentMatchedVertexPairs );

	if ( matTransformation )
	{
		// calculate vertex in ref and collect matched vertex pair
		double dbMatData[9];
		dbMatData[0] = cvmGet( matTransformation, 0, 0 );
		dbMatData[1] = cvmGet( matTransformation, 0, 1 );
		dbMatData[2] = cvmGet( matTransformation, 0, 2 );
		dbMatData[3] = cvmGet( matTransformation, 1, 0 );
		dbMatData[4] = cvmGet( matTransformation, 1, 1 );
		dbMatData[5] = cvmGet( matTransformation, 1, 2 );
		dbMatData[6] = cvmGet( matTransformation, 2, 0 );
		dbMatData[7] = cvmGet( matTransformation, 2, 1 );
		dbMatData[8] = cvmGet( matTransformation, 2, 2 );

		int iTempXInCur, iTempYInCur;	// x, y in current partition
		int iXInPano, iYInPano;
		vertex_coord vcPartitionVertexInRef;
		CvPoint ptCur, ptRef;	// point in current frame and ref
		// calculate left bottom vertex
		iTempXInCur = 0; iTempYInCur = 0;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset - 1;
		ptRef.x = vcPartitionVertexInRef.left_bottom_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y = vcPartitionVertexInRef.left_bottom_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y -= 1;
		AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );
		// calculate left top vertex
		iTempXInCur = 0; iTempYInCur = pImage1Parts[1]->height - 1;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset + 1;
		ptRef.x = vcPartitionVertexInRef.left_top_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y = vcPartitionVertexInRef.left_top_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y += 1;
		AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );
		// calculate right bottom vertex
		iTempXInCur = pImage1Parts[1]->width - 1; iTempYInCur = 0;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset - 1;
		ptRef.x = vcPartitionVertexInRef.right_bottom_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y = vcPartitionVertexInRef.right_bottom_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y -= 1;
		AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );
		// calculate right top vertex
		iTempXInCur = pImage1Parts[1]->width - 1; iTempYInCur = pImage1Parts[1]->height - 1;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset + 1;
		ptRef.x = vcPartitionVertexInRef.right_top_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y = vcPartitionVertexInRef.right_top_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y += 1;
		AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );

		// restore more point
		iTempXInCur = pImage1Parts[1]->width/3; iTempYInCur = 0;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset - 1;
		ptRef.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y -= 1;
		AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );
		iTempXInCur = pImage1Parts[1]->width*2/3; iTempYInCur = 0;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset - 1;
		ptRef.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y -= 1;
		AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );
		iTempXInCur = pImage1Parts[1]->width/3; iTempYInCur = pImage1Parts[1]->height - 1;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset + 1;
		ptRef.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y += 1;
		AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );
		iTempXInCur = pImage1Parts[1]->width*2/3; iTempYInCur = pImage1Parts[1]->height - 1;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset + 1;
		ptRef.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		ptRef.y += 1;
		AddMatchedVertexPairToVector( vMatchedVertexPairs, ptCur, ptRef );

		// calculate the mosaic rectangle in ref
		CvPoint ptLeftBottomVertexInRef, ptRightTopVertexInRef;
		if ( vcPartitionVertexInRef.left_bottom_vertex.x < vcPartitionVertexInRef.left_top_vertex.x )
			ptLeftBottomVertexInRef.x = vcPartitionVertexInRef.left_bottom_vertex.x;
		else
			ptLeftBottomVertexInRef.x = vcPartitionVertexInRef.left_top_vertex.x;
		if ( vcPartitionVertexInRef.left_bottom_vertex.y < vcPartitionVertexInRef.right_bottom_vertex.y )
			ptLeftBottomVertexInRef.y = vcPartitionVertexInRef.left_bottom_vertex.y;
		else
			ptLeftBottomVertexInRef.y = vcPartitionVertexInRef.right_bottom_vertex.y;
		if ( vcPartitionVertexInRef.right_bottom_vertex.x > vcPartitionVertexInRef.right_top_vertex.x )
			ptRightTopVertexInRef.x = vcPartitionVertexInRef.right_bottom_vertex.x;
		else
			ptRightTopVertexInRef.x = vcPartitionVertexInRef.right_top_vertex.x;
		if ( vcPartitionVertexInRef.left_top_vertex.y > vcPartitionVertexInRef.right_top_vertex.y )
			ptRightTopVertexInRef.y = vcPartitionVertexInRef.left_top_vertex.y;
		else
			ptRightTopVertexInRef.y = vcPartitionVertexInRef.right_top_vertex.y;

		// paste frame to panorama inversely
		CvScalar tmpScalar;
		cvInvert( matTransformation, matTransformationInvert, CV_LU );
		dbMatData[0] = cvmGet( matTransformationInvert, 0, 0 );
		dbMatData[1] = cvmGet( matTransformationInvert, 0, 1 );
		dbMatData[2] = cvmGet( matTransformationInvert, 0, 2 );
		dbMatData[3] = cvmGet( matTransformationInvert, 1, 0 );
		dbMatData[4] = cvmGet( matTransformationInvert, 1, 1 );
		dbMatData[5] = cvmGet( matTransformationInvert, 1, 2 );
		dbMatData[6] = cvmGet( matTransformationInvert, 2, 0 );
		dbMatData[7] = cvmGet( matTransformationInvert, 2, 1 );
		dbMatData[8] = cvmGet( matTransformationInvert, 2, 2 );
		for ( int x = ptLeftBottomVertexInRef.x; x <= ptRightTopVertexInRef.x; x++ )
		{
			for ( int y = ptLeftBottomVertexInRef.y; y < ptRightTopVertexInRef.y; y++ )
			{
				iTempXInCur = (int)((dbMatData[0] * x + dbMatData[1] * y + dbMatData[2]) / (dbMatData[6] * x + dbMatData[7] * y + dbMatData[8]) + 0.5);
				iTempYInCur = (int)((dbMatData[3] * x + dbMatData[4] * y + dbMatData[5]) / (dbMatData[6] * x + dbMatData[7] * y + dbMatData[8]) + 0.5);
				if ( iTempXInCur < 0 || iTempYInCur < 0 || iTempXInCur >= pImage1Parts[1]->width ||iTempYInCur >= pImage1Parts[1]->height )
				{
					continue;
				}
				else
				{
					tmpScalar = cvGet2D( pImage1Parts[1], iTempYInCur, iTempXInCur );
					cvSet2D( pRefMosaicRegion, y, x, tmpScalar );
					iXInPano = x + m_rectRefMosaicRegion.x;
					iYInPano = y + m_rectRefMosaicRegion.y;
					cvSet2D( m_pPanorama, iYInPano, iXInPano, tmpScalar );
				}
			}
		}
		cvSaveImage( "output/Ref.jpg", pRefMosaicRegion );
		cvSaveImage( "output/Pano.jpg", m_pPanorama );

		// release
		cvReleaseMat( &matTransformation );
		cvReleaseMat( &matTransformationInvert );
		if ( feat1 != NULL )
		{
			free( feat1 );
			feat1 = NULL;
		}
		if ( feat2 != NULL )
		{
			free( feat2 );
			feat2 = NULL;
		}
		if ( inliners != NULL )
		{
			free( inliners );
			inliners	=	NULL;
		}
		if ( in_n != NULL )
		{
			free( in_n );
			in_n = NULL;
		}
	}

	// mosaic bottom part
	iXOffset = 0;
	iYOffset = 0;
	matTransformationInvert = cvCreateMat(3,3,CV_64FC1);
	inliners = (feature***) calloc( 1, sizeof( feature** ) );
	in_n = (int*) calloc( 1, sizeof( int ) );
	iFeature1Num = sift_features( pImage1Parts[0], &feat1 );
	iFeature2Num = sift_features( pRefMosaicRegion, &feat2 );
	iMatchedFeaturesNum = FindMatchedFeatures( feat1, iFeature1Num, feat2, iFeature2Num );
	vAdjacentMatchedVertexPairs = FindIncludedVetexPairs( vMatchedVertexPairs, iXOffset, iYOffset, pImage1Parts[0]->width, pImage1Parts[0]->height );
	matTransformation = ransac_xform( feat1, iFeature1Num, FEATURE_FWD_MATCH, 4, 0.01, 3.0, inliners, in_n, vAdjacentMatchedVertexPairs );

	if ( matTransformation )
	{
		// calculate vertex in ref and collect matched vertex pair
		double dbMatData[9];
		dbMatData[0] = cvmGet( matTransformation, 0, 0 );
		dbMatData[1] = cvmGet( matTransformation, 0, 1 );
		dbMatData[2] = cvmGet( matTransformation, 0, 2 );
		dbMatData[3] = cvmGet( matTransformation, 1, 0 );
		dbMatData[4] = cvmGet( matTransformation, 1, 1 );
		dbMatData[5] = cvmGet( matTransformation, 1, 2 );
		dbMatData[6] = cvmGet( matTransformation, 2, 0 );
		dbMatData[7] = cvmGet( matTransformation, 2, 1 );
		dbMatData[8] = cvmGet( matTransformation, 2, 2 );

		int iTempXInCur, iTempYInCur;	// x, y in current partition
		int iXInPano, iYInPano;
		vertex_coord vcPartitionVertexInRef;
		CvPoint ptCur, ptRef;	// point in current frame and ref
		// calculate left bottom vertex
		iTempXInCur = 0; iTempYInCur = 0;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset;
		vcPartitionVertexInRef.left_bottom_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		vcPartitionVertexInRef.left_bottom_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);

		// calculate left top vertex
		iTempXInCur = 0; iTempYInCur = pImage1Parts[0]->height - 1;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset;
		vcPartitionVertexInRef.left_top_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		vcPartitionVertexInRef.left_top_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);

		// calculate right bottom vertex
		iTempXInCur = pImage1Parts[0]->width - 1; iTempYInCur = 0;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset;
		vcPartitionVertexInRef.right_bottom_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		vcPartitionVertexInRef.right_bottom_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);

		// calculate right top vertex
		iTempXInCur = pImage1Parts[0]->width - 1; iTempYInCur = pImage1Parts[0]->height - 1;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset;
		vcPartitionVertexInRef.right_top_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		vcPartitionVertexInRef.right_top_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);

		// update vcLastFrameRegion
		vcLastFrameRegion->left_bottom_vertex.x = (int)((dbMatData[0] * 0 + dbMatData[1] * 0 + dbMatData[2]) / (dbMatData[6] * 0 + dbMatData[7] * 0 + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.x;
		vcLastFrameRegion->left_bottom_vertex.y = (int)((dbMatData[3] * 0 + dbMatData[4] * 0 + dbMatData[5]) / (dbMatData[6] * 0 + dbMatData[7] * 0 + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.y;
		vcLastFrameRegion->right_bottom_vertex.x = (int)((dbMatData[0] * pImage1Parts[0]->width + dbMatData[1] * 0 + dbMatData[2]) / (dbMatData[6] * pImage1Parts[0]->width + dbMatData[7] * 0 + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.x;
		vcLastFrameRegion->right_bottom_vertex.y = (int)((dbMatData[3] * pImage1Parts[0]->width + dbMatData[4] * 0 + dbMatData[5]) / (dbMatData[6] * pImage1Parts[0]->width + dbMatData[7] * 0 + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.y;
		
		// calculate the mosaic rectangle in ref
		CvPoint ptLeftBottomVertexInRef, ptRightTopVertexInRef;
		if ( vcPartitionVertexInRef.left_bottom_vertex.x < vcPartitionVertexInRef.left_top_vertex.x )
			ptLeftBottomVertexInRef.x = vcPartitionVertexInRef.left_bottom_vertex.x;
		else
			ptLeftBottomVertexInRef.x = vcPartitionVertexInRef.left_top_vertex.x;
		if ( vcPartitionVertexInRef.left_bottom_vertex.y < vcPartitionVertexInRef.right_bottom_vertex.y )
			ptLeftBottomVertexInRef.y = vcPartitionVertexInRef.left_bottom_vertex.y;
		else
			ptLeftBottomVertexInRef.y = vcPartitionVertexInRef.right_bottom_vertex.y;
		if ( vcPartitionVertexInRef.right_bottom_vertex.x > vcPartitionVertexInRef.right_top_vertex.x )
			ptRightTopVertexInRef.x = vcPartitionVertexInRef.right_bottom_vertex.x;
		else
			ptRightTopVertexInRef.x = vcPartitionVertexInRef.right_top_vertex.x;
		if ( vcPartitionVertexInRef.left_top_vertex.y > vcPartitionVertexInRef.right_top_vertex.y )
			ptRightTopVertexInRef.y = vcPartitionVertexInRef.left_top_vertex.y;
		else
			ptRightTopVertexInRef.y = vcPartitionVertexInRef.right_top_vertex.y;

		// paste frame to panorama inversely
		CvScalar tmpScalar;
		cvInvert( matTransformation, matTransformationInvert, CV_LU );
		dbMatData[0] = cvmGet( matTransformationInvert, 0, 0 );
		dbMatData[1] = cvmGet( matTransformationInvert, 0, 1 );
		dbMatData[2] = cvmGet( matTransformationInvert, 0, 2 );
		dbMatData[3] = cvmGet( matTransformationInvert, 1, 0 );
		dbMatData[4] = cvmGet( matTransformationInvert, 1, 1 );
		dbMatData[5] = cvmGet( matTransformationInvert, 1, 2 );
		dbMatData[6] = cvmGet( matTransformationInvert, 2, 0 );
		dbMatData[7] = cvmGet( matTransformationInvert, 2, 1 );
		dbMatData[8] = cvmGet( matTransformationInvert, 2, 2 );
		for ( int x = ptLeftBottomVertexInRef.x; x <= ptRightTopVertexInRef.x; x++ )
		{
			for ( int y = ptLeftBottomVertexInRef.y; y < ptRightTopVertexInRef.y; y++ )
			{
				iTempXInCur = (int)((dbMatData[0] * x + dbMatData[1] * y + dbMatData[2]) / (dbMatData[6] * x + dbMatData[7] * y + dbMatData[8]) + 0.5);
				iTempYInCur = (int)((dbMatData[3] * x + dbMatData[4] * y + dbMatData[5]) / (dbMatData[6] * x + dbMatData[7] * y + dbMatData[8]) + 0.5);
				if ( iTempXInCur < 0 || iTempYInCur < 0 || iTempXInCur >= pImage1Parts[0]->width ||iTempYInCur >= pImage1Parts[0]->height )
				{
					continue;
				}
				else
				{
					tmpScalar = cvGet2D( pImage1Parts[0], iTempYInCur, iTempXInCur );
					cvSet2D( pRefMosaicRegion, y, x, tmpScalar );
					iXInPano = x + m_rectRefMosaicRegion.x;
					iYInPano = y + m_rectRefMosaicRegion.y;
					cvSet2D( m_pPanorama, iYInPano, iXInPano, tmpScalar );
				}
			}
		}
		cvSaveImage( "output/Ref.jpg", pRefMosaicRegion );
		cvSaveImage( "output/Pano.jpg", m_pPanorama );

		// release
		cvReleaseMat( &matTransformation );
		cvReleaseMat( &matTransformationInvert );
		if ( feat1 != NULL )
		{
			free( feat1 );
			feat1 = NULL;
		}
		if ( feat2 != NULL )
		{
			free( feat2 );
			feat2 = NULL;
		}
		if ( inliners != NULL )
		{
			free( inliners );
			inliners	=	NULL;
		}
		if ( in_n != NULL )
		{
			free( in_n );
			in_n = NULL;
		}
	}

	// mosaic top part
	iXOffset = 0;
	iYOffset = pImage1Parts[0]->height + pImage1Parts[1]->height;;
	matTransformationInvert = cvCreateMat(3,3,CV_64FC1);
	inliners = (feature***) calloc( 1, sizeof( feature** ) );
	in_n = (int*) calloc( 1, sizeof( int ) );
	iFeature1Num = sift_features( pImage1Parts[2], &feat1 );
	iFeature2Num = sift_features( pRefMosaicRegion, &feat2 );
	iMatchedFeaturesNum = FindMatchedFeatures( feat1, iFeature1Num, feat2, iFeature2Num );
	vAdjacentMatchedVertexPairs = FindIncludedVetexPairs( vMatchedVertexPairs, iXOffset, iYOffset, pImage1Parts[2]->width, pImage1Parts[2]->height );
	matTransformation = ransac_xform( feat1, iFeature1Num, FEATURE_FWD_MATCH, 4, 0.01, 3.0, inliners, in_n, vAdjacentMatchedVertexPairs );

	if ( matTransformation )
	{
		// calculate vertex in ref and collect matched vertex pair
		double dbMatData[9];
		dbMatData[0] = cvmGet( matTransformation, 0, 0 );
		dbMatData[1] = cvmGet( matTransformation, 0, 1 );
		dbMatData[2] = cvmGet( matTransformation, 0, 2 );
		dbMatData[3] = cvmGet( matTransformation, 1, 0 );
		dbMatData[4] = cvmGet( matTransformation, 1, 1 );
		dbMatData[5] = cvmGet( matTransformation, 1, 2 );
		dbMatData[6] = cvmGet( matTransformation, 2, 0 );
		dbMatData[7] = cvmGet( matTransformation, 2, 1 );
		dbMatData[8] = cvmGet( matTransformation, 2, 2 );

		int iTempXInCur, iTempYInCur;	// x, y in current partition
		int iXInPano, iYInPano;
		vertex_coord vcPartitionVertexInRef;
		CvPoint ptCur, ptRef;	// point in current frame and ref
		// calculate left bottom vertex
		iTempXInCur = 0; iTempYInCur = 0;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset;
		vcPartitionVertexInRef.left_bottom_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		vcPartitionVertexInRef.left_bottom_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);

		// calculate left top vertex
		iTempXInCur = 0; iTempYInCur = pImage1Parts[2]->height - 1;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset;
		vcPartitionVertexInRef.left_top_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		vcPartitionVertexInRef.left_top_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);

		// calculate right bottom vertex
		iTempXInCur = pImage1Parts[2]->width - 1; iTempYInCur = 0;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset;
		vcPartitionVertexInRef.right_bottom_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		vcPartitionVertexInRef.right_bottom_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);

		// calculate right top vertex
		iTempXInCur = pImage1Parts[2]->width - 1; iTempYInCur = pImage1Parts[2]->height - 1;
		ptCur.x = iTempXInCur + iXOffset;
		ptCur.y = iTempYInCur + iYOffset;
		vcPartitionVertexInRef.right_top_vertex.x = (int)((dbMatData[0] * iTempXInCur + dbMatData[1] * iTempYInCur + dbMatData[2]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);
		vcPartitionVertexInRef.right_top_vertex.y = (int)((dbMatData[3] * iTempXInCur + dbMatData[4] * iTempYInCur + dbMatData[5]) / (dbMatData[6] * iTempXInCur + dbMatData[7] * iTempYInCur + dbMatData[8]) + 0.5);

		// update vcLastFrameRegion
		vcLastFrameRegion->left_top_vertex.x = (int)((dbMatData[0] * 0 + dbMatData[1] * pImage1Parts[2]->height + dbMatData[2]) / (dbMatData[6] * 0 + dbMatData[7] * pImage1Parts[2]->height + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.x;
		vcLastFrameRegion->left_top_vertex.y = (int)((dbMatData[3] * 0 + dbMatData[4] * pImage1Parts[2]->height + dbMatData[5]) / (dbMatData[6] * 0 + dbMatData[7] * pImage1Parts[2]->height + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.y;
		vcLastFrameRegion->right_top_vertex.x = (int)((dbMatData[0] * pImage1Parts[2]->width + dbMatData[1] * pImage1Parts[2]->height + dbMatData[2]) / (dbMatData[6] * pImage1Parts[2]->width + dbMatData[7] * pImage1Parts[2]->height + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.x;
		vcLastFrameRegion->right_top_vertex.y = (int)((dbMatData[3] * pImage1Parts[2]->width + dbMatData[4] * pImage1Parts[2]->height + dbMatData[5]) / (dbMatData[6] * pImage1Parts[2]->width + dbMatData[7] * pImage1Parts[2]->height + dbMatData[8]) + 0.5) + m_rectRefMosaicRegion.y;

		// calculate the mosaic rectangle in ref
		CvPoint ptLeftBottomVertexInRef, ptRightTopVertexInRef;
		if ( vcPartitionVertexInRef.left_bottom_vertex.x < vcPartitionVertexInRef.left_top_vertex.x )
			ptLeftBottomVertexInRef.x = vcPartitionVertexInRef.left_bottom_vertex.x;
		else
			ptLeftBottomVertexInRef.x = vcPartitionVertexInRef.left_top_vertex.x;
		if ( vcPartitionVertexInRef.left_bottom_vertex.y < vcPartitionVertexInRef.right_bottom_vertex.y )
			ptLeftBottomVertexInRef.y = vcPartitionVertexInRef.left_bottom_vertex.y;
		else
			ptLeftBottomVertexInRef.y = vcPartitionVertexInRef.right_bottom_vertex.y;
		if ( vcPartitionVertexInRef.right_bottom_vertex.x > vcPartitionVertexInRef.right_top_vertex.x )
			ptRightTopVertexInRef.x = vcPartitionVertexInRef.right_bottom_vertex.x;
		else
			ptRightTopVertexInRef.x = vcPartitionVertexInRef.right_top_vertex.x;
		if ( vcPartitionVertexInRef.left_top_vertex.y > vcPartitionVertexInRef.right_top_vertex.y )
			ptRightTopVertexInRef.y = vcPartitionVertexInRef.left_top_vertex.y;
		else
			ptRightTopVertexInRef.y = vcPartitionVertexInRef.right_top_vertex.y;

		// paste frame to panorama inversely
		CvScalar tmpScalar;
		cvInvert( matTransformation, matTransformationInvert, CV_LU );
		dbMatData[0] = cvmGet( matTransformationInvert, 0, 0 );
		dbMatData[1] = cvmGet( matTransformationInvert, 0, 1 );
		dbMatData[2] = cvmGet( matTransformationInvert, 0, 2 );
		dbMatData[3] = cvmGet( matTransformationInvert, 1, 0 );
		dbMatData[4] = cvmGet( matTransformationInvert, 1, 1 );
		dbMatData[5] = cvmGet( matTransformationInvert, 1, 2 );
		dbMatData[6] = cvmGet( matTransformationInvert, 2, 0 );
		dbMatData[7] = cvmGet( matTransformationInvert, 2, 1 );
		dbMatData[8] = cvmGet( matTransformationInvert, 2, 2 );
		for ( int x = ptLeftBottomVertexInRef.x; x <= ptRightTopVertexInRef.x; x++ )
		{
			for ( int y = ptLeftBottomVertexInRef.y; y < ptRightTopVertexInRef.y; y++ )
			{
				iTempXInCur = (int)((dbMatData[0] * x + dbMatData[1] * y + dbMatData[2]) / (dbMatData[6] * x + dbMatData[7] * y + dbMatData[8]) + 0.5);
				iTempYInCur = (int)((dbMatData[3] * x + dbMatData[4] * y + dbMatData[5]) / (dbMatData[6] * x + dbMatData[7] * y + dbMatData[8]) + 0.5);
				if ( iTempXInCur < 0 || iTempYInCur < 0 || iTempXInCur >= pImage1Parts[2]->width ||iTempYInCur >= pImage1Parts[2]->height )
				{
					continue;
				}
				else
				{
					tmpScalar = cvGet2D( pImage1Parts[2], iTempYInCur, iTempXInCur );
					cvSet2D( pRefMosaicRegion, y, x, tmpScalar );
					iXInPano = x + m_rectRefMosaicRegion.x;
					iYInPano = y + m_rectRefMosaicRegion.y;
					cvSet2D( m_pPanorama, iYInPano, iXInPano, tmpScalar );
				}
			}
		}
		cvSaveImage( "output/Ref.jpg", pRefMosaicRegion );
		cvSaveImage( "output/Pano.jpg", m_pPanorama );

		// release
		cvReleaseMat( &matTransformation );
		cvReleaseMat( &matTransformationInvert );
		if ( feat1 != NULL )
		{
			free( feat1 );
			feat1 = NULL;
		}
		if ( feat2 != NULL )
		{
			free( feat2 );
			feat2 = NULL;
		}
		if ( inliners != NULL )
		{
			free( inliners );
			inliners	=	NULL;
		}
		if ( in_n != NULL )
		{
			free( in_n );
			in_n = NULL;
		}
	}

	cvReleaseImage( &pImage1Parts[0] );
	cvReleaseImage( &pImage1Parts[1] );
	cvReleaseImage( &pImage1Parts[2] );
	cvReleaseImage( &pRefMosaicRegion );
	if ( pImage1Parts != NULL )
	{
		free( pImage1Parts );
		pImage1Parts = NULL;
	}

	UpdatePanoramaAndRefMosaicRegion( vcLastFrameRegion, rectRefMosaicRegion, rectCurrentPanoramaRegion, pPanorama );

	return true;
}

// TODO: change iPartition to row and column
IplImage** CMosaic::DivideImage( IplImage* pImage, int iPartitionNum, int* iCornerFlag )
{
	int iWidthPartitionNum;
	int iHeightPartitionNum;
	int iPartitionWidth;
	int iPartitionHeight;

	IplImage** pDividedImages = ( IplImage** ) calloc ( iPartitionNum, sizeof(IplImage*) );

	// TODO: responsive
	if ( iPartitionNum == 4 )
	{
		iWidthPartitionNum = 2;
		iHeightPartitionNum = 2;
		iPartitionWidth = pImage->width / iWidthPartitionNum;
		iPartitionHeight = pImage->height / iHeightPartitionNum;
		iCornerFlag[0] = 0;	//refers to lb corner
		iCornerFlag[1] = 1;	//refers to lt corner
		iCornerFlag[2] = 2;	//refers to rb corner
		iCornerFlag[3] = 3;	//refers to rt corner
	}

	for ( int i = 0; i < iWidthPartitionNum; i++ )
	{
		for ( int j = 0; j <iHeightPartitionNum; j++ )
		{
			int k = i * 2 + j;
			pDividedImages[k] = cvCreateImage( cvSize( iPartitionWidth, iPartitionHeight ), IPL_DEPTH_8U, 3 );
			pDividedImages[k]->origin = 1;
			
			CvRect rectTemp;
			rectTemp.x = i * iPartitionWidth;
			rectTemp.y = j * iPartitionHeight;
			rectTemp.width = iPartitionWidth;
			rectTemp.height = iPartitionHeight;

			cvSetImageROI( pImage, rectTemp );
			cvCopy( pImage, pDividedImages[k], 0 );
			cvResetImageROI( pImage );
		}
	}

	return pDividedImages;
}

vector<matched_feature_pair> CMosaic::FindIncludedVetexPairs(  vector<matched_feature_pair> vMatchedVertexPairs, int iXOffset, int iYOffset, int iPartionWidth, int iPartitionHeight )
{
	vector<matched_feature_pair> _vmfp;
	matched_feature_pair _mfp;
	for ( int i = 0; i < vMatchedVertexPairs.size(); i++ )
	{
		if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x -= iXOffset;
			_mfp.cur_coord.y -= iYOffset;
			_vmfp.push_back( _mfp );
		}
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset + iPartionWidth - 1 ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x = _mfp.cur_coord.x - iXOffset;
			_mfp.cur_coord.y -= iYOffset;
			_vmfp.push_back( _mfp );
		} 
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset + iPartitionHeight - 1 ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x -= iXOffset;
			_mfp.cur_coord.y = _mfp.cur_coord.y - iYOffset;
			_vmfp.push_back( _mfp );
		}
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset + iPartionWidth - 1 ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset + iPartitionHeight - 1 ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x -= iXOffset;
			_mfp.cur_coord.y = _mfp.cur_coord.y - iYOffset;
			_vmfp.push_back( _mfp );
		}
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset + iPartionWidth / 3 ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x = _mfp.cur_coord.x - iXOffset;
			_mfp.cur_coord.y -= iYOffset;
			_vmfp.push_back( _mfp );
		} 
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset + iPartionWidth * 2 / 3 ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x = _mfp.cur_coord.x - iXOffset;
			_mfp.cur_coord.y -= iYOffset;
			_vmfp.push_back( _mfp );
		} 
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset + iPartionWidth / 3 ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset + iPartitionHeight - 1 ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x = _mfp.cur_coord.x - iXOffset;
			_mfp.cur_coord.y -= iYOffset;
			_vmfp.push_back( _mfp );
		} 
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset + iPartionWidth * 2 / 3 ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset + iPartitionHeight - 1 ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x = _mfp.cur_coord.x - iXOffset;
			_mfp.cur_coord.y -= iYOffset;
			_vmfp.push_back( _mfp );
		} 
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset + iPartionWidth / 2 ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x = _mfp.cur_coord.x - iXOffset;
			_mfp.cur_coord.y -= iYOffset;
			_vmfp.push_back( _mfp );
		} 
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset + iPartitionHeight / 2 ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x -= iXOffset;
			_mfp.cur_coord.y = _mfp.cur_coord.y - iYOffset;
			_vmfp.push_back( _mfp );
		}
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset + iPartionWidth / 2 ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset + iPartitionHeight - 1 ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x = _mfp.cur_coord.x - iXOffset;
			_mfp.cur_coord.y -= iYOffset;
			_vmfp.push_back( _mfp );
		} 
		else if ( ( (int)vMatchedVertexPairs[i].cur_coord.x == iXOffset + iPartionWidth - 1 ) && ( (int)vMatchedVertexPairs[i].cur_coord.y == iYOffset + iPartitionHeight / 2 ) )
		{
			_mfp = vMatchedVertexPairs[i];
			_mfp.cur_coord.x -= iXOffset;
			_mfp.cur_coord.y = _mfp.cur_coord.y - iYOffset;
			_vmfp.push_back( _mfp );
		}
	}
	return _vmfp;
}

void CMosaic::UpdatePanoramaAndRefMosaicRegion( struct vertex_coord* vcLastFrameRegion, CvRect &rectRefMosaicRegion, CvRect &rectCurrentPanoramaRegion, IplImage* pPanorama )
{
	int iRigthMax, iTopMax;
	CvPoint ptLastFrameLeftBottomVertex;
	CvPoint ptLastFrameRightTopVertex;
	
	if ( vcLastFrameRegion->left_bottom_vertex.x < vcLastFrameRegion->left_top_vertex.x )
		ptLastFrameLeftBottomVertex.x = vcLastFrameRegion->left_bottom_vertex.x;
	else
		ptLastFrameLeftBottomVertex.x = vcLastFrameRegion->left_top_vertex.x;

	if ( vcLastFrameRegion->left_bottom_vertex.y < vcLastFrameRegion->right_bottom_vertex.y )
		ptLastFrameLeftBottomVertex.y = vcLastFrameRegion->left_bottom_vertex.y;
	else
		ptLastFrameLeftBottomVertex.y = vcLastFrameRegion->right_bottom_vertex.y;

	if ( vcLastFrameRegion->right_top_vertex.x > vcLastFrameRegion->right_bottom_vertex.x )
		ptLastFrameRightTopVertex.x = vcLastFrameRegion->right_top_vertex.x;
	else
		ptLastFrameRightTopVertex.x = vcLastFrameRegion->right_bottom_vertex.x;

	if ( vcLastFrameRegion->right_top_vertex.y > vcLastFrameRegion->left_top_vertex.y )
		ptLastFrameRightTopVertex.y = vcLastFrameRegion->right_top_vertex.y;
	else
		ptLastFrameRightTopVertex.y = vcLastFrameRegion->left_top_vertex.y;

	// update rectCurrentPanoramaRegion
	if ( ptLastFrameRightTopVertex.x > rectCurrentPanoramaRegion.x + rectCurrentPanoramaRegion.width )
		iRigthMax = ptLastFrameRightTopVertex.x;
	else
		iRigthMax = rectCurrentPanoramaRegion.x + rectCurrentPanoramaRegion.width;

	if (ptLastFrameRightTopVertex.y > rectCurrentPanoramaRegion.y + rectCurrentPanoramaRegion.height )
		iTopMax = ptLastFrameRightTopVertex.y;
	else
		iTopMax = rectCurrentPanoramaRegion.y + rectCurrentPanoramaRegion.height;
	
	if ( ptLastFrameLeftBottomVertex.x < rectCurrentPanoramaRegion.x )
		rectCurrentPanoramaRegion.x = ptLastFrameLeftBottomVertex.x;

	if ( ptLastFrameLeftBottomVertex.y < rectCurrentPanoramaRegion.y )
		rectCurrentPanoramaRegion.y = ptLastFrameLeftBottomVertex.y;
	
	rectCurrentPanoramaRegion.width = iRigthMax - rectCurrentPanoramaRegion.x;
	rectCurrentPanoramaRegion.height = iTopMax - rectCurrentPanoramaRegion.y;

	// update rectRefMosaicRegion
	rectRefMosaicRegion.x = ptLastFrameLeftBottomVertex.x - (ptLastFrameRightTopVertex.x - ptLastFrameLeftBottomVertex.x);
	rectRefMosaicRegion.y = ptLastFrameLeftBottomVertex.y - (ptLastFrameRightTopVertex.y - ptLastFrameLeftBottomVertex.y) * 0.5;
	rectRefMosaicRegion.width = (ptLastFrameRightTopVertex.x - ptLastFrameLeftBottomVertex.x) * 3;
	rectRefMosaicRegion.height = (ptLastFrameRightTopVertex.y - ptLastFrameLeftBottomVertex.y) * 2;

	//cvSetImageROI( pPanorama, rectCurrentPanoramaRegion );
	//char chTempOutputPath[255];
	//sprintf( chTempOutputPath, "output/Pano%d.jpg", m_iMosaicFrameNo );
	//m_iMosaicFrameNo++;
	//cvSaveImage( chTempOutputPath, pPanorama);
	//cvResetImageROI( pPanorama );

	cvCircle( pPanorama, vcLastFrameRegion->left_bottom_vertex, 3, CV_RGB(0,255,0), -1, 8, 0 );
	cvCircle( pPanorama, vcLastFrameRegion->right_bottom_vertex, 3, CV_RGB(0,255,0), -1, 8, 0 );
	cvCircle( pPanorama, vcLastFrameRegion->right_top_vertex, 3, CV_RGB(0,255,0), -1, 8, 0 );
	cvCircle( pPanorama, vcLastFrameRegion->left_top_vertex, 3, CV_RGB(0,255,0), -1, 8, 0 );
 
  	cvSetImageROI( pPanorama, rectCurrentPanoramaRegion );
  	// TODO: release
  	IplImage* pCurrentPanoramaRegion = cvCreateImage( cvSize( rectCurrentPanoramaRegion.width, rectCurrentPanoramaRegion.height ), IPL_DEPTH_8U, 3 );
  	pCurrentPanoramaRegion->origin = 1;
  	cvCopy( pPanorama, pCurrentPanoramaRegion, 0 );
  	cvResetImageROI( pPanorama );
	cvSaveImage( "output/CurrentPanorama.jpg", pCurrentPanoramaRegion);
  	cvShowImage( "CurrentPanorama", pCurrentPanoramaRegion);
  	cvWaitKey( 0 );
  	cvReleaseImage( &pCurrentPanoramaRegion );
 
  	cvSetImageROI( pPanorama, rectRefMosaicRegion );
  	// TODO: release
  	IplImage* pRefMosaicRegion = cvCreateImage( cvSize( rectRefMosaicRegion.width, rectRefMosaicRegion.height ), IPL_DEPTH_8U, 3 );
  	pRefMosaicRegion->origin = 1;
  	cvCopy( pPanorama, pRefMosaicRegion, 0 );
  	cvResetImageROI( pPanorama );
	cvSaveImage( "output/RefMosaic.jpg", pRefMosaicRegion);
  	cvShowImage( "RefMosaic", pRefMosaicRegion);
  	cvWaitKey( 0 );
  	cvReleaseImage( &pRefMosaicRegion );
}

void CMosaic::AddMatchedVertexPairToVector( vector<matched_feature_pair>& vMatchedVertexPairs, CvPoint ptCur, CvPoint ptRef )
{
	for ( int i = 0; i < vMatchedVertexPairs.size(); i++ )
	{
		if ( ( vMatchedVertexPairs[i].cur_coord.x == ptCur.x ) && ( vMatchedVertexPairs[i].cur_coord.y == ptCur.y ) )
			return;
	}
	struct matched_feature_pair mfp;
	mfp.cur_coord.x = ptCur.x;
	mfp.cur_coord.y = ptCur.y;
	mfp.ref_coord.x = ptRef.x;
	mfp.ref_coord.y = ptRef.y;
	vMatchedVertexPairs.push_back( mfp );
}

int CMosaic::FindMatchedFeatures( struct feature* feat1, int iFeat1Num, struct feature* feat2, int iFeat2Num )
{
	int n, matchBest, matchBetter;
	double ddf, ddfBest, ddfBetter;	

	n = 0;
	
	for ( int i = 1; i < iFeat1Num; i++ )
	{
		matchBest = matchBetter = 0;
		ddf = 0.0;
		ddfBest = ddfBetter = 10000000.0;

		for ( int j = 1; j < iFeat2Num; j++ )
		{
			ddf = descr_dist_sq ( &feat1[i], &feat2[j] );

			if ( ddf <= ddfBest )
			{
				matchBetter = matchBest;
				ddfBetter = ddfBest;
				matchBest = j;
				ddfBest = ddf;
			}
			else if ( ddf < ddfBetter )
			{
				matchBetter = j;
				ddfBetter = ddf;
			}
		}
		if ( ddfBest < ddfBetter * NN_SQ_DIST_RATIO_THR )
		{
			n++;
			feat1[i].fwd_match = &feat2[matchBest];
		}
	}	
	return n;
}

IplImage* CMosaic::DrawMatchedFeatures( IplImage* img1,  IplImage* img2, struct feature* feat, int iFeatNum )
{
	IplImage* pStackedImg = stack_imgs( img1, img2 );
	pStackedImg->origin = 1;
	for ( int i = 0; i < iFeatNum; i++ )
	{
		// TODO: get_matched_features
		if ( feat[i].fwd_match != NULL )
		{
			cvLine( pStackedImg, cvPoint( (int) (feat[i].x),  (int) (feat[i].y) ), cvPoint( ( (int) (feat[i].fwd_match->x) + img1->width),  (int) (feat[i].fwd_match->y) ), CV_RGB( 255, 255, 0 ), 1, 8, 0 );
		}
	}
	return pStackedImg;
}

/*********************** Functions prototyped in opensift **********************/

/**
   Finds SIFT features in an image using default parameter values.  All
   detected features are stored in the array pointed to by \a feat.

   @param img the image in which to detect features
   @param feat a pointer to an array in which to store detected features

   @return Returns the number of features stored in \a feat or -1 on failure
   @see _sift_features()
*/
int CMosaic::sift_features( IplImage* img, struct feature** feat )
{
	return _sift_features( img, feat, SIFT_INTVLS, SIFT_SIGMA, SIFT_CONTR_THR,
		SIFT_CURV_THR, SIFT_IMG_DBL, SIFT_DESCR_WIDTH,
		SIFT_DESCR_HIST_BINS );
}

/**
   Finds SIFT features in an image using user-specified parameter values.  All
   detected features are stored in the array pointed to by \a feat.

   @param img the image in which to detect features
   @param fea a pointer to an array in which to store detected features
   @param intvls the number of intervals sampled per octave of scale space
   @param sigma the amount of Gaussian smoothing applied to each image level
     before building the scale space representation for an octave
   @param cont_thr a threshold on the value of the scale space function
     \f$\left|D(\hat{x})\right|\f$, where \f$\hat{x}\f$ is a vector specifying
     feature location and scale, used to reject unstable features;  assumes
     pixel values in the range [0, 1]
   @param curv_thr threshold on a feature's ratio of principle curvatures
     used to reject features that are too edge-like
   @param img_dbl should be 1 if image doubling prior to scale space
     construction is desired or 0 if not
   @param descr_width the width, \f$n\f$, of the \f$n \times n\f$ array of
     orientation histograms used to compute a feature's descriptor
   @param descr_hist_bins the number of orientations in each of the
     histograms in the array used to compute a feature's descriptor

   @return Returns the number of keypoints stored in \a feat or -1 on failure
   @see sift_keypoints()
*/
int CMosaic::_sift_features( IplImage* img, struct feature** feat, int intvls,
		    double sigma, double contr_thr, int curv_thr,
		    int img_dbl, int descr_width, int descr_hist_bins )
{
	IplImage* init_img;
	IplImage*** gauss_pyr, *** dog_pyr;
	CvMemStorage* storage;
	CvSeq* features;
	int octvs, i, n = 0;

	/* build scale space pyramid; smallest dimension of top level is ~4 pixels */
	init_img = create_init_img( img, img_dbl, sigma );
	octvs = 2; // log( (double) ( MIN( init_img->width, init_img->height ) ) ) / log( (double) 2 ) - 2; // modified by wangyue
	gauss_pyr = build_gauss_pyr( init_img, octvs, intvls, sigma );
	dog_pyr = build_dog_pyr( gauss_pyr, octvs, intvls );
  
	storage = cvCreateMemStorage( 0 );
	features = scale_space_extrema( dog_pyr, octvs, intvls, contr_thr, curv_thr, storage );
	calc_feature_scales( features, sigma, intvls );
	if( img_dbl )
		adjust_for_img_dbl( features );
	//calc_feature_oris( features, gauss_pyr ); // modified by wangyue
	compute_descriptors( features, gauss_pyr, descr_width, descr_hist_bins );

	/* sort features by decreasing scale and move from CvSeq to array */
	cvSeqSort( features, (CvCmpFunc)feature_cmp, NULL );
	n = features->total;
	*feat = (feature*) calloc( n, sizeof(struct feature) );
	*feat = (feature*) cvCvtSeqToArray( features, *feat, CV_WHOLE_SEQ );
	for( i = 0; i < n; i++ )
	{
		free( (*feat)[i].feature_data );
		(*feat)[i].feature_data = NULL;
	}
  
	cvReleaseMemStorage( &storage );
	cvReleaseImage( &init_img );
	release_pyr( &gauss_pyr, octvs, intvls + 3 );
	release_pyr( &dog_pyr, octvs, intvls + 2 );
	return n;
}

/*
  Converts an image to 8-bit grayscale and Gaussian-smooths it.  The image is
  optionally doubled in size prior to smoothing.

  @param img input image
  @param img_dbl if true, image is doubled in size prior to smoothing
  @param sigma total std of Gaussian smoothing
*/
IplImage* CMosaic::create_init_img( IplImage* img, int img_dbl, double sigma )
{
	IplImage* gray, * dbl;
	double sig_diff;

	gray = convert_to_gray32( img );
	if( img_dbl )
    {
		sig_diff = sqrt( sigma * sigma - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA * 4 );
		dbl = cvCreateImage( cvSize( img->width*2, img->height*2 ), IPL_DEPTH_32F, 1 );
		cvResize( gray, dbl, CV_INTER_CUBIC );
		cvSmooth( dbl, dbl, CV_GAUSSIAN, 0, 0, sig_diff, sig_diff );
		cvReleaseImage( &gray );
		return dbl;
    }
	else
    {
		sig_diff = sqrt( sigma * sigma - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA );
		cvSmooth( gray, gray, CV_GAUSSIAN, 0, 0, sig_diff, sig_diff );
		return gray;
	}
}

/*
  Converts an image to 32-bit grayscale

  @param img a 3-channel 8-bit color (BGR) or 8-bit gray image

  @return Returns a 32-bit grayscale image
*/
IplImage* CMosaic::convert_to_gray32( IplImage* img )
{
	IplImage* gray8, * gray32;

	gray32 = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
	if( img->nChannels == 1 )
		gray8 = (IplImage*) cvClone( img );
	else
	{
		gray8 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
		cvCvtColor( img, gray8, CV_BGR2GRAY );
    }
	cvConvertScale( gray8, gray32, 1.0 / 255.0, 0 );

	cvReleaseImage( &gray8 );
	return gray32;
}

/*
  Builds Gaussian scale space pyramid from an image

  @param base base image of the pyramid
  @param octvs number of octaves of scale space
  @param intvls number of intervals per octave
  @param sigma amount of Gaussian smoothing per octave

  @return Returns a Gaussian scale space pyramid as an octvs x (intvls + 3)
    array
*/
IplImage*** CMosaic::build_gauss_pyr( IplImage* base, int octvs, int intvls, double sigma )
{
	IplImage*** gauss_pyr;
	const int _intvls = intvls;
	double* sig = (double*) calloc ( intvls + 3, sizeof(double) );
	double k;
	int i, o;

	gauss_pyr = (IplImage***) calloc( octvs, sizeof( IplImage** ) );
	for( i = 0; i < octvs; i++ )
		gauss_pyr[i] = (IplImage**) calloc( intvls + 3, sizeof( IplImage *) );

  /*
    precompute Gaussian sigmas using the following formula:

    \sigma_{total}^2 = \sigma_{i}^2 + \sigma_{i-1}^2

    sig[i] is the incremental sigma value needed to compute 
    the actual sigma of level i. Keeping track of incremental
    sigmas vs. total sigmas keeps the gaussian kernel small.
  */
	k = pow( 2.0, 1.0 / intvls );
	sig[0] = sigma;
	sig[1] = sigma * sqrt( k*k- 1 );
	for (i = 2; i < intvls + 3; i++)
		sig[i] = sig[i-1] * k;

	for( o = 0; o < octvs; o++ )
		for( i = 0; i < intvls + 3; i++ )
		{
			if( o == 0  &&  i == 0 )
				gauss_pyr[o][i] = cvCloneImage(base);

			/* base of new octvave is halved image from end of previous octave */
			else if( i == 0 )
				gauss_pyr[o][i] = downsample( gauss_pyr[o-1][intvls] );
	  
			/* blur the current octave's last image to create the next one */
			else
			{
				gauss_pyr[o][i] = cvCreateImage( cvGetSize(gauss_pyr[o][i-1]), IPL_DEPTH_32F, 1 );
				cvSmooth( gauss_pyr[o][i-1], gauss_pyr[o][i], CV_GAUSSIAN, 0, 0, sig[i], sig[i] );
			}
		}
	
	free(sig);
	return gauss_pyr;
}

/*
  Downsamples an image to a quarter of its size (half in each dimension)
  using nearest-neighbor interpolation

  @param img an image

  @return Returns an image whose dimensions are half those of img
*/
IplImage* CMosaic::downsample( IplImage* img )
{
	IplImage* smaller = cvCreateImage( cvSize(img->width / 2, img->height / 2),
		img->depth, img->nChannels );
	cvResize( img, smaller, CV_INTER_NN );

	return smaller;
}

/*
  Builds a difference of Gaussians scale space pyramid by subtracting adjacent
  intervals of a Gaussian pyramid

  @param gauss_pyr Gaussian scale-space pyramid
  @param octvs number of octaves of scale space
  @param intvls number of intervals per octave

  @return Returns a difference of Gaussians scale space pyramid as an
    octvs x (intvls + 2) array
*/
IplImage*** CMosaic::build_dog_pyr( IplImage*** gauss_pyr, int octvs, int intvls )
{
	IplImage*** dog_pyr;
	int i, o;

	dog_pyr = (IplImage***) calloc( octvs, sizeof( IplImage** ) );
	for( i = 0; i < octvs; i++ )
		dog_pyr[i] = (IplImage**) calloc( intvls + 2, sizeof(IplImage*) );

	for( o = 0; o < octvs; o++ )
		for( i = 0; i < intvls + 2; i++ )
		{
			dog_pyr[o][i] = cvCreateImage( cvGetSize(gauss_pyr[o][i]),
				IPL_DEPTH_32F, 1 );
			cvSub( gauss_pyr[o][i+1], gauss_pyr[o][i], dog_pyr[o][i], NULL );
		}

	return dog_pyr;
}

/*
  Detects features at extrema in DoG scale space.  Bad features are discarded
  based on contrast and ratio of principal curvatures.

  @param dog_pyr DoG scale space pyramid
  @param octvs octaves of scale space represented by dog_pyr
  @param intvls intervals per octave
  @param contr_thr low threshold on feature contrast
  @param curv_thr high threshold on feature ratio of principal curvatures
  @param storage memory storage in which to store detected features

  @return Returns an array of detected features whose scales, orientations,
    and descriptors are yet to be determined.
*/
CvSeq* CMosaic::scale_space_extrema( IplImage*** dog_pyr, int octvs, int intvls,
	double contr_thr, int curv_thr, CvMemStorage* storage )
{
	CvSeq* features;
	double prelim_contr_thr = 0.5 * contr_thr / intvls;
	struct feature* feat;
	struct detection_data* ddata;
	int o, i, r, c;

	features = cvCreateSeq( 0, sizeof(CvSeq), sizeof(struct feature), storage );
	for( o = 0; o < octvs; o++ )
		for( i = 1; i <= intvls; i++ )
			for(r = SIFT_IMG_BORDER; r < dog_pyr[o][0]->height-SIFT_IMG_BORDER; r++)
				for(c = SIFT_IMG_BORDER; c < dog_pyr[o][0]->width-SIFT_IMG_BORDER; c++)
				{
					/* perform preliminary check on contrast */
					if( ABS( pixval32f( dog_pyr[o][i], r, c ) ) > prelim_contr_thr )
						if( is_extremum( dog_pyr, o, i, r, c ) )
						{
							feat = interp_extremum(dog_pyr, o, i, r, c, intvls, contr_thr);
							if( feat )
							{
								ddata = feat_detection_data( feat );
								if( ! is_too_edge_like( dog_pyr[ddata->octv][ddata->intvl], ddata->r, ddata->c, curv_thr ) )
								{
									cvSeqPush( features, feat );
								}
								else
									free( ddata );
								free( feat );
							}
						}
				}

	return features;
}

/*
  Determines whether a pixel is a scale-space extremum by comparing it to it's
  3x3x3 pixel neighborhood.

  @param dog_pyr DoG scale space pyramid
  @param octv pixel's scale space octave
  @param intvl pixel's within-octave interval
  @param r pixel's image row
  @param c pixel's image col

  @return Returns 1 if the specified pixel is an extremum (max or min) among
    it's 3x3x3 pixel neighborhood.
*/
int CMosaic::is_extremum( IplImage*** dog_pyr, int octv, int intvl, int r, int c )
{
	double val = pixval32f( dog_pyr[octv][intvl], r, c );
	int i, j, k;

	/* check for maximum */
	if( val > 0 )
    {
		for( i = -1; i <= 1; i++ )
			for( j = -1; j <= 1; j++ )
				for( k = -1; k <= 1; k++ )
					if( val < pixval32f( dog_pyr[octv][intvl+i], r + j, c + k ) )
						return 0;
    }

	/* check for minimum */
	else
    {
		for( i = -1; i <= 1; i++ )
			for( j = -1; j <= 1; j++ )
				for( k = -1; k <= 1; k++ )
					if( val > pixval32f( dog_pyr[octv][intvl+i], r + j, c + k ) )
						return 0;
    }

	return 1;
}

/*
  Interpolates a scale-space extremum's location and scale to subpixel
  accuracy to form an image feature.  Rejects features with low contrast.
  Based on Section 4 of Lowe's paper.  

  @param dog_pyr DoG scale space pyramid
  @param octv feature's octave of scale space
  @param intvl feature's within-octave interval
  @param r feature's image row
  @param c feature's image column
  @param intvls total intervals per octave
  @param contr_thr threshold on feature contrast

  @return Returns the feature resulting from interpolation of the given
    parameters or NULL if the given location could not be interpolated or
    if contrast at the interpolated loation was too low.  If a feature is
    returned, its scale, orientation, and descriptor are yet to be determined.
*/
struct feature* CMosaic::interp_extremum( IplImage*** dog_pyr, int octv, int intvl, int r, int c, int intvls, double contr_thr )
{
	struct feature* feat;
	struct detection_data* ddata;
	double xi, xr, xc, contr;
	int i = 0;
  
	while( i < SIFT_MAX_INTERP_STEPS )
    {
		interp_step( dog_pyr, octv, intvl, r, c, &xi, &xr, &xc );
		if( ABS( xi ) < 0.5  &&  ABS( xr ) < 0.5  &&  ABS( xc ) < 0.5 )
			break;
      
		c += cvRound( xc );
		 r += cvRound( xr );
		intvl += cvRound( xi );
      
		if( intvl < 1  ||
			intvl > intvls  ||
			c < SIFT_IMG_BORDER  ||
			r < SIFT_IMG_BORDER  ||
			c >= dog_pyr[octv][0]->width - SIFT_IMG_BORDER  ||
			r >= dog_pyr[octv][0]->height - SIFT_IMG_BORDER )
		{
			return NULL;
		}
      
		i++;
    }
  
	/* ensure convergence of interpolation */
	if( i >= SIFT_MAX_INTERP_STEPS )
		return NULL;
  
	contr = interp_contr( dog_pyr, octv, intvl, r, c, xi, xr, xc );
	if( ABS( contr ) < contr_thr / intvls )
		return NULL;

	feat = new_feature();
	ddata = feat_detection_data( feat );
	feat->img_pt.x = feat->x = ( c + xc ) * pow( 2.0, octv );
	feat->img_pt.y = feat->y = ( r + xr ) * pow( 2.0, octv );
	ddata->r = r;
	ddata->c = c;
	ddata->octv = octv;
	ddata->intvl = intvl;
	ddata->subintvl = xi;

	return feat;
}

/*
  Performs one step of extremum interpolation.  Based on Eqn. (3) in Lowe's
  paper.

  @param dog_pyr difference of Gaussians scale space pyramid
  @param octv octave of scale space
  @param intvl interval being interpolated
  @param r row being interpolated
  @param c column being interpolated
  @param xi output as interpolated subpixel increment to interval
  @param xr output as interpolated subpixel increment to row
  @param xc output as interpolated subpixel increment to col
*/

void CMosaic::interp_step( IplImage*** dog_pyr, int octv, int intvl, int r, int c, double* xi, double* xr, double* xc )
{
	CvMat* dD, * H, * H_inv, X;
	double x[3] = { 0 };
  
	dD = deriv_3D( dog_pyr, octv, intvl, r, c );
	H = hessian_3D( dog_pyr, octv, intvl, r, c );
	H_inv = cvCreateMat( 3, 3, CV_64FC1 );
	cvInvert( H, H_inv, CV_SVD );
	cvInitMatHeader( &X, 3, 1, CV_64FC1, x, CV_AUTOSTEP );
	cvGEMM( H_inv, dD, -1, NULL, 0, &X, 0 );
  
	cvReleaseMat( &dD );
	cvReleaseMat( &H );
	cvReleaseMat( &H_inv );

	*xi = x[2];
	*xr = x[1];
	*xc = x[0];
}

/*
  Computes the partial derivatives in x, y, and scale of a pixel in the DoG
  scale space pyramid.

  @param dog_pyr DoG scale space pyramid
  @param octv pixel's octave in dog_pyr
  @param intvl pixel's interval in octv
  @param r pixel's image row
  @param c pixel's image col

  @return Returns the vector of partial derivatives for pixel I
    { dI/dx, dI/dy, dI/ds }^T as a CvMat*
*/
CvMat* CMosaic::deriv_3D( IplImage*** dog_pyr, int octv, int intvl, int r, int c )
{
	CvMat* dI;
	double dx, dy, ds;

	dx = ( pixval32f( dog_pyr[octv][intvl], r, c+1 ) - pixval32f( dog_pyr[octv][intvl], r, c-1 ) ) / 2.0;
	dy = ( pixval32f( dog_pyr[octv][intvl], r+1, c ) - pixval32f( dog_pyr[octv][intvl], r-1, c ) ) / 2.0;
	ds = ( pixval32f( dog_pyr[octv][intvl+1], r, c ) - pixval32f( dog_pyr[octv][intvl-1], r, c ) ) / 2.0;
  
	dI = cvCreateMat( 3, 1, CV_64FC1 );
	cvmSet( dI, 0, 0, dx );
	cvmSet( dI, 1, 0, dy );
	cvmSet( dI, 2, 0, ds );

	return dI;
}

/*
  Computes the 3D Hessian matrix for a pixel in the DoG scale space pyramid.

  @param dog_pyr DoG scale space pyramid
  @param octv pixel's octave in dog_pyr
  @param intvl pixel's interval in octv
  @param r pixel's image row
  @param c pixel's image col

  @return Returns the Hessian matrix (below) for pixel I as a CvMat*

  / Ixx  Ixy  Ixs \ <BR>
  | Ixy  Iyy  Iys | <BR>
  \ Ixs  Iys  Iss /
*/
CvMat* CMosaic::hessian_3D( IplImage*** dog_pyr, int octv, int intvl, int r, int c )
{
	CvMat* H;
	double v, dxx, dyy, dss, dxy, dxs, dys;
  
	v = pixval32f( dog_pyr[octv][intvl], r, c );
	dxx = ( pixval32f( dog_pyr[octv][intvl], r, c+1 ) + pixval32f( dog_pyr[octv][intvl], r, c-1 ) - 2 * v );
	dyy = ( pixval32f( dog_pyr[octv][intvl], r+1, c ) + pixval32f( dog_pyr[octv][intvl], r-1, c ) - 2 * v );
	dss = ( pixval32f( dog_pyr[octv][intvl+1], r, c ) + pixval32f( dog_pyr[octv][intvl-1], r, c ) - 2 * v );
	dxy = ( pixval32f( dog_pyr[octv][intvl], r+1, c+1 ) -
		pixval32f( dog_pyr[octv][intvl], r+1, c-1 ) -
		pixval32f( dog_pyr[octv][intvl], r-1, c+1 ) +
		pixval32f( dog_pyr[octv][intvl], r-1, c-1 ) ) / 4.0;
	dxs = ( pixval32f( dog_pyr[octv][intvl+1], r, c+1 ) -
		pixval32f( dog_pyr[octv][intvl+1], r, c-1 ) -
		pixval32f( dog_pyr[octv][intvl-1], r, c+1 ) +
		pixval32f( dog_pyr[octv][intvl-1], r, c-1 ) ) / 4.0;
	dys = ( pixval32f( dog_pyr[octv][intvl+1], r+1, c ) -
		pixval32f( dog_pyr[octv][intvl+1], r-1, c ) -
		pixval32f( dog_pyr[octv][intvl-1], r+1, c ) +
		pixval32f( dog_pyr[octv][intvl-1], r-1, c ) ) / 4.0;
  
	H = cvCreateMat( 3, 3, CV_64FC1 );
	cvmSet( H, 0, 0, dxx );
	cvmSet( H, 0, 1, dxy );
	cvmSet( H, 0, 2, dxs );
	cvmSet( H, 1, 0, dxy );
	cvmSet( H, 1, 1, dyy );
	cvmSet( H, 1, 2, dys );
	cvmSet( H, 2, 0, dxs );
	cvmSet( H, 2, 1, dys );
	cvmSet( H, 2, 2, dss );

	return H;
}

/*
  Calculates interpolated pixel contrast.  Based on Eqn. (3) in Lowe's
  paper.

  @param dog_pyr difference of Gaussians scale space pyramid
  @param octv octave of scale space
  @param intvl within-octave interval
  @param r pixel row
  @param c pixel column
  @param xi interpolated subpixel increment to interval
  @param xr interpolated subpixel increment to row
  @param xc interpolated subpixel increment to col

  @param Returns interpolated contrast.
*/
double CMosaic::interp_contr( IplImage*** dog_pyr, int octv, int intvl, int r, int c, double xi, double xr, double xc )
{
	CvMat* dD, X, T;
	double t[1], x[3] = { xc, xr, xi };

	cvInitMatHeader( &X, 3, 1, CV_64FC1, x, CV_AUTOSTEP );
	cvInitMatHeader( &T, 1, 1, CV_64FC1, t, CV_AUTOSTEP );
	dD = deriv_3D( dog_pyr, octv, intvl, r, c );
	cvGEMM( dD, &X, 1, NULL, 0, &T,  CV_GEMM_A_T );
	cvReleaseMat( &dD );

	return pixval32f( dog_pyr[octv][intvl], r, c ) + t[0] * 0.5;
}

/*
  Allocates and initializes a new feature

  @return Returns a pointer to the new feature
*/
feature* CMosaic::new_feature( void )
{
	struct feature* feat;
	struct detection_data* ddata;

	feat = (feature*) malloc( sizeof( struct feature ) );
	memset( feat, 0, sizeof( struct feature ) );
	ddata = (detection_data*) malloc( sizeof( struct detection_data ) );
	memset( ddata, 0, sizeof( struct detection_data ) );
	feat->feature_data = ddata;
	feat->type = FEATURE_LOWE;

	return feat;
}

/*
  Determines whether a feature is too edge like to be stable by computing the
  ratio of principal curvatures at that feature.  Based on Section 4.1 of
  Lowe's paper.

  @param dog_img image from the DoG pyramid in which feature was detected
  @param r feature row
  @param c feature col
  @param curv_thr high threshold on ratio of principal curvatures

  @return Returns 0 if the feature at (r,c) in dog_img is sufficiently
    corner-like or 1 otherwise.
*/
int CMosaic::is_too_edge_like( IplImage* dog_img, int r, int c, int curv_thr )
{
	double d, dxx, dyy, dxy, tr, det;

	/* principal curvatures are computed using the trace and det of Hessian */
	d = pixval32f(dog_img, r, c);
	dxx = pixval32f( dog_img, r, c+1 ) + pixval32f( dog_img, r, c-1 ) - 2 * d;
	dyy = pixval32f( dog_img, r+1, c ) + pixval32f( dog_img, r-1, c ) - 2 * d;
	dxy = ( pixval32f(dog_img, r+1, c+1) - pixval32f(dog_img, r+1, c-1) -
		pixval32f(dog_img, r-1, c+1) + pixval32f(dog_img, r-1, c-1) ) / 4.0;
	tr = dxx + dyy;
	det = dxx * dyy - dxy * dxy;

	/* negative determinant -> curvatures have different signs; reject feature */
	if( det <= 0 )
		return 1;

	if( tr * tr / det < ( curv_thr + 1.0 )*( curv_thr + 1.0 ) / curv_thr )
		return 0;
	return 1;
}

/*
  Calculates characteristic scale for each feature in an array.

  @param features array of features
  @param sigma amount of Gaussian smoothing per octave of scale space
  @param intvls intervals per octave of scale space
*/
void CMosaic::calc_feature_scales( CvSeq* features, double sigma, int intvls )
{
	struct feature* feat;
	struct detection_data* ddata;
	double intvl;
	int i, n;

	n = features->total;
	for( i = 0; i < n; i++ )
    {
		feat = CV_GET_SEQ_ELEM( struct feature, features, i );
		ddata = feat_detection_data( feat );
		intvl = ddata->intvl + ddata->subintvl;
		feat->scl = sigma * pow( 2.0, ddata->octv + intvl / intvls );
		ddata->scl_octv = sigma * pow( 2.0, intvl / intvls );
    }
}

/*
  Halves feature coordinates and scale in case the input image was doubled
  prior to scale space construction.

  @param features array of features
*/
void CMosaic::adjust_for_img_dbl( CvSeq* features )
{
	struct feature* feat;
	int i, n;

	n = features->total;
	for( i = 0; i < n; i++ )
    {
		feat = CV_GET_SEQ_ELEM( struct feature, features, i );
		feat->x /= 2.0;
		feat->y /= 2.0;
		feat->scl /= 2.0;
		feat->img_pt.x /= 2.0;
		feat->img_pt.y /= 2.0;
    }
}

/*
  Computes a canonical orientation for each image feature in an array.  Based
  on Section 5 of Lowe's paper.  This function adds features to the array when
  there is more than one dominant orientation at a given feature location.

  @param features an array of image features
  @param gauss_pyr Gaussian scale space pyramid
*/
void CMosaic::calc_feature_oris( CvSeq* features, IplImage*** gauss_pyr )
{
	struct feature* feat;
	struct detection_data* ddata;
	double* hist;
	double omax;
	int i, j, n = features->total;

	for( i = 0; i < n; i++ )
    {
		feat = (feature*) malloc( sizeof( struct feature ) );
		cvSeqPopFront( features, feat );
		ddata = feat_detection_data( feat );
		hist = ori_hist( gauss_pyr[ddata->octv][ddata->intvl],
			ddata->r, ddata->c, SIFT_ORI_HIST_BINS,
			cvRound( SIFT_ORI_RADIUS * ddata->scl_octv ),
			SIFT_ORI_SIG_FCTR * ddata->scl_octv );
		for( j = 0; j < SIFT_ORI_SMOOTH_PASSES; j++ )
			smooth_ori_hist( hist, SIFT_ORI_HIST_BINS );
		omax = dominant_ori( hist, SIFT_ORI_HIST_BINS );
		add_good_ori_features( features, hist, SIFT_ORI_HIST_BINS, omax * SIFT_ORI_PEAK_RATIO, feat );
		free( ddata );
		free( feat );
		free( hist );
    }
}

/*
  Computes a gradient orientation histogram at a specified pixel.

  @param img image
  @param r pixel row
  @param c pixel col
  @param n number of histogram bins
  @param rad radius of region over which histogram is computed
  @param sigma std for Gaussian weighting of histogram entries

  @return Returns an n-element array containing an orientation histogram
    representing orientations between 0 and 2 PI.
*/
double* CMosaic::ori_hist( IplImage* img, int r, int c, int n, int rad, double sigma )
{
	double* hist;
	double mag, ori, w, exp_denom, PI2 = CV_PI * 2.0;
	int bin, i, j;

	hist = (double*) calloc( n, sizeof( double ) );
	exp_denom = 2.0 * sigma * sigma;
	for( i = -rad; i <= rad; i++ )
		for( j = -rad; j <= rad; j++ )
			if( calc_grad_mag_ori( img, r + i, c + j, &mag, &ori ) )
			{
				w = exp( -( i*i + j*j ) / exp_denom );
				bin = cvRound( n * ( ori + CV_PI ) / PI2 );
				bin = ( bin < n )? bin : 0;
				hist[bin] += w * mag;
			}

	return hist;
}

/*
  Calculates the gradient magnitude and orientation at a given pixel.

  @param img image
  @param r pixel row
  @param c pixel col
  @param mag output as gradient magnitude at pixel (r,c)
  @param ori output as gradient orientation at pixel (r,c)

  @return Returns 1 if the specified pixel is a valid one and sets mag and
    ori accordingly; otherwise returns 0
*/
int CMosaic::calc_grad_mag_ori( IplImage* img, int r, int c, double* mag, double* ori )
{
	double dx, dy;

	if( r > 0  &&  r < img->height - 1  &&  c > 0  &&  c < img->width - 1 )
    {
		dx = pixval32f( img, r, c+1 ) - pixval32f( img, r, c-1 );
		dy = pixval32f( img, r-1, c ) - pixval32f( img, r+1, c );
		*mag = sqrt( dx*dx + dy*dy );
		*ori = atan2( dy, dx );
		return 1;
	}
	else
		return 0;
}

/*
  Gaussian smooths an orientation histogram.

  @param hist an orientation histogram
  @param n number of bins
*/
void CMosaic::smooth_ori_hist( double* hist, int n )
{
	double prev, tmp, h0 = hist[0];
	int i;

	prev = hist[n-1];
	for( i = 0; i < n; i++ )
    {
		tmp = hist[i];
		hist[i] = 0.25 * prev + 0.5 * hist[i] + 0.25 * ( ( i+1 == n )? h0 : hist[i+1] );
		prev = tmp;
    }
}

/*
  Finds the magnitude of the dominant orientation in a histogram

  @param hist an orientation histogram
  @param n number of bins

  @return Returns the value of the largest bin in hist
*/
double CMosaic::dominant_ori( double* hist, int n )
{
	double omax;
	int maxbin, i;

	omax = hist[0];
	maxbin = 0;
	for( i = 1; i < n; i++ )
		if( hist[i] > omax )
		{
			omax = hist[i];
			maxbin = i;
		}
	return omax;
}

/*
  Adds features to an array for every orientation in a histogram greater than
  a specified threshold.

  @param features new features are added to the end of this array
  @param hist orientation histogram
  @param n number of bins in hist
  @param mag_thr new features are added for entries in hist greater than this
  @param feat new features are clones of this with different orientations
*/
void CMosaic::add_good_ori_features( CvSeq* features, double* hist, int n, double mag_thr, struct feature* feat )
{
	struct feature* new_feat;
	double bin, PI2 = CV_PI * 2.0;
	int l, r, i;

	for( i = 0; i < n; i++ )
	{
		l = ( i == 0 )? n - 1 : i-1;
		r = ( i + 1 ) % n;

		if( hist[i] > hist[l]  &&  hist[i] > hist[r]  &&  hist[i] >= mag_thr )
		{
			bin = i + interp_hist_peak( hist[l], hist[i], hist[r] );
			bin = ( bin < 0 )? n + bin : ( bin >= n )? bin - n : bin;
			new_feat = clone_feature( feat );
			new_feat->ori = ( ( PI2 * bin ) / n ) - CV_PI;
			cvSeqPush( features, new_feat );
			free( new_feat );
		}
	}
}

/*
  Makes a deep copy of a feature

  @param feat feature to be cloned

  @return Returns a deep copy of feat
*/
struct feature* CMosaic::clone_feature( struct feature* feat )
{
	struct feature* new_feat;
	struct detection_data* ddata;

	new_feat = new_feature();
	ddata = feat_detection_data( new_feat );
	memcpy( new_feat, feat, sizeof( struct feature ) );
	memcpy( ddata, feat_detection_data(feat), sizeof( struct detection_data ) );
	new_feat->feature_data = ddata;

	return new_feat;
}

/*
  Computes feature descriptors for features in an array.  Based on Section 6
  of Lowe's paper.

  @param features array of features
  @param gauss_pyr Gaussian scale space pyramid
  @param d width of 2D array of orientation histograms
  @param n number of bins per orientation histogram
*/
void CMosaic::compute_descriptors( CvSeq* features, IplImage*** gauss_pyr, int d, int n )
{
	struct feature* feat;
	struct detection_data* ddata;
	double*** hist;
	int i, k = features->total;

	for( i = 0; i < k; i++ )
    {
		feat = CV_GET_SEQ_ELEM( struct feature, features, i );
		ddata = feat_detection_data( feat );
		hist = descr_hist( gauss_pyr[ddata->octv][ddata->intvl], ddata->r, ddata->c, feat->ori, ddata->scl_octv, d, n );
		hist_to_descr( hist, d, n, feat );
		release_descr_hist( &hist, d );
	}
}

/*
  Computes the 2D array of orientation histograms that form the feature
  descriptor.  Based on Section 6.1 of Lowe's paper.

  @param img image used in descriptor computation
  @param r row coord of center of orientation histogram array
  @param c column coord of center of orientation histogram array
  @param ori canonical orientation of feature whose descr is being computed
  @param scl scale relative to img of feature whose descr is being computed
  @param d width of 2d array of orientation histograms
  @param n bins per orientation histogram

  @return Returns a d x d array of n-bin orientation histograms.
*/
double*** CMosaic::descr_hist( IplImage* img, int r, int c, double ori, double scl, int d, int n )
{
	double*** hist;
	double cos_t, sin_t, hist_width, exp_denom, r_rot, c_rot, grad_mag, grad_ori, w, rbin, cbin, obin, bins_per_rad, PI2 = 2.0 * CV_PI;
	int radius, i, j;

	hist = (double***) calloc( d, sizeof( double** ) );
	for( i = 0; i < d; i++ )
    {
		hist[i] = (double**) calloc( d, sizeof( double* ) );
		for( j = 0; j < d; j++ )
			hist[i][j] = (double*) calloc( n, sizeof( double ) );
	}
  
	cos_t = cos( ori );
	sin_t = sin( ori );
	bins_per_rad = n / PI2;
	exp_denom = d * d * 0.5;
	hist_width = SIFT_DESCR_SCL_FCTR * scl;
	radius = hist_width * sqrt( (double) 2) * ( d + 1.0 ) * 0.5 + 0.5;
	for( i = -radius; i <= radius; i++ )
		for( j = -radius; j <= radius; j++ )
		{
			/*
			  Calculate sample's histogram array coords rotated relative to ori.
			  Subtract 0.5 so samples that fall e.g. in the center of row 1 (i.e.
			  r_rot = 1.5) have full weight placed in row 1 after interpolation.
			*/
			c_rot = ( j * cos_t - i * sin_t ) / hist_width;
			r_rot = ( j * sin_t + i * cos_t ) / hist_width;
			rbin = r_rot + d / 2 - 0.5;
			cbin = c_rot + d / 2 - 0.5;
	
			if( rbin > -1.0  &&  rbin < d  &&  cbin > -1.0  &&  cbin < d )
				if( calc_grad_mag_ori( img, r + i, c + j, &grad_mag, &grad_ori ))
				{
					grad_ori -= ori;
					while( grad_ori < 0.0 )
						grad_ori += PI2;
					while( grad_ori >= PI2 )
						grad_ori -= PI2;

					obin = grad_ori * bins_per_rad;
					w = exp( -(c_rot * c_rot + r_rot * r_rot) / exp_denom );
					interp_hist_entry( hist, rbin, cbin, obin, grad_mag * w, d, n );
				}
		}

	return hist;
}

/*
  Interpolates an entry into the array of orientation histograms that form
  the feature descriptor.

  @param hist 2D array of orientation histograms
  @param rbin sub-bin row coordinate of entry
  @param cbin sub-bin column coordinate of entry
  @param obin sub-bin orientation coordinate of entry
  @param mag size of entry
  @param d width of 2D array of orientation histograms
  @param n number of bins per orientation histogram
*/
void CMosaic::interp_hist_entry( double*** hist, double rbin, double cbin, double obin, double mag, int d, int n )
{
	double d_r, d_c, d_o, v_r, v_c, v_o;
	double** row, * h;
	int r0, c0, o0, rb, cb, ob, r, c, o;

	r0 = cvFloor( rbin );
	c0 = cvFloor( cbin );
	o0 = cvFloor( obin );
	d_r = rbin - r0;
	d_c = cbin - c0;
	d_o = obin - o0;

	/*
		The entry is distributed into up to 8 bins.  Each entry into a bin
		is multiplied by a weight of 1 - d for each dimension, where d is the
		distance from the center value of the bin measured in bin units.
	 */
	for( r = 0; r <= 1; r++ )
    {
		rb = r0 + r;
		if( rb >= 0  &&  rb < d )
		{
			v_r = mag * ( ( r == 0 )? 1.0 - d_r : d_r );
			row = hist[rb];
			for( c = 0; c <= 1; c++ )
			{
				cb = c0 + c;
				if( cb >= 0  &&  cb < d )
				{
					v_c = v_r * ( ( c == 0 )? 1.0 - d_c : d_c );
					h = row[cb];
					 for( o = 0; o <= 1; o++ )
					{
						ob = ( o0 + o ) % n;
						v_o = v_c * ( ( o == 0 )? 1.0 - d_o : d_o );
						h[ob] += v_o;
					}
				}
			}
		}
    }
}

/*
  Converts the 2D array of orientation histograms into a feature's descriptor
  vector.
  
  @param hist 2D array of orientation histograms
  @param d width of hist
  @param n bins per histogram
  @param feat feature into which to store descriptor
*/
void CMosaic::hist_to_descr( double*** hist, int d, int n, struct feature* feat )
{
	int int_val, i, r, c, o, k = 0;

	for( r = 0; r < d; r++ )
		for( c = 0; c < d; c++ )
			for( o = 0; o < n; o++ )
				feat->descr[k++] = hist[r][c][o];

	feat->d = k;
	normalize_descr( feat );
	for( i = 0; i < k; i++ )
		if( feat->descr[i] > SIFT_DESCR_MAG_THR )
			feat->descr[i] = SIFT_DESCR_MAG_THR;
	normalize_descr( feat );

	/* convert floating-point descriptor to integer valued descriptor */
	for( i = 0; i < k; i++ )
    {
		int_val = SIFT_INT_DESCR_FCTR * feat->descr[i];
		feat->descr[i] = MIN( 255, int_val );
	}
}

/*
  Normalizes a feature's descriptor vector to unitl length

  @param feat feature
*/
void CMosaic::normalize_descr( struct feature* feat )
{
	double cur, len_inv, len_sq = 0.0;
	int i, d = feat->d;

	for( i = 0; i < d; i++ )
    {
		cur = feat->descr[i];
		len_sq += cur*cur;
    }
	len_inv = 1.0 / sqrt( len_sq );
	for( i = 0; i < d; i++ )
		feat->descr[i] *= len_inv;
}

/*
  Compares features for a decreasing-scale ordering.  Intended for use with
  CvSeqSort

  @param feat1 first feature
  @param feat2 second feature
  @param param unused

  @return Returns 1 if feat1's scale is greater than feat2's, -1 if vice versa,
    and 0 if their scales are equal
*/
int CMosaic::feature_cmp( void* feat1, void* feat2, void* param )
{
	struct feature* f1 = (struct feature*) feat1;
	struct feature* f2 = (struct feature*) feat2;

	if( f1->scl < f2->scl )
		return 1;
	if( f1->scl > f2->scl )
		return -1;
	return 0;
}

/*
  De-allocates memory held by a descriptor histogram

  @param hist pointer to a 2D array of orientation histograms
  @param d width of hist
*/
void CMosaic::release_descr_hist( double**** hist, int d )
{
	int i, j;

	for( i = 0; i < d; i++)
    {
		for( j = 0; j < d; j++ )
			free( (*hist)[i][j] );
		free( (*hist)[i] );
    }
	free( *hist );
	*hist = NULL;
}

/*
  De-allocates memory held by a scale space pyramid

  @param pyr scale space pyramid
  @param octvs number of octaves of scale space
  @param n number of images per octave
*/
void CMosaic::release_pyr( IplImage**** pyr, int octvs, int n )
{
	int i, j;
	for( i = 0; i < octvs; i++ )
    {
		for( j = 0; j < n; j++ )
			cvReleaseImage( &(*pyr)[i][j] );
		free( (*pyr)[i] );
    }
	free( *pyr );
	*pyr = NULL;
}

/*
  Draws a set of features on an image
  
  @param img image on which to draw features
  @param feat array of Oxford-type features
  @param n number of features
*/
void CMosaic::draw_features( IplImage* img, struct feature* feat, int n )
{
  int type;

  if( n <= 0  ||  ! feat )
    {
      fprintf( stderr, "Warning: no features to draw, %s line %d\n",
	       __FILE__, __LINE__ );
      return;
    }
  type = feat[0].type;
  switch( type )
    {
    case FEATURE_OXFD:
      draw_oxfd_features( img, feat, n );
      break;
    case FEATURE_LOWE:
      draw_lowe_features( img, feat, n );
      break;
    default:
      fprintf( stderr, "Warning: draw_features(): unrecognized feature" \
	       " type, %s, line %d\n", __FILE__, __LINE__ );
      break;
    }
}

/*
  Draws Oxford-type affine features
  
  @param img image on which to draw features
  @param feat array of Oxford-type features
  @param n number of features
*/
void CMosaic::draw_oxfd_features( IplImage* img, struct feature* feat, int n )
{
	CvScalar color = CV_RGB( 255, 255, 255 );
	int i;

	if( img-> nChannels > 1 )
		color = FEATURE_OXFD_COLOR;
	for( i = 0; i < n; i++ )
		draw_oxfd_feature( img, feat + i, color );
}

/*
  Draws a single Oxford-type feature

  @param img image on which to draw
  @param feat feature to be drawn
  @param color color in which to draw
*/
void CMosaic::draw_oxfd_feature( IplImage* img, struct feature* feat, CvScalar color )
{
	double m[4] = { feat->a, feat->b, feat->b, feat->c };
	double v[4] = { 0 };
	double e[2] = { 0 };
	CvMat M, V, E;
	double alpha, l1, l2;

	/* compute axes and orientation of ellipse surrounding affine region */
	cvInitMatHeader( &M, 2, 2, CV_64FC1, m, CV_AUTOSTEP );
	cvInitMatHeader( &V, 2, 2, CV_64FC1, v, CV_AUTOSTEP );
	cvInitMatHeader( &E, 2, 1, CV_64FC1, e, CV_AUTOSTEP );
	cvEigenVV( &M, &V, &E, DBL_EPSILON, 0, 0 );
	l1 = 1 / sqrt( e[1] );
	l2 = 1 / sqrt( e[0] );
	alpha = -atan2( v[1], v[0] );
	alpha *= 180 / CV_PI;

	cvEllipse( img, cvPoint( feat->x, feat->y ), cvSize( l2, l1 ), alpha, 0, 360, CV_RGB(0,0,0), 3, 8, 0 );
	cvEllipse( img, cvPoint( feat->x, feat->y ), cvSize( l2, l1 ), alpha, 0, 360, color, 1, 8, 0 );
	cvLine( img, cvPoint( feat->x+2, feat->y ), cvPoint( feat->x-2, feat->y ), color, 1, 8, 0 );
	cvLine( img, cvPoint( feat->x, feat->y+2 ), cvPoint( feat->x, feat->y-2 ), color, 1, 8, 0 );
}

/*
  Draws Lowe-type features
  
  @param img image on which to draw features
  @param feat array of Oxford-type features
  @param n number of features
*/
void CMosaic::draw_lowe_features( IplImage* img, struct feature* feat, int n )
{
	CvScalar color = CV_RGB( 255, 255, 255 );
	int i;

	if( img-> nChannels > 1 )
		color = FEATURE_LOWE_COLOR;
	for( i = 0; i < n; i++ )
		draw_lowe_feature( img, feat + i, color );
}

/*
  Draws a single Lowe-type feature

  @param img image on which to draw
  @param feat feature to be drawn
  @param color color in which to draw
*/
void CMosaic::draw_lowe_feature( IplImage* img, struct feature* feat, CvScalar color )
{
	int len, hlen, blen, start_x, start_y, end_x, end_y, h1_x, h1_y, h2_x, h2_y;
	double scl, ori;
	double scale = 5.0;
	double hscale = 0.75;
	CvPoint start, end, h1, h2;

	/* compute points for an arrow scaled and rotated by feat's scl and ori */
	start_x = cvRound( feat->x );
	start_y = cvRound( feat->y );
	scl = feat->scl;
	ori = feat->ori;
	len = cvRound( scl * scale );
	hlen = cvRound( scl * hscale );
	blen = len - hlen;
	end_x = cvRound( len *  cos( ori ) ) + start_x;
	end_y = cvRound( len * -sin( ori ) ) + start_y;
	h1_x = cvRound( blen *  cos( ori + CV_PI / 18.0 ) ) + start_x;
	h1_y = cvRound( blen * -sin( ori + CV_PI / 18.0 ) ) + start_y;
	h2_x = cvRound( blen *  cos( ori - CV_PI / 18.0 ) ) + start_x;
	h2_y = cvRound( blen * -sin( ori - CV_PI / 18.0 ) ) + start_y;
	start = cvPoint( start_x, start_y );
	end = cvPoint( end_x, end_y );
	h1 = cvPoint( h1_x, h1_y );
	h2 = cvPoint( h2_x, h2_y );

	cvLine( img, start, end, color, 1, 8, 0 );
	cvLine( img, end, h1, color, 1, 8, 0 );
	cvLine( img, end, h2, color, 1, 8, 0 );
}

/*
  Calculates the squared Euclidian distance between two feature descriptors.
  
  @param f1 first feature
  @param f2 second feature
  
  @return Returns the squared Euclidian distance between the descriptors of
    f1 and f2.
*/
double CMosaic::descr_dist_sq( struct feature* f1, struct feature* f2 )
{
	double diff, dsq = 0;
	double* descr1, * descr2;
	int i, d;

	d = f1->d;
	if( f2->d != d )
		return DBL_MAX;
	descr1 = f1->descr;
	descr2 = f2->descr;

	for( i = 0; i < d; i++ )
    {
		diff = descr1[i] - descr2[i];
		dsq += diff*diff;
    }
	return dsq;
}

/*
  Combines two images by scacking one on top of the other
  
  @param img1 top image
  @param img2 bottom image
  
  @return Returns the image resulting from stacking \a img1 on top if \a img2
*/
IplImage* CMosaic::stack_imgs( IplImage* img1, IplImage* img2 )
{
	IplImage* stacked = cvCreateImage( cvSize( img1->width + img2->width, MAX( img1->height, img2->height ) ), IPL_DEPTH_8U, 3 );

	cvZero( stacked );
	cvSetImageROI( stacked, cvRect( 0, 0, img1->width, img1->height ) );
	cvAdd( img1, stacked, stacked, NULL );
	cvSetImageROI( stacked, cvRect(img1->width, 0, img2->width, img2->height) );
	cvAdd( img2, stacked, stacked, NULL );
	cvResetImageROI( stacked );

	return stacked;
}

/*
  Calculates the squared distance between two points.

  @param p1 a point
  @param p2 another point
*/
double CMosaic::dist_sq_2D( CvPoint2D64f p1, CvPoint2D64f p2 )
{
	double x_diff = p1.x - p2.x;
	double y_diff = p1.y - p2.y;

	return x_diff * x_diff + y_diff * y_diff;
}



/*
  Calculates a best-fit image transform from image feature correspondences
  using RANSAC.
  
  For more information refer to:
  
  Fischler, M. A. and Bolles, R. C.  Random sample consensus: a paradigm for
  model fitting with applications to image analysis and automated cartography.
  <EM>Communications of the ACM, 24</EM>, 6 (1981), pp. 381--395.
  
  @param features an array of features; only features with a non-NULL match
    of type mtype are used in homography computation
  @param n number of features in feat
  @param mtype determines which of each feature's match fields to use
    for model computation; should be one of FEATURE_FWD_MATCH,
    FEATURE_BCK_MATCH, or FEATURE_MDL_MATCH; if this is FEATURE_MDL_MATCH,
    correspondences are assumed to be between a feature's img_pt field
    and its match's mdl_pt field, otherwise correspondences are assumed to
    be between the the feature's img_pt field and its match's img_pt field
  @param xform_fn pointer to the function used to compute the desired
    transformation from feature correspondences
  @param m minimum number of correspondences necessary to instantiate the
    model computed by xform_fn
  @param p_badxform desired probability that the final transformation
    returned by RANSAC is corrupted by outliers (i.e. the probability that
    no samples of all inliers were drawn)
  @param err_fn pointer to the function used to compute a measure of error
    between putative correspondences and a computed model
  @param err_tol correspondences within this distance of a computed model are
    considered as inliers
  @param inliers if not NULL, output as an array of pointers to the final
    set of inliers
  @param n_in if not NULL and \a inliers is not NULL, output as the final
    number of inliers
  
  @return Returns a transformation matrix computed using RANSAC or NULL
    on error or if an acceptable transform could not be computed.
*/
CvMat* CMosaic::ransac_xform( struct feature* features, int n, int mtype, int m, double p_badxform, double err_tol, struct feature*** inliers, int* n_in, vector<matched_feature_pair> vAdjacentMatchedVertexPairs )
{
	struct feature** matched, ** sample, ** consensus, ** consensus_max = NULL;
	struct ransac_data* rdata;
	CvPoint2D64f* pts, * mpts;
	CvMat* M = NULL;
	double p, in_frac = RANSAC_INLIER_FRAC_EST;
	int i, nm, in, in_min, in_max = 0, k = 0;

	nm = get_matched_features( features, n, mtype, &matched );
	if( nm < m )
    {
		fprintf( stderr, "Warning: not enough matches to compute xform, %s line %d\n", __FILE__, __LINE__ );
		goto end;
    }

	srand( time(NULL) );

	in_min = calc_min_inliers( nm, m, RANSAC_PROB_BAD_SUPP, p_badxform );
	p = pow( 1.0 - pow( in_frac, m ), k );
	while( p > p_badxform )
    {
		sample = draw_ransac_sample( matched, nm, m );
		extract_corresp_pts( sample, m, mtype, &pts, &mpts );
		M = lsq_homog( pts, mpts, m );
		if( ! M )
			goto iteration_end;
		in = find_consensus( matched, nm, mtype, M, err_tol, &consensus);
		if( in > in_max )
		{
			if( consensus_max )
				free( consensus_max );
			consensus_max = consensus;
			in_max = in;
			in_frac = (double)in_max / nm;
		}
		else
			free( consensus );
		cvReleaseMat( &M );

iteration_end:
		release_mem( pts, mpts, sample );
		p = pow( 1.0 - pow( in_frac, m ), ++k );
	}

	/* calculate final transform based on best consensus set */
	if( true/*in_max >= in_min*/ )
    {
		extract_corresp_pts( consensus_max, in_max, mtype, &pts, &mpts );
		M = lsq_homog( pts, mpts, in_max );
		in = find_consensus( matched, nm, mtype, M, err_tol, &consensus);
		cvReleaseMat( &M );
		release_mem( pts, mpts, consensus_max );
		extract_corresp_pts( consensus, in, mtype, &pts, &mpts );
		include_additional_corresp_pts( vAdjacentMatchedVertexPairs, &pts, &mpts, in );
		int exact_num = in;
		if ( vAdjacentMatchedVertexPairs.size() != 0 )
		{
			if ( exact_num > 10 )
				exact_num = 10;
		}
		/*if ( in > 20 )
		exact_num = 20;
		if ( vAdjacentMatchedVertexPairs.size() == 5)
		exact_num = 5;*/
		M = lsq_homog( pts, mpts, exact_num );
		if( inliers )
		{
			*inliers = consensus;
			consensus = NULL;
		}
		if( n_in )
			*n_in = in;
		release_mem( pts, mpts, consensus );
	}
	else if( consensus_max )
    {
		if( inliers )
		*inliers = NULL;
		if( n_in )
			*n_in = 0;
		free( consensus_max );
    }

 end:
	for( i = 0; i < nm; i++ )
    {
		rdata = feat_ransac_data( matched[i] );
		matched[i]->feature_data = rdata->orig_feat_data;
		free( rdata );
    }
	free( matched );
	return M;
}

/*
  Calculates a planar homography from point correspondeces using the direct
  linear transform.  Intended for use as a ransac_xform_fn.
  
  @param pts array of points
  @param mpts array of corresponding points; each pts[i], i=0..n-1,
    corresponds to mpts[i]
  @param n number of points in both pts and mpts; must be at least 4
  
  @return Returns the 3x3 planar homography matrix that transforms points
    in pts to their corresponding points in mpts or NULL if fewer than 4
    correspondences were provided
*/
CvMat* CMosaic::dlt_homog( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n )
{
	CvMat* H, * A, * VT, * D, h, v9;
	double _h[9];
	int i;

	if( n < 4 )
		return NULL;

	/* set up matrices so we can unstack homography into h; Ah = 0 */
	A = cvCreateMat( 2*n, 9, CV_64FC1 );
	cvZero( A );
	for( i = 0; i < n; i++ )
    {
		cvmSet( A, 2*i, 3, -pts[i].x );
		cvmSet( A, 2*i, 4, -pts[i].y );
		cvmSet( A, 2*i, 5, -1.0  );
		cvmSet( A, 2*i, 6, mpts[i].y * pts[i].x );
		cvmSet( A, 2*i, 7, mpts[i].y * pts[i].y );
		cvmSet( A, 2*i, 8, mpts[i].y );
		cvmSet( A, 2*i+1, 0, pts[i].x );
		cvmSet( A, 2*i+1, 1, pts[i].y );
		cvmSet( A, 2*i+1, 2, 1.0  );
		cvmSet( A, 2*i+1, 6, -mpts[i].x * pts[i].x );
		cvmSet( A, 2*i+1, 7, -mpts[i].x * pts[i].y );
		cvmSet( A, 2*i+1, 8, -mpts[i].x );
    }
	D = cvCreateMat( 9, 9, CV_64FC1 );
	VT = cvCreateMat( 9, 9, CV_64FC1 );
	cvSVD( A, D, NULL, VT, CV_SVD_MODIFY_A + CV_SVD_V_T );
	v9 = cvMat( 1, 9, CV_64FC1, NULL );
	cvGetRow( VT, &v9, 8 );
	h = cvMat( 1, 9, CV_64FC1, _h );
	cvCopy( &v9, &h, NULL );
	h = cvMat( 3, 3, CV_64FC1, _h );
	H = cvCreateMat( 3, 3, CV_64FC1 );
	cvConvert( &h, H );

	cvReleaseMat( &A );
	cvReleaseMat( &D );
	cvReleaseMat( &VT );
	return H;
}

/*
  Calculates a least-squares planar homography from point correspondeces.
  
  @param pts array of points
  @param mpts array of corresponding points; each pts[i], i=1..n, corresponds
    to mpts[i]
  @param n number of points in both pts and mpts; must be at least 4
  
  @return Returns the 3 x 3 least-squares planar homography matrix that
    transforms points in pts to their corresponding points in mpts or NULL if
    fewer than 4 correspondences were provided
*/
CvMat* CMosaic::lsq_homog( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n )
{
	CvMat* H, * A, * B, X;
	double x[9];
	int i;

	if( n < 4 )
    {
		fprintf( stderr, "Warning: too few points in lsq_homog(), %s line %d\n", __FILE__, __LINE__ );
		return NULL;
    }

	/* set up matrices so we can unstack homography into X; AX = B */
	A = cvCreateMat( 2*n, 8, CV_64FC1 );
	B = cvCreateMat( 2*n, 1, CV_64FC1 );
	X = cvMat( 8, 1, CV_64FC1, x );
	H = cvCreateMat(3, 3, CV_64FC1);
	cvZero( A );
	for( i = 0; i < n; i++ )
    {
		cvmSet( A, i, 0, pts[i].x );
		cvmSet( A, i+n, 3, pts[i].x );
		cvmSet( A, i, 1, pts[i].y );
		cvmSet( A, i+n, 4, pts[i].y );
		cvmSet( A, i, 2, 1.0 );
		cvmSet( A, i+n, 5, 1.0 );
		cvmSet( A, i, 6, -pts[i].x * mpts[i].x );
		cvmSet( A, i, 7, -pts[i].y * mpts[i].x );
		cvmSet( A, i+n, 6, -pts[i].x * mpts[i].y );
		cvmSet( A, i+n, 7, -pts[i].y * mpts[i].y );
		cvmSet( B, i, 0, mpts[i].x );
		cvmSet( B, i+n, 0, mpts[i].y );
    }
	cvSolve( A, B, &X, CV_SVD );
	x[8] = 1.0;
	X = cvMat( 3, 3, CV_64FC1, x );
	cvConvert( &X, H );

	cvReleaseMat( &A );
	cvReleaseMat( &B );
	return H;
}

/*
  Calculates the transfer error between a point and its correspondence for
  a given homography, i.e. for a point x, it's correspondence x', and
  homography H, computes d(x', Hx)^2.
  
  @param pt a point
  @param mpt pt's correspondence
  @param H a homography matrix
  
  @return Returns the transfer error between pt and mpt given H
*/
double CMosaic::homog_xfer_err( CvPoint2D64f pt, CvPoint2D64f mpt, CvMat* H )
{
	CvPoint2D64f xpt = persp_xform_pt( pt, H );
  
	return sqrt( dist_sq_2D( xpt, mpt ) );
}

/*
  Performs a perspective transformation on a single point.  That is, for a
  point (x, y) and a 3 x 3 matrix T this function returns the point
  (u, v), where
  
  [x' y' w']^T = T * [x y 1]^T,
  
  and
  
  (u, v) = (x'/w', y'/w').

  Note that affine transforms are a subset of perspective transforms.
  
  @param pt a 2D point
  @param T a perspective transformation matrix
  
  @return Returns the point (u, v) as above.
*/
CvPoint2D64f CMosaic::persp_xform_pt( CvPoint2D64f pt, CvMat* T )
{
	CvMat XY, UV;
	double xy[3] = { pt.x, pt.y, 1.0 }, uv[3] = { 0 };
	CvPoint2D64f rslt;

	cvInitMatHeader( &XY, 3, 1, CV_64FC1, xy, CV_AUTOSTEP );
	cvInitMatHeader( &UV, 3, 1, CV_64FC1, uv, CV_AUTOSTEP );
	cvMatMul( T, &XY, &UV );
	rslt = cvPoint2D64f( uv[0] / uv[2], uv[1] / uv[2] );

	return rslt;
}

/*
  Returns a feature's match according to a specified match type

  @param feat feature
  @param mtype match type, one of FEATURE_FWD_MATCH, FEATURE_BCK_MATCH, or
    FEATURE_MDL_MATCH

  @return Returns feat's match corresponding to mtype or NULL for bad mtype
*/
struct feature* CMosaic::get_match( struct feature* feat, int mtype )
{
	if( mtype == FEATURE_MDL_MATCH )
		return feat->mdl_match;
	if( mtype == FEATURE_BCK_MATCH )
		return feat->bck_match;
	if( mtype == FEATURE_FWD_MATCH )
		return feat->fwd_match;
	return NULL;
}

/*
  Finds all features with a match of a specified type and stores pointers
  to them in an array.  Additionally initializes each matched feature's
  feature_data field with a ransac_data structure.

  @param features array of features
  @param n number of features in features
  @param mtype match type, one of FEATURE_{FWD,BCK,MDL}_MATCH
  @param matched output as an array of pointers to features with a match of
    the specified type

  @return Returns the number of features output in matched.
*/
int CMosaic::get_matched_features( struct feature* features, int n, int mtype, struct feature*** matched )
{
	struct feature** _matched;
	struct ransac_data* rdata;
	int i, m = 0;

	_matched = ( feature** )calloc( n, sizeof( struct feature* ) );
	for( i = 0; i < n; i++ )
		if( get_match( features + i, mtype ) )
		{
			rdata = ( ransac_data* ) malloc( sizeof( struct ransac_data ) );
			memset( rdata, 0, sizeof( struct ransac_data ) );
			rdata->orig_feat_data = features[i].feature_data;
			_matched[m] = features + i;
			_matched[m]->feature_data = rdata;
			m++;
		}
	*matched = _matched;
	return m;
}

/*
  Calculates the minimum number of inliers as a function of the number of
  putative correspondences.  Based on equation (7) in
  
  Chum, O. and Matas, J.  Matching with PROSAC -- Progressive Sample Consensus.
  In <EM>Conference on Computer Vision and Pattern Recognition (CVPR)</EM>,
  (2005), pp. 220--226.

  @param n number of putative correspondences
  @param m min number of correspondences to compute the model in question
  @param p_badsupp prob. that a bad model is supported by a correspondence
  @param p_badxform desired prob. that the final transformation returned is bad
  
  @return Returns the minimum number of inliers required to guarantee, based
    on p_badsupp, that the probability that the final transformation returned
    by RANSAC is less than p_badxform
*/
int CMosaic::calc_min_inliers( int n, int m, double p_badsupp, double p_badxform )
{
	double pi, sum;
	int i, j;

	for( j = m+1; j <= n; j++ )
    {
		sum = 0;
		for( i = j; i <= n; i++ )
		{
			pi = (i-m) * log( p_badsupp ) + (n-i+m) * log( 1.0 - p_badsupp ) + log_factorial( n - m ) - log_factorial( i - m ) - log_factorial( n - i );
			/*
			* Last three terms above are equivalent to log( n-m choose i-m )
			*/
			sum += exp( pi );
		}
		if( sum < p_badxform )
			break;
	}
	return j;
}

/*
  Calculates the natural log of the factorial of a number

  @param n number

  @return Returns log( n! )
*/
double CMosaic::log_factorial( int n )
{
	double f = 0;
	int i;

	for( i = 1; i <= n; i++ )
		f += log( (double) i );

	return f;
}

/*
  Draws a RANSAC sample from a set of features.

  @param features array of pointers to features from which to sample
  @param n number of features in features
  @param m size of the sample

  @return Returns an array of pointers to the sampled features; the sampled
    field of each sampled feature's ransac_data is set to 1
*/
struct feature** CMosaic::draw_ransac_sample( struct feature** features, int n, int m )
{
	struct feature** sample, * feat;
	struct ransac_data* rdata;
	int i, x;

	for( i = 0; i < n; i++ )
    {
		rdata = feat_ransac_data( features[i] );
		rdata->sampled = 0;
    }

	sample = ( feature** ) calloc( m, sizeof( struct feature* ) );
	for( i = 0; i < m; i++ )
    {
		do
		{
			x = rand() % n;
			feat = features[x];
			rdata = feat_ransac_data( feat );
		}	while( rdata->sampled );
		sample[i] = feat;
		rdata->sampled = 1;
    }

	return sample;
}

/*
  Extrancs raw point correspondence locations from a set of features

  @param features array of features from which to extract points and match
    points; each of these is assumed to have a match of type mtype
  @param n number of features
  @param mtype match type; if FEATURE_MDL_MATCH correspondences are assumed
    to be between each feature's img_pt field and it's match's mdl_pt field,
    otherwise, correspondences are assumed to be between img_pt and img_pt
  @param pts output as an array of raw point locations from features
  @param mpts output as an array of raw point locations from features' matches
*/
void CMosaic::extract_corresp_pts( struct feature** features, int n, int mtype, CvPoint2D64f** pts, CvPoint2D64f** mpts )
{
	struct feature* match;
	CvPoint2D64f* _pts, * _mpts;
	int i;

	_pts = ( CvPoint2D64f* ) calloc( n, sizeof( CvPoint2D64f ) );
	_mpts = ( CvPoint2D64f* ) calloc( n, sizeof( CvPoint2D64f ) );

	if( mtype == FEATURE_MDL_MATCH )
		for( i = 0; i < n; i++ )
		{
			match = get_match( features[i], mtype );
			if( ! match )
				printf( "feature does not have match of type %d, %s line %d", mtype, __FILE__, __LINE__ );
			_pts[i] = features[i]->img_pt;
			_mpts[i] = match->mdl_pt;
      }

	else
		for( i = 0; i < n; i++ )
		{
			match = get_match( features[i], mtype );
			if( ! match )
				printf( "feature does not have match of type %d, %s line %d", mtype, __FILE__, __LINE__ );
			_pts[i] = features[i]->img_pt;
			_mpts[i] = match->img_pt;
		}

	*pts = _pts;
	*mpts = _mpts;
}

void CMosaic::include_additional_corresp_pts( vector<matched_feature_pair> vAdjacentMatchedVertexPairs, CvPoint2D64f** pts, CvPoint2D64f** mpts, int &n )
{
	int count = n + vAdjacentMatchedVertexPairs.size();
	CvPoint2D64f* _pts, * _mpts;
	_pts = (CvPoint2D64f*) calloc ( count, sizeof(CvPoint2D64f) );
	_mpts = (CvPoint2D64f*) calloc ( count, sizeof(CvPoint2D64f) );

	for ( int i = 0; i < vAdjacentMatchedVertexPairs.size(); i ++ )
	{
		_pts[i] = vAdjacentMatchedVertexPairs[i].cur_coord;
		_mpts[i] = vAdjacentMatchedVertexPairs[i].ref_coord;
	}
	for ( int i = vAdjacentMatchedVertexPairs.size(); i < count; i ++ )
	{
		_pts[i] = (*pts)[i - vAdjacentMatchedVertexPairs.size()];
		_mpts[i] = (*mpts)[i - vAdjacentMatchedVertexPairs.size()];
	}

	free(*pts);
	free(*mpts);
	*pts = NULL;
	*mpts = NULL;

	*pts = _pts;
	*mpts = _mpts;
	n = count;
}

/*
  For a given model and error function, finds a consensus from a set of
  feature correspondences.

  @param features set of pointers to features; every feature is assumed to
    have a match of type mtype
  @param n number of features in features
  @param mtype determines the match field of each feature against which to
    measure error; if this is FEATURE_MDL_MATCH, correspondences are assumed
    to be between the feature's img_pt field and the match's mdl_pt field;
    otherwise matches are assumed to be between img_pt and img_pt
  @param M model for which a consensus set is being found
  @param err_fn error function used to measure distance from M
  @param err_tol correspondences within this distance of M are added to the
    consensus set
  @param consensus output as an array of pointers to features in the
    consensus set

  @return Returns the number of points in the consensus set
*/
int CMosaic::find_consensus( struct feature** features, int n, int mtype,
			   CvMat* M, double err_tol,
			   struct feature*** consensus )
{
	struct feature** _consensus;
	struct feature* match;
	CvPoint2D64f pt, mpt;
	double err;
	int i, in = 0;

	_consensus = ( feature** ) calloc( n, sizeof( struct feature* ) );

	if( mtype == FEATURE_MDL_MATCH )
		for( i = 0; i < n; i++ )
		{
			match = get_match( features[i], mtype );
			if( ! match )
				printf( "feature does not have match of type %d, %s line %d", mtype, __FILE__, __LINE__ );
			pt = features[i]->img_pt;
			mpt = match->mdl_pt;
			err = homog_xfer_err( pt, mpt, M );
			if( err <= err_tol )
				_consensus[in++] = features[i];
		}

	else
		for( i = 0; i < n; i++ )
		{
			match = get_match( features[i], mtype );
			if( ! match )
				printf( "feature does not have match of type %d, %s line %d", mtype, __FILE__, __LINE__ );
			pt = features[i]->img_pt;
			mpt = match->img_pt;
			err = homog_xfer_err( pt, mpt, M );
			if( err <= err_tol )
				_consensus[in++] = features[i];
		}
	*consensus = _consensus;
	return in;
}

 /*
  Releases memory and reduces code size above

  @param pts1 an array of points
  @param pts2 an array of points
  @param features an array of pointers to features; can be NULL
*/
void CMosaic::release_mem( CvPoint2D64f* pts1, CvPoint2D64f* pts2, struct feature** features )
{
	free( pts1 );
	free( pts2 );
	if( features )
		free( features );
}

/********** Temp Method **********/
IplImage** CMosaic::DivideImageTo3Parts( IplImage* pImage )
{
	IplImage** pDividedImages = ( IplImage** ) calloc ( 3, sizeof(IplImage*) );
	
	for ( int i = 0; i < 3; i++ )
	{
		int iTempWidth, iTempHeight;
		iTempWidth = pImage->width;
		if ( i == 2)
			iTempHeight = pImage->height / 3 + pImage->height % 3;
		else
			iTempHeight = pImage->height / 3;
		
		pDividedImages[i] = cvCreateImage( cvSize( iTempWidth, iTempHeight ), IPL_DEPTH_8U, 3 );
		pDividedImages[i]->origin = 1;
		
		CvRect rectTemp;
		rectTemp.x = 0;
		rectTemp.y = 0;
		for ( int j = 0; j < i ; j++)
			rectTemp.y += pDividedImages[j]->height;
		rectTemp.width = iTempWidth;
		rectTemp.height = iTempHeight;

		cvSetImageROI( pImage, rectTemp );
		cvCopy( pImage, pDividedImages[i], 0 );
		cvResetImageROI( pImage );
	}

	return pDividedImages;
}
/********** Temp Method **********/