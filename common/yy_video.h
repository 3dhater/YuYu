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

	// Каждый вызов Create... создаёт абсолютно новый ресурс который останется в памяти на всё время
	// Ресурс хранит индекс на массив с указателями, которые указывают на реализацию ресурса (например текстура)
	// Реализацию можно выгрузить из памяти, и загрузить снова. Всё основано на подсчёте ссылок.
	//   это нужно иметь ввиду.
	// Если создаётся ресурс с использованием например yyImage* , 
	//   то, чтобы работали Unload... Load... нужно хранить этот yyImage* (ведь нужно знать из чего делать текстуру)
	// Unload... убавит счётчик ссылок на 1, и если будет 0 то реализация уничтожится
	// Load... Прибавит счётчик на 1. Если будет 1 то нужно заново создать реализацию. Если будет
	//   доступен resource->m_source то будет создано на основе него (если это текстура то m_source должен быть yyImage*)
	//   иначе будет попытка загрузить реализацию из файла
	yyResource* (*CreateTexture)(yyImage*, bool useLinearFilter)=nullptr;
	yyResource* (*CreateTextureFromFile)(const char* fileName, bool useLinearFilter)=nullptr;
	void (*UnloadTexture)(yyResource*)=nullptr; // --m_refCount; or unload
	void (*LoadTexture)(yyResource*)=nullptr; // ++m_refCount; or load

	yyResource* (*CreateModel)(yyModel*)=nullptr;
	void (*UnloadModel)(yyResource*)=nullptr;
	void (*LoadModel)(yyResource*)=nullptr;

	// yyResource::m_type MUST BE yyResourceType::Texture
	void (*SetTexture)(yyVideoDriverTextureSlot, yyResource*)=nullptr;
	// yyResource::m_type MUST BE yyResourceType::Model
	void (*SetModel)(yyResource*)=nullptr;
	// draw what we set above
	void (*Draw)()=nullptr;
	
	void (*DrawSprite)(yySprite*)=nullptr;
	void (*DrawLine3D)(const v4f& _p1, const v4f& _p2, const yyColor& color)=nullptr;

	enum MatrixType
	{
		World,
		View,
		Projection,
		ViewProjection, //For 3d line
	};
	void (*SetMatrix)(MatrixType, const Mat4&)=nullptr;
};

#endif