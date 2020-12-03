#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"

#include "yy_model.h"
#include "yy_input.h"

#include "engine.h"

extern Engine * g_engine;

yyGUIPictureBox::yyGUIPictureBox()
{
	m_type = yyGUIElementType::PictureBox;
}

yyGUIPictureBox::~yyGUIPictureBox()
{
	auto vAPI = yyGetVideoDriverAPI();
	if( this->m_pictureBoxModel )
		vAPI->UnloadModel(this->m_pictureBoxModel);
	
	if( this->m_texture )
		vAPI->UnloadTexture(this->m_texture);
}

bool pointInRect( float x, float y, const v4f& rect )
{
	if(x>rect.x){if(x<rect.z){if(y>rect.y){if(y<rect.w){return true;}}}}
	return false;
}

YY_API void YY_C_DECL yyGUIUpdate(f32 deltaTime)
{
	auto guiElement = g_engine->m_guiElements.head();
	for(size_t i = 0, sz = g_engine->m_guiElements.size(); i < sz; ++i)
	{
		switch (guiElement->m_data->m_type)
		{
		default:
			break;
		case yyGUIElementType::PictureBox:
		{
			yyGUIPictureBox* pictureBox = (yyGUIPictureBox*)guiElement->m_data;
			bool inRect = pointInRect(
				g_engine->m_inputContext->m_cursorCoords.x, 
				g_engine->m_inputContext->m_cursorCoords.y, 
				pictureBox->m_rect);

			if(pictureBox->m_onClick && g_engine->m_inputContext->m_isLMBDown)
			{
				if(inRect)
					pictureBox->m_onClick(guiElement->m_data, guiElement->m_data->m_id);
			}
		}break;
		}
		guiElement = guiElement->m_right;
	}
}

YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id)
{
	yyGUIPictureBox* element = yyCreate<yyGUIPictureBox>();
	element->m_rect = rect;
	element->m_texture = texture;
	element->m_id = id;

	
	auto vAPI = yyGetVideoDriverAPI();

	auto model = yyCreate<yyModel>();
	
	auto meshBuffer = yyCreate<yyMeshBuffer>();

	meshBuffer->m_iCount = 6;
	meshBuffer->m_vCount = 4;
	meshBuffer->m_stride = sizeof(yyVertexGUI);
	meshBuffer->m_vertexType = yyVertexType::GUI;
	meshBuffer->m_vertices = (u8*)yyMemAlloc(meshBuffer->m_vCount * meshBuffer->m_stride);
	meshBuffer->m_indices  = (u16*)yyMemAlloc(meshBuffer->m_iCount * sizeof(u16));

	yyVertexGUI * vertex = (yyVertexGUI*)meshBuffer->m_vertices;
	vertex->m_position.set(rect.x, rect.w, 0.f);
	vertex->m_tcoords.set(0.f,1.f);
	vertex++;
	vertex->m_position.set(rect.x, rect.y, 0.f);
	vertex->m_tcoords.set(0.f,0.f);
	vertex++;
	vertex->m_position.set(rect.z, rect.y, 0.f);
	vertex->m_tcoords.set(1.f,0.f);
	vertex++;
	vertex->m_position.set(rect.z, rect.w, 0.f);
	vertex->m_tcoords.set(1.f,1.f);
	vertex++;

	meshBuffer->m_indices[0] = 0;
	meshBuffer->m_indices[1] = 1;
	meshBuffer->m_indices[2] = 2;
	meshBuffer->m_indices[3] = 0;
	meshBuffer->m_indices[4] = 2;
	meshBuffer->m_indices[5] = 3;

	model->m_meshBuffers.push_back(meshBuffer);
	element->m_pictureBoxModel = vAPI->CreateModel(model);

	yyDestroy(model);

	g_engine->addGuiElement(element);
	//g_engine->m_videoAPI->CreateMesh();

	return element;
}

YY_API void YY_C_DECL yyGUIDrawAll()
{
	g_engine->m_videoAPI->BeginDrawGUI();
	
	auto guiElement = g_engine->m_guiElements.head();
	for(size_t i = 0, sz = g_engine->m_guiElements.size(); i < sz; ++i)
	{
		switch (guiElement->m_data->m_type)
		{
		default:
			break;
		case yyGUIElementType::PictureBox:
		{
			yyGUIPictureBox* pictureBox = (yyGUIPictureBox*)guiElement->m_data;
			if(pictureBox->m_texture)
				g_engine->m_videoAPI->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, pictureBox->m_texture);
			if(pictureBox->m_pictureBoxModel)
				g_engine->m_videoAPI->SetModel(pictureBox->m_pictureBoxModel);
			g_engine->m_videoAPI->Draw();
		}break;
		}
		guiElement = guiElement->m_right;
	}

	g_engine->m_videoAPI->EndDrawGUI();
}

