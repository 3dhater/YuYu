#ifndef __YYGAMEINFO_H__
#define __YYGAMEINFO_H__

#define GAME_TOOL
#include "io\xml_io.h"

struct yyGameEntity
{
	yyGameEntity()
	{
		m_visual_model_type = default_cube;
		m_modelForRayIntersection = nullptr;
		m_modelForGPU = nullptr;
	}
	~yyGameEntity()
	{
		if(m_modelForRayIntersection)
			yyDeleteModel(m_modelForRayIntersection);
		if (m_modelForGPU)
			yyGetVideoDriverAPI()->UnloadModel(m_modelForGPU);
	}

	yyString m_name;      // имя name="palyer_start"
	v3f m_entityPosition; // каждая энтити имеет как минимум позицию

	yyModel* m_modelForRayIntersection;
	yyResource* m_modelForGPU;

	enum visual_model_type
	{
		default_cube
	}m_visual_model_type;

	void copyFrom(yyGameEntity * other)
	{
		m_name = other->m_name;
		m_entityPosition = other->m_entityPosition;
		m_modelForRayIntersection = other->m_modelForRayIntersection;

		if (m_modelForRayIntersection)
		{
			yyDeleteModel(m_modelForRayIntersection);
			m_modelForRayIntersection = nullptr;
		}
		if (other->m_visual_model_type == default_cube)
		{
			m_modelForRayIntersection = yyGetModel("../res/worldeditor/cubeent.tr3d");
			m_modelForGPU = yyGetModelResource("../res/worldeditor/cubeent.tr3d", true);
		}

	}
};

class yyGameInfo
{
public:
	yyGameInfo()
	{

	}
	~yyGameInfo()
	{
		for (u16 i = 0, sz = m_ent_list.size(); i < sz; ++i)
		{
			yyDestroy(m_ent_list[i]);
		}
		/*auto head = m_ent_list.head();
		auto last = head->m_left;
		if (head)
		{
			while (true)
			{
				if (head->m_data)
					yyDestroy(head->m_data);
				if (head == last)
					break;
				head = head->m_right;
			}
		}*/
	}

	bool Init()
	{
		if (!yyFS::exists("game_info.xml"))
		{
			yyLogWriteError("game_info.xml not found\n");
			YY_PRINT_FAILED;
			return false;
		}

		yyPtr<kkXMLDocument> xml = yyCreate<kkXMLDocument>();
		if (!xml.m_data->Read("game_info.xml"))
		{
			yyLogWriteError("Can't reat game_info.xml\n");
			YY_PRINT_FAILED;
			return false;
		}
		
		auto root_node = xml.m_data->GetRootNode();
		if (!root_node)
		{
			yyLogWriteError("game_info.xml - root_node is nullptr\n");
			YY_PRINT_FAILED;
			return false;
		}

		auto ent_nodes = root_node->getNodes("ent");
		if (!ent_nodes.size())
		{
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
		}

		return true;
	}



	yyArraySmall<yyGameEntity*> m_ent_list;

	yyListFast<yyGameEntity*> m_ents_in_scene;
	yyListNode<yyGameEntity*>* addEntity(s32 newEntityID, const v4f& intersectionPoint)
	{
		yyGameEntity* newEnt = yyCreate<yyGameEntity>();
		for (u16 i = 0, sz = m_ent_list.size(); i < sz; ++i)
		{
			if (i != newEntityID)
				continue;

			newEnt->copyFrom(m_ent_list[i]);
			newEnt->m_entityPosition.x = intersectionPoint.x;
			newEnt->m_entityPosition.y = intersectionPoint.y;
			newEnt->m_entityPosition.z = intersectionPoint.z;
			break;
		}
		return m_ents_in_scene.push_back(newEnt);
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