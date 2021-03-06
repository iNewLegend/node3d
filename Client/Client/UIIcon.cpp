#include "UIIcon.h"
#include "RenderSystem.h"
#include "UIDialog.h"
#include "Graphics.h"
#include "SkeletonNode.h"
#include "RPGSkyUIGraph.h"
#include "MainRoot.h"
#include "Res.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//void* s_pTexture = NULL;

CUIIcon::CUIIcon()
	:m_nIconID(-1)
{
	m_TargetPoint.x=-1;
	m_TargetPoint.y=-1;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CUIIcon::~CUIIcon()
{
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnFrameMove(double fTime, float fElapsedTime)
{
	CUIButton::OnFrameMove(fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnFrameRender(const Matrix & mTransform, double fTime, float fElapsedTime)
{
	CUIButton::OnFrameRender(mTransform, fTime, fElapsedTime);
//	if ((m_TargetPoint.x!=-1&&m_TargetPoint.y!=-1)
	//	&&(m_TargetPoint.x!=m_rcBoundingBox.left&&m_TargetPoint.y!=m_rcBoundingBox.top))
//	{
//		int x = m_rcBoundingBox.left+(m_TargetPoint.x - m_rcBoundingBox.left)*0.5f;
//		int y = m_rcBoundingBox.top+(m_TargetPoint.y - m_rcBoundingBox.top)*0.5f;
//		OnMove(x,y);
//	}
	//Node3DUIGraph::getInstance().drawText(L"2", 3, rcDest, ALIGN_TYPE_CENTER);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnMouseMove(POINT point)
{
	CUIButton::OnMouseMove(point);
	// ----
	if (IsPressed())
	{
		//m_TargetPoint = point;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnMouseWheel(POINT point,short wheelDelta)
{
	CUIButton::OnMouseWheel(point, wheelDelta);
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnLButtonDown(POINT point)
{
	CUIButton::OnLButtonDown(point);
	CDlgMain::getInstance().setIconCursor(this);
	// ----
	//m_OldPoint.x = m_rcBoundingBox.left;
//	m_OldPoint.y = m_rcBoundingBox.top;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnLButtonUp(POINT point)
{
	CUIButton::OnLButtonUp(point);
	// ----
	//m_TargetPoint = m_OldPoint;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnRButtonDown(POINT point)
{
	CUIButton::OnRButtonDown(point);
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnRButtonUp(POINT point)
{
	CUIButton::OnRButtonUp(point);
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnMButtonDown(POINT point)
{
	CUIButton::OnMButtonDown(point);
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnMButtonUp(POINT point)
{
	CUIButton::OnMButtonUp(point);
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CUIIcon::OnSize(const CRect<int>& rc)
{
	CUIButton::OnSize(rc);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const ItemData& CUIIcon::getItemData()
{
	return m_ItemData;
}

void CUIIcon::setItemData(const ItemData& itemData)
{
	m_ItemData = itemData;
	// ----
	m_nIconID = m_ItemData.cType*16+m_ItemData.cIndex;
	// ----
	// # Set My Icon Rect
	if (m_Style.m_StyleSprites.size()>0)
	{
		UIGraph::getInstance().releaseTexture(m_Style.m_StyleSprites[0].m_pTexture);
		m_Style.m_StyleSprites[0].m_pTexture = UIGraph::getInstance().createTexture("data\\ui\\icon.tga");
		// ----
		if (m_nIconID>=0)
		{
			int x=m_nIconID%16;
			int y=m_nIconID/16;
			RECT rcSrc={x*32, y*32, x*32+32, y*32+32};
			m_Style.m_StyleSprites[0].m_nLayoutType=1;
			m_Style.m_StyleSprites[0].m_rcBorder = rcSrc;
		}
	}
	// ----
	CSVITEM.seek(0, m_ItemData.cType).seek(1, m_ItemData.cIndex);
	// ----
	std::string strTip;
	strTip.append( "[align=center]" );
	// name
	if (m_ItemData.level>6)
	{
		strTip.append( "[color=255,255,0]" );
	}
	else if (m_ItemData.level>4)
	{
		strTip.append( "[color=128,128,255]" );
	}
	else
	{
		strTip.append( "[color=255,255,255]" );
	}
	strTip.append( CSVITEM.getStr("Name") );
	if (m_ItemData.level>0)
	{
		strTip.append( " +" );
		char szTemp[256];
		strTip.append( itoa(m_ItemData.level,szTemp,10) );
	}
	strTip.append( "[/color][br]" );
	// DamMin-DamMax
	strTip.append( "攻击力: " );
	strTip.append( CSVITEM.getStr("DamMin") );
	strTip.append( "-" );
	strTip.append( CSVITEM.getStr("DamMax") );
	strTip.append( "[br]" );
	// Speed
	strTip.append( "攻击速度: " );
	strTip.append( CSVITEM.getStr("Speed") );
	strTip.append( "[br]" );
	// Dur
	strTip.append( "耐久度: 0\\" );
	strTip.append( CSVITEM.getStr("Dur") );
	strTip.append( "[br]" );
	// Str
	strTip.append( "所需力量: " );
	strTip.append( CSVITEM.getStr("Str") );
	strTip.append( "[br]" );
	//
	strTip.append( "[/align]" );
	// ----
	SetTip(s2ws(strTip));
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------