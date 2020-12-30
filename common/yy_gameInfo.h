#ifndef __YYGAMEINFO_H__
#define __YYGAMEINFO_H__

#define GAME_TOOL
#include "io\xml_io.h"
#include "scene/common.h"

#include "math\aabb.h"

struct yyGameEntity
{
	yyGameEntity(){
		m_modelForRayIntersection = nullptr;
		m_modelForGPU = nullptr;
		m_textureGPU = 0;
	}
	~yyGameEntity(){
		if(m_modelForRayIntersection) yyDeleteModel(m_modelForRayIntersection);
		if (m_modelForGPU)            yyGetVideoDriverAPI()->UnloadModel(m_modelForGPU);
		if (m_textureGPU) yyGetVideoDriverAPI()->UnloadTexture(m_textureGPU);
	}

	yyString								m_name;      // имя name="palyer_start"
	v4f										m_entityPosition; // каждая энтити имеет как минимум позицию
	yySceneObjectBase m_base;

	struct _bv{
		_bv() { m_radius = 0.f; }
		f32 m_radius;
		Aabb m_aabb;
		void update() { m_radius = m_aabb.radius(m_aabb.m_min) * 0.5f; }
	}m_bv;

	yyModel* m_modelForRayIntersection;
	yyResource* m_modelForGPU;
	yyResource* m_textureGPU;

	yyString m_visualAttrValue;
	yyString m_tdAttrValue;

	void copyFrom(yyGameEntity * other)
	{
		m_name = other->m_name;
		m_entityPosition = other->m_entityPosition;
		m_visualAttrValue = other->m_visualAttrValue;
		m_tdAttrValue = other->m_tdAttrValue;
		m_base = other->m_base;

		m_modelForGPU = other->m_modelForGPU;
		if (m_modelForGPU) ++m_modelForGPU->m_refCount; // m_refCount уменьшается при удалении энтити

		m_modelForRayIntersection = other->m_modelForRayIntersection;
		if (m_modelForRayIntersection) ++m_modelForRayIntersection->m_refCount;
		else{

			yyString modelPath("../res/models/");

			if (m_visualAttrValue.size())
			{
				auto p = modelPath;
				p += m_visualAttrValue;
				if (yyFS::exists(p.data()))
					modelPath = p;
				else
					modelPath = "../res/models/editor/cubeent.tr3d";
			}
			else
				modelPath = "../res/models/editor/cubeent.tr3d";

			//if (strcmp(other->m_visual_model_type.c_str(), "cube") == 0)
			{
				m_modelForRayIntersection = yyGetModel(modelPath.to_stringA().c_str());
				m_modelForGPU = yyGetModelResource(modelPath.to_stringA().c_str(), true);
			}
		}

		if (m_tdAttrValue.size())
		{
			yyString texturePath("../res/textures/");
			texturePath += m_tdAttrValue;
			if (yyFS::exists(texturePath.data()))
				m_textureGPU = yyGetTextureResource(texturePath.to_stringA().data(), true, false, true);
		}

		m_bv.m_aabb.reset();
		if (m_modelForRayIntersection){
			for (u16 i = 0, sz = m_modelForRayIntersection->m_meshBuffers.size(); i < sz; ++i){
				auto mb = m_modelForRayIntersection->m_meshBuffers[i];
				f32 * V = (f32 *)&mb->m_vertices[0];
				u32 stride = mb->m_stride / 4;
				for (u32 o = 0; o < mb->m_vCount; ++o){
					m_bv.m_aabb.add(v3f(V[0], V[1], V[2]));
					V += stride;
				}
			}
		}
		m_bv.update();
	}
};

// правильнее будет если игра не будет знать о yyGameInfo
// это всего лишь инструмент для сборки карты в редакторе
class yyGameInfo
{
public:
	yyGameInfo(){}
	~yyGameInfo(){
		for (u16 i = 0, sz = m_ent_list.size(); i < sz; ++i){
			yyDestroy(m_ent_list[i]);
		}
		m_ent_list.clear();
		clear();
	}

