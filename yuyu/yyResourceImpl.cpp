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

yyResourceImplementation* yyResourceImpl::GetImplementation() {
	return m_implementation;
}

void yyResourceImpl::GetTextureSize(v2f* ptr){ 
	m_implementation->GetTextureSize(ptr);
}
void yyResourceImpl::GetTextureHandle(void** ptr) {
	m_implementation->GetTextureHandle(ptr);
}
void yyResourceImpl::MapModelForWriteVerts(u8** v_ptr){
	m_implementation->MapModelForWriteVerts(v_ptr);
}
void yyResourceImpl::UnmapModelForWriteVerts() {
	m_implementation->UnmapModelForWriteVerts();
}

void yyResourceImpl::Load() {
	assert(m_implementation);
	++m_refCount;

	if (m_refCount == 1)
	{
		bool isSource = m_resourceData.m_source != nullptr;
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
				break;
			case yyResourceType::RenderTargetTexture:
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

u32 yyResourceImpl::GetRefCount() {
	return m_refCount;
}

bool yyResourceImpl::IsLoaded() {
	return m_refCount != 0;
}

void yyResourceImpl::InitTextureRenderTargetResourse(const v2f& size) {
	m_type = yyResourceType::RenderTargetTexture;
	m_resourceData.m_type = m_type;
	m_resourceData.m_imageData.m_size[0] = size.x;
	m_resourceData.m_imageData.m_size[1] = size.y;
	m_resourceData.m_imageData.m_anisotropicLevel = g_engine->m_textureAnisotropicLevel;
	m_resourceData.m_imageData.m_filter = g_engine->m_textureFilter;
	m_resourceData.m_imageData.m_addressMode = g_engine->m_textureAddressMode;
	m_resourceData.m_imageData.m_comparisonFunc = g_engine->m_textureComparisonFunc;
	if (!m_implementation)
		m_implementation = g_engine->m_videoAPI->CreateTextureImplementation();
}

void yyResourceImpl::InitTextureResourse(yyImage* img, const char* fileName){
	m_type = yyResourceType::Texture;
	m_resourceData.m_type = m_type;
	m_resourceData.m_path = fileName;
	m_resourceData.m_source = img;

	m_resourceData.m_imageData.m_anisotropicLevel = g_engine->m_textureAnisotropicLevel;
	m_resourceData.m_imageData.m_filter = g_engine->m_textureFilter;
	m_resourceData.m_imageData.m_addressMode = g_engine->m_textureAddressMode;
	m_resourceData.m_imageData.m_comparisonFunc = g_engine->m_textureComparisonFunc;
	if(!m_implementation)
		m_implementation = g_engine->m_videoAPI->CreateTextureImplementation();
}

void yyResourceImpl::InitModelResourse(yyModel* m) {
	m_type = yyResourceType::Model;
	m_resourceData.m_type = m_type;
	m_resourceData.m_source = m;

	if (!m_implementation)
		m_implementation = g_engine->m_videoAPI->CreateModelImplementation();
}