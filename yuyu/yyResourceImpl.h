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
	virtual u32 GetRefCount();
	virtual bool IsLoaded();

	virtual yyResourceImplementation* GetImplementation();

	virtual void GetTextureSize(v2f*);
	virtual void GetTextureHandle(void**);
	virtual void MapModelForWriteVerts(u8** v_ptr);
	virtual void UnmapModelForWriteVerts();

	void InitTextureResourse(yyImage*, const char* fileName);
	void InitTextureRenderTargetResourse(const v2f& size);
	void InitModelResourse(yyModel*);

	yyResourceType m_type;
	u32 m_refCount;
	u32 m_flags;
	yyResourceImplementation* m_implementation;
	yyResourceData m_resourceData;
};

#endif