#include "yy.h"

#include <cassert>

#include "yy_resource.h"
#include "yy_async.h"
#include "yy_gui.h"
#include "yy_input.h"
#include "yy_model.h"
#include "yy_material.h"
#include "scene/common.h"
#include "strings\string.h"
#include "strings\utils.h"

#include "engine.h"

#ifdef YY_PLATFORM_WINDOWS
#include <Windows.h>
#include <shobjidl.h> 
#include "Shlwapi.h"
#endif

#include <thread>

#define CHECK(cond, ...)                        \
    do {                                        \
        if (!(cond)) {                          \
            yyLogWriteError(					\
                    "%s:%d CHECK(%s) failed: ", \
                    __FILE__,                   \
                    __LINE__,                   \
                    #cond);                     \
            yyLogWriteError("" __VA_ARGS__);    \
            yyLogWriteError("\n");              \
            exit(1);                            \
        }                                       \
    } while (0)

Engine * g_engine = nullptr;


Engine::Engine(yyPoolSetup* ps)
	:
	m_inputContext(nullptr),
	m_state(yySystemState::Run),
	m_videoDriverLib(nullptr),
	m_videoDriverGetApi(nullptr),
	m_videoAPI(nullptr),
	m_asyncEventHandler(nullptr),
	m_backgroundWorker(nullptr),
	m_cctx(nullptr)
{
	m_cursorDisableAutoChange = false;
	m_GUIElementInputFocus = 0;
	for (u32 i = 0; i < (u32)yyCursorType::_count; ++i)
	{
		m_cursors[i] = 0;
		m_cursorsDefault[i] = 0;
	}

	m_mainWindow = 0;

	if (ps)
	{
		m_poolSetup = *ps;
	}
	else
	{
		m_poolSetup.m_resourceCount = yyPoolSetup_default_resourceCount;
		m_poolSetup.m_materialCount = yyPoolSetup_default_materialCount;
		m_poolSetup.m_modelCount = yyPoolSetup_default_modelCount;
	}
	m_poolResource.Init(m_poolSetup.m_resourceCount);
	m_poolMaterial.Init(m_poolSetup.m_materialCount);
	m_poolModel.Init(m_poolSetup.m_modelCount);


	m_textureAnisotropicLevel = 1;
	m_textureAddressMode = yyTextureAddressMode::Wrap;
	m_textureComparisonFunc = yyTextureComparisonFunc::Always;
	m_useMipmaps = true;
	m_textureFilter = yyTextureFilter::PPP;
	m_defaultTexture = 0;
	m_cursorInGUI = false;
	m_guiElementInMouseFocus = 0;
	m_mainGUIDrawGroup = yyCreate<yyGUIDrawGroup>();



	m_cctx = ZSTD_createCCtx();

	yyImageLoader imageLoader;
	imageLoader.ext = ".dds";
	imageLoader.image_loader_callback = ImageLoader_DDS;
	imageLoader.image_loader_getInfo_callback = ImageLoaderGetInfo_DDS;
	m_imageLoaders.push_back(imageLoader);
	
	imageLoader.ext = ".png";
	imageLoader.image_loader_callback = ImageLoader_PNG;
	imageLoader.image_loader_getInfo_callback = ImageLoaderGetInfo_PNG;
	m_imageLoaders.push_back(imageLoader);

	imageLoader.ext = ".bmp";
	imageLoader.image_loader_callback = ImageLoader_BMP;
	imageLoader.image_loader_getInfo_callback = ImageLoaderGetInfo_BMP;
	m_imageLoaders.push_back(imageLoader);

	imageLoader.ext = ".tga";
	imageLoader.image_loader_callback = ImageLoader_TGA;
	imageLoader.image_loader_getInfo_callback = ImageLoaderGetInfo_TGA;
	m_imageLoaders.push_back(imageLoader);

#ifdef YY_PLATFORM_WINDOWS
	m_fileSaveDialog = nullptr;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
		YY_PRINT_FAILED;

	hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&m_fileSaveDialog));
	if (FAILED(hr))
		YY_PRINT_FAILED;

	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&m_fileOpenDialog));
	if (FAILED(hr))
		YY_PRINT_FAILED;

	//m_workingDir
	wchar_t wcharBuffer[512];
	GetCurrentDirectory(512, wcharBuffer);
	m_workingDir = wcharBuffer;

	for (u32 i = 0; i < (u32)yyCursorType::_count; ++i)
	{
		m_cursorsDefault[i] = new yyCursor((yyCursorType)i);

		switch ((yyCursorType)i)
		{
		default:
		case yyCursorType::Arrow: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_ARROW); break;
		case yyCursorType::Cross: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_CROSS); break;
		case yyCursorType::Hand: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_HAND); break;
		case yyCursorType::Help: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_HELP); break;
		case yyCursorType::IBeam: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_IBEAM); break;
		case yyCursorType::No: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_NO); break;
		case yyCursorType::Size: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_SIZEALL); break;
		case yyCursorType::SizeNESW: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_SIZENESW); break;
		case yyCursorType::SizeNS: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_SIZENS); break;
		case yyCursorType::SizeNWSE: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_SIZENWSE); break;
		case yyCursorType::SizeWE: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_SIZEWE); break;
		case yyCursorType::UpArrow: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_UPARROW); break;
		case yyCursorType::Wait: m_cursorsDefault[i]->m_handle = LoadCursor(0, IDC_WAIT); break;
		}

		m_cursors[i] = m_cursorsDefault[i];
	}
