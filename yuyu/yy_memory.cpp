#include "yy.h"

#include <cstdlib>
#include <cassert>

extern "C"
{

	YY_API void * YY_C_DECL yyMemAlloc(u32 size)
	{
		assert(size != 0);
	
		return std::malloc(static_cast<size_t>(size));
	}

	YY_API void * YY_C_DECL yyMemRealloc(void * ptr,u32 size)
	{
		return std::realloc(ptr,static_cast<size_t>(size));
	}

	YY_API void   YY_C_DECL yyMemFree(void* ptr)
	{
		assert(ptr != nullptr);
		std::free(ptr);
	}

}
