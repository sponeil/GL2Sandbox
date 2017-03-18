// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __MixedHeightMapFactory_h__
#define __MixedHeightMapFactory_h__

#include "PlanetaryObject.h"
#include "PlanetaryMapNode.h"


class CMixedHeightMapFactory : public CSimpleHeightMapFactory
{
protected:
	int m_nBufferSize;
	CPixelBuffer m_pb[6];

public:
	typedef TReference<CMixedHeightMapFactory> Ref;
	CMixedHeightMapFactory(CObjectType *pType) : CSimpleHeightMapFactory(pType)
	{
	}

	virtual void Init(CPropertySet &prop);
	virtual void BuildNode(CPlanetaryMapNode *pNode);

	float GetHeight(const CPlanetaryMapCoord &coord);
};
DECLARE_GENERIC_TYPE_CLASS(CMixedHeightMapFactory, CSimpleHeightMapFactoryType);

#endif // __MixedHeightMapFactory_h__

