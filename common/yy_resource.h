#ifndef _YY_RES_H_
#define _YY_RES_H_

#include "strings/string.h"
#include "math\aabb.h"

#include "yy_fs.h"
//#include <filesystem>

enum class yyTextureComparisonFunc
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

enum class yyTextureAddressMode
{
	Wrap,
	Mirror,
	Clamp,
	Border,
	MirrorOnce
};

enum class yyTextureFilter
{
	// min mag mip / point linear
	PPP,
	PPL,
	PLP,
	PLL,
	LPP,
	LPL,
	LLP,
	LLL,
	ANISOTROPIC,
	// comparison
	CMP_PPP,
	CMP_PPL,
	CMP_PLP,
	CMP_PLL,
	CMP_LPP,
	CMP_LPL,
	CMP_LLP,
	CMP_LLL,
	CMP_ANISOTROPIC,
};

// LOADER PROTOTYPES
// don't forget to add this in Engine::m_imageLoaders
yyImage* ImageLoader_DDS(const char* p);
yyImage* ImageLoader_PNG(const char* p);
yyImage* ImageLoader_TGA(const char* p);
yyImage* ImageLoader_BMP(const char* p); 
void ImageLoaderGetInfo_DDS(const char* p, yyImage*);
void ImageLoaderGetInfo_PNG(const char* p, yyImage*);
void ImageLoaderGetInfo_TGA(const char* p, yyImage*);
void ImageLoaderGetInfo_BMP(const char* p, yyImage*);

typedef yyImage*(*ImageLoaderFunction_t)(const char* p);
typedef void(*ImageLoaderGetInfoFunction_t)(const char* p, yyImage*);
//using ImageLoaderExportFunction_t = bool(*)(yyImage* image, const char* fileName, const char* extName );

void MDL_loadVersion1(yyMDL** mdl, yyFileBuffer* file);

struct yyImageLoader
{
	yyImageLoader()
		:
		image_loader_callback(nullptr),
		image_loader_getInfo_callback(nullptr)
	{}

	yyStringA ext;
	ImageLoaderFunction_t image_loader_callback;
	ImageLoaderGetInfoFunction_t image_loader_getInfo_callback;
};

enum class yyResourceType : u16
{
	None,
	Texture,
	RenderTargetTexture,
	Model,
};

struct yyResourceDataImage
{
	yyResourceDataImage() {
		m_filter = yyTextureFilter::PPP;
		m_addressMode = yyTextureAddressMode::Wrap;
		m_comparisonFunc = yyTextureComparisonFunc::Always;
		m_size[0] = m_size[1] = 0.f;
		m_anisotropicLevel = 1;
	}
	yyTextureFilter m_filter;
	yyTextureAddressMode m_addressMode;
	yyTextureComparisonFunc m_comparisonFunc;
	f32 m_size[2];
	s32 m_anisotropicLevel;
};
struct yyResourceDataModel
{
	yyResourceDataModel() { m_material = 0; }
	~yyResourceDataModel() {
		if( m_material ) 
			yyMegaAllocator::Destroy(m_material);
	}
	yyMaterial* m_material;
};

struct yyResourceData
{
	yyResourceData(){
		m_type = yyResourceType::None;
		m_source = 0;
		m_imageData = 0;
		m_modelData = 0;
	}

	~yyResourceData(){
		switch (m_type)
		{
		default:
			break;
		case yyResourceType::Texture:
		case yyResourceType::RenderTargetTexture:
			if (m_imageData) yyDestroy(m_imageData);
			break;
		case yyResourceType::Model:
			if (m_modelData) yyDestroy(m_modelData);
			break;
		}
	}

	union {
		yyResourceDataImage* m_imageData;
		yyResourceDataModel* m_modelData;
	};

	yyResourceType m_type;
	void * m_source;
	yyStringA m_path;
};

// ресурсы в видеодрайвере наследуют это
class yyResourceImplementation
{
public:
	yyResourceImplementation() {}
	virtual ~yyResourceImplementation() {}

	// Load и Unload делает напрямую, refCount'а нет.
	virtual void Load(yyResourceData*) = 0;
	virtual void Unload() = 0;

	virtual void GetTextureSize(v2f*) = 0;
	virtual void GetTextureHandle(void**) = 0;
	virtual void MapModelForWriteVerts(u8** v_ptr) = 0;
	virtual void UnmapModelForWriteVerts() = 0;
};


