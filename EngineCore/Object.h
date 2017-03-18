// Originally copyrighted by Sean O'Neil (sean.p.oneil@gmail.com) in 2000,
// who is now releasing it into the public domain.
// For more information, please refer to <http://unlicense.org>.

#ifndef __Object_h__
#define __Object_h__


///////////////////////////////////////////////////////////////////////////////
// Class: CObjectType
///////////////////////////////////////////////////////////////////////////////
// CObjectType is the root of a tree of RTTI type classes. Class ID's are
// determined from the class name using a string hash function. Although this
// class is a singleton, it's not derived from TSingleton because many classes
// derived from it will also be derived from TSingleton.
///////////////////////////////////////////////////////////////////////////////
class CObjectBase;
class CObjectType : public CManualRefCounter
{
public:
	typedef TReference<CObjectType> Ref;
	typedef std::list<CObjectType *>::iterator iterator;
	typedef std::list<CObjectType *>::const_iterator const_iterator;

protected:
	static CObjectType m_root;					// The root of the object type tree

	unsigned int m_nTypeID;						// A numeric ID of this type of object
	std::string m_strTypeName;					// A descriptive name of this type of object
	CObjectType::Ref m_refParentType;			// The parent type that this type is derived from
	std::list<CObjectType *> m_lDerivedTypes;	// A list of types derived directly from this type

	// Use a simple hashing function to generate the Type ID
	static unsigned int GenerateTypeID(const char *psz)
	{
		unsigned int nHash = 0;
		while(*psz)
		{
			char c = *psz++;
			if(c >= 'A' && c <= 'Z')
				c -= 0x20;
			nHash = ((nHash<<3) + (nHash>>28) + c);
		}
		return nHash;
	}

public:
	CObjectType()
	{
		m_strTypeName = "CObjectBase";
		m_nTypeID = 0;
	}

	CObjectType(CObjectType *pParent, const char *pszTypeName)
	{
		LogAssert(pParent);
		m_refParentType = pParent;
		m_strTypeName = pszTypeName;
		m_nTypeID = GenerateTypeID(m_strTypeName.c_str());
		LogDebug("Creating object type %s (TypeID=%u).", m_strTypeName.c_str(), m_nTypeID);
		CObjectType *pFind = FindType(m_nTypeID);
		if(pFind)
			LogError("Object type %s (TypeID=%u) and object type %s (ID=%u) have conflicting type ID's", m_strTypeName.c_str(), m_nTypeID, pFind->GetTypeName(), pFind->GetTypeID());
		if(m_refParentType)
			m_refParentType->m_lDerivedTypes.push_back(this);
	}

	virtual ~CObjectType()
	{
		while(!m_lDerivedTypes.empty())
			delete m_lDerivedTypes.front();

		if(m_nTypeID)
		{
			LogDebug("Destroying object type %s (TypeID=%u).", m_strTypeName.c_str(), m_nTypeID);
			LogAssert(m_nReferenceCount == 0);
			for(iterator it = m_refParentType->m_lDerivedTypes.begin(); it != m_refParentType->m_lDerivedTypes.end(); it++)
			{
				if(*it == this)
				{
					m_refParentType->m_lDerivedTypes.erase(it);
					break;
				}
			}
		}
	}

	void AddRef()
	{
		LogSpam("Creating object %s (TypeID=%u).", m_strTypeName.c_str(), m_nTypeID);
		CManualRefCounter::AddRef();
	}

	void Release()
	{
		LogSpam("Destroying object %s (TypeID=%u).", m_strTypeName.c_str(), m_nTypeID);
		CManualRefCounter::Release();
	}

	static CObjectType *GetInstance()						{ return &m_root; }
	static CObjectType *FindType(unsigned int nTypeID)		{ return m_root.FindDerivedType(nTypeID); }
	static CObjectType *FindType(const char *pszTypeName)	{ return FindType(GenerateTypeID(pszTypeName)); }

	CObjectType *FindDerivedType(unsigned int nTypeID) const
	{
		if(nTypeID == m_nTypeID)
			return (CObjectType *)this;

		CObjectType *pType = NULL;
		for(const_iterator it = m_lDerivedTypes.begin(); !pType && it != m_lDerivedTypes.end(); it++)
			pType = (*it)->FindDerivedType(nTypeID);
		return pType;
	}

