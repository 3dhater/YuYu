#ifndef __YY_ALLOCATOR_H__
#define __YY_ALLOCATOR_H__

class yyDefault_allocator
{
public:
	yyDefault_allocator(){}
	~yyDefault_allocator(){}

	void* allocate(size_t size )
	{
		return yyMemAlloc(size);
	}
	void free( void * ptr )
	{
		yyMemFree(ptr);
	}
};

template<typename Type, typename allocator = yyDefault_allocator>
class yyAllocator
{
	allocator m_allocator;
public:
	yyAllocator(){}
	yyAllocator(allocator a):m_allocator(a){}

	Type * allocate(size_t size )
	{
		return static_cast<Type*>(m_allocator.allocate(size * sizeof( Type )));
	}

	void construct( Type * new_data, const Type& old_data )
	{
		new(new_data) Type( old_data );
	}

	void deallocate( Type * ptr )
	{
		m_allocator.free(ptr);
	}

	void destruct( Type * ptr )
	{
		ptr->~Type();
	}
};


#endif