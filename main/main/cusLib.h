#ifndef CUSLIB_H	
#define CUSLIB_H

#include <stdlib.h>
#include <iostream>

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>

#include <osg/MatrixTransform>
#include <osg/TriangleFunctor>
#include <vector>

#include <osg/ComputeBoundsVisitor>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/LineWidth>



//������¼����Ƭ�����Ϣ�Ľṹ��
struct TriangleInfo
{
	int triangleID;

	osg::Vec3f* vecInfo[3];

	float GetXmin()
	{
		float xTmp = vecInfo[0]->x();
		if(vecInfo[1]->x() < xTmp) 
		{	
			xTmp = vecInfo[1]->x();
		}
		if (vecInfo[2]->x() < xTmp)
		{
			xTmp = vecInfo[2]->x();
		}

		return xTmp;
	}


	float GetYmin()
	{
		float yTmp = vecInfo[0]->y();
		if(vecInfo[1]->y() < yTmp) 
		{	
			yTmp = vecInfo[1]->y();
		}
		if (vecInfo[2]->y() < yTmp)
		{
			yTmp = vecInfo[2]->y();
		}

		return yTmp;
	}

	float GetZmin()
	{
		float zTmp = vecInfo[0]->z();
		if(vecInfo[1]->z() < zTmp) 
		{	
			zTmp = vecInfo[1]->z();
		}
		if (vecInfo[2]->z() < zTmp)
		{
			zTmp = vecInfo[2]->z();
		}

		return zTmp;
	}

};

//������¼������ƬID�Լ���ѡ�ָ�ƽ��Ľṹ��
struct TriangleCandidateSplitPlane
{
	int triangleID;
	float xCandidateSplitPlane;
	float yCandidateSplitPlane;
	float zCandidateSplitPlane;
};

//������¼һ������Ӧ������Ƭ��AABB����Ϣ�Ľṹ��
struct DrawableInfo
{
	/*std::vector<osg::ref_ptr<osg::Geode>> res_TP;
	std::vector<osg::ref_ptr<osg::Geode>> res_AABB;*/
	std::vector<TriangleInfo*> triangleInfoArray;
	std::vector<TriangleCandidateSplitPlane> triangleCandidateSplitPlaneArray;
	osg::Vec3Array* vertexList;
};

//����һ��������Ӧ��AABB�����ڵ�����ķ����޹أ�AABB�����涼����ƽ�еģ�
osg::ref_ptr<osg::Geode> createboundingbox(osg::Node* node)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ComputeBoundsVisitor boundvisitor ;
	node->accept(boundvisitor);
	osg::BoundingBox bb = boundvisitor.getBoundingBox();

	float lengthx = bb.xMax()-bb.xMin();
	float lengthy = bb.yMax()-bb.yMin();
	float lengthz = bb.zMax()-bb.zMin();
	osg::Vec3 center= osg::Vec3((bb.xMax()+bb.xMin())/2,(bb.yMax()+bb.yMin())/2,(bb.zMax()+bb.zMin())/2);

	osg::ref_ptr<osg::ShapeDrawable>  drawable = new osg::ShapeDrawable(new osg::Box(center,lengthx,lengthy,lengthz));
	drawable->setColor(osg::Vec4(1.0,1.0,0.0,1.0));

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset= drawable->getOrCreateStateSet();
	osg::ref_ptr<osg::PolygonMode> polygon = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
	stateset->setAttributeAndModes(polygon);

	//�����߿�
	osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth(3.0);
	stateset->setAttribute(linewidth);


	geode->addDrawable(drawable);
	return geode;

}

//������������ȷ��һ����ά��Ƭ
osg::ref_ptr<osg::Geode> GetTrainglePlane(osg::Vec3f* p1,osg::Vec3f* p2,osg::Vec3f* p3,osg::ref_ptr<osg::Geode> parent)
{
	/*osg::Geode* geode=new osg::Geode;
	osg::Geometry* polyGeom = new osg::Geometry;*/

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> polyGeom =  new osg::Geometry;

	osg::Vec3f p1t = (*p1)*computeLocalToWorld(parent->getParentalNodePaths()[0]);
	osg::Vec3f p2t = (*p2)*computeLocalToWorld(parent->getParentalNodePaths()[0]);
	osg::Vec3f p3t = (*p3)*computeLocalToWorld(parent->getParentalNodePaths()[0]);
	
	osg::Vec3 myCoords[]=
	{
		p1t,
		p2t,
		p3t
	};

	
	int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);
	osg::Vec3Array* vertices = new osg::Vec3Array(numCoords,myCoords);
	polyGeom->setVertexArray(vertices);
	polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,numCoords));
	geode->addDrawable(polyGeom);
	//geode->addDrawable(polyGeom);
	//getTriangles(*polyGeom);
	return geode;
}

