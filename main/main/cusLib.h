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

#include <osgManipulator/CommandManager>
#include <osgManipulator/TranslateAxisDragger>

#include <osg/Light>
#include <osg/LightSource>
#include <osg/BoundingSphere>
#include <osg/BoundingBox>
#include <osgUtil/Optimizer>

#include <osg/TexGen>
#include <osg/Texture2D>
#include <osg/TexEnv>

#include <osg/Material>


typedef enum 
{
	PRINT_INFO = 0xf00000,
};

//分割节点
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

//初始化分割节点
void InitialSplitNode(struct SplitNode *mNode)
{
	mNode->beg = -1;
	mNode->end = -1;
	mNode->leftChild = -1;
	mNode->rightChild = -1;
}

//用来记录三家片相关信息的结构体
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

	float GetXmax()
	{
		float xTmp = vecInfo[0]->x();
		if(vecInfo[1]->x() > xTmp) 
		{	
			xTmp = vecInfo[1]->x();
		}
		if (vecInfo[2]->x() > xTmp)
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

	float GetYmax()
	{
		float yTmp = vecInfo[0]->y();
		if(vecInfo[1]->y() > yTmp) 
		{	
			yTmp = vecInfo[1]->y();
		}
		if (vecInfo[2]->y() > yTmp)
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

	float GetZmax()
	{
		float zTmp = vecInfo[0]->z();
		if(vecInfo[1]->z() > zTmp) 
		{	
			zTmp = vecInfo[1]->z();
		}
		if (vecInfo[2]->z() > zTmp)
		{
			zTmp = vecInfo[2]->z();
		}

		return zTmp;
	}

};

//用来记录三角面片ID以及候选分隔平面的结构体
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

//用来记录一个结点对应三角面片跟AABB等信息的结构体
struct DrawableInfo
{
	/*std::vector<osg::ref_ptr<osg::Geode>> res_TP;
	std::vector<osg::ref_ptr<osg::Geode>> res_AABB;*/
	std::vector<TriangleInfo*> triangleInfoArray;
	std::vector<TriangleCandidateSplitPlane> triangleCandidateSplitPlaneArray;
	osg::Vec3Array* vertexList;
};

//根据一个结点求对应的AABB（跟节点物体的方向无关，AABB各个面都是轴平行的）
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

	//设置线宽
	osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth(3.0);
	stateset->setAttribute(linewidth);


	geode->addDrawable(drawable);
	return geode;

}

//根据三个顶点确定一个三维面片
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

//三角面片访问结构体
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


//节点访问器，用于获取场景中节点的世界坐标,一阶更节点数组

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
			  if ( 0 == node.getNumParents() ) //到达根节点，此时节点路径也已记录完整
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

// 对于场景中的合法节点，返回osg::Matrix格式的世界坐标。
//用户创建用于更新世界坐标矩阵的访问器之后，既可获取该矩阵。
// （此函数也可以作为节点派生类的成员函数。）

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


//获取一个Drawable的三角面片集合
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


