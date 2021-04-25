/*
Copyright (c) 2003-2013 Gino van den Bergen / Erwin Coumans  http://bulletphysics.org
Copyright (c) 2021 Artyom Basov

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _YUYU_MEMORY_H_
#define _YUYU_MEMORY_H_

#include <utility>
#include <cassert>

// all memory allocation must be in YuYu.dll

extern "C"
{
	YY_API void * YY_C_DECL				yyMemAlloc(u32 size); // malloc
	YY_API void   YY_C_DECL				yyMemFree(void*);     // free
	YY_API void * YY_C_DECL				yyMemRealloc(void *,u32 size); // realloc
}

template<typename _type>
class yyObjectCreator
{
public:

	/*template<typename... Args>
	static _type * create(Args&&... args)
	{
		_type * ptr = (_type*)yyMemAlloc(sizeof(_type));
		if( ptr )
		{
			new(ptr) _type(std::forward<Args>(args)...);
		}
		return ptr;
	}*/
	static _type * create()
	{
		_type * ptr = (_type*)yyMemAlloc(sizeof(_type));
		if( ptr )
			new(ptr) _type();
		return ptr;
	}

	template<typename arg1>
	static _type * create1(arg1 a){
		_type * ptr = (_type*)yyMemAlloc(sizeof(_type));
		if (ptr) new(ptr) _type(a);
		return ptr;
	}
	template<typename arg1, typename arg2>
	static _type * create2(arg1 a1, arg2 a2) {
		_type * ptr = (_type*)yyMemAlloc(sizeof(_type));
		if (ptr) new(ptr) _type(a1, a2);
		return ptr;
	}
	template<typename arg1, typename arg2, typename arg3>
	static _type * create3(arg1 a1, arg2 a2, arg3 a3) {
		_type * ptr = (_type*)yyMemAlloc(sizeof(_type));
		if (ptr) new(ptr) _type(a1, a2, a3);
		return ptr;
	}
	template<typename arg1, typename arg2, typename arg3, typename arg4>
	static _type * create4(arg1 a1, arg2 a2, arg3 a3, arg4 a4) {
		_type * ptr = (_type*)yyMemAlloc(sizeof(_type));
		if (ptr) new(ptr) _type(a1, a2, a3, a4);
		return ptr;
	}


	static void destroy( _type * ptr )
	{
		assert(ptr);
		ptr->~_type();
		yyMemFree( ptr );
	}
};

// Create object
//template<typename _type, typename... Args>
//_type * yyCreate(Args&&... args)
template<typename _type>
_type * yyCreate()
{
	//return yyObjectCreator<_type>::create(std::forward<Args>(args)...);
	return yyObjectCreator<_type>::create();
}
template<typename _type, typename arg1>
_type * yyCreate1(arg1 a1){ return yyObjectCreator<_type>::create1(a1); }

template<typename _type, typename arg1, typename arg2>
_type * yyCreate2(arg1 a1, arg2 a2) { return yyObjectCreator<_type>::create2(a1, a2); }

template<typename _type, typename arg1, typename arg2, typename arg3>
_type * yyCreate3(arg1 a1, arg2 a2, arg3 a3) { return yyObjectCreator<_type>::create3(a1, a2, a3); }

template<typename _type, typename arg1, typename arg2, typename arg3, typename arg4>
_type * yyCreate4(arg1 a1, arg2 a2, arg3 a3, arg4 a4) { return yyObjectCreator<_type>::create4(a1, a2, a3, a4); }

// Destroy object
// If *ptr is void* ptr, don't forget to set type 
//   like this yyDestroy((yyImage*)m_resourceData.m_source);
template<typename _type>
void yyDestroy( _type * ptr )
{
	assert(ptr);
	yyObjectCreator<_type>::destroy(ptr);
}

// чтобы увеличить время компиляции, выделять\освобождать память 
//  используя только forward declaration
class YY_API yyMegaAllocator
{
public:
	yyMegaAllocator();
	~yyMegaAllocator();

	static yyMaterial* CreateMaterial();
	static yyResource* CreateResource();
	
	static void Destroy(yyMaterial*);
	static void Destroy(yyResource*);
};


template<typename _type>
class yyPoolAllocator
{
	s32				m_elemSize;
	s32				m_maxElements;
	s32				m_freeCount;
	void*			m_firstFree;
	u8*				m_pool;

public:
	yyPoolAllocator() {
		m_pool = 0;
	}
	yyPoolAllocator(s32 max_elements) {
		m_pool = 0;
		Init(max_elements);
	}
	~yyPoolAllocator() {
		yyMemFree(m_pool);
	}

	void Init(s32 max_elements) {
		assert(!m_pool);
		m_elemSize = sizeof(_type);
		m_maxElements = max_elements;

		m_pool = (u8*)yyMemAlloc(static_cast<u32>(m_elemSize*m_maxElements));

		unsigned char* p = m_pool;
		m_firstFree = p;
		m_freeCount = m_maxElements;
		int count = m_maxElements;
		while (--count) {
			*(void**)p = (p + m_elemSize);
			p += m_elemSize;
		}
		*(void**)p = 0;
	}

	_type* GetObject() {
		assert(m_freeCount>0);
		_type* object = (_type*)m_firstFree;
		m_firstFree = *(void**)m_firstFree;
		--m_freeCount;
		return object;
	}

	void PutBack(_type* ptr)
	{
		if (!ptr)
			return;

		assert((u8*)ptr >= m_pool && (u8*)ptr < m_pool + m_maxElements * m_elemSize);

		*(void**)ptr = m_firstFree;
		m_firstFree = ptr;
		++m_freeCount;
	}

	s32 GetUsedCount() const{
		return m_maxElements - m_freeCount;
	}
	s32	GetFreeCount() const{
		return m_freeCount;
	}
};

// default pool setup will be in class Engine
struct yyPoolSetup
{
	s32 m_resourceCount; // 100
	s32 m_materialCount; // 100
};


#endif