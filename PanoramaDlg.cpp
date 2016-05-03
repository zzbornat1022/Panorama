
// PanoramaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Panorama.h"
#include "PanoramaDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPanoramaDlg dialog




CPanoramaDlg::CPanoramaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPanoramaDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bFileIsOpen = false;
	m_iFrameNo = 0;
	m_iFrameInterval = 1;
	m_iFrameWidth = 0;
	m_iFrameHeight = 0;
	m_pImages = NULL;
	m_iMosaicFrameAmount = 0;
	m_iPanoramaWidth = 0;
	m_iPanoramaHeight = 0;
}

void CPanoramaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPanoramaDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_OPEN_FILE, &CPanoramaDlg::OnOpenYUVFile)
	ON_COMMAND(ID_EDIT_START_MOSAIC, &CPanoramaDlg::OnEditStartMosaic)
	ON_COMMAND(ID_EXIT_APP, &CPanoramaDlg::OnExitApp)
END_MESSAGE_MAP()


// CPanoramaDlg message handlers

BOOL CPanoramaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// Get the Full Screen Width and Height
	int iFullScreenX = GetSystemMetrics( SM_CXFULLSCREEN );
	int iFullScreenY = GetSystemMetrics( SM_CYFULLSCREEN );

	SetWindowPos( NULL, 0, 0, iFullScreenX, iFullScreenY, SWP_SHOWWINDOW );

	m_pWndPanorama = GetDlgItem( IDC_PANORAMA );

	// TODO: Dynamically Set Panorama Picture Control
	m_pWndPanorama->SetWindowPos( NULL, 0, 0, 1024, 768, SWP_SHOWWINDOW );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPanoramaDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPanoramaDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPanoramaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPanoramaDlg::OnOpenYUVFile()
{
	// TODO: Add your command handler code here
	TCHAR szFilters[]= _T("YUV Files (*.yuv)|*.yuv|");
	CFileDialog fileDlg( TRUE, NULL, _T("*.yuv"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters );
	CString csInputFilePath;
	if( fileDlg.DoModal() == IDOK )
	{
		csInputFilePath = fileDlg.GetPathName();
	}

	if ( csInputFilePath !=  "" )
	{
		// Convert CString to const char
		CStringA csTempInputFilePath( csInputFilePath );

		// TODO: If open failed
		m_fileInputYUV = fopen( csTempInputFilePath, "rb" );
		m_bFileIsOpen = true;
	}
	else
	{
		return;
	}

	// TODO: width and height should not be fixed
	m_iFrameWidth = 704;
	m_iFrameHeight = 576;
	m_iFrameInterval = 1;
	// TODO: if m_iFrameWidth * m_iFrameHeight * 1.5 isn't an int
	int iFrameByteNum = m_iFrameWidth * m_iFrameHeight * 1.5;

	// TODO: release
	unsigned char* uchYUV = new unsigned char[iFrameByteNum];

	IplImage** pMoreImages = NULL;

	CvRect m_rectCutMarginRegion;
	m_rectCutMarginRegion.x = MOSAIC_MARGIN;
	m_rectCutMarginRegion.y = MOSAIC_MARGIN;
	m_rectCutMarginRegion.width = m_iFrameWidth - 2*MOSAIC_MARGIN;
	m_rectCutMarginRegion.height = m_iFrameHeight - 2*MOSAIC_MARGIN;

	// TODO: If open failed
	while ( true )
	{
		fseek( m_fileInputYUV, m_iFrameNo*iFrameByteNum, SEEK_SET );
		if ( fread( uchYUV, 1, iFrameByteNum, m_fileInputYUV ) == iFrameByteNum )
		{
			m_iMosaicFrameAmount++;

			pMoreImages = (IplImage**) realloc ( m_pImages, m_iMosaicFrameAmount * sizeof(IplImage*) );
			if ( pMoreImages != NULL )
			{
				//TODO: release
				IplImage* pTempImage =  cvCreateImage( cvSize ( m_iFrameWidth , m_iFrameHeight ) , IPL_DEPTH_8U , 3 );
				pTempImage->origin = 1;
				ConvertYUVToRGB( uchYUV, (unsigned char *)pTempImage->imageData, m_iFrameWidth, m_iFrameHeight  );

				m_pImages = pMoreImages;
				m_pImages[m_iMosaicFrameAmount-1] = cvCreateImage( cvSize ( m_iFrameWidth - 2*MOSAIC_MARGIN , m_iFrameHeight - 2*MOSAIC_MARGIN ) , IPL_DEPTH_8U , 3 );
				m_pImages[m_iMosaicFrameAmount-1]->origin = 1;

				cvSetImageROI( pTempImage, m_rectCutMarginRegion );
				cvCopy( pTempImage, m_pImages[m_iMosaicFrameAmount-1], 0 );
				cvResetImageROI( pTempImage );
				cvReleaseImage( &pTempImage );

			}
			else 
			{
				free (m_pImages);
				AfxMessageBox ( _T("Error (re)allocating memory") );
				exit (1);
			}

			m_iFrameNo += m_iFrameInterval;
		}
		else
		{
			AfxMessageBox ( _T("Read Complete") );
			free( uchYUV );
			break;
		}
	}

//  	 Save each frame to output
// 	char chTempOutputPath[255];
// 	for ( int i = 0; i < m_iMosaicFrameAmount; i++)
// 	{
// 		sprintf( chTempOutputPath, "output/Images%d.jpg", i );
// 		cvSaveImage( chTempOutputPath, m_pImages[i] );
// 	}

	// Show image in picture control
	ShowImgInControl ( m_pWndPanorama, m_pImages[0] );
}

void CPanoramaDlg::OnExitApp()
{
	// TODO: Add your command handler code here
	if( MessageBox( _T("Confirm Exit?"), _T("Exit"), MB_YESNO | MB_ICONQUESTION ) == IDYES  )
	{
		free (m_pImages);
		if (m_bFileIsOpen)
			fclose(m_fileInputYUV);
		this->SendMessage( WM_CLOSE );
	}
}

void CPanoramaDlg::OnEditStartMosaic()
{
	// TODO: Add your command handler code here
	if ( m_pImages == NULL )
	{
		AfxMessageBox( _T("Please Load Video First!"), MB_ICONINFORMATION );
		return;
	}
	
	m_iPanoramaWidth = 3000;
	m_iPanoramaHeight = 2000;
	// TODO: Arrange the Mosaic seq
	m_pPanorama = m_cMosaic.Mosaic( m_pImages, m_iMosaicFrameAmount, m_pImages[0]->width, m_pImages[0]->height, m_iPanoramaWidth, m_iPanoramaHeight );
	
	ShowImgInControl( m_pWndPanorama, m_pPanorama );
}

void CPanoramaDlg::ShowImgInControl( CWnd* pCWnd, IplImage* pImg )
{
	CDC* pDC = pCWnd->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	RECT rectCon;
	pCWnd->GetClientRect( &rectCon );

	m_cvvImg.CopyOf( pImg );
	m_cvvImg.DrawToHDC( hDC, &rectCon );

	ReleaseDC( pDC );
}

// Convert YUV Frame to RGB
void CPanoramaDlg::ConvertYUVToRGB(unsigned char * yuv, unsigned char * rgb, int width, int height)
{
	int R, G, B;
	int Y, U, V;
	int x, y;

	int nWidth = width >> 1;
	unsigned char * yuv420[3];

	yuv420[0] = yuv;
	yuv420[1] = yuv + width * height;
	yuv420[2] = yuv + width * height * 5 / 4 ;

	//色度信号宽度
	for ( y = 0; y < height; y++ )
	{
		for ( x=0; x < width; x++ )
		{
			Y = * ( yuv420[0] + y * width + x );
			U = * ( yuv420[1] + ( ( y >> 1 ) * nWidth ) + ( x >> 1 ) );
			V = * ( yuv420[2] + ( ( y >> 1 ) * nWidth ) + ( x >> 1 ) );

			R = (int) ( 1.164 * ( Y - 16 ) + 1.596 * ( V - 128 ) );
			G = (int) ( 1.164 * ( Y - 16 ) - 0.392 * ( U - 128 ) - 0.813 * ( V - 128 ) );
			B = (int) ( 1.164 * ( Y - 16 ) + 2.017 * ( U - 128 ) );

			//防止越界
			if ( R > 255 )	R = 255;
			if ( R < 0 )	R = 0;
			if ( G > 255 )	G = 255;
			if ( G < 0 )	G = 0;
			if ( B > 255 )	B = 255;
			if ( B < 0 )	B = 0;

			* ( rgb + ( ( height - y - 1 ) * width + x ) * 3 ) = B;
			* ( rgb + ( ( height - y - 1 ) * width + x ) * 3 + 1 ) = G;
			* ( rgb + ( ( height - y - 1 ) * width + x ) * 3 + 2 ) = R;
		}
	}
}