//获取集合体的三角片信息
DrawableInfo* getTriangles(osg::Drawable& drawable, osg::Matrixf* mat, int &firstID)
{
	//使用TriangleFunctor获取三角片数据
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
	for(osg::Vec3Array::iterator itr=res->vertexList->begin();
		itr!=res->vertexList->end();
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
			resTriangleCandidateSplitPlane->xMin = resTrianglesInfo->GetXmin();
			resTriangleCandidateSplitPlane->yMin = resTrianglesInfo->GetYmin();
			resTriangleCandidateSplitPlane->zMin = resTrianglesInfo->GetZmin();
			resTriangleCandidateSplitPlane->xMax = resTrianglesInfo->GetXmax();
			resTriangleCandidateSplitPlane->yMax = resTrianglesInfo->GetYmax();
			resTriangleCandidateSplitPlane->zMax = resTrianglesInfo->GetZmax();
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

	//使用getVectexArray的方法来获得三角面片数据
	//TriangleInfo* resTrianglesInfo = new TriangleInfo;
	//TriangleCandidateSplitPlane* resTriangleCandidateSplitPlane = new TriangleCandidateSplitPlane;
	//DrawableInfo* res = new DrawableInfo;

	//osg::Vec3f* p1 = new osg::Vec3f;
	//osg::Vec3f* p2 = new osg::Vec3f;
	//osg::Vec3f* p3 = new osg::Vec3f;

	//res->vertexList =(osg::Vec3Array*) drawable.asGeometry()->getVertexArray();



	//int i = 1;
	//for(osg::Vec3Array::iterator itr=res->vertexList->begin();
	//	itr!=res->vertexList->end();
	//	itr++)
	//{
	//	//std::cout<<itr->x()<<" "<<itr->y()<<" "<<itr->z()<<std::endl;
	//	//std::cout<<std::endl;

	//	
	//	switch(i)
	//	{
	//	case 1:
	//		p1 = new osg::Vec3f;
	//		p1 = &(*itr);
	//		*p1 = (*p1)*(*mat);
	//		break;
	//	case 2:
	//		p2 = new osg::Vec3f;
	//		p2 = &(*itr);
	//		*p2 = (*p2)*(*mat);

	//		break;
	//	default:
	//		p3 = new osg::Vec3f;
	//		p3 = &(*itr);
	//		*p3 = (*p3)*(*mat);


	//		//	std::cout<<"================================"<<std::endl;
	//		resTrianglesInfo = new TriangleInfo;
	//		resTriangleCandidateSplitPlane = new TriangleCandidateSplitPlane;
	//		GetTraingleInfo(p1, p2, p3, firstID, resTrianglesInfo);
	//		resTriangleCandidateSplitPlane->triangleID = firstID;
	//		resTriangleCandidateSplitPlane->xMin = resTrianglesInfo->GetXmin();
	//		resTriangleCandidateSplitPlane->yMin = resTrianglesInfo->GetYmin();
	//		resTriangleCandidateSplitPlane->zMin = resTrianglesInfo->GetZmin();
	//		resTriangleCandidateSplitPlane->xMax = resTrianglesInfo->GetXmax();
	//		resTriangleCandidateSplitPlane->yMax = resTrianglesInfo->GetYmax();
	//		resTriangleCandidateSplitPlane->zMax = resTrianglesInfo->GetZmax();
	//		firstID++;
	//		res->triangleInfoArray.push_back(&(*resTrianglesInfo));
	//		res->triangleCandidateSplitPlaneArray.push_back(*resTriangleCandidateSplitPlane);
	//		//	std::cout<<std::endl;
	//		auto pTmp = p2;
	//		p2 = p3;
	//		p1 = pTmp;
	//		break;
	//	}



	//	i++;
	//}

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

void createGeode(TriangleInfo* triangleInfo, osg::Geode* geode,osg::ref_ptr<osg::Texture2D> texture)
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

	osg::Vec2Array* texcoords = new osg::Vec2Array(3);
	(*texcoords)[0].set(0, 0);
	(*texcoords)[1].set(0, 1);
	(*texcoords)[2].set(1, 0);
	polyGeom->setTexCoordArray(0, texcoords);

	geode->addDrawable(polyGeom);

	osg::StateSet* state = geode->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);

}


bool checkErr(cl_int status, char* string)
{
	if ( PRINT_INFO == status)
	{
		std::cout<<string<<std::endl;
		system("echo =================================");
		system("pause");
	}
	if (CL_SUCCESS != status)
	{
		std::cout<<string<<std::endl;
		system("echo =================================");
		system("pause");
		exit(0);
	}
	return true;
}

//获取比m大的最小的2次幂数
int getMin2Power(int m)
{
	int res = 1;
	while (res < m)
	{
		res<<=1;
	}
	return res;
}

//寻找最大值
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

//最大值填充
void fillTo2PowerScale(std::vector<TriangleCandidateSplitPlane> &input)
{
	int length = getMin2Power(input.size());
	TriangleCandidateSplitPlane max = findMax(input);

	for (int i = input.size(); i < length; i++)
	{
		input.push_back(max);
	}

};


//获取比childnNodeNum大的最小的以2为底的幂
int GetNodeArrayMaxLength(int childNodeNum)
{
	int layerNum = log((float) childNodeNum) / log(2.0) + 1;
	int maxNodeNum = pow(2.0, layerNum) - 1;
	return maxNodeNum;
}

//创建Geode
osg::ref_ptr<osg::Geode> CreateNode(int no, DrawableInfo* all, osg::ref_ptr<osg::Texture2D> texture)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::Geometry* polyGeom = new osg::Geometry;
	osg::Vec3f myCoords[]=
	{
		*(all->triangleInfoArray[no]->vecInfo[0]),
		*(all->triangleInfoArray[no]->vecInfo[1]),
		*(all->triangleInfoArray[no]->vecInfo[2]),

	};

	int numCoords = sizeof(myCoords)/sizeof(osg::Vec3f);
	osg::Vec3Array* vertices = new osg::Vec3Array(numCoords,myCoords);
	polyGeom->setVertexArray(vertices);
	polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,numCoords));

	
	osg::Vec2Array* texcoords = new osg::Vec2Array(3);
	(*texcoords)[0].set(0, 0);
	(*texcoords)[1].set(0, 0);
	(*texcoords)[2].set(0, 0);
	polyGeom->setTexCoordArray(0, texcoords);

	geode->addDrawable(polyGeom);

	osg::StateSet* state = geode->getOrCreateStateSet();

	//创建材质对象
	osg::ref_ptr<osg::Material> mat=new osg::Material;
	//设置正面散射颜色
	mat->setDiffuse(osg::Material::FRONT,osg::Vec4(1.0,0.0,0.0,1.0));
	//设置正面镜面颜色
	mat->setSpecular(osg::Material::FRONT,osg::Vec4(1.0,0.0,0.0,1.0));
	//设置正面指数
	mat->setShininess(osg::Material::FRONT,90.0);
	state->setAttribute(mat.get());

	state->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);

	return geode;
}

