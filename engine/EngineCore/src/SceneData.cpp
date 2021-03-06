#include "SceneData.h"
#include <fstream>
#include "interpolation.h"
#include "IORead.h"
#include "Intersect.h"

#define		MAX_PICK_COUNT	200
CSceneData::CSceneData()
:m_nWidth(0)
,m_nHeight(0)
{
}

CSceneData::~CSceneData()
{
}

void CSceneData::clear()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_Cells.clear();
}

void CSceneData::create(size_t width, size_t height)
{
	clear();
	resize(width,height);
}
template <class T>
inline void resizeVector(std::vector<T>& v, size_t srcWidth, size_t srcHeight,size_t destWidth, size_t destHeight,T val=0)
{
	std::vector<T> srcV=v;
	v.resize(destWidth*destHeight,val);
	if (srcV.empty())
	{
		return;
	}
	size_t width = min(srcWidth,destWidth);
	size_t height = min(srcHeight,destHeight);
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			v[y*destWidth+x] = srcV[y*srcWidth+x];
		}
	}
}

bool CSceneData::resize(size_t width, size_t height)
{
	TerrainCell val={0,255,0xFFFFFFFF,0.0f,0};
	resizeVector(m_Cells,m_nWidth+1,m_nHeight+1,width+1,height+1,val);

	m_nWidth = width;
	m_nHeight = height;

	return true;
}

TerrainCell* CSceneData::getCell(int x, int y)
{
	int nCellIndex = y*(m_nWidth+1)+x;
	return &m_Cells[nCellIndex];
}

const TerrainCell* CSceneData::getCell(int x, int y)const
{
	int nCellIndex = y*(m_nWidth+1)+x;
	return &m_Cells[nCellIndex];
}

bool CSceneData::hasGrass(int nCellX, int nCellY)const
{
	const TerrainCell* cell = getCell(nCellX,nCellY);
	const unsigned char uAttribute = getCellAttribute(nCellX,nCellY);
	return 0==cell->uTileID[0]&&255==cell->uTileID[1]&&0==(cell->uAttribute&ATTRIBUTE_GRASS);
}

bool CSceneData::pickCell(int nCellX, int nCellY, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos)const
{
	if (isCellIn(nCellX,nCellY))
	{
		const TerrainCell* cell = getCell(nCellX,nCellY);
		Vec3D v0((float)(nCellX),	cell->fHeight,					(float)(nCellY));
		Vec3D v1((float)(nCellX),	(cell+m_nWidth+1)->fHeight,		(float)(nCellY+1));
		Vec3D v2((float)(nCellX+1),	(cell+m_nWidth+1+1)->fHeight,	(float)(nCellY+1));
		Vec3D v3((float)(nCellX+1),	(cell+1)->fHeight,				(float)(nCellY));
		Vec3D vOut;
		if(IntersectTri(v1,v2,v0,vRayPos,vRayDir,vOut))
		{
			if (pPos)
			{
				*pPos = vOut;
			}
			return true;
		}
		else if (IntersectTri(v3,v0,v2,vRayPos,vRayDir,vOut))
		{
			if (pPos)
			{
				*pPos = vOut;
			}
			return true;
		}
	}
	return false;
}