//������Ƭ���ʽṹ��
struct GetVertex
{
	void operator() (const osg::Vec3& v1,const osg::Vec3& v2,const osg::Vec3& v3, bool) const 
	{
		vertexList->push_back(v1);
		vertexList->push_back(v2);
		vertexList->push_back(v3);
	}

	osg::Vec3Array* vertexList;

};


//�ڵ�����������ڻ�ȡ�����нڵ����������,һ�׸��ڵ�����

class GetWorldCoordOfNodeVisitor : public osg::NodeVisitor 
{
public:
	GetWorldCoordOfNodeVisitor():
	  osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS), done(false)
	  {
		  wcMatrix= new osg::Matrixf();
	  }
	  virtual void apply(osg::Node &node)
	  {
		  if (!done)
		  {
			  if ( 0 == node.getNumParents() ) //������ڵ㣬��ʱ�ڵ�·��Ҳ�Ѽ�¼����
			  {
				  wcMatrix->set( osg::computeLocalToWorld(this->getNodePath()) );
				  done = true;
			  }
			  traverse(node);
		  }
	  }
	  osg::Matrixf* giveUpDaMat() 
	  {
		  return wcMatrix;
	  }

	 
private:
	bool done;
	osg::Matrixf* wcMatrix;
};

// ���ڳ����еĺϷ��ڵ㣬����osg::Matrix��ʽ���������ꡣ
//�û��������ڸ��������������ķ�����֮�󣬼ȿɻ�ȡ�þ���
// ���˺���Ҳ������Ϊ�ڵ�������ĳ�Ա��������

osg::Matrixf* getWorldCoords( osg::Node* node) 
{
	GetWorldCoordOfNodeVisitor* ncv = new GetWorldCoordOfNodeVisitor();
	if (node && ncv)
	{
		node->accept(*ncv);
		return ncv->giveUpDaMat();
	}
	else
	{
		return NULL;
	}
} 



void GetTraingleInfo(osg::Vec3f* p1, osg::Vec3f* p2, osg::Vec3f* p3, int ID, TriangleInfo* res)
{
	res->vecInfo[0] =(osg::Vec3f*) p1;
	res->vecInfo[1] =(osg::Vec3f*) p2;
	res->vecInfo[2] =(osg::Vec3f*) p3;

	res->triangleID = ID;
}


//��ȡһ��Drawable��������Ƭ����
//DrawableInfo* getTriangles(osg::Drawable& drawable,osg::ref_ptr<osg::Geode> geode)
//{
//
//	/*std::vector<osg::ref_ptr<osg::Geode>> res_TP;
//	std::vector<osg::ref_ptr<osg::Geode>> res_AABB;*/
//
//	std::vector<TriangleInfo*> resInfo;
//
//	osg::Vec3* p1 = new osg::Vec3;
//	osg::Vec3* p2 = new osg::Vec3;
//	osg::Vec3* p3 = new osg::Vec3;
//
//	osg::TriangleFunctor<GetVertex> tf;
//	tf.vertexList=new osg::Vec3Array;
//
//	drawable.accept(tf);
//	int i = 0;
//	for(osg::Vec3Array::iterator itr=tf.vertexList->begin();
//		itr!=tf.vertexList->end();
//		itr++)
//	{
//		osg::Vec3 vertex=*itr;
//		//std::cout<<vertex<<std::endl;
//		std::cout<<itr->x()<<" "<<itr->y()<<" "<<itr->z()<<std::endl;
//		//std::cout<<std::endl;
//
//		/*if (i == 2)
//		{
//			std::cout<<std::endl;
//			i = 0;
//		}
//		else
//		{
//			i++;
//		}*/
//
//		//osg::ref_ptr<osg::Geode> geode_TP = new osg::Geode;
//		//osg::ref_ptr<osg::Geode> geode_AABB = new osg::Geode;
//		TriangleInfo* traingleInfo;
//		switch(i)
//		{
//			case 0:
//				p1 = &vertex;
//				i++;
//				break;
//			case 1:
//				p2 = &vertex;
//				i++;
//				break;
//			case 2:
//				p3 = &vertex;
//				std::cout<<std::endl;
//				i = 0;
//				/*geode_TP = GetTrainglePlane(p1, p2, p3,geode);
//				res_TP.push_back(geode_TP);
//
//				geode_AABB = createboundingbox(geode_TP);
//				res_AABB.push_back(geode_AABB);*/
//
//				traingleInfo = GetTraingleInfo(p1, p2, p3);
//				resInfo.push_back(traingleInfo);
//				break;
//		}
//
//	}
//	
//	std::cout<<std::endl;
//
//	DrawableInfo* res = new DrawableInfo;
//	/*res->res_TP = res_TP;
//	res->res_AABB = res_AABB;*/
//
//	res->triangleInfoArray = resInfo;
//
//	return res;
//}


