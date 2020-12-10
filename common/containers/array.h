#ifndef _YY_ARRAY_H_
#define _YY_ARRAY_H_

#include "common/allocator.h"
template<typename type, typename allocator = yyDefault_allocator>
class yyArray
{

	typedef type* pointer;
	typedef type& reference;
	typedef const type& const_reference;

	pointer m_data;
	u32     m_size;
	u32     m_allocated;
	u32     m_addMemory;
	yyAllocator<type,allocator> m_allocator;

	void reallocate( u32 new_capacity )
	{
		new_capacity += m_addMemory;
		pointer new_data = m_allocator.allocate( new_capacity /** sizeof( type )*/ );

		if( m_data )
		{
			for( u32 i = 0u; i < m_size; ++i )
			{
				m_allocator.construct( &new_data[i], m_data[i] );
				m_allocator.destruct( &m_data[i] );
			}

			m_allocator.deallocate( m_data );
		}

		m_data = new_data;
		m_allocated = new_capacity;
	}

public:

	pointer begin()
	{
		return m_data;
	}

	pointer end()
	{
		return (m_data+(m_size));
	}

	yyArray( u32 addSize = 8u )
	:
		m_data( nullptr ),
		m_size( 0u ),
		m_allocated( 0u ),
		m_addMemory( addSize )
	{
	}

	yyArray( yyAllocator<type,allocator> _allocator, u32 addSize = 8u )
	:
		m_data( nullptr ),
		m_size( 0u ),
		m_allocated( 0u ),
		m_addMemory( addSize )
	{
		m_allocator = _allocator;
	}

	yyArray( u32 addSize, u32 reserve )
	:
		m_data( nullptr ),
		m_size( 0u ),
		m_allocated( 0u ),
		m_addMemory( addSize )
	{
		reallocate( reserve );
	}

	yyArray( const yyArray& other )
	:
		m_data( nullptr ),
		m_size( 0u ),
		m_allocated( 0u ),
		m_addMemory( 8u )
	{
		*this = other;
	}

	~yyArray(){ clear(); }
		
	void setData(pointer ptr) { m_data = ptr; }
	pointer data() const     { return m_data; }
	void    setSize( u32 s ) { m_size = s; }
	u32     size() const     { return m_size; }
	u32     capacity() const { return m_allocated; }
		
		
	void    setAddMemoryValue( u32 v ){ m_addMemory = v; }
		
		
	bool    empty() const    { return m_size == 0u; }

	const_reference at( u32 id ) const { return m_data[id]; }
	reference       at( u32 id ){ return m_data[id]; }
	const_reference operator[]( u32 id ) const { return m_data[id]; }
	reference       operator[]( u32 id ){ return m_data[id]; }
	reference       back(){ return m_data[ m_size - 1u ]; }
	const_reference back() const { return m_data[ m_size - 1u ]; }
	const_reference front() const { return m_data[ 0u ]; }

	void reserve( u32 new_capacity )
	{
		if( new_capacity > m_allocated )
			reallocate( new_capacity );
	}

	void push_back( const_reference object )
	{
		u32 new_size = m_size + 1u;
		if( new_size > m_allocated )
			reallocate( new_size );
		m_allocator.construct( &m_data[m_size], object );
		m_size = new_size;
	}

	void insert(u32 where, const_reference object)
	{
		u32 new_size = m_size + 1u;
		if( new_size > m_allocated )
			reallocate( new_size );
		for(u32 i = m_size; i > where;)
		{
			m_data[i] = m_data[i-1];
			--i;
		}
		m_allocator.construct( &m_data[where], object );
		m_size = new_size;
	}
	
	void clear()
	{
		if( m_data )
		{
			for( u32 i = 0u; i < m_size; ++i )
			{
				m_allocator.destruct( &m_data[i] );
			}
			m_allocator.deallocate( m_data );

			m_allocated = m_size = 0u;
			m_data = nullptr;
		}
	}

	void shrink_to_fit()
	{
		reallocate( m_size - m_addMemory );
	}

	void erase( u32 index )
	{
		erase( index, index );
	}

	void erase_first( const_reference ref )
	{
		for( u32 i = 0u; i < m_size; ++i )
		{
			if( m_data[ i ] == ref )
			{
				erase( i );
				return;
			}
		}
	}