#endif

	m_events_num = 0;
	m_events_current = 0;
}

Engine::~Engine(){
	for (u32 i = 0; i < (u32)yyCursorType::_count; ++i)
	{
		if(m_cursorsDefault[i]) delete m_cursorsDefault[i];
	}
	/*if(m_inputContext)
	{
		yyDestroy(m_inputContext);
	}*/

	for (auto n : m_textureCache)
	{
		yyMegaAllocator::Destroy(n.m_resource);
	}

	for (auto n : m_modelCache)
	{
		yyDestroy(n.m_resource);
	}

#ifdef YY_PLATFORM_WINDOWS
	if (m_fileSaveDialog) m_fileSaveDialog->Release();
	if (m_fileOpenDialog) m_fileOpenDialog->Release();
	CoUninitialize();
#endif

	if (m_mainGUIDrawGroup) {
		yyDestroy(m_mainGUIDrawGroup);
	}

	{
		auto guiNode = m_GUIDrawGroups.head();
		if (guiNode)
		{
			for (size_t i = 0, sz = m_GUIDrawGroups.size(); i < sz; ++i)
			{
				yyDestroy(guiNode->m_data);
				guiNode = guiNode->m_right;
			}
		}
	}
	
	/*auto guiNode = m_guiElements.head();
	if(guiNode)
	{
		for(size_t i = 0, sz = m_guiElements.size(); i < sz; ++i)
		{
			yyDestroy( guiNode->m_data );
			guiNode = guiNode->m_right;
		}
	}*/

	if(m_videoDriverLib)
	{
		m_videoAPI->Destroy();

		yyFreeLibrary(m_videoDriverLib);
		m_videoDriverLib = nullptr;
	}
	ZSTD_freeCCtx(m_cctx);
}

void Engine::AddEvent(const yyEvent& event, bool unique) {
	if (m_events_num == YY_EVENT_MAX)
	{
		yyLogWriteWarning("Too many events\n");
		return;
	}
	if (unique)
	{
		for (u32 i = 0; i < m_events_num; ++i)
		{
			if (m_events[i].is_equal(event))
			{
				return;
			}
		}
	}

	m_events[m_events_num] = event;
	++m_events_num;
}

