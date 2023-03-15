//////////////////////////////////////////////////////////////////////
// Listener.cpp: implementation of the CListener class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Listener.h"
#include "MicroScopeDlg.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern void Process(smart_ptr<MemBuffer> pImg,CMicroScopeDlg *pDlg);
CListener::CListener()
{
	m_pParent = NULL;
	m_pDrawCWnd = NULL;
}

//////////////////////////////////////////////////////////////////////////

CListener::~CListener() 
{
}

//////////////////////////////////////////////////////////////////////////

void CListener::SetParent( CWnd *pParent)
{
	m_pParent = pParent;
}

//////////////////////////////////////////////////////////////////////////
// Notify parent, device is lost.
//////////////////////////////////////////////////////////////////////////

void CListener::deviceLost( Grabber& param) 
{
	if( m_pParent != NULL)
		m_pParent->PostMessage(MESSAGEDEVICELOST,0,0);
}

//////////////////////////////////////////////////////////////////////////
// Set the CWnd that will be used to render the resulting image.
//////////////////////////////////////////////////////////////////////////

void CListener::SetViewCWnd(CWnd *pView)
{
	m_pDrawCWnd = pView;
	
	RECT r;
	m_pDrawCWnd->GetClientRect(&r);
	m_WindowSize.cx = r.right;
	m_WindowSize.cy = r.bottom;
}
//////////////////////////////////////////////////////////////////////////
// Callback handler.
//////////////////////////////////////////////////////////////////////////

void CListener::frameReady( Grabber& param, smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber)
{
	pBuffer->lock();
	//DoImageProcessing( pBuffer );
	Process(pBuffer,(CMicroScopeDlg *)m_pParent);
	DrawBuffer(pBuffer);
/*
	TCHAR lpInfo[64];
	_stprintf(lpInfo,"%d\n",FrameNumber);
	if(FrameNumber==100)
		//saveToFileBMP( *pBuffer, "e:\\test.bmp" );
		//saveToFileJPEG(*pBuffer,"e:\\²âÊÔ\\test.jpg",75);
		saveToFileJPEG(*pBuffer,_T("e:\\²âÊÔ\\²âÊÔ75.jpg"),75);
	else if(FrameNumber==105)
		saveToFileJPEG(*pBuffer,_T("e:\\²âÊÔ\\²âÊÔ80.jpg"),80);
	else if(FrameNumber==110)
		saveToFileJPEG(*pBuffer,_T("e:\\²âÊÔ\\²âÊÔ90.jpg"),90);
		//saveToFileBMP( *pBuffer, filename );
		//saveToFileBMP(
	//OutputDebugString(lpInfo);
*/
	pBuffer->unlock();
}

//////////////////////////////////////////////////////////////////////////
// Draw the image buffer into the DrawCWnd.
//////////////////////////////////////////////////////////////////////////

void CListener::DrawBuffer( smart_ptr<MemBuffer> pBuffer)
{
	if( m_pDrawCWnd != NULL)
	{
		if( pBuffer != 0 )
		{ 
#if 1
			CRect rectDest;
			CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
			pWnd->AdjRect(rectDest);
			m_WindowSize.cx = rectDest.right;
			m_WindowSize.cy = rectDest.bottom;
#else
			RECT r;
			m_pDrawCWnd->GetClientRect(&r);
			m_WindowSize.cx = r.right;
			m_WindowSize.cy = r.bottom;
#endif
			CDC *pDC = m_pDrawCWnd->GetDC();
			pDC->SetStretchBltMode(COLORONCOLOR);

			smart_ptr<BITMAPINFOHEADER> pInf = pBuffer->getBitmapInfoHeader();
			

			
			int nLines = StretchDIBits( 	pDC->GetSafeHdc(),// Handle to the device
											0, 
											0,
											m_WindowSize.cx,	// Dest. rectangle width.
											m_WindowSize.cy,	// Dest. rectangle height.
											0,				// X-coordinate of lower-left corner of the source rect.
											0,				// Y-coordinate of lower-left corner of the source rect.
											pInf->biWidth,	// Source rectangle width.
											pInf->biHeight, // Number of scan lines.
											pBuffer->getPtr(),	// Modified address of array with DIB bits.
											reinterpret_cast<LPBITMAPINFO>( &*pInf ),	// Address of structure with bitmap info.
											DIB_RGB_COLORS,	// RGB or palette indices.
											SRCCOPY
											);
			m_pDrawCWnd->ReleaseDC(pDC);
		}	
	}	
}
//////////////////////////////////////////////////////////////////////////
// The image processing is done here. For the sake of simplicity, the
// image data is inverted by this sample code.
//////////////////////////////////////////////////////////////////////////

void CListener::DoImageProcessing( smart_ptr<MemBuffer> pBuffer)
{
	// Get the bitmap info header from the membuffer. It contains the bits per pixel,
	// width and height.
	smart_ptr<BITMAPINFOHEADER> pInf = pBuffer->getBitmapInfoHeader();

	// Now retrieve a pointer to the image. For organization of the image data, please
	// refer to:
	// http://www.imagingcontrol.com/ic/docs/html/class/Pixelformat.htm

	BYTE* pImageData = pBuffer->getPtr();

	// Calculate the size of the image.
	int iImageSize = pInf->biWidth * pInf->biHeight * pInf->biBitCount / 8 ;
/*
	// Now loop through the data and change every byte. This small sample inverts every pixel.
	for( int i = 0; i < iImageSize; i++)
	{
		pImageData[i] = 255 - pImageData[i];
	}
*/
}
