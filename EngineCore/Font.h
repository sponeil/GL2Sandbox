// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __Font_h__
#define __Font_h__

/*******************************************************************************
* Class: CFont
********************************************************************************
* This is just a quick and dirty class for using wglUseFontBitmaps. I don't like
* using it because it's slow, but for now that's all I have to use for fonts.
* It would be nice to test drawing to a normal Windows bitmap using GDI commands,
* then blitting the text to OpenGL somehow.
*******************************************************************************/
class CFont
{
protected:
	int m_nListBase;
	float m_fXPos;
	float m_fYPos;

public:
	CFont()
	{
		m_nListBase = -1;
		m_fXPos = 0;
		m_fYPos = 0;
	}
	~CFont()					{ Cleanup(); }
	void Init()
	{
		Cleanup();
		m_nListBase = glGenLists(256);
#ifdef _WIN32
		wglUseFontBitmaps(::wglGetCurrentDC(), 0, 255, m_nListBase);
#endif
	}
	void Cleanup()
	{
		if(m_nListBase != -1)
		{
			glDeleteLists(m_nListBase, 256);
			m_nListBase = -1;
		}
	}
	void SetPosition(int x, int y)
	{
		m_fXPos = (float)x;
		m_fYPos = (float)y;
	}
	void Begin()
	{
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		int nViewport[4];
		glGetIntegerv(GL_VIEWPORT, nViewport);
		glOrtho(0, nViewport[2], nViewport[3], 0, -1, 1);
	}
	void Print(const char *pszMessage)
	{
		glRasterPos2f(m_fXPos, m_fYPos+11);
		glListBase(m_nListBase);
		glCallLists(strlen(pszMessage), GL_UNSIGNED_BYTE, pszMessage);
	}
	void End()
	{
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
};

#endif // __Font_h__

