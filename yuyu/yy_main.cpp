﻿#include "yy.h"

#include <cassert>

#include "yy_resource.h"
#include "yy_async.h"
#include "yy_gui.h"
#include "yy_input.h"
#include "scene/common.h"

#include "engine.h"

#include <thread>

Engine * g_engine = nullptr;

Engine::Engine()
{
	m_sceneRootObject = new yySceneObjectBase;
	m_cctx = ZSTD_createCCtx();

	yyImageLoader imageLoader;
	imageLoader.ext = ".dds";
	imageLoader.image_loader_callback = ImageLoader_DDS;
	m_imageLoaders.push_back(imageLoader);

	imageLoader.ext = ".png";
	imageLoader.image_loader_callback = ImageLoader_PNG;
	m_imageLoaders.push_back(imageLoader);

	yyModelLoader modelLoader;
	modelLoader.ext = ".tr3d";
	modelLoader.model_loader_callback = ModelLoader_TR3D;
	m_modelLoaders.push_back(modelLoader);
}
Engine::~Engine()
{
	/*if(m_inputContext)
	{
		yyDestroy(m_inputContext);
	}*/

	delete m_sceneRootObject;


	auto guiNode = m_guiElements.head();
	if(guiNode)
	{
		for(size_t i = 0, sz = m_guiElements.size(); i < sz; ++i)
		{
			yyDestroy( guiNode->m_data );
			guiNode = guiNode->m_right;
		}
	}

	if(m_videoDriverLib)
	{
		m_videoAPI->Destroy();

		yyFreeLybrary(m_videoDriverLib);
		m_videoDriverLib = nullptr;
	}
	ZSTD_freeCCtx(m_cctx);
}

void Engine::addGuiElement(yyGUIElement* el)
{
	m_guiElements.push_back(el);
}

class EngineDestroyer
{
public:
	EngineDestroyer(){}
	~EngineDestroyer()
	{
		if(g_engine) // Visual Studio want to delete g_engine more than 1 time
		{
			yyDestroy( g_engine );
			g_engine = nullptr;
		}
	}
};
EngineDestroyer g_engineDestroyer;

extern "C"
{

YY_API yySystemState* YY_C_DECL yyStart(yyInputContext* input)
{
	assert(!g_engine);
	g_engine = yyCreate<Engine>();
	g_engine->m_inputContext = input;
	//g_engine->m_resourceManager = new yyResourceManager;
	g_engine->m_backgroundWorker = new std::thread(yyBackgroundWorkerFunction);
	//g_engine->m_inputContext = yyCreate<yyInputContext>();

	return &g_engine->m_state;
}

YY_API void YY_C_DECL yyStop()
{
	if(g_engine)	
	{
		if(g_engine->m_backgroundWorker)
		{
			g_engine->m_workerCommands.put(BackgroundWorkerCommands(BackgroundWorkerCommands::type::ExitThread,0,0));
			g_engine->m_backgroundWorker->join();
			delete g_engine->m_backgroundWorker;
		}

		/*if(g_engine->m_resourceManager)
		{
			delete g_engine->m_resourceManager;
			g_engine->m_resourceManager = nullptr;
		}*/

		

		yyDestroy( g_engine );
		g_engine = nullptr;
	}
}

YY_API void YY_C_DECL yyQuit()
{
	if(g_engine)
	{
		g_engine->m_state = yySystemState::Quit;
	}
}

//YY_API yyInputContext* YY_C_DECL yyGetInputContext()
//{
//	return g_engine->m_inputContext;
//}

u8* Engine::compressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size)
{
	u8* out_data = (u8*)yyMemAlloc(in_data_size);
	if( !out_data )
	{
		YY_PRINT_FAILED;
		return out_data;
	}

	auto compressBound = ZSTD_compressBound(in_data_size);

	size_t const cSize = ZSTD_compressCCtx( m_cctx, out_data, compressBound, in_data, in_data_size, 1);
    if( ZSTD_isError(cSize) )
	{
		yyMemFree(out_data);
		return out_data;
	}

	yyMemRealloc(out_data,cSize);
	out_data_size = (u32)cSize;
	return out_data;
}

