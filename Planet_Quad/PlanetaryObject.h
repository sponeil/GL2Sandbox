// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __PlanetaryObect_h__
#define __PlanetaryObect_h__


class CPlanetaryMapNode; // Forward declaration


class CPlanetaryObject : public CObjectBase
{
public:
	typedef TReference<CPlanetaryObject> Ref;
	CPlanetaryObject(CObjectType *pType) : CObjectBase(pType) {}

	virtual void Init(CPropertySet &prop) {}
	virtual void Update() {}
	virtual void Draw() {}
};
DECLARE_GENERIC_TYPE_CLASS(CPlanetaryObject, CObjectType);

class CPlanetaryFactory : public CPlanetaryObject
{
public:
	typedef TReference<CPlanetaryFactory> Ref;
	CPlanetaryFactory(CObjectType *pType) : CPlanetaryObject(pType) {}
	virtual bool AffectsNode(CPlanetaryMapNode *pNode) { return true; }
	virtual void BuildNode(CPlanetaryMapNode *pNode) {}
	virtual void DestroyNode(CPlanetaryMapNode *pNode) {}
};
DECLARE_GENERIC_TYPE_CLASS(CPlanetaryFactory, CPlanetaryObjectType);

class CSimpleHeightMapFactory : public CPlanetaryFactory, public CFractal
{
protected:
	// Members for fractal routine
	int m_nFractalSeed;
	float m_fOctaves;
	float m_fRoughness;

public:
	typedef TReference<CSimpleHeightMapFactory> Ref;
	CSimpleHeightMapFactory(CObjectType *pType) : CPlanetaryFactory(pType)
	{
		m_nFractalSeed = 0;
	}

	virtual void Init(CPropertySet &prop)
	{
		m_nFractalSeed = prop.GetIntProperty("seed");
		m_fOctaves = prop.GetFloatProperty("octaves");
		m_fRoughness = prop.GetFloatProperty("roughness");
		CFractal::Init(3, m_nFractalSeed, 1.0f - m_fRoughness, 2.0f);
	}
	virtual void BuildNode(CPlanetaryMapNode *pNode);
};
DECLARE_GENERIC_TYPE_CLASS(CSimpleHeightMapFactory, CPlanetaryFactoryType);

class CSimpleCraterFactory : public CPlanetaryFactory, public CFractal
{
protected:
	int m_nSeed;
	int m_nTopLevel;
	int m_nBottomLevel;
	int m_nCratersPerLevel;
	float m_fCraterScale[50];

public:
	typedef TReference<CSimpleCraterFactory> Ref;
	CSimpleCraterFactory(CObjectType *pType) : CPlanetaryFactory(pType)
	{
		m_nSeed = 0;
	}

	virtual void Init(CPropertySet &prop)
	{
		m_nSeed = prop.GetIntProperty("seed");
		m_nTopLevel = prop.GetIntProperty("level.top");
		m_nBottomLevel = prop.GetIntProperty("level.bottom");
		m_nCratersPerLevel = prop.GetIntProperty("craters.per.level");
		m_fCraterScale[0] = 0.0f;
	}
	virtual void BuildNode(CPlanetaryMapNode *pNode);
};
DECLARE_GENERIC_TYPE_CLASS(CSimpleCraterFactory, CPlanetaryFactoryType);

class CSimpleColorMapFactory : public CPlanetaryFactory
{
protected:
	std::vector<CColor> m_vColors;

public:
	typedef TReference<CSimpleColorMapFactory> Ref;
	CSimpleColorMapFactory(CObjectType *pType) : CPlanetaryFactory(pType)
	{
	}

	virtual void Init(CPropertySet &prop)
	{
		for(int i=1; ; i++)
		{
			char szProperty[256];
			sprintf(szProperty, "color.%d", i);
			const char *pszColor = prop.GetProperty(szProperty);
			if(!pszColor)
				break;
			int n[4];
			sscanf(pszColor, "%d, %d, %d, %d", &n[0], &n[1], &n[2], &n[3]);
			m_vColors.push_back(CColor(n[0], n[1], n[2], n[3]));
		}
	}
	virtual void BuildNode(CPlanetaryMapNode *pNode);
};
DECLARE_GENERIC_TYPE_CLASS(CSimpleColorMapFactory, CPlanetaryFactoryType);


class CCrater
{
public:
	float x, y, m_fRadius;

	CCrater()
	{
	}
	void Init()
	{
		x = ((float)rand()/(float)RAND_MAX)*0.5f + 0.25f;
		y = ((float)rand()/(float)RAND_MAX)*0.5f + 0.25f;
		m_fRadius = ((float)rand()/(float)RAND_MAX)*0.125f + 0.125f;
	}
};

class CCraterMap
{
protected:
	int m_nSeed;
	int m_nCraters;
	CCrater m_pCrater[20];

public:
	// To go to each child, add a different prime number to minimize the chances of the same seed coming up
	enum {TopLeft=11, TopRight=101, BottomLeft=1009, BottomRight=10007};
	CCraterMap()							{}
	~CCraterMap()							{}

	void Init(int nSeed, int nCraters, bool bCreate=true)
	{
		int i;
		m_nSeed = nSeed;
		srand(m_nSeed);
		m_nCraters = nCraters;
		for(i=0; i<nCraters; i++)
		{
			if((float)rand()/(float)RAND_MAX < 0.5f)
				m_nCraters--;
		}
		for(i=0; i<m_nCraters; i++)
			m_pCrater[i].Init();
	}

	static int GetChildSeed(int nSeed, float &x, float &y)
	{
		nSeed = nSeed * 100;
		if(x < 0.5f)
		{
			if(y < 0.5f)
				nSeed += TopLeft;
			else
			{
				y -= 0.5f;
				nSeed += BottomLeft;
			}
		}
		else
		{
			if(y < 0.5f)
			{
				x -= 0.5f;
				nSeed += TopRight;
			}
			else
			{
				x -= 0.5f;
				y -= 0.5f;
				nSeed += BottomRight;
			}
		}
		x *= 2.0f;
		y *= 2.0f;
		return nSeed;
	}
	
	float GetDistance(int i, float x, float y)
	{
		float dx = x-m_pCrater[i].x;
		float dy = y-m_pCrater[i].y;
		dx *= dx;
		dy *= dy;
		return dx + dy;
	}
	float GetOffset(float x, float y, float fScale)
	{
		float fOffset = 0;
		for(int i=0; i<m_nCraters; i++)
		{
			float fDist = GetDistance(i, x, y);
			float fRadius = m_pCrater[i].m_fRadius * m_pCrater[i].m_fRadius;
			if(fDist < fRadius)
			{
				fDist /= fRadius;
				float fDist2 = fDist*fDist;
				fOffset += ((fDist2 - 0.25f) * 2.0f * m_pCrater[i].m_fRadius) * (1-fDist);
			}
		}
		return fOffset * fScale;
	}
};


#endif // __PlanetaryObect_h__
