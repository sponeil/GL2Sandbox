// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __Reference_h__
#define __Reference_h__


template <class T>
class TReference
{
protected:
	T *m_pObject;

public:
	// Constructor/destructor methods
	TReference(T *p=NULL)
	{
		if(m_pObject = p)
			m_pObject->AddRef();
	}
	TReference(const TReference &ref)
	{
		if(m_pObject = ref.m_pObject)
			m_pObject->AddRef();
	}
	~TReference()
	{
		if(m_pObject)
			m_pObject->Release();
	}

	// Manual manipulation methods
	void AddRef()			{ m_pObject->AddRef(); }
	void Release()			{ m_pObject->Release(); m_pObject = NULL; }

	// Implicit conversion operators
	operator T*() const
	{
		return m_pObject;
	}
	T& operator*() const
	{
		LogAssert(m_pObject);
		return *m_pObject;
	}
	T* operator->() const
	{
		LogAssert(m_pObject);
		return m_pObject;
	}

	// Assignment operators
	TReference &operator=(const TReference &ref)
	{
		if(m_pObject != ref.m_pObject)
		{
			if(m_pObject)
				m_pObject->Release();
			if(m_pObject = ref.m_pObject)
				m_pObject->AddRef();
		}
		return *this;
	}
	TReference &operator=(T* p)
	{
		if(m_pObject != p)
		{
			if(m_pObject)
				m_pObject->Release();
			if(m_pObject = p)
				m_pObject->AddRef();
		}
		return *this;
	}

	// Comparison operators (compare addresses)
	bool operator==(T* p) const						{ return m_pObject == p; }
	bool operator!=(T* p) const						{ return m_pObject != p; }
	bool operator==(const TReference &ref) const	{ return m_pObject == ref.m_pObject; }
	bool operator!=(const TReference &ref) const	{ return m_pObject != ref.m_pObject; }

	// Comparison methods (compare values, relies on operator overloading in class being referenced)
	bool eq(const TReference &ref) const			{ return (*m_pObject) == (*ref.m_pObject); }
	bool ne(const TReference &ref) const			{ return (*m_pObject) != (*ref.m_pObject); }
	bool gt(const TReference &ref) const			{ return (*m_pObject) > (*ref.m_pObject); }
	bool lt(const TReference &ref) const			{ return (*m_pObject) < (*ref.m_pObject); }
	bool ge(const TReference &ref) const			{ return (*m_pObject) >= (*ref.m_pObject); }
	bool le(const TReference &ref) const			{ return (*m_pObject) <= (*ref.m_pObject); }
};

// A simple reference counting base class. It is called manual because it does not perform
// automatic cleanup of the object. It has no virtual methods and all methods are inline.
// If performance is critical, you can override the Release() method and keep the cleanup
// code entirely inline. (Warning: As with pointers, if you're keeping a reference to a
// base class of an object, the derived class's cleanup code won't be called unless it
// is properly casted or relies on virtual methods.)
class CManualRefCounter
{
protected:
	unsigned short m_nReferenceCount;
	CManualRefCounter()			{ m_nReferenceCount = 0; }
	~CManualRefCounter()		{ LogAssert(m_nReferenceCount == 0); }

public:
	unsigned short AddRef()
	{
		LogAssert(GetReferenceCount() < 0x7FFF);
		m_nReferenceCount++;
		//m_nReferenceCount = InterlockedIncrement((LPLONG)&m_nReferenceCount);
		return GetReferenceCount();
	}
	unsigned short Release()
	{
		LogAssert(GetReferenceCount() > 0);
		m_nReferenceCount--;
		//m_nReferenceCount = InterlockedDecrement((LPLONG)&m_nReferenceCount);
		return GetReferenceCount();
	}
	unsigned short GetReferenceCount()	{ return (m_nReferenceCount & 0x7FFF); }
	bool IsDead()						{ return (m_nReferenceCount & 0x8000) == 0; }
	void Kill()							{ m_nReferenceCount |= 0x8000; }
};

// This class automatically deletes the object when its reference counter reaches 0.
// Because the object's class is unknown, it relies on virtual destructors. Use this when
// you want the object to be deleted when its reference count reaches 0 and you're not
// concerned about performance (or you're keeping references to a base class).
class CAutoRefCounter : public CManualRefCounter
{
protected:
	virtual ~CAutoRefCounter()		{}
public:
	void Release()					{ if(!CManualRefCounter::Release()) delete this; }
};

// This templatized version of CAutoRefCounter can execute the correct destructor without
// it being virtual. Unfortunately, this can't be used for any class that has other classes
// derive from it.
template <class T>
class TAutoRefCounter : public CManualRefCounter
{
public:
	void Release()					{ if(!CManualRefCounter::Release()) delete (T *)this; }
	typedef TReference<T> Ref;
};

// If you want automatic cleanup but do not want an object to be deleted when the reference
// counter reaches 0 (i.e. when it is part of an object pool), derive from this class and
// override the pure virtual Destroy() method.
class CAbstractRefCounter : public CManualRefCounter
{
protected:
	virtual void Destroy() = 0;
	virtual ~CAbstractRefCounter()	{}
public:
	void Release()					{ if(!CManualRefCounter::Release()) Destroy(); }
};

#endif // __Reference_h__