bool Engine::PollEvent(yyEvent& event) {
	if (m_events_num == 0) 
	{
		m_events_current = 0;
		return false;
	}

	event = m_events[m_events_current];
	--m_events_num;
	++m_events_current;

	return true;
}

class EngineDestroyer
{
public:
	EngineDestroyer(){}
	~EngineDestroyer(){
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
	YY_API void YY_C_DECL yySetMaterial(const yyMaterial& mat) {
		g_engine->m_currentMaterial = mat;
	}
	YY_API yyMaterial* YY_C_DECL yyGetMaterial() {
		return &g_engine->m_currentMaterial;
	}
	YY_API void YY_C_DECL yySetEyePosition(const v3f& v) {
		g_engine->m_eyePosition = v;
	}
	YY_API v3f* YY_C_DECL yyGetEyePosition() {
		return &g_engine->m_eyePosition;
	}
	YY_API void YY_C_DECL yySetMatrix(yyMatrixType t, const Mat4& m) {
		switch (t)
		{
		case World:
			g_engine->m_matrixWorld = m;
			break;
		case View:
			g_engine->m_matrixView = m;
			break;
		case Projection:
			g_engine->m_matrixProjection = m;
			break;
		case ViewProjection:
			g_engine->m_matrixViewProjection = m;
			break;
		case WorldViewProjection:
			g_engine->m_matrixWorldViewProjection = m;
			break;
		case ViewInvert:
			g_engine->m_matrixViewInvert = m;
			break;
		default:
			YY_PRINT_FAILED;
			break;
		}
	}
	YY_API void YY_C_DECL yySetBoneMatrix(u32 boneIndex, const Mat4& m) {
		assert(boneIndex < YY_MAX_BONES);
		g_engine->m_matrixBones[boneIndex] = m;
	}
	YY_API Mat4* YY_C_DECL yyGetMatrix(yyMatrixType t) {
		switch (t)
		{
		case World:
			return &g_engine->m_matrixWorld;
			break;
		case View:
			return &g_engine->m_matrixView;
			break;
		case Projection:
			return &g_engine->m_matrixProjection;
			break;
		case ViewProjection:
			return &g_engine->m_matrixViewProjection;
			break;
		case WorldViewProjection:
			return &g_engine->m_matrixWorldViewProjection;
			break;
		case ViewInvert:
			return &g_engine->m_matrixViewInvert;
			break;
		default:
			YY_PRINT_FAILED;
			break;
		}
		return 0;
	}
	YY_API Mat4* YY_C_DECL yyGetBoneMatrix(u32 boneIndex) {
		assert(boneIndex < YY_MAX_BONES);
		return &g_engine->m_matrixBones[boneIndex];
	}

	YY_API bool YY_C_DECL yyRun(f32* deltaTime) {
		static u64 t1 = 0;
		u64 t2 = yyGetTime();
		f32 tick = f32(t2 - t1);
		t1 = t2;
		*deltaTime = tick / 1000.f;

		g_engine->m_inputContext->Reset();
#ifdef YY_PLATFORM_WINDOWS
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
#else
#error For windows
#endif
		
		static f32 lmb_dbl_timer = 0.f;
		static f32 rmb_dbl_timer = 0.f;
		static f32 mmb_dbl_timer = 0.f;
		static f32 x1mb_dbl_timer = 0.f;
		static f32 x2mb_dbl_timer = 0.f;
		lmb_dbl_timer += *deltaTime;
		rmb_dbl_timer += *deltaTime;
		mmb_dbl_timer += *deltaTime;
		x1mb_dbl_timer += *deltaTime;
		x2mb_dbl_timer += *deltaTime;

		if (g_engine->m_inputContext->m_isLMBDown)
		{
			++g_engine->m_inputContext->m_LMBClickCount;
			lmb_dbl_timer = 0.f;
		}

		if (g_engine->m_inputContext->m_isRMBDown)
		{
			++g_engine->m_inputContext->m_RMBClickCount;
			rmb_dbl_timer = 0.f;
		}

		if (g_engine->m_inputContext->m_isMMBDown)
		{
			++g_engine->m_inputContext->m_MMBClickCount;
			mmb_dbl_timer = 0.f;
		}

		if (g_engine->m_inputContext->m_isX1MBDown)
		{
			++g_engine->m_inputContext->m_X1MBClickCount;
			x1mb_dbl_timer = 0.f;
		}

		if (g_engine->m_inputContext->m_isX2MBDown)
		{
			++g_engine->m_inputContext->m_X2MBClickCount;
			x2mb_dbl_timer = 0.f;
		}

		if (lmb_dbl_timer > g_engine->m_inputContext->m_LMBMultipleClickTime)
			g_engine->m_inputContext->m_LMBClickCount = 0;

		if (rmb_dbl_timer > g_engine->m_inputContext->m_RMBMultipleClickTime)
			g_engine->m_inputContext->m_RMBClickCount = 0;

		if (mmb_dbl_timer > g_engine->m_inputContext->m_MMBMultipleClickTime)
			g_engine->m_inputContext->m_MMBClickCount = 0;

		if (x1mb_dbl_timer > g_engine->m_inputContext->m_X1MBMultipleClickTime)
			g_engine->m_inputContext->m_X1MBClickCount = 0;

		if (x2mb_dbl_timer > g_engine->m_inputContext->m_X2MBMultipleClickTime)
			g_engine->m_inputContext->m_X2MBClickCount = 0;


		return g_engine->m_state != yySystemState::Quit;
	}

	YY_API yyInputContext* YY_C_DECL yyGetInputContext() {
		return g_engine->m_inputContext;
	}

	YY_API yyResource* YY_C_DECL yyGetDefaultTexture() {
		return g_engine->m_defaultTexture;
	}

	YY_API void YY_C_DECL yySetDefaultTexture(yyResource* r) {
		if (!r->IsLoaded())
			r->Load(false);
		g_engine->m_defaultTexture = r;
	}

	YY_API void YY_C_DECL yyAddEvent(const yyEvent& event, bool unique) {
		g_engine->AddEvent(event, unique);
	}

	YY_API bool YY_C_DECL yyPollEvent(yyEvent& event) {
		return g_engine->PollEvent(event);
	}

	YY_API bool YY_C_DECL yyIsValidVideoDriver(const char* dll_file_name){
		assert(dll_file_name);
		auto lib = yyLoadLibrary(dll_file_name);
		if (!lib)
		{
			yyLogWriteWarning("%s: can't load library [%s]\n", YY_FUNCTION, dll_file_name);
			return false;
		}
		auto get_api = (Engine::videoGetApi)yyGetProcAddress(lib, "GetAPI");
		if (!get_api)
		{
			yyLogWriteWarning("%s: can't find get API function in [%s]\n", YY_FUNCTION, dll_file_name);
			yyFreeLibrary(lib);
			return false;
		}

		if (get_api()->GetAPIVersion() != yyVideoDriverAPIVersion)
		{
			yyLogWriteWarning("%s: wrong API version. File [%s] Version [%u] : need [%u]\n", YY_FUNCTION, dll_file_name,
				get_api()->GetAPIVersion(), yyVideoDriverAPIVersion);
			yyFreeLibrary(lib);
			return false;
		}
		
		yyFreeLibrary(lib);

		return true;
	}

	YY_API const wchar_t* YY_C_DECL yyGetWorkingDir(){
		assert(g_engine);
		return g_engine->m_workingDir.data();
	}

	YY_API yyString* YY_C_DECL yyGetRelativePath(const wchar_t* path){
		yyString * returnPath = yyCreate<yyString>();
		returnPath->append((const char16_t*)path);

	#ifdef YY_PLATFORM_WINDOWS
		wchar_t szOut[MAX_PATH] = L"";
		if (PathRelativePathTo(szOut, yyGetWorkingDir(), FILE_ATTRIBUTE_DIRECTORY, path, FILE_ATTRIBUTE_NORMAL)
			== TRUE)
		{
			returnPath->clear();
			returnPath->append((const char16_t*)szOut);
		}
	#else
	#error Need to implement
	#endif

		return returnPath;
	}


	YY_API yySystemState* YY_C_DECL yyStart(yyInputContext* input, yyPoolSetup* poolSetup){
		assert(!g_engine);
		g_engine = yyCreate1<Engine>(poolSetup);
		g_engine->m_inputContext = input;
		//g_engine->m_resourceManager = new yyResourceManager;
	
		g_engine->m_backgroundWorker = new std::thread(yyBackgroundWorkerFunction);
		//g_engine->m_inputContext = yyCreate<yyInputContext>();

		return &g_engine->m_state;
	}

	YY_API void YY_C_DECL yyStop(){
		YY_DEBUG_PRINT_FUNC;
		if(g_engine)
		{
			if(g_engine->m_backgroundWorker)
			{
				g_engine->m_workerCommands.put(BackgroundWorkerCommands(BackgroundWorkerCommands::type::ExitThread,0));
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

	YY_API void YY_C_DECL yyQuit(){
		YY_DEBUG_PRINT_FUNC;
		if(g_engine)
		{
			g_engine->m_state = yySystemState::Quit;
		}
	}
	YY_API void YY_C_DECL yySetCursorDisableAutoChange(bool v) {
		g_engine->m_cursorDisableAutoChange = v;
	}
	YY_API bool YY_C_DECL yyGetCursorDisableAutoChange() {
		return g_engine->m_cursorDisableAutoChange;
	}
	YY_API void YY_C_DECL yySetCursor(yyCursorType ct, yyCursor* c) {
		g_engine->m_cursors[(u32)ct] = c;
	}
	YY_API void YY_C_DECL yyResetCursor(yyCursorType ct) {
		g_engine->m_cursors[(u32)ct] = g_engine->m_cursorsDefault[(u32)ct];
	}
	YY_API yyCursor* YY_C_DECL yyGetCursor(yyCursorType ct) {
		return g_engine->m_cursors[(u32)ct];
	}

	YY_API u64 YY_C_DECL yyGetTime(){
		static bool isInit = false;
		static u64 baseTime;
		if( !isInit )
		{
			auto now = std::chrono::high_resolution_clock::now();
			baseTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

			isInit = true;
		}
		auto now = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		return ms - baseTime;
	}

//YY_API yyInputContext* YY_C_DECL yyGetInputContext()
//{
//	return g_engine->m_inputContext;
//}

	u8* Engine::compressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size){
		YY_DEBUG_PRINT_FUNC;
		u8* out_data = (u8*)yyMemAlloc(in_data_size);
		if( !out_data )
		{
			YY_PRINT_FAILED;
			return nullptr;
		}

		auto compressBound = ZSTD_compressBound(in_data_size);

		size_t const cSize = ZSTD_compressCCtx( m_cctx, out_data, compressBound, in_data, in_data_size, 1);
		if( ZSTD_isError(cSize) )
		{
			yyMemFree(out_data);
			return nullptr;
		}

		//yyMemRealloc(out_data,(u32)cSize);
		out_data_size = (u32)cSize;
		return out_data;
	}

	u8* Engine::decompressData_zstd( u8* in_data, u32 in_data_size, u32& out_data_size){
		YY_DEBUG_PRINT_FUNC;
		unsigned long long const rSize = ZSTD_getFrameContentSize(in_data, in_data_size);
		CHECK(rSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!");
		CHECK(rSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!");

		u8* out_data = (u8*)yyMemAlloc((u32)rSize);
		if( !out_data )
		{
			YY_PRINT_FAILED;
			return nullptr;
		}

		size_t const dSize = ZSTD_decompress(out_data, (size_t)rSize, in_data, in_data_size);
		out_data_size = (u32)dSize;
		return out_data;
	}

	YY_API u8* YY_C_DECL yyCompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct ){
		YY_DEBUG_PRINT_FUNC;
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

	YY_API u8* YY_C_DECL yyDecompressData( u8* in_data, u32 in_data_size, u32& out_data_size, yyCompressType ct ){
		YY_DEBUG_PRINT_FUNC;
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

	YY_API void YY_C_DECL yySetAsyncLoadEventHandler(yyAsyncLoadEventHandler f){
		g_engine->m_asyncEventHandler = f;
	}

	YY_API void YY_C_DECL yyUpdateAsyncLoader(){
		/*if(!g_engine->m_asyncEventHandler)
			return;*/

		for(int i = 0, sz = g_engine->m_workerResults.m_size; i < sz; ++i)
		{
			auto & obj = g_engine->m_workerResults.get();
			switch (obj.m_type)
			{
			default:
				break;
			case BackgroundWorkerResults::type::None:
				break;
			case BackgroundWorkerResults::type::LoadSource:
				//g_engine->m_asyncEventHandler( obj.m_id, obj.m_resource );
				obj.m_resource->LoadImplementation();
				yyLogWriteInfo("Async LoadImplementation\n");
				obj.m_resource->DestroySource();
				break;
			}
		}
	}
	
	YY_API yyVideoDriverAPI* YY_C_DECL yyGetVideoDriverAPI(){
		return g_engine->m_videoAPI;
	}

	YY_API bool YY_C_DECL yyInitVideoDriver(const char* dl, yyWindow* window){
		YY_DEBUG_PRINT_FUNC;
		if(g_engine)
		{
			if(g_engine->m_videoDriverLib)
			{
				yyLogWriteWarning("Video driver already initialized\n");
				return false;
			}

			g_engine->m_videoDriverLib = yyLoadLibrary(dl);
			if(!g_engine->m_videoDriverLib)
			{
				yyLogWriteWarning("Can't load video driver library\n");
				yyFreeLibrary(g_engine->m_videoDriverLib);
				g_engine->m_videoDriverLib = nullptr;
				return false;
			}

			g_engine->m_videoDriverGetApi = (Engine::videoGetApi)yyGetProcAddress(g_engine->m_videoDriverLib, "GetAPI");
			if(!g_engine->m_videoDriverGetApi)
			{
				yyLogWriteWarning("Can't get GetApi function from video driver library\n");
				yyFreeLibrary(g_engine->m_videoDriverLib);
				g_engine->m_videoDriverLib = nullptr;
				return false;
			}

			g_engine->m_videoAPI = g_engine->m_videoDriverGetApi();
			if(!g_engine->m_videoAPI)
			{
				yyLogWriteWarning("Can't get video driver API\n");
				yyFreeLibrary(g_engine->m_videoDriverLib);
				g_engine->m_videoDriverLib = nullptr;
				return false;
			}

			if(g_engine->m_videoAPI->GetAPIVersion() != yyVideoDriverAPIVersion)
			{
				yyLogWriteWarning("Wrong Video Driver API version; %u, need %u\n", g_engine->m_videoAPI->GetAPIVersion(), yyVideoDriverAPIVersion);
				yyFreeLibrary(g_engine->m_videoDriverLib);
				g_engine->m_videoDriverLib = nullptr;
				return false;
			}

			//g_engine->m_resourceManager->m_videoDriverAPI = g_engine->m_videoAPI;

			auto result = g_engine->m_videoAPI->Init(window);
			if (!result)
			{
				yyFreeLibrary(g_engine->m_videoDriverLib);
				g_engine->m_videoDriverLib = nullptr;
				return false;
			}
			return result;
		}
		return false;
	}

	YY_API void YY_C_DECL yySetMainWindow(yyWindow* w){
		g_engine->m_mainWindow = w;
	}
	YY_API yyWindow* YY_C_DECL yyGetMainWindow(){
		return g_engine->m_mainWindow;
	}

	YY_API yyString* YY_C_DECL yySaveFileDialog(const char* title, const char* okButtonLabel,
		const char* extension)
	{
		assert(g_engine);
		assert(g_engine->m_mainWindow);
		assert(title);
		assert(okButtonLabel);
		yyString * returnPath = 0;
		yyStringW titleW;
		titleW = title;
		yyStringW okButtonLabelW;
		okButtonLabelW = okButtonLabel;
		yyStringW extensionTitleW;
		extensionTitleW = extension;
	#ifdef YY_PLATFORM_WINDOWS
		g_engine->m_fileSaveDialog->SetTitle(titleW.data());
		g_engine->m_fileSaveDialog->SetOkButtonLabel(okButtonLabelW.data());
		COMDLG_FILTERSPEC rgSpec;
		rgSpec.pszName = extensionTitleW.data();

		yyStringW wstr;
		wstr = "*.";
		wstr += extension;
		rgSpec.pszSpec = wstr.data();

		g_engine->m_fileSaveDialog->SetFileTypes(1, &rgSpec);
		auto hr = g_engine->m_fileSaveDialog->Show(g_engine->m_mainWindow->m_hWnd);
		if (SUCCEEDED(hr))
		{
			IShellItem *pItem;
			hr = g_engine->m_fileSaveDialog->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				if (SUCCEEDED(hr))
				{
					returnPath = yyCreate<yyString>();
					returnPath->append((const char16_t*)pszFilePath);
					CoTaskMemFree(pszFilePath);
				}
				pItem->Release();
			}
		}
		g_engine->m_fileSaveDialog->Release();
		CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&g_engine->m_fileSaveDialog));
	#else
	#error Need to implement
	#endif
		return returnPath;
	}

	YY_API yyString* YY_C_DECL yyOpenFileDialog(const char* title, const char* okButtonLabel,
		const char* extensions, const char* extensionTitle)
	{
		assert(g_engine);
		assert(g_engine->m_mainWindow);
		assert(title);
		assert(okButtonLabel);
		assert(extensions);
		assert(extensionTitle);

		std::vector<yyString> extensions_array;
		util::stringGetWords<yyString>(&extensions_array, yyString(extensions));

		yyStringW titleW;
		yyStringW okButtonLabelW;
		yyStringW extensionTitleW;
		titleW = title;
		okButtonLabelW = okButtonLabel;
		extensionTitleW = extensionTitle;

		yyString * returnPath = 0;

	#ifdef YY_PLATFORM_WINDOWS
		g_engine->m_fileOpenDialog->SetTitle(titleW.data());
		g_engine->m_fileOpenDialog->SetOkButtonLabel(okButtonLabelW.data());

		COMDLG_FILTERSPEC rgSpec;
		yyStringW wstr;
		for (u32 i = 0, sz = (u32)extensions_array.size(); i < sz; ++i)
		{
			wstr += L"*.";
			wstr += extensions_array[i].data();
			if (i < sz - 1)
				wstr += L";";
		}
		rgSpec.pszName = extensionTitleW.data();
		rgSpec.pszSpec = wstr.data();
		g_engine->m_fileOpenDialog->SetFileTypes(1, &rgSpec);
		auto hr = g_engine->m_fileOpenDialog->Show(g_engine->m_mainWindow->m_hWnd);
		if (SUCCEEDED(hr))
		{
			IShellItem *pItem;
			hr = g_engine->m_fileOpenDialog->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				if (SUCCEEDED(hr))
				{
					returnPath = yyCreate<yyString>();
					returnPath->append( (const char16_t*)pszFilePath );
					CoTaskMemFree(pszFilePath);
				}
				pItem->Release();
			}
		}
		g_engine->m_fileOpenDialog->Release();
		CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&g_engine->m_fileOpenDialog));
	#else
	#error Need to implement
	#endif

		return returnPath;
	}

}