	bool Init()
	{
		if (!yyFS::exists("game_info.xml")){
			yyLogWriteError("game_info.xml not found\n");
			YY_PRINT_FAILED;
			return false;
		}
		yyPtr<kkXMLDocument> xml = yyCreate<kkXMLDocument>();
		if (!xml.m_data->Read("game_info.xml")){
			yyLogWriteError("Can't reat game_info.xml\n");
			YY_PRINT_FAILED;
			return false;
		}
		auto root_node = xml.m_data->GetRootNode();
		if (!root_node){
			yyLogWriteError("game_info.xml - root_node is nullptr\n");
			YY_PRINT_FAILED;
			return false;
		}
		auto ent_nodes = root_node->getNodes("ent");
		if (!ent_nodes.size()){
			yyLogWriteError("game_info.xml - `ent` not found\n");
			YY_PRINT_FAILED;
			return false;
		}

		for (u32 i = 0, sz = ent_nodes.size(); i < sz; ++i)
		{
			auto ent_node = ent_nodes[i];
			auto attr = ent_node->getAttribute("name");
			if (!attr)
				continue;

			yyGameEntity* newEnt = yyCreate<yyGameEntity>();
			m_ent_list.push_back(newEnt);
			newEnt->m_name = attr->value;

			attr = ent_node->getAttribute("visual");
			if (attr) newEnt->m_visualAttrValue = attr->value;

			if (newEnt->m_visualAttrValue.size())
			{
				wprintf(L"VISUAL: %s\n", (const wchar_t*)newEnt->m_visualAttrValue.data());
			}

			attr = ent_node->getAttribute("td");
			if (attr) newEnt->m_tdAttrValue = attr->value;

		}
		return true;
	}

	yyArraySmall<yyGameEntity*>		m_ent_list;
	yyListFast<yyGameEntity*>		m_ents_in_scene;