//分配三角片信息
osg::ref_ptr<osg::Node> DistributeTrianglesNode(SplitNode *node, std::vector<SplitNode> &splitNodeArray, DrawableInfo* all, osg::ref_ptr<osg::Texture2D> texture)
{
	osg::ref_ptr<osg::Group> group = new osg::Group;
	if ((node->beg != -1) && (node->end != -1))
	{
		if ((node->leftChild != -1) && (node->rightChild != -1))
		{
			
			
			osg::ref_ptr<osg::Node> leftNode = DistributeTrianglesNode(&splitNodeArray[node->leftChild], splitNodeArray, all, texture);
			osg::ref_ptr<osg::Node> rightNode = DistributeTrianglesNode(&splitNodeArray[node->rightChild], splitNodeArray, all, texture);
			group->addChild(leftNode);
			group->addChild(rightNode);
		}
		else 
		{
			for (int i = node->beg; i < node->end; i++)
			{
				osg::ref_ptr<osg::Geode> geode = CreateNode(i, all, texture);
				group->addChild(geode);
			}
		}
	}
	return group;
}




class PickModelHandler : public osgGA::GUIEventHandler
{
public:
	PickModelHandler() : _activeDragger(0) {}

	bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor* )
	{
		osgViewer::View* view = dynamic_cast<osgViewer::View*>( &aa );
		if ( !view ) return false;

		if ( (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_SHIFT)==0 )
			return false;

		switch ( ea.getEventType() )
		{
		case osgGA::GUIEventAdapter::PUSH:
			{
				_pointer.reset();

				osgUtil::LineSegmentIntersector::Intersections intersections;
				if ( view->computeIntersections(ea.getX(), ea.getY(), intersections) )
				{
					_pointer.setCamera( view->getCamera() );
					_pointer.setMousePosition( ea.getX(), ea.getY() );

					for ( osgUtil::LineSegmentIntersector::Intersections::iterator hitr=intersections.begin();
						hitr != intersections.end(); ++hitr)
					{
						_pointer.addIntersection( hitr->nodePath, hitr->getLocalIntersectPoint() );
					}

					for ( osg::NodePath::iterator itr=_pointer._hitList.front().first.begin();
						itr != _pointer._hitList.front().first.end(); ++itr )
					{
						osgManipulator::Dragger* dragger = dynamic_cast<osgManipulator::Dragger*>( *itr );
						if ( dragger )
						{
							dragger->handle( _pointer, ea, aa );
							_activeDragger = dragger;
							break;
						}                   
					}
				}
				break;
			}

		case osgGA::GUIEventAdapter::DRAG:
		case osgGA::GUIEventAdapter::RELEASE:
			{
				if ( _activeDragger )
				{
					_pointer._hitIter = _pointer._hitList.begin();
					_pointer.setCamera( view->getCamera() );
					_pointer.setMousePosition( ea.getX(), ea.getY() );

					_activeDragger->handle( _pointer, ea, aa );
				}

				if ( ea.getEventType()==osgGA::GUIEventAdapter::RELEASE )
				{
					_activeDragger = NULL;
					_pointer.reset();
				}
				break;
			}

		default:
			break;
		}
		return true;
	}

protected:
	osgManipulator::Dragger* _activeDragger;
	osgManipulator::PointerInfo _pointer;
};

//向场景中添加光源
osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Group> lightRoot= new osg::Group();
	lightRoot->addChild(node);

	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT0,osg::StateAttribute::ON);

	//计算包围盒
	osg::BoundingSphere bs ;
	node->computeBound() ;
	bs=node->getBound() ;

	//创建一个Light对象
	osg::ref_ptr<osg::Light> light = new osg::Light();
	light->setLightNum(0);
	//设置方向
	light->setDirection(osg::Vec3(0.0f,0.0f,-1.0f));
	//设置位置
	light->setPosition(osg::Vec4(bs.center().x(),bs.center().y(),bs.center().z()+bs.radius(),1.0f));
	//设置环境光的颜色
	light->setAmbient(osg::Vec4(0.0f,1.0f,1.0f,1.0f));
	//设置散射光的颜色
	light->setDiffuse(osg::Vec4(0.0f,1.0f,1.0f,1.0f));

	//设置恒衰减指数
	light->setConstantAttenuation(1.0f);
	//设置线形衰减指数
	light->setLinearAttenuation(0.0f);
	//设置二次方衰减指数
	light->setQuadraticAttenuation(0.0f);

	//创建光源
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
	lightSource->setLight(light.get());

	lightRoot->addChild(lightSource.get());

	return lightRoot.get() ;
}

#endif