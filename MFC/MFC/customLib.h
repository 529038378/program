#ifndef _CUSTOMLIB_H
#define _CUSTOMLIB_H

#include <iostream>
#include <vector>
#include <cl/cl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stdafx.h"

 //KD-Tree��������
#define MAXDEPTH 20
#define T0 100	//ģ���˻��㷨�еĳ�ʼ�¶�
#define NMAX 100	//ģ���˻��㷨��һ�ν��µĲ�����
//#define MAXITER	2
//#define DEC_SPEED 0.5


//#define __ONEDIMCAL__
#define __TWODIMCAL__

#define __OPT__
//#define __COMM__

#define __REALTIME__
//#define __NREALTIME__
typedef enum 
{
	PRINT_INFO = 0xf00000,
};

bool checkErr(cl_int status, char* string)
{
	if ( PRINT_INFO == status)
	{
		std::cout<<string<<std::endl;
		system("echo =================================");
		//system("pause");
		Sleep(1000);
	}
	else if (CL_SUCCESS != status)
	{
		std::cout<<string<<std::endl;
		system("echo =================================");
		system("pause");
		exit(0);
	}
	return true;
}

//������¼����Ƭ�����Ϣ�Ľṹ��
struct TriangleInfo
{
	int triangleID;

	glm::vec3 vecInfo[3];

	glm::vec3 vecNormal;

	float GetXmin()
	{
		float xTmp = vecInfo[0].x;
		if(vecInfo[1].x < xTmp) 
		{	
			xTmp = vecInfo[1].x;
		}
		if (vecInfo[2].x < xTmp)
		{
			xTmp = vecInfo[2].x;
		}

		return xTmp;
	}

	float GetXmax()
	{
		float xTmp = vecInfo[0].x;
		if(vecInfo[1].x > xTmp) 
		{	
			xTmp = vecInfo[1].x;
		}
		if (vecInfo[2].x > xTmp)
		{
			xTmp = vecInfo[2].x;
		}

		return xTmp;
	}

	float GetYmin()
	{
		float yTmp = vecInfo[0].y;
		if(vecInfo[1].y < yTmp) 
		{	
			yTmp = vecInfo[1].y;
		}
		if (vecInfo[2].y < yTmp)
		{
			yTmp = vecInfo[2].y;
		}

		return yTmp;
	}

	float GetYmax()
	{
		float yTmp = vecInfo[0].y;
		if(vecInfo[1].y > yTmp) 
		{	
			yTmp = vecInfo[1].y;
		}
		if (vecInfo[2].y > yTmp)
		{
			yTmp = vecInfo[2].y;
		}

		return yTmp;
	}

	float GetZmin()
	{
		float zTmp = vecInfo[0].z;
		if(vecInfo[1].z < zTmp) 
		{	
			zTmp = vecInfo[1].z;
		}
		if (vecInfo[2].z < zTmp)
		{
			zTmp = vecInfo[2].z;
		}

		return zTmp;
	}

	float GetZmax()
	{
		float zTmp = vecInfo[0].z;
		if(vecInfo[1].z > zTmp) 
		{	
			zTmp = vecInfo[1].z;
		}
		if (vecInfo[2].z > zTmp)
		{
			zTmp = vecInfo[2].z;
		}

		return zTmp;
	}

};

//������¼������ƬID�Լ���ѡ�ָ�ƽ��Ľṹ��
struct TriangleCandidateSplitPlane
{
	int triangleID;
	float xMin;
	float yMin;
	float zMin;
	float xMax;
	float yMax;
	float zMax;
};

//������¼һ������Ӧ������Ƭ��AABB����Ϣ�Ľṹ��
struct DrawableInfo
{
	/*std::vector<osg::ref_ptr<osg::Geode>> res_TP;
	std::vector<osg::ref_ptr<osg::Geode>> res_AABB;*/
	std::vector<TriangleInfo> triangleInfoArray;
	std::vector<TriangleCandidateSplitPlane> triangleCandidateSplitPlaneArray;
	glm::vec3* vertexList;
};

void GetTraingleInfo(glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, glm::vec3* nor, int ID, TriangleInfo* res)
{
	res->vecInfo[0] = *p1;
	res->vecInfo[1] = *p2;
	res->vecInfo[2] = *p3;

	res->vecNormal = *nor;
	
	res->triangleID = ID;
}

