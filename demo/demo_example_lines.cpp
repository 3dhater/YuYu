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
	m_pointModel = 0;
}

DemoExample_Lines::~DemoExample_Lines(){
	Shutdown();
}

bool DemoExample_Lines::Init(){
	{
		yyModel model;
		model.m_stride = sizeof(yyVertexPoint);
		model.m_vertexType = yyVertexType::Point;

		yyArray<v3f> points;
		points.push_back(v3f(-1.f, 0.f, 0.f));
		points.push_back(v3f(1.f, 0.f, 0.f));
		points.push_back(v3f(0.f, 0.f, 2.f));

		// 1 point = 4 verts
		model.m_vCount = points.size() * 4;
		model.m_vertices = (u8*)yyMemAlloc(model.m_vCount * model.m_stride);

		yyVertexPoint* vertex = (yyVertexPoint*)model.m_vertices;
		
		f32 size = 0.1f;

		for (u32 i = 0, sz = points.size(); i < sz; ++i)
		{
			auto & point = points[i];

			vertex->WorldPosition = point;
			vertex->Color.set(1.f,0.f,0.f,1.f);
			vertex->Position.set(-size, -size, 0.f);
			vertex++;
			
			vertex->WorldPosition = point;
			vertex->Color.set(1.f, 0.f, 0.f, 1.f);
			vertex->Position.set(-size, size, 0.f);
			vertex++;
			
			vertex->WorldPosition = point;
			vertex->Color.set(1.f, 0.f, 0.f, 1.f);
			vertex->Position.set(size, size, 0.f);
			vertex++;

			vertex->WorldPosition = point;
			vertex->Color.set(1.f, 0.f, 0.f, 1.f);
			vertex->Position.set(size, -size, 0.f);
			vertex++;
		}

		model.m_iCount = points.size() * 6;
		model.m_indices = (u8*)yyMemAlloc(model.m_iCount * sizeof(u16));
		
		u16* index = (u16*)model.m_indices;
		
		u16 index_base = 0;
		u16 index_count = 1;
		for (u32 i = 0; i < model.m_iCount;)
		{
			*index = index_base; index++;
			*index = index_count; index++;
			*index = index_count+1; index++;
			*index = index_base; index++;
			*index = index_count+1; index++;
			*index = index_count+2; index++;

			index_base = index_count + 3;
			index_count = index_base + 1;

			i += 6;
		}

		m_pointModel = yyCreateModel(&model);
		m_pointModel->Load();
	}
	yyModel model;
	model.m_stride = sizeof(yyVertexLine);
	model.m_vertexType = yyVertexType::LineModel;
	
	model.m_vCount = 44;
	model.m_vertices = (u8*)yyMemAlloc(model.m_vCount * model.m_stride);
	
	yyVertexLine* vertex = (yyVertexLine*)model.m_vertices;

	v4f colorWhite(1.f);
	v4f colorRed(1.f, 0.f, 0.f, 1.f);
	v4f colorGreen(0.f, 1.f, 0.f, 1.f);

	f32 pos = -5.f;
	for (s32 i = 0; i < 11; ++i)
	{
		v4f color = colorWhite;

		if(i == 5)
			color = colorGreen;

		vertex->Position.set(pos,0.f,-5);
		vertex->Normal.set(0.f, 1.f, 0.f);
		vertex->Color = color;
		vertex++;
		vertex->Position.set(pos, 0.f, 5);
		vertex->Normal.set(0.f, 1.f, 0.f);
		vertex->Color = color;
		vertex++;

		pos += 1.f;
	}

	pos = -5.f;
	for (s32 i = 0; i < 11; ++i)
	{
		v4f color = colorWhite;

		if (i == 5)
			color = colorRed;

		vertex->Position.set(-5, 0.f, pos);
		vertex->Normal.set(0.f, 1.f, 0.f);
		vertex->Color = color;
		vertex++;
		vertex->Position.set(5, 0.f, pos);
		vertex->Normal.set(0.f, 1.f, 0.f);
		vertex->Color = color;
		vertex++;

		pos += 1.f;
	}



	model.m_iCount = 44;
	model.m_indices = (u8*)yyMemAlloc(model.m_iCount * sizeof(u16));
	u16* index = (u16*)model.m_indices;
	for (u32 i = 0; i < model.m_iCount; ++i)
	{
		*index = (u16)i;
		index++;
	}

	m_lineModel = yyCreateModel(&model);
	m_lineModel->Load();
	m_lineModelMaterial.SetFogStart(1.f);
	

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
	if (m_pointModel)
	{
		yyMegaAllocator::Destroy(m_pointModel);
		m_pointModel = 0;
	}
	if (m_lineModel)
	{
		yyMegaAllocator::Destroy(m_lineModel);
		m_lineModel = 0;
	}
	if (m_editorCamera)
	{
		yyDestroy(m_editorCamera);
		m_editorCamera = 0;
	}
}

