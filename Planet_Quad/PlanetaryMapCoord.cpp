// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#include "StdAfx.h"
#include "PlanetaryMapCoord.h"


unsigned char GetFace(const CVector &v)
{
	unsigned char nFace;
	float x = CMath::Abs(v.x), y = CMath::Abs(v.y), z = CMath::Abs(v.z);
	if(x > y && x > z)
		nFace = (v.x > 0) ? RightFace : LeftFace;
	else if(y > x && y > z)
		nFace = (v.y > 0) ? TopFace : BottomFace;
	else
		nFace = (v.z > 0) ? FrontFace : BackFace;
	return nFace;
}

void GetFaceCoordinates(unsigned char nFace, const CVector &v, float &x, float &y)
{
	// The vector passed in may not be in the specified face.
	// If not, the coordinates within nFace closest to v are returned.
	// (This helps find the shortest distance from a point to a node in the quad-tree)
	float xABS = CMath::Abs(v.x), yABS = CMath::Abs(v.y), zABS = CMath::Abs(v.z);
	switch(nFace)
	{
		case RightFace:
			x = xABS <= zABS ? (v.z > 0 ? -1 : 1) : (-v.z / xABS);
			y = xABS <= yABS ? (v.y > 0 ? -1 : 1) : (-v.y / xABS);
			// If v is on the opposite face, we need to force the coordinates to the edge of this face.
			if(v.x < 0.0f && xABS > zABS && xABS > yABS)
			{
				if(CMath::Abs(x) > CMath::Abs(y))
					x = x > 0 ? 1 : -1;
				else
					y = y > 0 ? 1 : -1;
			}
			break;
		case LeftFace:
			x = xABS <= zABS ? (v.z > 0.0f ? 1.0f : -1.0f) : (v.z / xABS);
			y = xABS <= yABS ? (v.y > 0.0f ? -1.0f : 1.0f) : (-v.y / xABS);
			// If v is on the opposite face, we need to force the coordinates to the edge of this face.
			if(v.x > 0.0f && xABS > zABS && xABS > yABS)
			{
				if(CMath::Abs(x) > CMath::Abs(y))
					x = x > 0 ? 1 : -1;
				else
					y = y > 0 ? 1 : -1;
			}
			break;
		case TopFace:
			x = yABS <= xABS ? (v.x > 0.0f ? 1.0f : -1.0f) : (v.x / yABS);
			y = yABS <= zABS ? (v.z > 0.0f ? 1.0f : -1.0f) : (v.z / yABS);
			// If v is on the opposite face, we need to force the coordinates to the edge of this face.
			if(v.y < 0.0f && yABS > xABS && yABS > zABS)
			{
				if(CMath::Abs(x) > CMath::Abs(y))
					x = x > 0 ? 1 : -1;
				else
					y = y > 0 ? 1 : -1;
			}
			break;
		case BottomFace:
			x = yABS <= xABS ? (v.x > 0.0f ? 1.0f : -1.0f) : (v.x / yABS);
			y = yABS <= zABS ? (v.z > 0.0f ? -1.0f : 1.0f) : (-v.z / yABS);
			// If v is on the opposite face, we need to force the coordinates to the edge of this face.
			if(v.y > 0.0f && yABS > xABS && yABS > zABS)
			{
				if(CMath::Abs(x) > CMath::Abs(y))
					x = x > 0 ? 1 : -1;
				else
					y = y > 0 ? 1 : -1;
			}
			break;
		case FrontFace:
			x = zABS <= xABS ? (v.x > 0.0f ? 1.0f : -1.0f) : (v.x / zABS);
			y = zABS <= yABS ? (v.y > 0.0f ? -1.0f : 1.0f) : (-v.y / zABS);
			// If v is on the opposite face, we need to force the coordinates to the edge of this face.
			if(v.z < 0.0f && zABS > xABS && zABS > yABS)
			{
				if(CMath::Abs(x) > CMath::Abs(y))
					x = x > 0 ? 1 : -1;
				else
					y = y > 0 ? 1 : -1;
			}
			break;
		case BackFace:
			x = zABS <= xABS ? (v.x > 0.0f ? -1.0f : 1.0f) : (-v.x / zABS);
			y = zABS <= yABS ? (v.y > 0.0f ? -1.0f : 1.0f) : (-v.y / zABS);
			// If v is on the opposite face, we need to force the coordinates to the edge of this face.
			if(v.z > 0.0f && zABS > xABS && zABS > yABS)
			{
				if(CMath::Abs(x) > CMath::Abs(y))
					x = x > 0 ? 1 : -1;
				else
					y = y > 0 ? 1 : -1;
			}
			break;
	}

	// x and y should be approximately from -1 to 1, scale and clamp to range from 0 to 1
	x = CMath::Max(0.0f, CMath::Min(1.0f, (x + 1.0f) * 0.5f));
	y = CMath::Max(0.0f, CMath::Min(1.0f, (y + 1.0f) * 0.5f));
}

CVector GetPlanetaryVector(unsigned char nFace, float x, float y, float fLength)
{
	CVector v;
	float fx = (x * 2.0f) - 1.0f;
	float fy = (y * 2.0f) - 1.0f;
	switch(nFace)
	{
		case RightFace:
			v.x = 1;
			v.y = -fy;
			v.z = -fx;
			break;
		case LeftFace:
			v.x = -1;
			v.y = -fy;
			v.z = fx;
			break;
		case TopFace:
			v.x = fx;
			v.y = 1;
			v.z = fy;
			break;
		case BottomFace:
			v.x = fx;
			v.y = -1;
			v.z = -fy;
			break;
		case FrontFace:
			v.x = fx;
			v.y = -fy;
			v.z = 1;
			break;
		case BackFace:
			v.x = -fx;
			v.y = -fy;
			v.z = -1;
			break;
	}

	float fScale = fLength / v.Magnitude();
	return v * fScale;
}
