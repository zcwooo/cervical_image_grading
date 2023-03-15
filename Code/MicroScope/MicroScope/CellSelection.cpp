#include "stdafx.h"
#include "CellSelection.h"

//ϸ�������ʷ��࣬ϸ�����Ϊ1�����ʱ��Ϊ0
int CellSelect(Character* parameter)
{
	int isCell = 0;

	if(parameter->dEllipseDegreeArea<=0.987487018108 && parameter->dEllipseDegreeArea<=0.978341460228 && parameter->dMajorAxis<=15.7720003128) isCell = 0;
	if(parameter->dEllipseDegreeArea<=0.987487018108 && parameter->dEllipseDegreeArea<=0.978341460228 && parameter->dMajorAxis>15.7720003128 && parameter->dCompactness<=4.40082645416) isCell = 0;
	if(parameter->dEllipseDegreeArea<=0.987487018108 && parameter->dEllipseDegreeArea<=0.978341460228 && parameter->dMajorAxis>15.7720003128 && parameter->dCompactness>4.40082645416 && parameter->dMinorAxis<=19.2850551605) isCell = 0;
	if(parameter->dEllipseDegreeArea<=0.987487018108 && parameter->dEllipseDegreeArea<=0.978341460228 && parameter->dMajorAxis>15.7720003128 && parameter->dCompactness>4.40082645416 && parameter->dMinorAxis>19.2850551605) isCell = 0;
	if(parameter->dEllipseDegreeArea<=0.987487018108 && parameter->dEllipseDegreeArea>0.978341460228 && parameter->dArea<=706.0 && parameter->dEllipseDegreeLength<=0.913842499256 && parameter->dEccentricity<=0.741135001183) isCell = 0;
	if(parameter->dEllipseDegreeArea<=0.987487018108 && parameter->dEllipseDegreeArea>0.978341460228 && parameter->dArea<=706.0 && parameter->dEllipseDegreeLength<=0.913842499256 && parameter->dEccentricity>0.741135001183) isCell = 0;
	if(parameter->dEllipseDegreeArea<=0.987487018108 && parameter->dEllipseDegreeArea>0.978341460228 && parameter->dArea<=706.0 && parameter->dEllipseDegreeLength>0.913842499256 && parameter->dMu_m02<=346431.6875) isCell = 1;
	if(parameter->dEllipseDegreeArea<=0.987487018108 && parameter->dEllipseDegreeArea>0.978341460228 && parameter->dArea<=706.0 && parameter->dEllipseDegreeLength>0.913842499256 && parameter->dMu_m02>346431.6875) isCell = 0;
	if(parameter->dEllipseDegreeArea<=0.987487018108 && parameter->dEllipseDegreeArea>0.978341460228 && parameter->dArea>706.0) isCell = 1;
	if(parameter->dEllipseDegreeArea>0.987487018108 && parameter->dMinorAxis<=19.2734985352 && parameter->dEccentricity<=0.61796849966) isCell = 1;
	if(parameter->dEllipseDegreeArea>0.987487018108 && parameter->dMinorAxis<=19.2734985352 && parameter->dEccentricity>0.61796849966) isCell = 0;
	if(parameter->dEllipseDegreeArea>0.987487018108 && parameter->dMinorAxis>19.2734985352) isCell = 1;




	return isCell;
}


//�ܰ�ϸ������Ƥϸ�����࣬�ܰ�ϸ�����Ϊ1����Ƥϸ�����Ϊ2
int LymSelect(Character* parameter)
{
	int isLymphocyte = 0;

	if(parameter->dMajorAxis<=28.0651855469 && parameter->dCircleDegreeArea<=0.726181030273 && parameter->dEllipseDegreeLength<=0.924414038658 && parameter->dCircleDegreeLength<=0.780708491802) isLymphocyte = 1;
	if(parameter->dMajorAxis<=28.0651855469 && parameter->dCircleDegreeArea<=0.726181030273 && parameter->dEllipseDegreeLength<=0.924414038658 && parameter->dCircleDegreeLength>0.780708491802) isLymphocyte = 2;
	if(parameter->dMajorAxis<=28.0651855469 && parameter->dCircleDegreeArea<=0.726181030273 && parameter->dEllipseDegreeLength>0.924414038658) isLymphocyte = 1;
	if(parameter->dMajorAxis<=28.0651855469 && parameter->dCircleDegreeArea>0.726181030273 && parameter->dEllipseDegreeLength<=0.89247751236) isLymphocyte = 1;
	if(parameter->dMajorAxis<=28.0651855469 && parameter->dCircleDegreeArea>0.726181030273 && parameter->dEllipseDegreeLength>0.89247751236) isLymphocyte = 1;
	if(parameter->dMajorAxis>28.0651855469 && parameter->dMajorAxis<=30.2201404572) isLymphocyte = 2;
	if(parameter->dMajorAxis>28.0651855469 && parameter->dMajorAxis>30.2201404572) isLymphocyte = 2;



	return isLymphocyte;
}


//����תstring
void int2str(const int &int_temp,string &string_temp)  
{  
	stringstream stream;  
	stream<<int_temp;  
	string_temp=stream.str(); 
}  

