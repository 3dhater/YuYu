#ifndef _YY_VID_H_
#define _YY_VID_H_

enum class yyVideoDriverTextureSlot : u32
{
	Texture0,
	Texture1,
	Texture2,
	Texture3,
	Texture4,
	Texture5,
	Texture6,
	Texture7,
	Texture8,
	Texture9,
	Texture10,
	Texture11,
	Texture12,
	Texture13,
	Texture14,
	Texture15,
	Texture16,
	Texture17,
	Texture18,
	Texture19,
	Texture20,

	Count
};

constexpr u32 yyVideoDriverAPIVersion = 1;
struct yyVideoDriverAPI
{
	u32 (*GetAPIVersion)() = nullptr;

	bool (*Init)(yyWindow*)=nullptr;
	void (*Destroy)()=nullptr;
	
	//void (*UseClearColor)(bool)=nullptr;
	//void (*UseClearDepth)(bool)=nullptr;

	void (*UseVSync)(bool)=nullptr;
	
	void (*SetClearColor)(f32 r, f32 g, f32 b, f32 a)=nullptr;
	
	void (*BeginDrawClearDepth)()=nullptr;
	void (*BeginDrawClearColor)()=nullptr;
	void (*BeginDrawClearAll)()=nullptr;
	void (*BeginDrawNotClear)()=nullptr;
	void (*EndDraw)()=nullptr;

	void (*BeginDrawGUI)()=nullptr;
	void (*EndDrawGUI)()=nullptr;

	yyResource* (*GetTexture)(const char* fileName, bool useLinearFilter)=nullptr; // AddRef every time.
	yyResource* (*CreateTexture)(yyImage*, bool useLinearFilter)=nullptr;          // create new every call. m_refCount = 1
	void (*ReleaseTexture)(yyResource*)=nullptr;             // Release, --m_refCount;

	yyResource* (*CreateModel)(yyModel*)=nullptr;
	void (*ReleaseModel)(yyResource*)=nullptr;

	// yyResource::m_type MUST BE yyResourceType::Texture
	void (*SetTexture)(yyVideoDriverTextureSlot, yyResource*)=nullptr;
	// yyResource::m_type MUST BE yyResourceType::Model
	void (*SetModel)(yyResource*)=nullptr;
	// draw what we set above
	void (*Draw)()=nullptr;
	
	void (*DrawSprite)(yySprite*)=nullptr;
};

#endif