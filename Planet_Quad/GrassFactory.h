// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __GrassFactory_h__
#define __GrassFactory_h__

#include "PlanetaryObject.h"
#include "PlanetaryMapNode.h"


class CGrassFactory : public CPlanetaryFactory
{
protected:
	CGLShaderObject m_shBillboard;
	CTexture m_tGrass;
	std::map<CPlanetaryMapNode *, std::list<CPlanetaryMapCoord> > m_mapGrass;

public:
	typedef TReference<CGrassFactory> Ref;
	CGrassFactory(CObjectType *pType) : CPlanetaryFactory(pType)
	{
	}

	virtual void Init(CPropertySet &prop)
	{
		CPixelBuffer pb;
		pb.LoadPNG("grassPack.png");
		m_tGrass.Init(&pb, true, false);
		m_shBillboard.Init("shaders\\BillboardVert.glsl", "shaders\\BillboardFrag.glsl");
	}
	virtual void BuildNode(CPlanetaryMapNode *pNode);
	virtual void DestroyNode(CPlanetaryMapNode *pNode);
	virtual void Update();
	virtual void Draw();
};
DECLARE_GENERIC_TYPE_CLASS(CGrassFactory, CPlanetaryFactoryType);

#endif // __GrassFactory_h__
