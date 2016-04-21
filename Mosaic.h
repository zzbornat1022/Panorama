#pragma once

#include <opencv2/opencv.hpp>

using namespace std;

/******************************* Defs and macros *****************************/

// /*define margin*/
// #define MOSAIC_MARGIN 4

/* absolute value */
#ifndef ABS
#define ABS(x) ( ( (x) < 0 )? -(x) : (x) )
#endif

/** default number of sampled intervals per octave */
#define SIFT_INTVLS 3

/** default sigma for initial gaussian smoothing */
#define SIFT_SIGMA 1.6

/** default threshold on keypoint contrast |D(x)| */
#define SIFT_CONTR_THR 0.04

/** default threshold on keypoint ratio of principle curvatures */
#define SIFT_CURV_THR 10

/** double image size before pyramid construction? */
#define SIFT_IMG_DBL 1

/** default width of descriptor histogram array */
#define SIFT_DESCR_WIDTH 4

/** default number of bins per histogram in descriptor array */
#define SIFT_DESCR_HIST_BINS 8

/* assumed gaussian blur for input image */
#define SIFT_INIT_SIGMA 0.5

/* width of border in which to ignore keypoints */
#define SIFT_IMG_BORDER 5

/* maximum steps of keypoint interpolation before failure */
#define SIFT_MAX_INTERP_STEPS 5

/* default number of bins in histogram for orientation assignment */
#define SIFT_ORI_HIST_BINS 36

/* determines gaussian sigma for orientation assignment */
#define SIFT_ORI_SIG_FCTR 1.5

/* determines the radius of the region used in orientation assignment */
#define SIFT_ORI_RADIUS 3.0 * SIFT_ORI_SIG_FCTR

/* number of passes of orientation histogram smoothing */
#define SIFT_ORI_SMOOTH_PASSES 2

/* orientation magnitude relative to max that results in new feature */
#define SIFT_ORI_PEAK_RATIO 0.8

/* determines the size of a single descriptor orientation histogram */
#define SIFT_DESCR_SCL_FCTR 3.0

/* threshold on magnitude of elements of descriptor vector */
#define SIFT_DESCR_MAG_THR 0.2

/* factor used to convert floating-point descriptor to unsigned char */
#define SIFT_INT_DESCR_FCTR 512.0

/* returns a feature's detection data */
#define feat_detection_data(f) ( (struct detection_data*)(f->feature_data) )

/** FEATURE_OXFD <BR> FEATURE_LOWE */
enum feature_type
{
	FEATURE_OXFD,
	FEATURE_LOWE,
};

/** FEATURE_FWD_MATCH <BR> FEATURE_BCK_MATCH <BR> FEATURE_MDL_MATCH */
enum feature_match_type
{
	FEATURE_FWD_MATCH,
	FEATURE_BCK_MATCH,
	FEATURE_MDL_MATCH,
};

/*
  Interpolates a histogram peak from left, center, and right values
*/
#define interp_hist_peak( l, c, r ) ( 0.5 * ((l)-(r)) / ((l) - 2.0*(c) + (r)) )

/* colors in which to display different feature types */
#define FEATURE_OXFD_COLOR CV_RGB(255,255,0)
#define FEATURE_LOWE_COLOR CV_RGB(255,0,255)

/** max feature descriptor length */
#define FEATURE_MAX_D 128

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

// xform

/* RANSAC error tolerance in pixels */
#define RANSAC_ERR_TOL 3

/** pessimistic estimate of fraction of inlers for RANSAC */
#define RANSAC_INLIER_FRAC_EST 0.25

/** estimate of the probability that a correspondence supports a bad model */
#define RANSAC_PROB_BAD_SUPP 0.10

/* extracts a feature's RANSAC data */
#define feat_ransac_data( feat ) ( (struct ransac_data*) (feat)->feature_data )

/**
   Prototype for transformation functions passed to ransac_xform().  Functions
   of this type should compute a transformation matrix given a set of point
   correspondences.

   @param pts array of points
   @param mpts array of corresponding points; each \a pts[\a i], \a i=0..\a
     n-1, corresponds to \a mpts[\a i]
   @param n number of points in both \a pts and \a mpts
   
   @return Should return a transformation matrix that transforms each point in
     \a pts to the corresponding point in \a mpts or NULL on failure.
*/
typedef CvMat* (*ransac_xform_fn)( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n );


