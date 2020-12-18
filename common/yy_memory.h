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
/// Create object
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

/// Destroy object
template<typename _type>
void yyDestroy( _type * ptr )
{
	assert(ptr);
	yyObjectCreator<_type>::destroy(ptr);
}



#endif