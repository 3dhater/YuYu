#include "yy.h"
#include "yyResourceImpl.h"

#include "engine.h"
extern Engine * g_engine;

yyResourceImpl::yyResourceImpl() {
	m_type = yyResourceType::None;
	m_refCount = 0;
	m_implementation = 0;
	m_flags = 0;
}

yyResourceImpl::~yyResourceImpl() {
	if (m_implementation)
	{
		if(m_refCount)
			m_implementation->Unload();
		yyDestroy(m_implementation);
	}
}

yyResourceType yyResourceImpl::GetType() {
	return m_type;
}

void yyResourceImpl::Load() {
	assert(m_implementation);
	++m_refCount;

	if (m_refCount == 1)
	{
		bool isSource = m_resourceData.m_source;
		if (!isSource)
		{
			switch (m_resourceData.m_type)
			{
			default:
				YY_DEBUGBREAK;
				break;
			case yyResourceType::Texture:
				m_resourceData.m_source = yyLoadImage(m_resourceData.m_path.data());
				break;
			case yyResourceType::Model:
				YY_DEBUGBREAK;
				break;
			}
		}
		
		assert(m_resourceData.m_source);

		m_implementation->Load(&m_resourceData);

		if (!isSource)
		{
			yyDestroy(m_resourceData.m_source);
			m_resourceData.m_source = 0;
		}
	}
}

void yyResourceImpl::Unload() {
	assert(m_implementation);
	if (m_refCount == 0)
	{
		yyLogWriteWarning("%s - m_refCount is already 0\n", YY_FUNCTION);
		return;
	}

	--m_refCount;

	if (m_refCount == 0)
	{
		m_implementation->Unload();
	}
}

s32 yyResourceImpl::GetRefCount() {
	return m_refCount;
}

void yyResourceImpl::InitTextureResourse(yyImage* img, const char* fileName){
	m_type = yyResourceType::Texture;
	m_resourceData.m_type = m_type;
	m_resourceData.m_path = fileName;
	m_resourceData.m_source = img;

	m_resourceData.m_imageData.m_filter = g_engine->m_textureFilter;
	if(!m_implementation)
		m_implementation = g_engine->m_videoAPI->CreateTextureImplementation();
}

void yyResourceImpl::InitModelResourse(yyModel* m) {
	m_type = yyResourceType::Model;
	m_resourceData.m_type = m_type;
	m_resourceData.m_source = m;

	if (!m_implementation)
		m_implementation = g_engine->m_videoAPI->CreateTextureImplementation();
}