//��ȡ�����������Ƭ��Ϣ
DrawableInfo* getTriangles(osg::Drawable& drawable, osg::Matrixf* mat, int &firstID)
{
	osg::TriangleFunctor<GetVertex> tf;
	tf.vertexList=new osg::Vec3Array;

	drawable.accept(tf);

	TriangleInfo* resTrianglesInfo = new TriangleInfo;
	TriangleCandidateSplitPlane* resTriangleCandidateSplitPlane = new TriangleCandidateSplitPlane;
	DrawableInfo* res = new DrawableInfo;

	osg::Vec3f* p1 = new osg::Vec3f;
	osg::Vec3f* p2 = new osg::Vec3f;
	osg::Vec3f* p3 = new osg::Vec3f;

	res->vertexList = tf.vertexList;
	




	int i = 1;
	for(osg::Vec3Array::iterator itr=tf.vertexList->begin();
		itr!=tf.vertexList->end();
		itr++)
	{
		//std::cout<<itr->x()<<" "<<itr->y()<<" "<<itr->z()<<std::endl;
		//std::cout<<std::endl;



		switch(i)
		{
		case 1:
			p1 = new osg::Vec3f;
			p1 = &(*itr);
			*p1 = (*p1)*(*mat);
			break;
		case 2:
			p2 = new osg::Vec3f;
			p2 = &(*itr);
			*p2 = (*p2)*(*mat);

			break;
		case 3:
			p3 = new osg::Vec3f;
			p3 = &(*itr);
			*p3 = (*p3)*(*mat);


			//	std::cout<<"================================"<<std::endl;
			resTrianglesInfo = new TriangleInfo;
			resTriangleCandidateSplitPlane = new TriangleCandidateSplitPlane;
			GetTraingleInfo(p1, p2, p3, firstID, resTrianglesInfo);
			resTriangleCandidateSplitPlane->triangleID = firstID;
			resTriangleCandidateSplitPlane->xCandidateSplitPlane = resTrianglesInfo->GetXmin();
			resTriangleCandidateSplitPlane->yCandidateSplitPlane = resTrianglesInfo->GetYmin();
			resTriangleCandidateSplitPlane->zCandidateSplitPlane = resTrianglesInfo->GetZmin();

			firstID++;
			res->triangleInfoArray.push_back(&(*resTrianglesInfo));
			res->triangleCandidateSplitPlaneArray.push_back(*resTriangleCandidateSplitPlane);
			//	std::cout<<std::endl;
			i = 0;
			break;
		default:
			break;
		}



		i++;
	}

	//	std::cout<<std::endl;
	return res;
}

DrawableInfo* transDIfromLocalToWorld(DrawableInfo* di)
{
	DrawableInfo* res = new DrawableInfo;

	//std::vector<osg::ref_ptr<osg::Geode>>::iterator it1 = di->res_AABB.begin();
	//std::vector<osg::ref_ptr<osg::Geode>>::iterator it2 = di->res_TP.begin();

	//for (;it1 != di->res_AABB.end();it1++)
	//{
	//	/*int i ;
	//	i = (*it1)->getNumParents();*/
	//	(*it1)->getBound().center()*osg::computeLocalToWorld((*it1)->getParentalNodePaths()[0]);
	//	//printf("%d",i);
	//}

	return res;

}

void createGeode(TriangleInfo* triangleInfo, osg::Geode* geode)
{
	osg::Geometry* polyGeom = new osg::Geometry;
	osg::Vec3f myCoords[]=
	{
		*(triangleInfo->vecInfo[0]),
		*(triangleInfo->vecInfo[1]),
		*(triangleInfo->vecInfo[2]),

	};

	int numCoords = sizeof(myCoords)/sizeof(osg::Vec3f);
	osg::Vec3Array* vertices = new osg::Vec3Array(numCoords,myCoords);
	polyGeom->setVertexArray(vertices);
	polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,numCoords));
	geode->addDrawable(polyGeom);
}


bool checkErr(cl_int status, char* string)
{
	if (CL_SUCCESS != status)
	{
		std::cout<<string<<std::endl;
		system("echo =================================");
		system("pause");
		exit(0);
	}
	return true;
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
		if ( it->xCandidateSplitPlane > res.xCandidateSplitPlane)
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

#endif