	void erase( u32 begin, u32 end )
	{
		if( m_size )
		{
			u32 last = m_size - 1u;
			u32 len = end - begin;
			for( u32 i = begin; i < m_size; ++i )
			{
				m_allocator.destruct( &m_data[ i ] );
				if( i < last )
				{
					u32 next = i + 1u + len;
					if( next < m_size )
					{
						m_allocator.construct( &m_data[ i ], m_data[ next ] );
					}
				}
			}
			m_size = m_size - 1u - len;
		}
	}

	void assign( const yyArray<type, allocator>& other )
	{
		clear();
		m_size = other.size();
		m_allocated = other.capacity();
		reallocate( m_allocated );
		for( u32 i = 0u; i < m_size; ++i )
		{
			m_allocator.construct( &m_data[ i ], other.m_data[ i ] );
		}
	}

	yyArray<type, allocator>& operator=( const yyArray<type, allocator>& other )
	{
		clear();
		m_size = other.size();
		m_allocated = other.capacity();
		reallocate( m_allocated );
		for( u32 i = 0u; i < m_size; ++i )
		{
			m_allocator.construct( &m_data[ i ], other.m_data[ i ] );
		}
		return *this;
	}

};

#pragma pack(1)
template<typename type>
class yyArraySmall
{

	typedef type* pointer;
	typedef type& reference;
	typedef const type& const_reference;

	u16     m_size;
	u16     m_allocated;

	void reallocate( u16 new_capacity )
	{
		new_capacity += 4;
		pointer new_data = static_cast<type*>(yyMemAlloc(new_capacity * sizeof( type )));

		if( m_data )
		{
			for( u16 i = 0u; i < m_size; ++i )
			{
				new(&new_data[i]) type( m_data[i] );
				(&m_data[i])->~type();
			}
			yyMemFree(m_data);
		}
		m_data = new_data;
		m_allocated = new_capacity;
	}

public:
	pointer begin()
	{
		return m_data;
	}
	pointer end()
	{
		return (m_data+(m_size));
	}

	yyArraySmall()
		:
		m_size(0),
		m_allocated(0),
		m_data(nullptr)
	{}
	~yyArraySmall(){ clear(); }
		
	void setData(pointer ptr) { m_data = ptr; }
	pointer data() const     { return m_data; }
	void    setSize( u16 s ) { m_size = s; }
	u16     size() const     { return m_size; }
	u16     capacity() const { return m_allocated; }
	bool    empty() const    { return m_size == 0u; }

	const_reference at( u16 id ) const { return m_data[id]; }
	reference       at( u16 id ){ return m_data[id]; }
	const_reference operator[]( u16 id ) const { return m_data[id]; }
	reference       operator[]( u16 id ){ return m_data[id]; }
	reference       back(){ return m_data[ m_size - 1u ]; }
	const_reference back() const { return m_data[ m_size - 1u ]; }
	const_reference front() const { return m_data[ 0u ]; }

	void reserve( u16 new_capacity )
	{
		if( new_capacity > m_allocated )
			reallocate( new_capacity );
	}

	void push_back( const_reference object )
	{
		u16 new_size = m_size + 1u;
		if( new_size > m_allocated )
			reallocate( new_size );
		new(&m_data[m_size]) type( object );
		m_size = new_size;
	}

	void insert(u16 where, const_reference object)
	{
		u16 new_size = m_size + 1u;
		if( new_size > m_allocated )
			reallocate( new_size );
		for(u16 i = m_size; i > where;)
		{
			m_data[i] = m_data[i-1];
			--i;
		}
		new(&m_data[where]) type( object );
		m_size = new_size;
	}
	
	void clear()
	{
		if( m_data )
		{
			for( u16 i = 0u; i < m_size; ++i )
			{
				(&m_data[i])->~type();
			}
			yyMemFree(m_data);

			m_allocated = m_size = 0u;
			m_data = nullptr;
		}
	}

	void shrink_to_fit()
	{
		reallocate( m_size );
	}

	void erase( u16 index )
	{
		erase( index, index );
	}

	void erase_first( const_reference ref )
	{
		for( u16 i = 0u; i < m_size; ++i )
		{
			if( m_data[ i ] == ref )
			{
				erase( i );
				return;
			}
		}
	}

	void erase( u16 begin, u16 end )
	{
		if( m_size )
		{
			u16 last = m_size - 1u;
			u16 len = end - begin;
			for( u16 i = begin; i < m_size; ++i )
			{
				(&m_data[i])->~type();
				if( i < last )
				{
					u16 next = i + 1u + len;
					if( next < m_size )
					{
						new(&m_data[i]) type( m_data[ next ] );
					}
				}
			}
			m_size = m_size - 1u - len;
		}
	}
	
	pointer m_data;
};
#pragma pack()

#endif