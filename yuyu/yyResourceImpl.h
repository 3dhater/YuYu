#ifndef _YY_RESIMPL_H_
#define _YY_RESIMPL_H_

class yyResourceImpl : public yyResource
{
public:
	yyResourceImpl();
	virtual ~yyResourceImpl();

	virtual yyResourceType GetType();
	virtual void Load();
	virtual void Unload();
	virtual s32 GetRefCount();

	void InitTextureResourse(yyImage*, const char* fileName);
	void InitModelResourse(yyModel*);

	yyResourceType m_type;
	u32 m_refCount;
	u32 m_flags;
	yyResourceImplementation* m_implementation;
	yyResourceData m_resourceData;
};

#endif