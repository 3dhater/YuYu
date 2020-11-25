#ifndef _YY_VID_H_
#define _YY_VID_H_

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

	yyResource* (*GetTexture)(const char* fileName, bool useLinearFilter)=nullptr; // AddRef every time.
	yyResource* (*CreateTexture)(yyImage*, bool useLinearFilter)=nullptr;          // create new every call. m_refCount = 1
	void (*ReleaseTexture)(yyResource*)=nullptr;             // Release, --m_refCount;
};

#endif