#include "yy.h"
#include "scene\common.h"
#include "scene\mdl_object.h"

#include "MDLCreator_common.h"
#include "SceneObject.h"

extern yyVideoDriverAPI* g_videoDriver;

Hitbox::Hitbox()
{
	m_min.set(-0.1f, -0.1f, -0.1f);
	m_max.set(0.1f, 0.1f, 0.1f);

	m_type = type::Box;
	m_gpuModel = 0;

	m_material.m_wireframe = true;
	m_material.m_baseColor = ColorLime;
}
Hitbox::~Hitbox()
{
	if (m_gpuModel) g_videoDriver->DeleteModel(m_gpuModel);
}

void Hitbox::rebuild()
{
	if (m_gpuModel) g_videoDriver->DeleteModel(m_gpuModel);
	if (m_hitbox.m_mesh) yyDestroy(m_hitbox.m_mesh);

	if (m_type == type::Box)
	{
		m_hitbox.m_mesh = yyCreate<yyModel>();
		m_hitbox.m_mesh->m_vertexType = yyVertexType::Model;
		m_hitbox.m_mesh->m_indexType = yyMeshIndexType::u16;
		m_hitbox.m_mesh->m_stride = sizeof(yyVertexModel);
		m_hitbox.m_mesh->m_vCount = 8;
		m_hitbox.m_mesh->m_iCount = 36;
		m_hitbox.m_mesh->m_vertices = (u8*)yyMemAlloc(m_hitbox.m_mesh->m_vCount * m_hitbox.m_mesh->m_stride);
		m_hitbox.m_mesh->m_indices = (u8*)yyMemAlloc(m_hitbox.m_mesh->m_iCount * sizeof(u16));

		yyVertexModel* vertex = (yyVertexModel*)m_hitbox.m_mesh->m_vertices;
		u16* index = (u16*)m_hitbox.m_mesh->m_indices;

		vertex[0].Position.set(m_min.x, m_min.y, m_min.z);
		vertex[1].Position.set(m_min.x, m_min.y, m_max.z);
		vertex[2].Position.set(m_max.x, m_min.y, m_min.z);
		vertex[3].Position.set(m_max.x, m_min.y, m_max.z);

		vertex[4].Position.set(m_min.x, m_max.y, m_min.z);
		vertex[5].Position.set(m_min.x, m_max.y, m_max.z);
		vertex[6].Position.set(m_max.x, m_max.y, m_min.z);
		vertex[7].Position.set(m_max.x, m_max.y, m_max.z);

		// Bottom
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 2;
		index[4] = 1;
		index[5] = 3;

		// Top
		index[6] = 4;
		index[7] = 6;
		index[8] = 5;
		index[9] = 6;
		index[10] = 7;
		index[11] = 5;

		// Left
		index[12] = 3;
		index[13] = 6;
		index[14] = 2;
		index[15] = 3;
		index[16] = 7;
		index[17] = 6;

		// Right
		index[18] = 0;
		index[19] = 5;
		index[20] = 1;
		index[21] = 0;
		index[22] = 4;
		index[23] = 5;

		// front
		index[24] = 2;
		index[25] = 4;
		index[26] = 0;
		index[27] = 2;
		index[28] = 6;
		index[29] = 4;

		// back
		index[30] = 3;
		index[31] = 5;
		index[32] = 1;
		index[33] = 3;
		index[34] = 7;
		index[35] = 5;

		m_gpuModel = g_videoDriver->CreateModel(m_hitbox.m_mesh);
	}
}

SceneObject::SceneObject()
{
	m_mdlObject = 0;
}
SceneObject::~SceneObject()
{
	for (u16 i = 0, sz = m_hitboxes.size(); i < sz; ++i)
	{
		yyDestroy(m_hitboxes[i]);
	}
	for (u16 i = 0, sz = m_MDLObjectStates.size(); i < sz; ++i)
	{
		yyDestroy(m_MDLObjectStates[i]);
	}
	if (m_mdlObject)
	{
		yyDestroy(m_mdlObject);
	}
}
bool SceneObject::init()
{
	m_mdlObject = yyCreate<yyMDLObject>();
	m_mdlObject->m_mdl = yyCreate<yyMDL>();
	return true;
}
void SceneObject::update(f32 dt)
{
	m_mdlObject->Update(dt);
}
yyMDLObjectState* SceneObject::getState(const char* name)
{
	for (u16 i = 0; i < m_MDLObjectStates.size(); ++i)
	{
		if (strcmp(m_MDLObjectStates[i]->m_name.data(), name) == 0)
			return m_MDLObjectStates[i];
	}
	return nullptr;
}

