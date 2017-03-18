// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __CommonIncludes_h__
#define __CommonIncludes_h__


#define interface struct

// C Runtime headers
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>

// Windows headers
#ifdef _WIN32
//#pragma inline_depth( 255 )
//#pragma inline_recursion( on )
//#pragma auto_inline( on )
#pragma warning(disable:4786)

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <crtdbg.h>
#include <windows.h>
#include <mmsystem.h>
#endif

// C++ runtime headers
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

// libpng headers
extern "C" {
	#include <png.h>
};

// OpenGL/SDL headers
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>


#endif // __CommonIncludes_h__
