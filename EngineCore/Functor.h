// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __Functor_h__
#define __Functor_h__


class CFunctor : public CAutoRefCounter
{
public:
	typedef TReference<CFunctor> Ref;
	virtual void operator ()()=0;
	virtual bool operator==(void *p)=0;
};

template<class T>
class TFunctor : public CFunctor
{
protected:
	TReference<T> m_refObject;
	typedef void (T::*FunctionType)();
	FunctionType m_pFunction;

public:
	TFunctor(T *pObject, FunctionType pFunction)
	{
		m_refObject = pObject;
		m_pFunction = pFunction;
	}
	virtual void operator ()()			{ (m_refObject->*m_pFunction)(); }
	virtual bool operator==(void *p)	{ return p == (void *)((T *)m_refObject); }
};

template<class T>
class TPtrFunctor : public CFunctor
{
protected:
	T *m_pObject;
	typedef void (T::*FunctionType)();
	FunctionType m_pFunction;

public:
	TPtrFunctor(T *pObject, FunctionType pFunction)
	{
		m_pObject = pObject;
		m_pFunction = pFunction;
	}
	virtual void operator ()()			{ (m_pObject->*m_pFunction)(); }
	virtual bool operator==(void *p)	{ return p == (void *)m_pObject; }
};

#endif // __Functor_h__
