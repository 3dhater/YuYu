#ifndef _YUYU_ENGINE_H_
#define _YUYU_ENGINE_H_

#include <zstd.h>

#include "yy_async.h"
#include "containers/fixed_fifo.h"
#include "containers/list.h"
#include "containers/array.h"
#include "yy_fs.h"
#include "yy_material.h"
#include "yyResourceImpl.h"

void yyBackgroundWorkerFunction();

struct BackgroundWorkerResults
{
	enum type
	{
		None,
		LoadSource,
	};

	BackgroundWorkerResults()
	:
		m_type(type::None),
		m_id(-1),
		m_resource(nullptr)
	{}
	BackgroundWorkerResults(type t)
		:
		m_type(t),
		m_id(-1),
		m_resource(nullptr)
	{}

	~BackgroundWorkerResults(){}
	
	type m_type;
	s32 m_id;
	yyResource* m_resource;
};

struct BackgroundWorkerCommands
{
	enum type
	{
		None,
		ExitThread,
		LoadSource,

	};

	BackgroundWorkerCommands()
	:
		m_type(type::None),
		m_id(-1)
	{
		m_resource = 0;
	}
	BackgroundWorkerCommands(type t,  s32 id)
	:
		m_type(t),
		m_id(id)
	{}
	~BackgroundWorkerCommands(){}


	type m_type;
	s32 m_id;
	yyResource* m_resource;
};

template<typename T>
struct CacheNode
{
	CacheNode()
		:
		m_resource(nullptr)
	{}
	yy_fs::path m_path;
	T* m_resource;
};

struct IFileSaveDialog;
struct IFileOpenDialog;

#define YY_EVENT_MAX 30


class yyEngine
{
	void GUIRebuildElement(yyGUIElement*);
public:
	yyEngine(yyPoolSetup*);
	~yyEngine();
	
	yyWindow* m_mainWindow;

	yyPoolSetup m_poolSetup;
	yyPoolAllocator<yyResourceImpl> m_poolResource;
	yyPoolAllocator<yyMaterial> m_poolMaterial;
	yyPoolAllocator<yyModel> m_poolModel;
	
	yyInputContext* m_inputContext;

	bool m_cursorDisableAutoChange;
	yyCursor* m_cursors[(u32)yyCursorType::_count];
	yyCursor* m_cursorsDefault[(u32)yyCursorType::_count];

	yySystemState m_state;
	dll_handle m_videoDriverLib;
	
	typedef yyVideoDriverAPI*(*videoGetApi)();
	videoGetApi m_videoDriverGetApi;
	yyVideoDriverAPI* m_videoAPI;

	void AddEvent(const yyEvent&, bool unique);
	bool PollEvent(yyEvent&);
	yyEvent m_events[YY_EVENT_MAX];
	u32 m_events_num;
	u32 m_events_current;

	yyArraySmall<yyImageLoader> m_imageLoaders;

	s32 m_textureAnisotropicLevel;
	yyTextureAddressMode m_textureAddressMode;
	yyTextureComparisonFunc m_textureComparisonFunc;
	yyTextureFilter m_textureFilter;
	bool m_useMipmaps;

	yyAsyncLoadEventHandler m_asyncEventHandler;
	std::thread* m_backgroundWorker;
	yyFixedFIFOThread<BackgroundWorkerCommands, 20> m_workerCommands;
	
	yyFixedFIFOThread<BackgroundWorkerResults, 20> m_workerResults;

	//void addGuiElement(yyGUIElement*, yyGUIDrawGroup*);
	//yyList<yyGUIElement*> m_guiElements;
	yyGUIDrawGroup* m_mainGUIDrawGroup;
	yyList<yyGUIDrawGroup*> m_GUIDrawGroups;
	void GUIUpdateDrawGroup(yyGUIDrawGroup*, f32 dt);
	void GUIDrawDrawGroup(yyGUIDrawGroup*, f32 dt);
	void GUIRebuildDrawGroup(yyGUIDrawGroup*);
	bool m_cursorInGUI;
	
	yyGUIElement* m_GUIElementInputFocus;
	//bool m_inputInGUI;

	// input only elements on top
	bool m_guiIgnoreUpdateInput;
	yyGUIElement* m_guiElementInMouseFocus;

	ZSTD_CCtx* m_cctx;
	u8* compressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size);
	u8* decompressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size);

	//std::vector<CacheNode<yyResource>> m_textureCache;
	yyList<CacheNode<yyResource>> m_textureCache;
	yyList<CacheNode<yyMDL>> m_modelCache;

	yyStringW m_workingDir;

	yyResource* m_defaultTexture;

	yyMaterial m_currentMaterial;
	v3f m_eyePosition;
	Mat4 m_matrixWorld;
	Mat4 m_matrixView;
	Mat4 m_matrixViewInvert;
	Mat4 m_matrixProjection;
	Mat4 m_matrixViewProjection;
	Mat4 m_matrixWorldViewProjection;
	Mat4 m_matrixBones[YY_MAX_BONES];

#ifdef YY_PLATFORM_WINDOWS
	IFileSaveDialog * m_fileSaveDialog;
	IFileOpenDialog * m_fileOpenDialog;
#endif
};

#endif