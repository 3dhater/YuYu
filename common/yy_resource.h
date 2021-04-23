#ifndef _YY_RES_H_
#define _YY_RES_H_

#include "strings/string.h"
#include "math\aabb.h"

#include "yy_fs.h"
//#include <filesystem>

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

typedef yyImage*(*ImageLoaderFunction_t)(const char* p);
//using ImageLoaderExportFunction_t = bool(*)(yyImage* image, const char* fileName, const char* extName );

void MDL_loadVersion1(yyMDL** mdl, yyFileBuffer* file);

struct yyImageLoader
{
	yyImageLoader()
		:
		image_loader_callback(nullptr)
	{}

	yyStringA ext;
	ImageLoaderFunction_t image_loader_callback;
};

// for yyResource
enum class yyResourceType : u16
{
	None,
	Texture,
	RenderTargetTexture,
	Model,
};

struct yyResourceDataImage
{
	yyTextureFilter m_filter;
};

struct yyResourceData
{
	yyResourceData()
	{
		m_type = yyResourceType::None;
		m_source = 0;
	}

	union {
		yyResourceDataImage m_imageData;
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
	virtual s32 GetRefCount() = 0;
};

extern "C"
{
	// all resources are unloaded
	// call newRes->Load(); for loading

	// get from cache. if not found, create GPU resource, add to cache.
	// ++m_refCount;
	// call yyRemoveTextureFromCache(newRes); before yyDestroy(newRes); 
	//	if you want delete by yourself
	YY_API yyResource* YY_C_DECL yyGetTextureFromCache(const char*);
	YY_API void YY_C_DECL yyRemoveTextureFromCache(yyResource*);
	
	// call yyDestroy(newRes); for destroy
	YY_API yyResource* YY_C_DECL yyCreateTexture(yyImage*);
	YY_API void YY_C_DECL yySetTextureFilter(yyTextureFilter);
	YY_API yyTextureFilter YY_C_DECL yyGetTextureFilter();
	YY_API void YY_C_DECL yyUseMipMaps(bool);
	YY_API bool YY_C_DECL yyIsUseMipMaps();

	// call yyDestroy(newRes); for destroy
	YY_API yyResource* YY_C_DECL yyCreateModel(yyModel*);

	// загрузить модель и поместить её в кеш. следующий вызов - получить из кеша
	YY_API yyMDL* YY_C_DECL yyGetMDL(const char*);
	// after loading, you must call yyDeleteModel
	YY_API yyMDL* YY_C_DECL yyLoadMDL(const char*);
	YY_API void YY_C_DECL yyDeleteMDL(yyMDL*);

	YY_API yyImage* YY_C_DECL yyLoadImage(const char*); // after loading, you must call yyDestroy(image)
	YY_API void YY_C_DECL yyLoadImageAsync(const char*, s32 id); // after loading, you must call yyDestroyImage
}
#endif