/**
   Prototype for error functions passed to ransac_xform().  For a given
   point, its correspondence, and a transform, functions of this type should
   compute a measure of error between the correspondence and the point after
   the point has been transformed by the transform.

   @param pt a point
   @param mpt \a pt's correspondence
   @param T a transform

   @return Should return a measure of error between \a mpt and \a pt after
     \a pt has been transformed by the transform \a T.
*/
typedef double (*ransac_err_fn)( CvPoint2D64f pt, CvPoint2D64f mpt, CvMat* T );

/******************************** Structures *********************************/

/** vertex coord */
struct vertex_coord 
{
	CvPoint left_bottom_vertex;
	CvPoint left_top_vertex;
	CvPoint right_top_vertex;
	CvPoint right_bottom_vertex;
};

struct matched_feature_pair
{
	CvPoint2D64f cur_coord;
	CvPoint2D64f ref_coord;
};

/** holds feature data relevant to detection */
struct detection_data
{
	int r;
	int c;
	int octv;
	int intvl;
	double subintvl;
	double scl_octv;
};

/**
   Structure to represent an affine invariant image feature.  The fields
   x, y, a, b, c represent the affine region around the feature:

   a(x-u)(x-u) + 2b(x-u)(y-v) + c(y-v)(y-v) = 1
*/
struct feature
{
  double x;                      /**< x coord */
  double y;                      /**< y coord */
  double a;                      /**< Oxford-type affine region parameter */
  double b;                      /**< Oxford-type affine region parameter */
  double c;                      /**< Oxford-type affine region parameter */
  double scl;                    /**< scale of a Lowe-style feature */
  double ori;                    /**< orientation of a Lowe-style feature */
  int d;                         /**< descriptor length */
  double descr[FEATURE_MAX_D];   /**< descriptor */
  int type;                      /**< feature type, OXFD or LOWE */
  int category;                  /**< all-purpose feature category */
  struct feature* fwd_match;     /**< matching feature from forward image */
  struct feature* bck_match;     /**< matching feature from backmward image */
  struct feature* mdl_match;     /**< matching feature from model */
  CvPoint2D64f img_pt;           /**< location in image */
  CvPoint2D64f mdl_pt;           /**< location in model */
  void* feature_data;            /**< user-definable data */
};

/** holds feature data relevant to ransac */
struct ransac_data
{
	void* orig_feat_data;
	int sampled;
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
	vertex_coord* m_vcLastFrameRegion;

	int m_iPanoramaPreWidth;
	int m_iPanoramaPreHeight;

	int m_iPartitionNum;

	int m_iMosaicFrameNo;

	CvPoint m_ptFirstFrameLeftBottomVertex;
	CvRect m_rectCurrentPanoramaRegion;
	CvRect m_rectRefMosaicRegion;

private:
	bool CreatePanorama( IplImage** pBackground, int iWidth, int iHeight );
	void SetBackgroundColor( IplImage* pImg, int iColor );
	void StickFirstFrame( IplImage* pFirstFrame, CvPoint ptPosition, IplImage* pPanorama );
	bool MosaicFrame( IplImage* pFrame, CvRect &rectPosition, IplImage* pPanorama, CvRect &rectCurrentPanoramaRegion, struct vertex_coord* vcLastFrameRegion, int iPartitionNum );
	IplImage** DivideImage( IplImage* pImage, int iPartitionNum, int* iCornerFlag );
	vector<matched_feature_pair> FindIncludedVetexPairs(  vector<matched_feature_pair> vMatchedVertexPairs, int iXOffset, int iYOffset, int iPartitionWidth, int iPartitionHeight );
	void UpdatePanoramaAndRefMosaicRegion( struct vertex_coord* vcLastFrameRegion, CvRect &rectRefMosaicRegion, CvRect &rectCurrentPanoramaRegion, IplImage* pPanorama );
	void AddMatchedVertexPairToVector( vector<matched_feature_pair>& vMatchedVertexPairs, CvPoint ptCur, CvPoint ptRef );

	int FindMatchedFeatures( struct feature* feat1, int iFeat1Num, struct feature* feat2, int iFeat2Num );
	IplImage* DrawMatchedFeatures( IplImage* img1,  IplImage* img2, struct feature* feat, int iFeatNum );

	/*********************** Functions prototyped in opensift **********************/
	int sift_features( IplImage* img, struct feature** feat );
	int _sift_features( IplImage* img, struct feature** feat, int intvls, double sigma, double contr_thr, int curv_thr, int img_dbl, int descr_width, int descr_hist_bins );

	static __inline float pixval32f( IplImage* img, int r, int c )
	{
		return ( (float*)(img->imageData + img->widthStep*r) )[c];
	}

