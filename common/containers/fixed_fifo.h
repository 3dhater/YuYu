#ifndef _YUYU_FIXFIFO_H_
#define _YUYU_FIXFIFO_H_

#include <mutex>

template<typename T, u32 size>
class yyFixedFIFO
{
	u32 m_first = 0;
	u32 m_put = 0;

	yyAllocator<T,yyDefault_allocator> m_allocator;

public:
	yyFixedFIFO()
	{
		//m_data = new T[m_capacity];
		m_data = m_allocator.allocate( m_capacity );
		for( u32 i = 0u; i < m_size; ++i )
		{
			m_allocator.construct( &m_data[i], T() );
		}
	}
	~yyFixedFIFO()
	{
		//delete[] m_data;
		for( u32 i = 0u; i < m_size; ++i )
		{
			m_allocator.destruct( &m_data[i] );
		}
		m_allocator.deallocate(m_data);
	}
	
	void put(const T& data)
	{
		if(m_size < m_capacity)
		{
			++m_size;
			m_data[m_put] = data;
			++m_put;
			if(m_put >= m_capacity)
				m_put = 0;
		}
	}
	
	// !!! Use only like this:
	/*
		for(int i = 0, sz = fifo.m_size; i < sz; ++i)
		{
			auto obj = fifo.get();
		}
	*/
	T& get()
	{
		T& r = m_data[m_first];
		++m_first;
		if(m_first >= m_capacity)
			m_first = 0;
		--m_size;
		return r;
	}
	
	T* m_data = nullptr;
	u32 m_capacity = size;	
	u32 m_size = 0;
};

template<typename T, u32 size>
class yyFixedFIFOThread
{
	u32 m_first = 0;
	u32 m_put = 0;
	std::mutex m_mutex;
	yyAllocator<T,yyDefault_allocator> m_allocator;
public:
	yyFixedFIFOThread()
	{
		//m_data = new T[m_capacity];
		m_data = m_allocator.allocate( m_capacity );
		for( u32 i = 0u; i < m_size; ++i )
		{
			m_allocator.construct( &m_data[i], T() );
		}
	}
	~yyFixedFIFOThread()
	{
	//	delete[] m_data;
		for( u32 i = 0u; i < m_size; ++i )
		{
			m_allocator.destruct( &m_data[i] );
		}
		m_allocator.deallocate(m_data);
	}
	
	void put(const T& data)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		//m_mutex.lock();
		
		if(m_size < m_capacity)
		{
			++m_size;
			m_data[m_put] = data;
			++m_put;
			if(m_put >= m_capacity)
				m_put = 0;
		}

		//m_mutex.unlock();
	}
	
	T& get()
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		//m_mutex.lock();

		T& r = m_data[m_first];
		++m_first;
		if(m_first >= m_capacity)
			m_first = 0;
		--m_size;

		//m_mutex.unlock();

		return r;
	}
	
	T* m_data = nullptr;
	u32 m_capacity = size;	
	u32 m_size = 0;
};

#endif