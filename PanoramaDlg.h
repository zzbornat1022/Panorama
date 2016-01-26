
// PanoramaDlg.h : header file
//

#pragma once

#include <opencv2/opencv.hpp>
#include "CvvImage.h"
#include "Mosaic.h"

// CPanoramaDlg dialog
class CPanoramaDlg : public CDialogEx
{
// Construction
public:
	CPanoramaDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PANORAMA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	
	// TODO: release
	FILE* m_fileInputYUV;
	FILE* m_fileOutputYUV;
	bool m_bFileIsOpen;

	int m_iFrameNo;
	int m_iFrameInterval;
	int m_iFrameWidth;
	int m_iFrameHeight;
	// TODO: release
	CWnd* m_pWndPanorama;
	// TODO: release
	CvvImage m_cvvImg;
	// TODO: release
	IplImage** m_pImages;
	int m_iMosaicFrameAmount;
	// TODO: release
	IplImage* m_pPanorama;
	int m_iPanoramaWidth;
	int m_iPanoramaHeight;

	// TODO: release
	CMosaic m_cMosaic;

public:
	afx_msg void OnOpenFile();
	afx_msg void OnExitApp();
	afx_msg void OnEditStartMosaic();

	// Convert YUV Frame to RGB
	void ShowImgInControl( CWnd* pCWnd, IplImage* pImg );
	void ConvertYUVToRGB(unsigned char* yuv, unsigned char* rgb, int width, int height);

};
