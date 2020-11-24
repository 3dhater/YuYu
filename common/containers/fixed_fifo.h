﻿#ifndef _YUYU_FIXFIFO_H_
#define _YUYU_FIXFIFO_H_

#include <mutex>

template<typename T, int size>
class yyFixedFIFO
{
	int m_first = 0;
	int m_put = 0;
public:
	yyFixedFIFO()
	{
		m_data = new T[m_capacity];
	}
	~yyFixedFIFO()
	{
		delete[] m_data;
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
	int m_capacity = size;	
	int m_size = 0;
};

template<typename T, int size>
class yyFixedFIFOThread
{
	int m_first = 0;
	int m_put = 0;
	std::mutex m_mutex;
public:
	yyFixedFIFOThread()
	{
		m_data = new T[m_capacity];
	}
	~yyFixedFIFOThread()
	{
		delete[] m_data;
	}
	
	void put(const T& data)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		
		if(m_size < m_capacity)
		{
			++m_size;
			m_data[m_put] = data;
			++m_put;
			if(m_put >= m_capacity)
				m_put = 0;
		}
	}
	
	T& get()
	{
		std::lock_guard<std::mutex> lg(m_mutex);

		T& r = m_data[m_first];
		++m_first;
		if(m_first >= m_capacity)
			m_first = 0;
		--m_size;
		return r;
	}
	
	T* m_data = nullptr;
	int m_capacity = size;	
	int m_size = 0;
};

#endif