	yyListNode<yyGameEntity*>* addEntity(s32 newEntityID, const v4f& intersectionPoint)
	{
		yyGameEntity* newEnt = yyCreate<yyGameEntity>();
		for (u16 i = 0, sz = m_ent_list.size(); i < sz; ++i){
			if (i != newEntityID)
				continue;

			newEnt->copyFrom(m_ent_list[i]);
			newEnt->m_entityPosition = intersectionPoint;
			break;
		}
		wprintf(L"add entity - %s\n", newEnt->m_name.c_str());
		return m_ents_in_scene.push_back(newEnt);
	}
	yyListNode<yyGameEntity*>* cloneEntity(yyListNode<yyGameEntity*>*pickedEntity)
	{
		yyGameEntity* newEnt = yyCreate<yyGameEntity>();
		newEnt->copyFrom(pickedEntity->m_data);
		return m_ents_in_scene.push_back(newEnt);
	}
	void deleteEntity(yyListNode<yyGameEntity*>* pickedEntity)
	{
		m_ents_in_scene.erase_node(pickedEntity);
	}
	void clear()
	{
		auto ent_head = m_ents_in_scene.head();
		if (ent_head)
		{
			auto ent_last = ent_head->m_left;
			while (true)
			{
				if (ent_head->m_data)
				{
					yyDestroy(ent_head->m_data);
					ent_head->m_data = nullptr;
				}
				if (ent_head == ent_last)
					break;
				ent_head = ent_head->m_right;
			}
		}
		m_ents_in_scene.clear();
	}
	void load()
	{
		yyPtr<kkXMLDocument> xml = yyCreate<kkXMLDocument>();
		if (!xml.m_data->Read("../res/world/map_info.xml")){
			yyLogWriteError("Can't reat map_info.xml\n");
			YY_PRINT_FAILED;
			return;
		}

		auto root_node = xml.m_data->GetRootNode();
		if (!root_node){
			yyLogWriteError("map_info.xml - root_node is nullptr\n");
			YY_PRINT_FAILED;
			return;
		}

		auto ent_nodes = root_node->getNodes("ent");
		if (!ent_nodes.size()){
			yyLogWriteError("map_info.xml - `ent` not found\n");
			YY_PRINT_FAILED;
			return;
		}

		for (u32 i = 0, sz = ent_nodes.size(); i < sz; ++i)
		{
			auto ent_node = ent_nodes[i];
			auto attr = ent_node->getAttribute("name");
			if (!attr)
				continue;

	//		wprintf(L"ent - %s\n", attr->value.c_str());

			s32 entID = -1;
			for (u16 o = 0, osz = m_ent_list.size(); o < osz; ++o)
			{
				if (m_ent_list[o]->m_name == attr->value)
				{
					entID = o;
					break;
				}
			}
			v4f position;
			f32 * position_data = position.data();
			attr = ent_node->getAttribute("position");
			if (attr)
			{
				std::vector<yyString> words;
				util::stringGetWords(&words, attr->value);
				for (u32 o = 0; o < words.size(); ++o)
				{
					if (o > 2)
						break;
					position_data[o] = util::to_float(words[o].data());
				}
			}

			Mat3 matrix;
			f32 * matrix_data = matrix.getPtr();
			attr = ent_node->getAttribute("matrix");
			if (attr)
			{
				std::vector<yyString> words;
				util::stringGetWords(&words, attr->value);
				for (u32 o = 0; o < words.size(); ++o)
				{
					if (o > 8)
						break;
					matrix_data[o] = util::to_float(words[o].data());
				}
			}
			if (entID != -1)
			{
				auto newEnt = addEntity(entID, position);
				newEnt->m_data->m_base.m_globalMatrix.m_data[0].x = matrix.m_data[0].x;
				newEnt->m_data->m_base.m_globalMatrix.m_data[0].y = matrix.m_data[0].y;
				newEnt->m_data->m_base.m_globalMatrix.m_data[0].z = matrix.m_data[0].z;
				newEnt->m_data->m_base.m_globalMatrix.m_data[1].x = matrix.m_data[1].x;
				newEnt->m_data->m_base.m_globalMatrix.m_data[1].y = matrix.m_data[1].y;
				newEnt->m_data->m_base.m_globalMatrix.m_data[1].z = matrix.m_data[1].z;
				newEnt->m_data->m_base.m_globalMatrix.m_data[2].x = matrix.m_data[2].x;
				newEnt->m_data->m_base.m_globalMatrix.m_data[2].y = matrix.m_data[2].y;
				newEnt->m_data->m_base.m_globalMatrix.m_data[2].z = matrix.m_data[2].z;
				newEnt->m_data->m_base.m_orientation = math::matToQuat(newEnt->m_data->m_base.m_globalMatrix);

				// при добавлении энтити, newEnt имеет дефолтные параметры, соответствующие
				//  ent из game_info.xml
				// далее надо прочитать параметры из карты, и изменить параметры newEnt

			}// if (entID != -1)
		}
	}
	void save()
	{
		yyPtr<kkXMLDocument> xml = yyCreate<kkXMLDocument>();
		auto xmlRoot = xml.m_data->GetRootNode();
		xmlRoot->name = "map_info";

		auto ent_curr = m_ents_in_scene.head();
		if (ent_curr)
		{
			auto ent_last = ent_curr->m_left;
			while (true)
			{
				kkXMLNode* newNode = yyCreate<kkXMLNode>();
				newNode->name = "ent";
				
				kkXMLAttribute* newAttribute = yyCreate<kkXMLAttribute>();
				newAttribute->name = "position";
				newAttribute->value.clear();
				newAttribute->value += ent_curr->m_data->m_entityPosition.x;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_entityPosition.y;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_entityPosition.z;
				newNode->addAttribute(newAttribute);

				newAttribute = yyCreate<kkXMLAttribute>();
				newAttribute->name = "matrix";
				newAttribute->value.clear();
				newAttribute->value += ent_curr->m_data->m_base.m_globalMatrix[0].x;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_base.m_globalMatrix[0].y;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_base.m_globalMatrix[0].z;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_base.m_globalMatrix[1].x;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_base.m_globalMatrix[1].y;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_base.m_globalMatrix[1].z;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_base.m_globalMatrix[2].x;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_base.m_globalMatrix[2].y;
				newAttribute->value += " ";
				newAttribute->value += ent_curr->m_data->m_base.m_globalMatrix[2].z;
				newAttribute->value += " ";
				newNode->addAttribute(newAttribute);

				/*std::vector<yyString> words;
				util::stringGetWords(&words, newAttribute->value);
				for (int i = 0; i < words.size(); ++i)
				{
					wprintf(L"WORD [%s] = %f\n", words[i].data(), util::to_float(words[i].data()));
				}*/

				newAttribute = yyCreate<kkXMLAttribute>();
				newAttribute->name = "name";
				newAttribute->value = ent_curr->m_data->m_name;
				newNode->addAttribute(newAttribute);

				newAttribute = yyCreate<kkXMLAttribute>();
				newAttribute->name = "visual";
				newAttribute->value = ent_curr->m_data->m_visualAttrValue;
				newNode->addAttribute(newAttribute);

				newAttribute = yyCreate<kkXMLAttribute>();
				newAttribute->name = "td";
				newAttribute->value = ent_curr->m_data->m_tdAttrValue;
				newNode->addAttribute(newAttribute);

				
				xmlRoot->addNode(newNode);

				if (ent_curr == ent_last)
					break;
				ent_curr = ent_curr->m_right;
			}
		}

		xml.m_data->Write((const char16_t*)L"../res/world/map_info.xml", true);
	}
};

#endif