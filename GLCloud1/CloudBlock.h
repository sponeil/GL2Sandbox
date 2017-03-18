// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __CloudBlock_h__
#define __CloudBlock_h__

class CCloudCell
{
public:
	unsigned char m_cDensity, m_cBrightness;
};

class CCloudBlock : public CSRTTransform
{
protected:
	unsigned short nx, ny, nz;
	CCloudCell *m_pGrid;

public:
	CCloudBlock()
	{
		m_pGrid = NULL;
	}
	void Cleanup()
	{
		if(m_pGrid)
		{
			delete m_pGrid;
			m_pGrid = NULL;
		}
	}
	void Init(unsigned short x, unsigned short y, unsigned short z)
	{
		Cleanup();
		nx = x;
		ny = y;
		nz = z;
		m_pGrid = new CCloudCell[nx*ny*nz];
	}

	int GetCellPos(unsigned short x, unsigned short y, unsigned short z)
	{
		if(x >= nx || y >= ny || z >= nz)
			return -1;
		return ((z * ny + y) * nx) + x;
	}
	CCloudCell *GetCloudCell(unsigned short x, unsigned short y, unsigned short z)
	{
		int nPos = GetCellPos(x, y, z);
		if(nPos < 0)
			return NULL;
		return &m_pGrid[nPos];
	}

	void NoiseFill(int nSeed);
	void Light(CVector &vLight);
	void Draw(const CSRTTransform &camera, float fHalfSize);
};

#endif // __CloudBlock_h__
