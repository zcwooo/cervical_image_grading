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


//细胞与杂质分类，细胞标记为1，杂质标记为0
int CellSelect(Character* parameter);

//淋巴细胞、上皮细胞分类，淋巴细胞标记为1，上皮细胞标记为2
int LymSelect(Character* parameter);

//数字转string
void int2str(const int &int_temp,string &string_temp);

#endif