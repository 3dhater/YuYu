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
	template<typename... Args>
	static _type * create(Args&&... args)
	{
		_type * ptr = (_type*)yyMemAlloc(sizeof(_type));
		if( ptr )
		{
			new(ptr) _type(std::forward<Args>(args)...);
		}
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
template<typename _type, typename... Args>
_type * yyCreate(Args&&... args)
{
	return yyObjectCreator<_type>::create(std::forward<Args>(args)...);
}
/// Destroy object
template<typename _type>
void yyDestroy( _type * ptr )
{
	assert(ptr);
	yyObjectCreator<_type>::destroy(ptr);
}



#endif