	IplImage* create_init_img( IplImage*, int, double );
	IplImage* convert_to_gray32( IplImage* );
	IplImage*** build_gauss_pyr( IplImage*, int, int, double );
	IplImage* downsample( IplImage* );
	IplImage*** build_dog_pyr( IplImage***, int, int );
	CvSeq* scale_space_extrema( IplImage*** dog_pyr, int octvs, int intvls, double contr_thr, int curv_thr, CvMemStorage* storage );
	int is_extremum( IplImage*** dog_pyr, int octv, int intvl, int r, int c );
	struct feature* interp_extremum( IplImage*** dog_pyr, int octv, int intvl, int r, int c, int intvls, double contr_thr );
	void interp_step( IplImage*** dog_pyr, int octv, int intvl, int r, int c, double* xi, double* xr, double* xc );
	CvMat* deriv_3D( IplImage*** dog_pyr, int octv, int intvl, int r, int c );
	CvMat* hessian_3D( IplImage*** dog_pyr, int octv, int intvl, int r, int c );
	double interp_contr( IplImage*** dog_pyr, int octv, int intvl, int r, int c, double xi, double xr, double xc );
	struct feature* new_feature( void );
	int is_too_edge_like( IplImage* dog_img, int r, int c, int curv_thr );
	void calc_feature_scales( CvSeq* features, double sigma, int intvls );
	void adjust_for_img_dbl( CvSeq* features );
	void calc_feature_oris( CvSeq* features, IplImage*** gauss_pyr );
	double* ori_hist( IplImage* img, int r, int c, int n, int rad, double sigma );
	int calc_grad_mag_ori( IplImage* img, int r, int c, double* mag, double* ori );
	void smooth_ori_hist( double* hist, int n );
	double dominant_ori( double* hist, int n );
	void add_good_ori_features( CvSeq* features, double* hist, int n, double mag_thr, struct feature* feat );
	struct feature* clone_feature( struct feature* feat );
	void compute_descriptors( CvSeq* features, IplImage*** gauss_pyr, int d, int n );
	double*** descr_hist( IplImage* img, int r, int c, double ori, double scl, int d, int n );
	void interp_hist_entry( double*** hist, double rbin, double cbin, double obin, double mag, int d, int n );
	void hist_to_descr( double*** hist, int d, int n, struct feature* feat );
	void normalize_descr( struct feature* feat );
	static int feature_cmp( void* feat1, void* feat2, void* param );
	void release_descr_hist( double**** hist, int d );
	void release_pyr( IplImage**** pyr, int octvs, int n );

	void draw_features( IplImage* img, struct feature* feat, int n );
	void draw_oxfd_features( IplImage* img, struct feature* feat, int n );
	void draw_oxfd_feature( IplImage* img, struct feature* feat, CvScalar color );
	void draw_lowe_features( IplImage* img, struct feature* feat, int n );
	void draw_lowe_feature( IplImage* img, struct feature* feat, CvScalar color );

	double descr_dist_sq( struct feature* f1, struct feature* f2 );
	IplImage* stack_imgs( IplImage* img1, IplImage* img2 );
	double dist_sq_2D( CvPoint2D64f p1, CvPoint2D64f p2 );

	CvMat* ransac_xform( struct feature* features, int n, int mtype, int m, double p_badxform, double err_tol, struct feature*** inliers, int* n_in, vector<matched_feature_pair> vAdjacentMatchedVertexPairs );
	CvMat* dlt_homog( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n );
	CvMat* lsq_homog( CvPoint2D64f* pts, CvPoint2D64f* mpts, int n );
	double homog_xfer_err( CvPoint2D64f pt, CvPoint2D64f mpt, CvMat* H );
	CvPoint2D64f persp_xform_pt( CvPoint2D64f pt, CvMat* T );
	struct feature* get_match( struct feature* feat, int mtype );
	int get_matched_features( struct feature* features, int n, int mtype, struct feature*** matched );
	int calc_min_inliers( int n, int m, double p_badsupp, double p_badxform );
	double log_factorial( int n );
	struct feature** draw_ransac_sample( struct feature** features, int n, int m );
	void extract_corresp_pts( struct feature** features, int n, int mtype, CvPoint2D64f** pts, CvPoint2D64f** mpts );
	void include_additional_corresp_pts( vector<matched_feature_pair> vAdjacentMatchedVertexPairs, CvPoint2D64f* pts, CvPoint2D64f* mpts, int n );
	int find_consensus( struct feature**, int, int, CvMat*, double, struct feature*** );
	void release_mem( CvPoint2D64f*, CvPoint2D64f*, struct feature** );
};