	CObjectType *FindParentType(unsigned int nTypeID) const
	{
		CObjectType *pType = (CObjectType *)this;
		while(pType)
		{
			if(nTypeID == pType->m_nTypeID)
				return pType;
			pType = pType->m_refParentType;
		}
		return NULL;
	}

	bool IsA(unsigned int nTypeID) const		{ return FindParentType(nTypeID) != NULL; }
	bool IsA(const char *pszTypeName) const		{ return IsA(GenerateTypeID(pszTypeName)); }
	bool IsA(CObjectType *pType) const			{ return IsA(pType->GetTypeID()); }

	unsigned int GetTypeID() const				{ return m_nTypeID; }
	const char *GetTypeName() const				{ return m_strTypeName.c_str(); }
	CObjectType *GetParentType() const			{ return m_refParentType; }
	const_iterator begin() const				{ return m_lDerivedTypes.begin(); }
	const_iterator end() const					{ return m_lDerivedTypes.end(); }

	virtual CObjectBase *CreateObject()			{ return NULL; }
	static CObjectBase *CreateObject(unsigned int nTypeID)
	{
		CObjectType *pType = m_root.FindDerivedType(nTypeID);
		if(!pType)
			return NULL;
		return pType->CreateObject();
	}
	static CObjectBase *CreateObject(const char *pszTypeName){ return CreateObject(GenerateTypeID(pszTypeName)); }
};

///////////////////////////////////////////////////////////////////////////////
// Template: TObjectType
///////////////////////////////////////////////////////////////////////////////
// TObjectType, along with the macro below, provides a quick way to create a
// singleton type class. If you want to add functionality to the type class,
// derive a custom class from TObjectType. Otherwise, simply use the macro to
// create the derived class for you.
///////////////////////////////////////////////////////////////////////////////
template <class ObjectType, class ParentType>
class TObjectType : public CObjectType, public TAutoSingleton<ObjectType>
{
public:
	TObjectType(const char *pszObjectName) : CObjectType(ParentType::GetInstance(), pszObjectName) {}
};

#define DECLARE_GENERIC_TYPE_CLASS(ObjectClass, ParentTypeClass) \
class ObjectClass##Type : public TObjectType<ObjectClass##Type, ParentTypeClass> \
{\
public:\
	ObjectClass##Type() : TObjectType<ObjectClass##Type, ParentTypeClass>(#ObjectClass) {}\
	static CObjectType *GetInstance()	{ return GetPtr(); }\
	static void Load()					{ new ObjectClass##Type(); }\
	static void Unload()				{ delete ObjectClass##Type::GetPtr(); }\
	virtual CObjectBase *CreateObject()	{ return new ObjectClass(this); }\
}



///////////////////////////////////////////////////////////////////////////////
// Class: CObjectBase
///////////////////////////////////////////////////////////////////////////////
// CObjectBase is the root of the RTTI class tree using CObjectType. On top of
// its virtual function table pointer, this class has 6 bytes of overhead per
// instance for its reference counter and its reference to a CObjectType.
///////////////////////////////////////////////////////////////////////////////
class CObjectBase : public CAutoRefCounter
{
protected:
	const CObjectType::Ref m_refType;

protected:
	CObjectBase(CObjectType *pType=CObjectType::GetInstance()) : m_refType(pType)	{}
	virtual ~CObjectBase() {}

public:
	typedef TReference<CObjectBase> Ref;

	CObjectType *GetType() const					{ return m_refType; }
	bool IsA(unsigned int nTypeID) const			{ return m_refType->IsA(nTypeID); }
	bool IsA(const char *pszTypeName) const			{ return m_refType->IsA(pszTypeName); }
	bool IsA(CObjectType *pType) const				{ return m_refType->IsA(pType); }
	unsigned int GetTypeID() const					{ return m_refType->GetTypeID(); }
	const char *GetTypeName() const					{ return m_refType->GetTypeName(); }
};


#endif // __Object_h__
