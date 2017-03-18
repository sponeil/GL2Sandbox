// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __EngineCore_h__
#define __EngineCore_h__


// Fixed includes (third-party, almost never change)
#include "CommonIncludes.h"

// Some common constants and inline functions
#include "CommonDefines.h"

// Utility headers
#include "DateTime.h"
#include "Log.h"
#include "Singleton.h"
#include "Reference.h"
#include "PropertySet.h"
#include "Functor.h"
#include "Profiler.h"
#include "Factory.h"
#include "Object.h"
#include "Allocator.h"


// Math headers
#include "Noise.h"
#include "Matrix.h"
#include "Geometry.h"

// Miscellaneous headers
#include "PixelBuffer.h"
#include "Texture.h"
#include "Font.h"

// Kernel headers
#include "Kernel.h"
#include "TimerTask.h"
#include "Trigger.h"
#include "Interpolator.h"
#include "InputTask.h"
#include "VideoTask.h"
#include "CameraTask.h"
#include "ConsoleTask.h"
#include "CaptureTask.h"

// Miscellaneous headers
#include "GLBufferObject.h"
#include "GLShaderObject.h"
#include "GLFrameBufferObject.h"


extern const std::string g_strBuildStamp;

#define DECLARE_CORE_GLOBALS\
	const std::string g_strBuildStamp = std::string(__DATE__) + std::string(" ") + std::string(__TIME__);\
	CLog *CLog::m_pSingleton = NULL;\
	CObjectType CObjectType::m_root


#endif // __EngineCore_h__
