#ifndef __INC_Q_IMAGE_LIB__
#define __INC_Q_IMAGE_LIB__



#ifdef __cplusplus
extern "C" {
#endif

//#define WIDTHBYTES(bits)		(((bits) + 31) / 32 * 4)
#define WIDTHBYTES(x)         ( (((x) + 31UL) >> 5) << 2 )

/*
功能：
	把图像文件调入到内存位图句柄。返回内存位图句柄。
	图像句柄用完后，用FreeDIBHandle()释放内存。
参数：
	lpszPathName：	图像文件名。
返回值：
	NULL：		错误。错误信息在日志文件中记录。
	非NULL：	成功，返回内存位图句柄。
*/
HANDLE   LoadImageFile( LPCTSTR lpszPathName );

/*
功能：
	保存内存图像句柄到文件。
参数：
	m_hDib：		内存图像句柄。
	lpszPathName：	文件名。
	nJpegQuality:	如果是保存jpeg文件，则指定图像质量。默认值是75. 如果是100，则是无损压缩.

返回值：
	TRUE：			成功。
	FALSE：			失败。错误信息在日志文件中记录。
*/
BOOL   SaveImageFile( HANDLE m_hDib, LPCTSTR lpszPathName, int nJpegQuality);


/*
功能：
	在指定设备上显示图像。
参数：
	hDC：		显示图像的设备句柄。非NULL。
	m_hDib：	图像句柄。非NULL。
	rcDest：	在显示设备上显示图像的区域。为NULL时，用图像的原始大小在设备的(0,0)点显示。
返回值：
	TRUE：			成功。
	FALSE：			失败。错误信息在日志文件中记录。
*/
BOOL   DrawImage( HDC hDC, HANDLE m_hDib, LPRECT rcDest = NULL );

/*
功能：
	获取内存图像句柄代表的图像宽、高、颜色位数等信息。
参数：
	m_hDib：			内存图像句柄，非NULL。
	m_pImageSize：		存储图像大小的指针。可以为NULL。
	pnPlanes：			存储颜色通道数的指针，可以是NULL。
	pnBitsPerPixel：	存储颜色位深的指针，可以是NULL。
	pnNumColors：		当是256色图像时，用于存储调色板颜色数的指针，可以是NULL。24位图像时，调色板颜色数目是0。
返回值：
	无。
*/
void   GetImageInfo( HANDLE m_hDib, SIZE*  m_pImageSize, int *pnPlanes, int *pnBitsPerPixel, int *pnNumColors );

/*
功能：
	用于得到位图句柄的数据指针。调用该函数时，必须先调用GlobalLock()来锁定句柄。
参数：
	lpbi：	调用GlobalLock()后得到的内存指针。lpbi=GlobalLock(HGLOBAL handle);
返回值：
	图像的数据指针的首地址。
*/
LPSTR FindDIBBits( LPSTR lpbi );

/*
功能：
	是否内存位图句柄、是否内存。
参数：
	m_hDIB：	内存位图句柄，非NULL。
返回值：
	无。
*/
void FreeDIBHandle( HGLOBAL m_hDIB );
#define SAFE_FreeDIBHandle(m_hDIB)	{ if( (m_hDIB) != NULL) { FreeDIBHandle( (m_hDIB) );	(m_hDIB) = NULL;	}	}

/*
功能：
	用指定的宽、高和颜色位数，创建一个内存位图句柄，并分配内存。当是8位黑白图像时，使用黑白调色板。
参数：
	nImgWidth：		图像宽度。
	nImgHeight：	图像高度。
	nImgBits：		图像颜色位数。是8或者24。
返回值：
	NULL：			错误，错误信息在日志文件中记录。
	非NULL：		成功，返回内存位图句柄。
*/
HANDLE CreateDib( int size_x, int size_y, int BitsPerPixel );


/*
功能：
	改变8Bits图像的调色板.从序号0开始。
参数：
	hDib：			内存位图句柄，非NULL。
	nPalNum：		要改变的调色板数目
	pPalette：		要改变的调色板
返回值：
	FALSE：		错误，错误信息在日志文件中记录。
	TRUE：		成功。
*/
BOOL ChangePalette(HANDLE hDib,int nPalNum,PALETTEENTRY *pPalette);


//把Jpeg缓冲区解压得到已知图像
BOOL JpegBuf2DibEx(HANDLE *phDib,unsigned char* jpeg_buf,int jpegdata_size);
#ifdef __cplusplus
}
#endif				


#endif //!__INC_Q_IMAGE_LIB__
