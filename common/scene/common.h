#ifndef _YUYU_SCENE_CMN_H_
#define _YUYU_SCENE_CMN_H_

#include "containers/list.h"
#include "strings/string.h"
#include "math/quat.h"
#include "math/mat.h"
#include "math/aabb.h"


extern "C"
{
	YY_API yySceneObjectBase* YY_C_DECL yySceneGetRootObject();
	YY_API yyCamera* YY_C_DECL yySceneGetActiveCamera();
	YY_API void YY_C_DECL yySceneSetActiveCamera(yyCamera*);
}

struct yySceneObjectFamily;
struct yySceneObjectBase
{
	yySceneObjectBase(){}
	~yySceneObjectBase()
	{
		DeleteFamily();
	}
	void DeleteFamily();
	void InitFamily();

	yyString m_name;
	v4f m_localPosition;
	v4f m_globalPosition;
	v4f m_scale = v4f(1.f);
	v4f m_rotation, m_rotationOld;
	Quat m_orientation;

	Mat4 m_localMatrix;
	Mat4 m_globalMatrix; // world
	Mat4 m_rotationScaleMatrix;

	Aabb m_aabbWithoutTransforms;
	Aabb m_aabb;
	Obb  m_obb;

	s32 m_id = -1;
	bool m_isVisible = true;
	bool m_isTransparent = false;

	f32 m_distanceToCamera = 0.f;

	void        SetRotation(const v4f& rotation)
	{
		m_rotation = rotation;
		v4f r =  rotation - m_rotationOld;
		Quat q(r);
		m_orientation = q * m_orientation;
		m_orientation.normalize();
		m_rotationOld = rotation;
	}
	void SetGlobalMatrix(f32* ptr)
	{
		auto dst = m_globalMatrix.getPtr();
		dst[0] = ptr[0];
		dst[1] = ptr[1];
		dst[2] = ptr[2];
		dst[3] = ptr[3];
		dst[4] = ptr[4];
		dst[5] = ptr[5];
		dst[6] = ptr[6];
		dst[7] = ptr[7];
		dst[8] = ptr[8];
		dst[9] = ptr[9];
		dst[10] = ptr[10];
		dst[11] = ptr[11];
		dst[12] = ptr[12];
		dst[13] = ptr[13];
		dst[14] = ptr[14];
		dst[15] = ptr[15];
	}

	enum ObjectType
	{
		Dummy,
		Camera
	};
	ObjectType m_objectType = ObjectType::Dummy;
	void(*m_updateImplementation)(void*implementation) = nullptr;
	void * m_implementationPtr = nullptr;
	yySceneObjectFamily* m_family = nullptr;
};

struct yySceneObjectFamily
{
	yySceneObjectBase* m_object = nullptr;
	yySceneObjectFamily* m_parent = nullptr;
	yyList<yySceneObjectFamily*> m_children;

	bool _removeChild(yySceneObjectFamily * child)
	{
		bool result = m_children.erase_first(child);
		return result;
	}
	bool _addChild(yySceneObjectFamily * child)
	{
		bool result = false;
		if(!m_children.is_contain(child))
		{
			m_children.push_back(child);
			result = true;
		}
		return result;
	}
	void SetParent(yySceneObjectFamily * parent)
	{
		auto root_object = yySceneGetRootObject();
		if(parent)
		{
			if(m_parent == root_object->m_family)
				root_object->m_family->_removeChild(this);
			m_parent = parent;
		}
		else
		{
			if(m_parent != root_object->m_family)
				m_parent->_removeChild(this);
			m_parent = root_object->m_family;
		}
		m_parent->_addChild(this);
	}
	void AddChild(yySceneObjectFamily * child)
	{
		assert(child);
		if(_addChild(child))
		{
			child->SetParent(this);
		}
	}
	void ClearChildren()
	{
		auto head = m_children.head();
		if(head)
		{
			auto last = head->m_left;
			while(true)
			{
				auto next = head->m_right;
				head->m_data->SetParent(nullptr);
				if(head == last)
					break;
				head = next;
			}
			m_children.clear();
		}
	}
	bool RemoveChild(yySceneObjectFamily* child)
	{
		bool result = _removeChild(child);
		if(result)
			child->SetParent(nullptr);
		return result;
	}
};

YY_FORCE_INLINE void yySceneObjectBase::DeleteFamily()
{
	if(m_family)
	{
		yyDestroy(m_family);
		m_family = nullptr;
	}
}

YY_FORCE_INLINE void yySceneObjectBase::InitFamily()
{
	m_family = yyCreate<yySceneObjectFamily>();
	m_family->m_object = this;
}

#endif