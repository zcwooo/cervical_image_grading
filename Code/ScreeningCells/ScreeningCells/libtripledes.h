/*
* =====================================================================================
 *
 *       Filename:  libtripledes.h
 *
 *    Description:  GuangZhou Metro AFC
 *
 *        Version:  1.0
 *        Created:  
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ZhouJian (), zhoujian@revenco.com
 *        Company:  Goldsign
 *
 * =====================================================================================
 */

#ifndef _DES_H_
#define _DES_H_

#ifdef __cplusplus
extern "C" {
#endif

#define uchar unsigned char

void libDes(uchar *source, uchar * inkey, int flg);

/**  libTripleDes: 3Des ����64λΪ��λ�Ŀ����ݽ��мӽ���
 @param[out]  ����������
 @param[in]   ������ӽ������ݿ�(8Bytes)
 @param[in]   �ӽ��� ��Կ(16Bytes)
 @param[in]   ���� ����:1, ����:0
 @return      ��
*/
void libTripleDes( uchar *pOut, uchar *pIn, uchar *pKey, uchar Type );

#ifdef  __cplusplus
}
#endif

#endif