// Это такой ресурс, к которому нет прямого доступа. Например всё что связано с видео драйвером.
class yyResource
{
public:

	yyResource() {};
	virtual ~yyResource() {};

	virtual yyResourceType GetType() = 0;

	/*	Ресурс должен иметь методы для загрузки и удаления данных ресурса
		При этом, сам указатель на ресурс остаётся активным. В любое
		время можно будет загрузить ресурс или удалить.
		Для повторной загрузки, необходимо знать, откуда загружать(создавать)
		ресурс. Строка на файл, или указатель на yyModel\yyImage, или ещё что.
	*/
	/*	Ресурс имеет счётчик ссылок. Когда вызывается Load, и счётчик ссылок 
		равен 1 - то происходит загрузка ресурса, иначе просто увеличение 
		счётчика. Когда вызывается Unload, значение счётчика уменьшается на 1
		и если равен 0 то данные ресурса удаляются.
	*/
	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual u32 GetRefCount() = 0;
	virtual bool IsLoaded() = 0;
	virtual bool IsFromCache() = 0;

	// используется при асинхронной загрузке
	virtual void LoadSource() = 0;
	virtual void LoadImplementation() = 0;
	virtual void DestroySource() = 0;

	virtual yyResourceImplementation* GetImplementation() = 0;
	
	virtual void GetTextureSize(v2f*) = 0;
	virtual void GetTextureHandle(void**) = 0;// Get ID3D11ShaderResourceView* or OpenGL texture ID
	virtual void MapModelForWriteVerts(u8** v_ptr) = 0;
	virtual void UnmapModelForWriteVerts() = 0;
};

extern "C"
{
	YY_API yyImage* YY_C_DECL yyLoadImage(const char*); // after loading, you must call yyDestroy(image)
	YY_API void YY_C_DECL yyLoadImageGetInfo(const char*, yyImage*); // get width\height and other without loading
	//YY_API void YY_C_DECL yyLoadImageAsync(const char*, s32 id); // after loading, you must call yyDestroyImage

	// all resources(and MDL) are unloaded
	// call newRes->Load(); for loading

	// get from cache. if not found, create GPU resource, add to cache.
	// ++m_refCount;
	// call yyRemoveTextureFromCache(newRes); before yyMegaAllocator::Destroy(newRes); 
	//	if you want delete by yourself
	YY_API yyResource* YY_C_DECL yyGetTextureFromCache(const char*);
	YY_API void YY_C_DECL yyRemoveTextureFromCache(yyResource*);
	
	// call yyMegaAllocator::Destroy(newRes); for destroy
	YY_API yyResource* YY_C_DECL yyCreateTexture(yyImage*);
	// call yyMegaAllocator::Destroy(newRes); for destroy
	YY_API yyResource* YY_C_DECL yyCreateTextureFromFile(const char*);
	// do not call Load(); if yyResource is render target texture
	YY_API yyResource* YY_C_DECL yyCreateRenderTargetTexture(const v2f& size);
	YY_API void YY_C_DECL yySetTextureFilter(yyTextureFilter);
	YY_API yyTextureFilter YY_C_DECL yyGetTextureFilter();
	YY_API void YY_C_DECL yyUseMipMaps(bool);
	YY_API bool YY_C_DECL yyIsUseMipMaps();
	YY_API void YY_C_DECL yySetTextureAddressMode(yyTextureAddressMode);
	YY_API yyTextureAddressMode YY_C_DECL yyGetTextureAddressMode();
	YY_API void YY_C_DECL yySetTextureAnisotropicLevel(s32);
	YY_API s32 YY_C_DECL yyGetTextureAnisotropicLevel();
	YY_API void YY_C_DECL yySetTextureComparisonFunc(yyTextureComparisonFunc);
	YY_API yyTextureComparisonFunc YY_C_DECL yyGetTextureComparisonFunc();
	

	// don't forget to call newRes->Load(); 
	// call yyMegaAllocator::Destroy(newRes); for destroy
	YY_API yyResource* YY_C_DECL yyCreateModel(yyModel*);

	// загрузить модель и поместить её в кеш. следующий вызов - получить из кеша
	YY_API yyMDL* YY_C_DECL yyGetMDLFromCache(const char*);
	YY_API void YY_C_DECL yyRemoveMDLFromCache(yyMDL*);
	// after loading, you must call yyDestroy(mdl);
	YY_API yyMDL* YY_C_DECL yyCreateMDLFromFile(const char*);
}
#endif