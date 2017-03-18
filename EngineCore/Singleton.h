// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __Singleton_h__
#define __Singleton_h__


// This is a useful templatized base class for making a class into a singleton. The
// singleton can be created and destroyed dynamically, but there can only be one.
// Static methods will return a pointer or reference to the singleton.
template <class T>
class TSingleton
{
private:
	static T *m_pSingleton;

protected:
	TSingleton()
	{
		LogAssert(!m_pSingleton);
		m_pSingleton = (T *)this;	// Not sure about other compilers, but MSVC++ is smart enough to cast this correctly

		// Manual way to do the pointer arithmetic
		//int nOffset = (int)(T*)1 - (int)(TSingleton <T>*)(T*)1;
		//m_pSingleton = (T*)((int)this + nOffset);
	}
	~TSingleton()
	{
		LogAssert(m_pSingleton);
		m_pSingleton = NULL;
	}

public:
	static bool IsValid()	{ return m_pSingleton != NULL; }
	static T *GetPtr()
	{
		LogAssert(m_pSingleton);
		return m_pSingleton;
	}
	static T &GetRef()
	{
		LogAssert(m_pSingleton);
		return *m_pSingleton;
	}
};

// Because it's templatized, this static member doesn't need to be declared in a CPP file
template <class T> T *TSingleton<T>::m_pSingleton = NULL;


// This is an instance of the singleton class that creates itself if it doesn't already exist
template <class T>
class TAutoSingleton : public TSingleton<T>
{
public:
	static T *GetPtr()
	{
		if(!TSingleton<T>::IsValid())
			new T();
		return TSingleton<T>::GetPtr();
	}
	static T &GetRef()
	{
		if(!TSingleton<T>::IsValid())
			new T();
		return TSingleton<T>::GetRef();
	}
};

#endif // __Singleton_h__