u8* Engine::decompressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size)
{
	unsigned long long const rSize = ZSTD_getFrameContentSize(in_data, in_data_size);
	u8* out_data = (u8*)yyMemAlloc((u32)rSize);
	if( !out_data )
	{
		YY_PRINT_FAILED;
		return out_data;
	}

	size_t const dSize = ZSTD_decompress(out_data, (size_t)rSize, in_data, in_data_size);
	out_data_size = (u32)dSize;
	return out_data;
}

YY_API u8* YY_C_DECL yyCompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct )
{
	switch (ct)
	{
	case yyCompressType::WithoutCompress:
		break;
	case yyCompressType::ZStd:
		return g_engine->compressData_zstd(in_data,  in_data_size,  out_data_size);
	default:
		yyLogWriteWarning("Need implement\n");
		YY_PRINT_FAILED;
		break;
	}
	return nullptr;
}
YY_API u8* YY_C_DECL yyDecompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct )
{
	switch (ct)
	{
	case yyCompressType::WithoutCompress:
		break;
	case yyCompressType::ZStd:
		return g_engine->decompressData_zstd(in_data,  in_data_size,  out_data_size);
	default:
		yyLogWriteWarning("Need implement\n");
		YY_PRINT_FAILED;
		break;
	}
	return nullptr;
}
YY_API void YY_C_DECL yySetAsyncLoadEventHandler(yyAsyncLoadEventHandler f)
{
	g_engine->m_asyncEventHandler = f;
}
YY_API void YY_C_DECL yyUpdateAsyncLoader()
{
	if(!g_engine->m_asyncEventHandler)
		return;

	for(int i = 0, sz = g_engine->m_workerResults.m_size; i < sz; ++i)
	{
		auto & obj = g_engine->m_workerResults.get();
		switch (obj.m_type)
		{
		default:
			break;
		case BackgroundWorkerResults::type::None:
			break;
		case BackgroundWorkerResults::type::LoadImage:
			g_engine->m_asyncEventHandler( obj.m_id, obj.m_rawData );
			break;
		}
	}
}

YY_API yyVideoDriverAPI* YY_C_DECL yyGetVideoDriverAPI()
{
	return g_engine->m_videoAPI;
}

YY_API bool YY_C_DECL yyInitVideoDriver(const char* dl, yyWindow* window)
{
	if(g_engine)
	{
		if(g_engine->m_videoDriverLib)
		{
			yyLogWriteWarning("Video driver already initialized\n");
			return false;
		}

		g_engine->m_videoDriverLib = yyLoadLybrary(dl);
		if(!g_engine->m_videoDriverLib)
		{
			yyLogWriteWarning("Can't load video driver library\n");
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		g_engine->m_videoDriverGetApi = (Engine::videoGetApi)yyGetProcAddress(g_engine->m_videoDriverLib, "GetAPI");
		if(!g_engine->m_videoDriverGetApi)
		{
			yyLogWriteWarning("Can't get GetApi function from video driver library\n");
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		g_engine->m_videoAPI = g_engine->m_videoDriverGetApi();
		if(!g_engine->m_videoAPI)
		{
			yyLogWriteWarning("Can't get video driver API\n");
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		if(g_engine->m_videoAPI->GetAPIVersion() != yyVideoDriverAPIVersion)
		{
			yyLogWriteWarning("Wrong Video Driver API version; %u, need %u\n", g_engine->m_videoAPI->GetAPIVersion(), yyVideoDriverAPIVersion);
			yyFreeLybrary(g_engine->m_videoDriverLib);
			g_engine->m_videoDriverLib = nullptr;
			return false;
		}

		//g_engine->m_resourceManager->m_videoDriverAPI = g_engine->m_videoAPI;

		return g_engine->m_videoAPI->Init(window);
	}
	return false;
}

}