//��������Ϣ����Ϊ����Ƭ��Ϣ
DrawableInfo* getTriangles(glm::vec3* v3Vertex, glm::vec3* v3Normal, int iVerLen)
{
	int ID = 0;
	TriangleInfo* resTrianglesInfo = new TriangleInfo;
	TriangleCandidateSplitPlane* resTriangleCandidateSplitPlane = new TriangleCandidateSplitPlane;
	DrawableInfo* res = new DrawableInfo;

	glm::vec3* p1 = new glm::vec3;
	glm::vec3* p2 = new glm::vec3;
	glm::vec3* p3 = new glm::vec3;
	glm::vec3* nor = new glm::vec3;
	res->vertexList = v3Vertex;

	int i = 1;
	for(int j=0 ; j<iVerLen; j++)
	{
		switch(i)
		{
		case 1:
			p1 = new glm::vec3;
			p1 = &v3Vertex[j];
			break;
		case 2:
			p2 = new glm::vec3;
			p2 = &v3Vertex[j];

			break;
		case 3:
			p3 = new glm::vec3;
			p3 = &v3Vertex[j];

			nor = new glm::vec3;
			nor = &v3Normal[j];

			//	std::cout<<"================================"<<std::endl;
			resTrianglesInfo = new TriangleInfo;
			resTriangleCandidateSplitPlane = new TriangleCandidateSplitPlane;
			GetTraingleInfo(p1, p2, p3, nor, ID, resTrianglesInfo);
			resTriangleCandidateSplitPlane->triangleID = ID;
			ID++;
			resTriangleCandidateSplitPlane->xMin = resTrianglesInfo->GetXmin();
			resTriangleCandidateSplitPlane->yMin = resTrianglesInfo->GetYmin();
			resTriangleCandidateSplitPlane->zMin = resTrianglesInfo->GetZmin();
			resTriangleCandidateSplitPlane->xMax = resTrianglesInfo->GetXmax();
			resTriangleCandidateSplitPlane->yMax = resTrianglesInfo->GetYmax();
			resTriangleCandidateSplitPlane->zMax = resTrianglesInfo->GetZmax();
			res->triangleInfoArray.push_back(*resTrianglesInfo);
			res->triangleCandidateSplitPlaneArray.push_back(*resTriangleCandidateSplitPlane);
			//	std::cout<<std::endl;
			i = 0;
			break;
		default:
			break;
		}



		i++;
	}
	return res;
}

//��ȡ��m�����С��2������
int getMin2Power(int m)
{
	int res = 1;
	while (res < m)
	{
		res<<=1;
	}
	return res;
}

//Ѱ�����ֵ
TriangleCandidateSplitPlane findMax(std::vector<TriangleCandidateSplitPlane> input)
{
	TriangleCandidateSplitPlane res = input[0];
	for (auto it = input.begin(); it < input.end(); it++)
	{
		if ( it->xMin > res.xMin)
		{
			res = *it;
		}
	}
	return res;
}

//���ֵ���
void fillTo2PowerScale(std::vector<TriangleCandidateSplitPlane> &input)
{
	int length = getMin2Power(input.size());
	TriangleCandidateSplitPlane max = findMax(input);

	for (int i = input.size(); i < length; i++)
	{
		input.push_back(max);
	}

};

//�ָ�ڵ�
struct SplitNode
{
	int beg;
	int end;
	int leftChild;
	int rightChild;
	float xMin;
	float xMax;
	float yMin;
	float yMax;
	float zMin;
	float zMax;
};

//��ʼ���ָ�ڵ�
void InitialSplitNode(struct SplitNode *mNode)
{
	mNode->beg = -1;
	mNode->end = -1;
	mNode->leftChild = -1;
	mNode->rightChild = -1;
}

//��ȡ��childnNodeNum�����С����2Ϊ�׵���
int GetNodeArrayMaxLength(int childNodeNum)
{
	int layerNum = log((float) childNodeNum) / log(2.0) + 1;
	int maxNodeNum = pow(2.0, layerNum) - 1;
	return maxNodeNum;
}

//MFC ���
struct Argument
{
	char*	pcFileName;
	int		iIters;
	int		iKDTreeDepth;
	int		iInitTemp;
	int		iSampleTimes;
	int		iDesSpeed;	
	bool	bRealTime;
};

#endif
