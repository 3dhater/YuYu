#include "yy.h"
#include "yy_image.h"
#include "yyResourceImpl.h"

#include "engine.h"
extern Engine * g_engine;

yyResourceImpl::yyResourceImpl() {
	YY_DEBUG_PRINT_FUNC;
	m_isSource = false;
	m_type = yyResourceType::None;
	m_refCount = 0;
	m_implementation = 0;
	m_flags = 0;
}

yyResourceImpl::~yyResourceImpl() {
	YY_DEBUG_PRINT_FUNC;
	if (m_implementation)
	{
		if(m_refCount)
			m_implementation->Unload();
		yyDestroy(m_implementation);
	}
}

bool yyResourceImpl::IsFromCache() {
	return ((m_flags & this->flag_fromCache) == this->flag_fromCache);
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

void yyResourceImpl::LoadSource() {
	m_isSource = m_resourceData.m_source != nullptr;
	if (!m_isSource)
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
}
void yyResourceImpl::LoadImplementation() {
	m_implementation->Load(&m_resourceData);
}

void yyResourceImpl::DestroySource() {
	switch (m_resourceData.m_type)
	{
	default:
		YY_DEBUGBREAK;
		break;
	case yyResourceType::Texture:
	{
		yyDestroy((yyImage*)m_resourceData.m_source);
	}break;
	case yyResourceType::Model:
		break;
	case yyResourceType::RenderTargetTexture:
		break;
	}
	m_resourceData.m_source = 0;
}

void yyResourceImpl::Load() {
	assert(m_implementation);
	++m_refCount;

	if (m_refCount == 1)
	{
		LoadSource();
		LoadImplementation();
		DestroySource();
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
	m_resourceData.m_imageData = yyCreate<yyResourceDataImage>();
	m_resourceData.m_imageData->m_size[0] = size.x;
	m_resourceData.m_imageData->m_size[1] = size.y;
	m_resourceData.m_imageData->m_anisotropicLevel = g_engine->m_textureAnisotropicLevel;
	m_resourceData.m_imageData->m_filter = g_engine->m_textureFilter;
	m_resourceData.m_imageData->m_addressMode = g_engine->m_textureAddressMode;
	m_resourceData.m_imageData->m_comparisonFunc = g_engine->m_textureComparisonFunc;
	if (!m_implementation)
		m_implementation = g_engine->m_videoAPI->CreateTextureImplementation();
}

void yyResourceImpl::InitTextureResourse(yyImage* img, const char* fileName){
	m_type = yyResourceType::Texture;
	m_resourceData.m_type = m_type;
	m_resourceData.m_path = fileName;
	m_resourceData.m_source = img;

	//yyImage img;
	//yyLoadImageAsync

	m_resourceData.m_imageData = yyCreate<yyResourceDataImage>();
	m_resourceData.m_imageData->m_anisotropicLevel = g_engine->m_textureAnisotropicLevel;
	m_resourceData.m_imageData->m_filter = g_engine->m_textureFilter;
	m_resourceData.m_imageData->m_addressMode = g_engine->m_textureAddressMode;
	m_resourceData.m_imageData->m_comparisonFunc = g_engine->m_textureComparisonFunc;
	if(!m_implementation)
		m_implementation = g_engine->m_videoAPI->CreateTextureImplementation();
}

void yyResourceImpl::InitModelResourse(yyModel* m) {
	m_type = yyResourceType::Model;
	m_resourceData.m_type = m_type;
	m_resourceData.m_source = m;
	m_resourceData.m_modelData = yyCreate<yyResourceDataModel>();
	m_resourceData.m_modelData->m_material = yyMegaAllocator::CreateMaterial();

	if (!m_implementation)
		m_implementation = g_engine->m_videoAPI->CreateModelImplementation();
}