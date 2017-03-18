// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#include "StdAfx.h"
#include "PlanetaryObject.h"
#include "PlanetaryMapNode.h"


void CSimpleHeightMapFactory::BuildNode(CPlanetaryMapNode *pNode)
{
	PROFILE("CSimpleHeightMapFactory::BuildNode", 3);
	for(int nIndex=0; nIndex<BORDER_MAP_COUNT; nIndex++)
	{
		CVector vNormalized = builder.coord[nIndex].GetDirectionVector();
		float fHeight = builder.coord[nIndex].GetHeight();
		if(m_nFractalSeed)
		{
			CVector v = vNormalized;
			fHeight += fBmTest3(v, m_fOctaves, 1.0f, -0.1f) * pNode->GetPlanetaryMap()->GetMaxHeight();
		}
		builder.coord[nIndex].SetHeight(fHeight);
	}
}

void CSimpleCraterFactory::BuildNode(CPlanetaryMapNode *pNode)
{
	PROFILE("CSimpleCraterFactory::BuildNode", 3);

	if(m_fCraterScale[0] == 0.0f)
	{
		float fScale = pNode->GetPlanetaryMap()->GetRadius() * 0.5f;
		for(int i=0; i<50; i++)
		{
			m_fCraterScale[i] = fScale / (2.0f * powf(fScale, 0.25f));
			fScale *= 0.5f;
		}
	}

	for(int nIndex=0; nIndex<BORDER_MAP_COUNT; nIndex++)
	{
		float fHeight = builder.coord[nIndex].GetHeight();
		float x = builder.coord[nIndex].x;
		float y = builder.coord[nIndex].y;

		// Skip over the top levels of the crater tree, generating the appropriate seeds on the way
		int i, nSeed = m_nSeed + (pNode->GetFace()<<4);
		for(i=0; i<m_nTopLevel; i++)
			nSeed = CCraterMap::GetChildSeed(nSeed, x, y);

		// Now add all the applicable crater offsets down to the bottom of the tree
		CCraterMap node;
		while(i <= m_nBottomLevel)
		{
			node.Init(nSeed, m_nCratersPerLevel);
			fHeight += node.GetOffset(x, y, m_fCraterScale[i]);
			nSeed = CCraterMap::GetChildSeed(nSeed, x, y);
			i++;
		}

		builder.coord[nIndex].SetHeight(fHeight);
	}
}

void CSimpleColorMapFactory::BuildNode(CPlanetaryMapNode *pNode)
{
	PROFILE("CSimpleColorMapFactory::BuildNode", 3);
	// Initialize the color map
	builder.pb.Init(HEIGHT_MAP_WIDTH, HEIGHT_MAP_WIDTH, 1);
	unsigned char *pBuffer = (unsigned char *)builder.pb.GetBuffer();
	for(int y=0; y<HEIGHT_MAP_WIDTH; y++)
	{
		int nCoord = (y+1)*BORDER_MAP_WIDTH + 1;
		for(int x=0; x<HEIGHT_MAP_WIDTH; x++)
		{
			float fAltitude = builder.coord[nCoord++].GetHeight() - pNode->GetPlanetaryMap()->GetRadius();
			float fHeight = fAltitude / pNode->GetPlanetaryMap()->GetMaxHeight();
			fHeight = (m_vColors.size()-1) * CMath::Clamp(0.001f, 0.999f, (fHeight+1.0f) * 0.5f);
			int nHeight = (int)fHeight;
			float fRatio = fHeight - nHeight;
			CColor c = m_vColors[nHeight] * (1-fRatio) + m_vColors[nHeight+1] * fRatio;

			*pBuffer++ = c.r;
			*pBuffer++ = c.g;
			*pBuffer++ = c.b;
		}
	}
}
