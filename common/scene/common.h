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
	yySceneObjectBase()
	:
		m_scale(v4f(1.f)),
		m_id(-1),
		m_isVisible(true),
		m_isTransparent(false),
		m_distanceToCamera(0.f),
		m_objectType(ObjectType::Dummy),
		m_updateImplementation(nullptr),
		m_implementationPtr(nullptr),
		m_family(nullptr)
	{
		m_radiusAabb = 0.f;
	}

	~yySceneObjectBase()
	{
		DeleteFamily();
	}
	void DeleteFamily();
	void InitFamily();

	yyString m_name;
	v4f m_localPosition;
	v4f m_globalPosition;
	v4f m_scale;
	v4f m_rotation, m_rotationOld;
	Quat m_orientation;

	Mat4 m_localMatrix;
	Mat4 m_globalMatrix; // world
	Mat4 m_rotationScaleMatrix;

	Aabb m_aabbWithoutTransforms;
	Aabb m_aabb;
	f32  m_radiusAabb;
	//Obb  m_obb;

	s32 m_id;
	bool m_isVisible;
	bool m_isTransparent;

	f32 m_distanceToCamera;

	void SetRotation(const v4f& rotation)
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
		Camera,
		Sprite,
	};
	ObjectType m_objectType;
	void(*m_updateImplementation)(void*implementation);
	void * m_implementationPtr ;
	yySceneObjectFamily* m_family ;

	void UpdateBase();
};

struct yySceneObjectFamily
{
	yySceneObjectFamily()
		:
		m_object(nullptr),
		m_parent(nullptr)
	{}

	yySceneObjectBase* m_object ;
	yySceneObjectFamily* m_parent ;
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

YY_FORCE_INLINE void yySceneObjectBase::UpdateBase()
{
	Mat4 tMatrix;
	math::makeTranslationMatrix( m_localPosition, tMatrix );
	Mat4 rMatrix;
	math::makeRotationMatrix( rMatrix, m_orientation );
	Mat4 sMatrix;
	sMatrix[ 0u ].x = m_scale.x;
	sMatrix[ 1u ].y = m_scale.y;
	sMatrix[ 2u ].z = m_scale.z;

	m_rotationScaleMatrix = rMatrix*sMatrix;

	m_localMatrix = tMatrix*m_rotationScaleMatrix;
	m_globalMatrix = m_localMatrix;

	if(m_family)
	{
		if(m_family->m_parent)
			m_globalMatrix = m_family->m_parent->m_object->m_globalMatrix * m_localMatrix;
	}

	m_globalPosition = m_globalMatrix[3];

	// НАДО УБРАТЬ ОББ 
	// оптимизация по frustum должна быть либо по сфере либо по аабб - на выбор. сфера по умолчанию
	/*m_obb.v1 = m_aabbWithoutTransforms.m_min;
	m_obb.v2 = m_aabbWithoutTransforms.m_max;
	m_obb.v3.set( m_obb.v1.x, m_obb.v1.y, m_obb.v2.z, 1.f );
	m_obb.v4.set( m_obb.v2.x, m_obb.v1.y, m_obb.v1.z, 1.f );		
	m_obb.v5.set( m_obb.v1.x, m_obb.v2.y, m_obb.v1.z, 1.f );		
	m_obb.v6.set( m_obb.v1.x, m_obb.v2.y, m_obb.v2.z, 1.f );		
	m_obb.v7.set( m_obb.v2.x, m_obb.v1.y, m_obb.v2.z, 1.f );		
	m_obb.v8.set( m_obb.v2.x, m_obb.v2.y, m_obb.v1.z, 1.f );	
	m_obb.v1.w = 1.f;
	m_obb.v2.w = 1.f;*/
			
	auto W = m_globalMatrix;
	W[3] = v4f(0.f,0.f,0.f,1.f);

	/*m_obb.v1 = math::mul(m_obb.v1, W) + m_globalPosition;
	m_obb.v2 = math::mul(m_obb.v2, W) + m_globalPosition;
	m_obb.v3 = math::mul(m_obb.v3, W) + m_globalPosition;
	m_obb.v4 = math::mul(m_obb.v4, W) + m_globalPosition;
	m_obb.v5 = math::mul(m_obb.v5, W) + m_globalPosition;
	m_obb.v6 = math::mul(m_obb.v6, W) + m_globalPosition;
	m_obb.v7 = math::mul(m_obb.v7, W) + m_globalPosition;
	m_obb.v8 = math::mul(m_obb.v8, W) + m_globalPosition;*/

	//m_aabb.reset();
	m_radiusAabb = m_aabb.m_min.distance(m_aabb.m_max);

	/*m_aabb.add(m_obb.v1);
	m_aabb.add(m_obb.v2);
	m_aabb.add(m_obb.v3);
	m_aabb.add(m_obb.v4);
	m_aabb.add(m_obb.v5);
	m_aabb.add(m_obb.v6);
	m_aabb.add(m_obb.v7);
	m_aabb.add(m_obb.v8);*/

	if(m_family)
	{
		auto children_head = m_family->m_children.head();
		if(children_head)
		{
			auto last = children_head->m_left;
			while(true)
			{
				auto next = children_head->m_right;
				children_head->m_data->m_object->UpdateBase();
				if(children_head->m_data->m_object->m_updateImplementation)
					children_head->m_data->m_object->m_updateImplementation(children_head->m_data->m_object->m_implementationPtr);
				if(children_head == last)
					break;
				children_head = next;
			}
		}
	}
}

#endif