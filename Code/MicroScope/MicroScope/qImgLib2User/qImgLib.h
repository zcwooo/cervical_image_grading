#ifndef __INC_Q_IMAGE_LIB__
#define __INC_Q_IMAGE_LIB__



#ifdef __cplusplus
extern "C" {
#endif

//#define WIDTHBYTES(bits)		(((bits) + 31) / 32 * 4)
#define WIDTHBYTES(x)         ( (((x) + 31UL) >> 5) << 2 )

/*
���ܣ�
	��ͼ���ļ����뵽�ڴ�λͼ����������ڴ�λͼ�����
	ͼ�����������FreeDIBHandle()�ͷ��ڴ档
������
	lpszPathName��	ͼ���ļ�����
����ֵ��
	NULL��		���󡣴�����Ϣ����־�ļ��м�¼��
	��NULL��	�ɹ��������ڴ�λͼ�����
*/
HANDLE   LoadImageFile( LPCTSTR lpszPathName );

/*
���ܣ�
	�����ڴ�ͼ�������ļ���
������
	m_hDib��		�ڴ�ͼ������
	lpszPathName��	�ļ�����
	nJpegQuality:	����Ǳ���jpeg�ļ�����ָ��ͼ��������Ĭ��ֵ��75. �����100����������ѹ��.

����ֵ��
	TRUE��			�ɹ���
	FALSE��			ʧ�ܡ�������Ϣ����־�ļ��м�¼��
*/
BOOL   SaveImageFile( HANDLE m_hDib, LPCTSTR lpszPathName, int nJpegQuality);


/*
���ܣ�
	��ָ���豸����ʾͼ��
������
	hDC��		��ʾͼ����豸�������NULL��
	m_hDib��	ͼ��������NULL��
	rcDest��	����ʾ�豸����ʾͼ�������ΪNULLʱ����ͼ���ԭʼ��С���豸��(0,0)����ʾ��
����ֵ��
	TRUE��			�ɹ���
	FALSE��			ʧ�ܡ�������Ϣ����־�ļ��м�¼��
*/
BOOL   DrawImage( HDC hDC, HANDLE m_hDib, LPRECT rcDest = NULL );

/*
���ܣ�
	��ȡ�ڴ�ͼ���������ͼ����ߡ���ɫλ������Ϣ��
������
	m_hDib��			�ڴ�ͼ��������NULL��
	m_pImageSize��		�洢ͼ���С��ָ�롣����ΪNULL��
	pnPlanes��			�洢��ɫͨ������ָ�룬������NULL��
	pnBitsPerPixel��	�洢��ɫλ���ָ�룬������NULL��
	pnNumColors��		����256ɫͼ��ʱ�����ڴ洢��ɫ����ɫ����ָ�룬������NULL��24λͼ��ʱ����ɫ����ɫ��Ŀ��0��
����ֵ��
	�ޡ�
*/
void   GetImageInfo( HANDLE m_hDib, SIZE*  m_pImageSize, int *pnPlanes, int *pnBitsPerPixel, int *pnNumColors );

/*
���ܣ�
	���ڵõ�λͼ���������ָ�롣���øú���ʱ�������ȵ���GlobalLock()�����������
������
	lpbi��	����GlobalLock()��õ����ڴ�ָ�롣lpbi=GlobalLock(HGLOBAL handle);
����ֵ��
	ͼ�������ָ����׵�ַ��
*/
LPSTR FindDIBBits( LPSTR lpbi );

/*
���ܣ�
	�Ƿ��ڴ�λͼ������Ƿ��ڴ档
������
	m_hDIB��	�ڴ�λͼ�������NULL��
����ֵ��
	�ޡ�
*/
void FreeDIBHandle( HGLOBAL m_hDIB );
#define SAFE_FreeDIBHandle(m_hDIB)	{ if( (m_hDIB) != NULL) { FreeDIBHandle( (m_hDIB) );	(m_hDIB) = NULL;	}	}

/*
���ܣ�
	��ָ���Ŀ��ߺ���ɫλ��������һ���ڴ�λͼ������������ڴ档����8λ�ڰ�ͼ��ʱ��ʹ�úڰ׵�ɫ�塣
������
	nImgWidth��		ͼ���ȡ�
	nImgHeight��	ͼ��߶ȡ�
	nImgBits��		ͼ����ɫλ������8����24��
����ֵ��
	NULL��			���󣬴�����Ϣ����־�ļ��м�¼��
	��NULL��		�ɹ��������ڴ�λͼ�����
*/
HANDLE CreateDib( int size_x, int size_y, int BitsPerPixel );


/*
���ܣ�
	�ı�8Bitsͼ��ĵ�ɫ��.�����0��ʼ��
������
	hDib��			�ڴ�λͼ�������NULL��
	nPalNum��		Ҫ�ı�ĵ�ɫ����Ŀ
	pPalette��		Ҫ�ı�ĵ�ɫ��
����ֵ��
	FALSE��		���󣬴�����Ϣ����־�ļ��м�¼��
	TRUE��		�ɹ���
*/
BOOL ChangePalette(HANDLE hDib,int nPalNum,PALETTEENTRY *pPalette);


//��Jpeg��������ѹ�õ���֪ͼ��
BOOL JpegBuf2DibEx(HANDLE *phDib,unsigned char* jpeg_buf,int jpegdata_size);
#ifdef __cplusplus
}
#endif				


#endif //!__INC_Q_IMAGE_LIB__
