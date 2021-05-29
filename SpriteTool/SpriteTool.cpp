#include "SpriteTool.h"

SpriteTool* g_app = 0;

void log_onError(const char* message) { fprintf(stderr, message); }
void log_onInfo(const char* message) { fprintf(stdout, message); }
void window_onCLose(yyWindow* window) {
	window->Hide();
	yyQuit(); // change yySystemState - set yySystemState::Quit
}

void loadImage_onRelease(yyGUIElement* elem, s32 m_id) {
	g_app->LoadImage();
}

SpriteTool::SpriteTool() {
	m_texture = 0;
	m_inputContext = 0;
	m_engineContext = 0;
	m_window = 0;
	m_gpu = 0;
	m_dt = 0.f;
	m_buttonLoadImage = 0;
	m_defaultFont = 0;
	g_app = this;
}

SpriteTool::~SpriteTool() {
	_deleteTexture();
	if (m_window) yyDestroy(m_window);
	if (m_engineContext) yyDestroy(m_engineContext);
	if (m_inputContext) yyDestroy(m_inputContext);
}

bool SpriteTool::Init(const char* videoDriver) {
	m_inputContext = yyCreate<yyInputContext>();
	m_engineContext = yyCreate<yyEngineContext>();
	m_engineContext->init(m_inputContext);

	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onError);

	m_window = yyCreate<yyWindow>();
	u32 windowStyle = 0;
	if (!m_window->init(800, 600, windowStyle))
	{
		YY_PRINT_FAILED;
		return false;
	}

	yySetMainWindow(m_window);
	m_window->m_onClose = window_onCLose;

	if (!yyInitVideoDriver(videoDriver, m_window))
	{
		yyLogWriteWarning("Can't load video driver : %s\n", videoDriver);
		for (auto & entry : yy_fs::directory_iterator(yy_fs::current_path()))
		{
			auto path = entry.path();
			if (path.has_extension())
			{
				auto ex = path.extension();
				if (ex == ".dll" && yyIsValidVideoDriver(path.generic_string().c_str()))
				{
					yyLogWriteWarning("Trying to load video driver : %s\n", path.generic_string().c_str());

					if (yyInitVideoDriver(path.generic_string().c_str(), m_window))
					{
						goto vidOk;
					}
					else
					{
						yyLogWriteWarning("Can't load video driver : %s\n", path.generic_string().c_str());
					}
				}
			}
		}
		YY_PRINT_FAILED;
		return false;
	}

vidOk:

	m_gpu = yyGetVideoDriverAPI();
	m_gpu->GetDepthRange(&m_gpuDepthRange);
	m_gpu->UseVSync(true);
	yySetDefaultTexture(yyGetTextureFromCache("../res/gui/white.dds"));

	m_gpu->SetClearColor(0.7f, 0.7f, 0.7f, 1.f);
	m_window->SetTitle("SpriteTool");

	m_defaultFont = yyGUILoadFont("../res/fonts/Noto/notosans.txt");
	if (!m_defaultFont)
	{
		YY_PRINT_FAILED;
		return false;
	}

	m_buttonLoadImage = yyGUICreateButton(v4f(0.f, 0.f, 70.f, 20.f), 0, -1, 0, 0);
	m_buttonLoadImage->SetColor(yyColor(0.4f), 0);
	m_buttonLoadImage->SetColor(yyColor(0.5f), 1);
	m_buttonLoadImage->SetColor(yyColor(0.3f), 2);
	m_buttonLoadImage->SetText(L"Load image", m_defaultFont, false);
	m_buttonLoadImage->SetTextColor(ColorWhite);
	m_buttonLoadImage->m_isAnimated = true;
	m_buttonLoadImage->m_onRelease = loadImage_onRelease;

	yyGUIRebuild();
	return true;
}

void SpriteTool::MainLoop() {
	f32 fps_timer = 0.f;
	u32 fps = 0;
	u32 fps_counter = 0;

	yyEvent currentEvent;

	while (yyRun(&m_dt))
	{
		//	yySetCursor(yyCursorType::Arrow, m_cursors[(u32)yyCursorType::Arrow]);

		m_isCursorMove = (m_inputContext->m_mouseDelta.x != 0.f) || (m_inputContext->m_mouseDelta.y != 0.f);


		m_isCursorInWindow = false;

		if (math::pointInRect(m_inputContext->m_cursorCoords.x, m_inputContext->m_cursorCoords.y,
			v4f(0.f, 0.f, (f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y)))
		{
			m_isCursorInWindow = true;
		}

		++fps_counter;
		fps_timer += m_dt;

		if (fps_timer > 1.f)
		{
			fps_timer = 0.f;
			fps = fps_counter;
			fps_counter = 0;
		}

		m_isCursorInGUI = yyGUIUpdate(m_dt);
		m_isGUIInputFocus = yyGUIIsInputFocus();

		while (yyPollEvent(currentEvent))
		{
			switch (currentEvent.m_type)
			{
			default:
			case yyEventType::Engine:
				break;
			case yyEventType::System:
				break;
			case yyEventType::Window: {
				if (currentEvent.m_event_window.m_event == yyEvent_Window::size_changed) {
					yyGetVideoDriverAPI()->UpdateMainRenderTarget(m_window->m_currentSize,
						v2f((f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y));
					yyGUIRebuild();
				}
			}break;
			case yyEventType::User: {
				switch (currentEvent.m_event_user.m_id)
				{
				default:
					break;
				}
			}break;
			}
		}

		switch (*m_engineContext->m_state)
		{
		default:
			break;
		case yySystemState::Run:
		{
			m_gpu->BeginDraw();
			m_gpu->ClearAll();
			yyGUIDrawAll(m_dt);
			m_gpu->EndDraw();
			m_gpu->SwapBuffers();
		}
		}
	}
}

void SpriteTool::LoadImage() {
	auto path = yyOpenFileDialog("Load image", "Load", "png dds tga", "Supported files");
	if (path)
	{
		yyStringA stra;
		stra += path->data();
		_deleteTexture();
		m_texture = yyCreateTextureFromFile(stra.data());
		yyDestroy(path);
	}
}

void SpriteTool::_deleteTexture() {
	if (m_texture)
	{
		yyMegaAllocator::Destroy(m_texture);
		m_texture = 0;
	}
}