const wchar_t* DemoExample_Lines::GetTitle(){
	return L"Lines/points";
}

const wchar_t* DemoExample_Lines::GetDescription(){
	return L"Line/point model";
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
	
	// camera direction
	//v3f d(0.f, 0.f, 1.f);
	//d = -math::mulBasis(d, m_editorCamera->m_viewMatrix);
	//printf("%f %f %f\n", d.x, d.y, d.z);

	yySetMatrix(yyMatrixType::World, WorldMatrix);
	yySetMatrix(yyMatrixType::WorldViewProjection,
		m_editorCamera->m_projectionMatrix * m_editorCamera->m_viewMatrix * WorldMatrix);

	m_gpu->SetModel(m_lineModel);
	m_gpu->SetMaterial(&m_lineModelMaterial);
	m_gpu->Draw();
	
	//m_lineModelMaterial.m_cullBackFace = true;
	//m_gpu->SetMaterial(&m_lineModelMaterial);
	//m_editorCamera->m_viewMatrix.m_data[3].set(0.f, 0.f, 0.f, 1.f);
	/*printf("%f %f %f %f ; %f %f %f %f ; %f %f %f %f ; %f %f %f %f \n", 
		m_editorCamera->m_viewMatrix.m_data[0].x, m_editorCamera->m_viewMatrix.m_data[0].y, m_editorCamera->m_viewMatrix.m_data[0].z, m_editorCamera->m_viewMatrix.m_data[0].w,
		m_editorCamera->m_viewMatrix.m_data[1].x, m_editorCamera->m_viewMatrix.m_data[1].y, m_editorCamera->m_viewMatrix.m_data[1].z, m_editorCamera->m_viewMatrix.m_data[1].w,
		m_editorCamera->m_viewMatrix.m_data[2].x, m_editorCamera->m_viewMatrix.m_data[2].y, m_editorCamera->m_viewMatrix.m_data[2].z, m_editorCamera->m_viewMatrix.m_data[2].w,
		m_editorCamera->m_viewMatrix.m_data[3].x, m_editorCamera->m_viewMatrix.m_data[3].y, m_editorCamera->m_viewMatrix.m_data[3].z, m_editorCamera->m_viewMatrix.m_data[3].w);*/
	math::makeScaleMatrix(v4f(1.f),WorldMatrix);
	WorldMatrix.m_data[3].set(0.f, 0.f, 0.f, 1.f);
	
	auto Vi = m_editorCamera->m_viewMatrix;
	Vi.m_data[3].set(0.f,0.f,0.f,1.f);
	Vi.invert();

	yySetMatrix(yyMatrixType::World, WorldMatrix );
	yySetMatrix(yyMatrixType::View, m_editorCamera->m_viewMatrix);


	yySetMatrix(yyMatrixType::ViewInvert, Vi);
	yySetMatrix(yyMatrixType::Projection, m_editorCamera->m_projectionMatrix);
	m_gpu->SetModel(m_pointModel);
	m_gpu->Draw();
	
	yySetMatrix(yyMatrixType::ViewProjection, m_editorCamera->m_viewProjectionMatrix);
	m_gpu->DrawLine3D(v4f(0.f), v4f(10.f,0.f,10.f,0.f), ColorYellow);

	return g_demo->m_inputContext->IsKeyHit(yyKey::K_ESCAPE) == false;
}