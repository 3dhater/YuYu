#include "demo.h"
#include "demo_example.h"
#include "demo_example_lines.h"

#include "yy_input.h"
#include "yy_gui.h"
#include "yy_model.h"

extern Demo* g_demo;

DemoExample_Lines::DemoExample_Lines(){
	m_lineModel = 0;
	m_editorCamera = 0;
}

DemoExample_Lines::~DemoExample_Lines(){
	Shutdown();
}

bool DemoExample_Lines::Init(){
	yyModel model;
	model.m_stride = sizeof(yyVertexLineModel);
	model.m_vertexType = yyVertexType::LineModel;
	
	model.m_vCount = 20000;
	model.m_vertices = (u8*)yyMemAlloc(model.m_vCount * model.m_stride);
	
	yyVertexLineModel* vertex = (yyVertexLineModel*)model.m_vertices;

	v4f colorWhite(1.f);
	v4f colorRed(1.f, 0.f, 0.f, 1.f);
	v4f colorGreen(0.f, 1.f, 0.f, 1.f);

	f32 pos = -2500.f;
	for (s32 i = 0; i < 5000; ++i)
	{
		v4f color = colorWhite;

		if(i == 2500)
			color = colorGreen;

		vertex->Position.set(pos,0.f,-10000);
		vertex->Color = color;
		vertex++;
		vertex->Position.set(pos, 0.f, 10000);
		vertex->Color = color;
		vertex++;

		pos += 1.f;
	}

	pos = -2500.f;
	for (s32 i = 0; i < 5000; ++i)
	{
		v4f color = colorWhite;

		if (i == 2500)
			color = colorRed;

		vertex->Position.set(-10000, 0.f, pos);
		vertex->Color = color;
		vertex++;
		vertex->Position.set(10000, 0.f, pos);
		vertex->Color = color;
		vertex++;

		pos += 1.f;
	}



	model.m_iCount = 20000;
	model.m_indices = (u8*)yyMemAlloc(model.m_iCount * sizeof(u16));
	u16* index = (u16*)model.m_indices;
	for (s32 i = 0; i < model.m_iCount; ++i)
	{
		*index = (u16)i;
		index++;
	}

	m_lineModel = m_gpu->CreateModel(&model);
	m_lineModelMaterial.SetFogStart(0.996f);
	

	m_editorCamera = yyCreate<yyOrbitCamera>();
	m_editorCamera->Reset();

	yyColor clColor;
	clColor.setAsByteRed(118);
	clColor.setAsByteGreen(118);
	clColor.setAsByteBlue(118);
	m_lineModelMaterial.SetFogColor(clColor);

	m_gpu->SetClearColor(clColor.m_data[0], clColor.m_data[1], clColor.m_data[2], 1.f);

	return true;
}


void DemoExample_Lines::Shutdown(){
	if (m_lineModel)
	{
		m_gpu->DeleteModel(m_lineModel);
		m_lineModel = 0;
	}
	if (m_editorCamera)
	{
		yyDestroy(m_editorCamera);
		m_editorCamera = 0;
	}
}

const wchar_t* DemoExample_Lines::GetTitle(){
	return L"Lines";
}

const wchar_t* DemoExample_Lines::GetDescription(){
	return L"Line model";
}

bool DemoExample_Lines::DemoStep(f32 deltaTime){
	m_editorCamera->Update();
	if (g_demo->m_inputContext->m_isMMBHold)
	{
		if (g_demo->m_inputContext->IsKeyHold(yyKey::K_LALT))
			m_editorCamera->Rotate(g_demo->m_inputContext->m_mouseDelta, g_demo->m_dt);
		else
			m_editorCamera->PanMove(g_demo->m_inputContext->m_mouseDelta, g_demo->m_dt);
	}

	if (g_demo->m_inputContext->IsKeyHold(yyKey::K_LALT))
	{
		if (g_demo->m_inputContext->m_isLMBHold)
			m_editorCamera->RotateZ(g_demo->m_inputContext->m_mouseDelta.x, g_demo->m_dt);
		if (g_demo->m_inputContext->m_isRMBHold)
			m_editorCamera->ChangeFOV(g_demo->m_inputContext->m_mouseDelta.x, g_demo->m_dt);
	}

	if(g_demo->m_inputContext->m_wheelDelta)
		m_editorCamera->Zoom(g_demo->m_inputContext->m_wheelDelta);

	Mat4 WorldMatrix;
	
	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, WorldMatrix);
	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection,
		m_editorCamera->m_projectionMatrix * m_editorCamera->m_viewMatrix * WorldMatrix);

	m_gpu->SetModel(m_lineModel);
	m_gpu->SetMaterial(&m_lineModelMaterial);
	m_gpu->Draw();

	return g_demo->m_inputContext->IsKeyHit(yyKey::K_ESCAPE) == false;
}