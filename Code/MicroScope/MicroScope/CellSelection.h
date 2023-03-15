#ifndef _CELLSELECTION_H_

#define _CELLSELECTION_H_

#include "opencv2\opencv.hpp"
#include "cv.h"
using namespace std;
using namespace cv;

typedef struct Character
{
	double dArea;  
	double dLength;	 
	double dMajorAxis;
	double dMinorAxis;
	double dCompactness;
	double dEccentricity;
	double dCircleDegreeArea;
	double dCircleDegreeLength;
	double dEllipseDegreeArea;
	double dEllipseDegreeLength;
	double dMu_m20;
	double dMu_m02;
	double dMu_m11;

}Character;


//ϸ�������ʷ��࣬ϸ�����Ϊ1�����ʱ��Ϊ0
int CellSelect(Character* parameter);

//�ܰ�ϸ������Ƥϸ�����࣬�ܰ�ϸ�����Ϊ1����Ƥϸ�����Ϊ2
int LymSelect(Character* parameter);

//����תstring
void int2str(const int &int_temp,string &string_temp);

#endif