bool CSceneData::pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos)const
{
	// 格子地图的快速PICK
	int nIncX = vRayDir.x>0?1:-1;
	int nIncY = vRayDir.z>0?1:-1;
	float fK = vRayDir.z/vRayDir.x;
	float fB = vRayPos.z - fK*vRayPos.x;
	int nX = int(vRayPos.x)-nIncX;
	int nY = int(vRayPos.z)-nIncY;

	if (fK<1&&fK>-1)
	{
		for (int i = 0; i < MAX_PICK_COUNT; i++)
		{
			int nCellX = nX + nIncX;
			int nCellY = (int)(fK*(float)nCellX+fB);
			if (nCellY!=nY)
			{
				nY = nCellY;
				nCellX = nX - (nIncX<0);
				nCellY = nY - nIncY*(nIncX<0);
			}
			else
			{
				nX = nCellX;
			}
			if (pickCell(nCellX, nCellY, vRayPos, vRayDir, pPos))
			{
				return true;
			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_PICK_COUNT; i++)
		{
			int nCellY = nY + nIncY;
			int nCellX = (int)(((float)nCellY-fB)/fK);
			if (nCellX!=nX)
			{
				nX = nCellX;
				nCellX = nX - nIncX*(nIncY<0);
				nCellY = nY - (nIncY<0);
			}
			else
			{
				nY = nCellY;
			}
			if (pickCell(nCellX, nCellY, vRayPos, vRayDir, pPos))
			{
				return true;
			}
		}
	}
	return false;
}

unsigned long CSceneData::getVertexIndex(int nCellX, int nCellY)const
{
	return nCellY*(m_nWidth+1) + nCellX;
}

int CSceneData::getCellXByVertexIndex(unsigned long uVertexIndex)const
{
	return uVertexIndex%(m_nWidth+1);
}

int CSceneData::getCellYByVertexIndex(unsigned long uVertexIndex)const
{ 
	return uVertexIndex/(m_nWidth+1);
}

bool CSceneData::isCellIn(int x, int y)const
{
	return x>=0 && x<m_nWidth && y>=0 && y<m_nHeight;
}

bool CSceneData::isPointIn(int x, int y)const
{
	return x>=0 && x<=m_nWidth && y>=0 && y<=m_nHeight;
}

unsigned char CSceneData::getCellTileID(int x, int y, size_t layer)const
{
	if (isCellIn(x,y))
	{
		if (2>layer)
		{
			return getCell(x,y)->uTileID[layer];
		}
	}
	return 0;
}

void CSceneData::setCellTileID(int x, int y, unsigned char uTileID, size_t layer)
{
	if (isCellIn(x,y))
	{
		if (2>layer)
		{
			getCell(x,y)->uTileID[layer] = uTileID;
		}
	}
}

unsigned char CSceneData::getCellAttribute(int x, int y)const
{
	if (isCellIn(x,y))
	{
		return getCell(x,y)->uAttribute;
	}
	return 0;
}

void CSceneData::setCellAttribute(int x, int y, unsigned char uAtt)
{
	if (isCellIn(x,y))
	{
		getCell(x,y)->uAttribute = uAtt;
	}
}

float CSceneData::getVertexHeight(int x, int y)const
{
	if (isPointIn(x,y))
	{
		return getCell(x,y)->fHeight;
	}
	return 0.0f;
}

void CSceneData::setVertexHeight(int x, int y, float fHeight)
{
	if (isPointIn(x,y))
	{
		getCell(x,y)->fHeight = fHeight;
	}
}

Vec3D CSceneData::getVertexNormal(int x, int y)const
{
	float a = getVertexHeight(x,	y);
	float b = getVertexHeight(x,	y+1);
	float c = getVertexHeight(x+1,	y);
	Vec3D vVector0(0,(b-a),1);
	Vec3D vVector1(1,(c-a),0);
	Vec3D vN = vVector0.cross(vVector1);
	return vN.normalize();
}

Color32 CSceneData::getVertexColor(int x, int y)const
{
	if (isPointIn(x,y))
	{
		return getCell(x,y)->color;
	}
	return 0xFFFFFFFF;
}

void CSceneData::setVertexColor(int x, int y, Color32 color)
{
	if (isPointIn(x,y))
	{
		getCell(x,y)->color = color;
	}
}

float CSceneData::getHeight(float fX, float fY)const
{
	int nCellX = fX;
	int nCellY = fY;
	float u = fX - nCellX;
	float v = fY - nCellY;
	float a = getVertexHeight(nCellX,	nCellY);
	float b = getVertexHeight(nCellX+1,	nCellY);
	float c = getVertexHeight(nCellX,	nCellY+1);
	float d = getVertexHeight(nCellX+1,	nCellY+1);
	return bilinearInterpolation(a,b,c,d,u,v);
}

Vec4D CSceneData::getColor(float fX, float fY)const
{
	int nCellX = fX;
	int nCellY = fY;
	float u = fX - nCellX;
	float v = fY - nCellY;
	Vec4D a = getVertexColor(nCellX,	nCellY);
	Vec4D b = getVertexColor(nCellX+1,	nCellY);
	Vec4D c = getVertexColor(nCellX,	nCellY+1);
	Vec4D d = getVertexColor(nCellX+1,	nCellY+1);
	return bilinearInterpolation(a,b,c,d,u,v);
}

#define DIR_COUNT 8
const int DX[DIR_COUNT] = { 0, 1, 1, 1, 0,-1,-1,-1};
const int DY[DIR_COUNT] = {-1,-1, 0, 1, 1, 1, 0,-1};
const int ASTAR_F[DIR_COUNT] = {5,7,5,7,5,7,5,7};

void CSceneData::add2Hash(int x,int y)
{
	getCell(x,y)->SearchedFlag = m_SearchedFlag;
}

bool CSceneData::checkHash(int x,int y)
{
	if (!isCellIn(x,y))
	{
		return false;
	}
	TerrainCell* pCell = getCell(x,y);

	if (pCell->uAttribute&TERRAIN_ATT_TYPE_BALK)
	{
		return false;
	}
	if (pCell->SearchedFlag == m_SearchedFlag)// 可检查节点是否访问过
	{
		return false;
	}
	return true;
}

unsigned char CSceneData::getPath(int sx,int sy,int tx,int ty, std::deque<char>& path)
{
	m_SearchedFlag++;
	m_nNodeCount=0;
	m_nAllNodeCount=0;
	m_nMinH = 1000;
	m_nMinHNode = 0;

	m_tx = tx;
	m_ty = ty;

	int dir = 0;
	addNode(sx,sy,-1,0,0);

	for(;m_nNodeCount>0;)
	{
		Node a = getNode(); // 取出一个节点
		if ((a.x == m_tx) && (a.y == m_ty)) // 目标
		{
			dir = m_allnode[a.n].dir;
			path.clear();
			m_nAllNodeCount = a.n;
			while(m_allnode[m_nAllNodeCount].dir != -1)
			{
				path.push_front(m_allnode[m_nAllNodeCount].dir);
				m_nAllNodeCount = m_allnode[m_nAllNodeCount].father;
			}
			return dir;
			break;
		}
		for (int i = 0; i<DIR_COUNT; i++)
		{
			addNode(a.x+DX[i], a.y+DY[i], i, a.level+ASTAR_F[i], a.n); // 扩展此节点
			if (m_nAllNodeCount>=MAX_ALLNODE || m_nNodeCount>=MAX_NODE)
			{
				break;
			}
		}
	}
	m_nAllNodeCount = m_nMinHNode;
	dir = m_allnode[m_nAllNodeCount].dir;
	path.clear();
	while(m_allnode[m_nAllNodeCount].dir != -1)
	{
		path.push_front(m_allnode[m_nAllNodeCount].dir);
		m_nAllNodeCount = m_allnode[m_nAllNodeCount].father;
	}
	return dir;
}
