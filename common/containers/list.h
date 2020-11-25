#ifndef _yyList_H_
#define _yyList_H_

#include "common/allocator.h"

template<typename T>
struct yyListNode
{
	T m_data;

	yyListNode* m_left  = nullptr;
	yyListNode* m_right = nullptr;
};

template<typename T>
class yyList
{
	std::size_t m_size = 0;
	yyListNode<T>* m_head = nullptr;
	yyAllocator<yyListNode<T>> m_allocator;
	
public:

	yyList()
	{
	}

	yyList(const yyList& other) = delete;
	yyList(yyList&& other) = delete;

	~yyList()
	{
		clear();
	}

	bool erase_first(const T& object)
	{
		if(m_head)
		{
			auto head = m_head;
			auto last = m_head->m_left;
			while(true)
			{
				auto next = head->m_right;
				if(head->m_data == object)
				{
					if(head == m_head)
						pop_front();
					else
					{
						--m_size;
						head->m_left->m_right = head->m_right;
						head->m_right->m_left = head->m_left;
						m_allocator.destruct(head);
						m_allocator.deallocate(head);
					}
					return true;
				}
				if(head == last)
					break;
				head = next;
			}
		}
		return false;
	}

	bool is_contain(const T& object)
	{
		if(m_head)
		{
			auto head = m_head;
			auto last = m_head->m_left;
			while(true)
			{
				auto next = head->m_right;
				if(head->m_data == object)
					return true;
				if(head == last)
					break;
				head = next;
			}
		}
		return false;
	}

	void clear() noexcept
	{
		if(!m_head)
			return;
		auto last = m_head->m_left;
		while(true)
		{
			auto next = m_head->m_right;
			m_allocator.destruct(m_head);
			m_allocator.deallocate(m_head);
			if(m_head == last)
				break;
			m_head = next;
		}
		m_head = nullptr;
		m_size = 0;
	}

	void push_back( const T& data )
	{
		yyListNode<T>* node = m_allocator.allocate(1);
		m_allocator.construct(node, yyListNode<T>());
		node->m_data = data;

		++m_size;
		if(!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left  = m_head;
		}
		else
		{
			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
	}

	void push_front( const T& data )
	{
		/*yyListNode<T>* node = m_allocator.allocate(1);
		m_allocator.construct(node, data);
		++m_size;
		if(!m_first)
		{
			m_first = node;
			m_last  = node;
		}
		else
		{
			node->m_next = m_first;
			m_first->m_prev = node;
			m_first = node;
		}*/
	}

	size_t size() const 
	{
		return m_size; 
	}

	T& front()
	{
		return m_head->m_data;
	}



	void pop_front()
	{
		if(!m_size)
			return;
		--m_size;
		auto next = m_head->m_right;
		auto last = m_head->m_left;
		m_allocator.destruct(m_head);
		m_allocator.deallocate(m_head);
		if(next == m_head)
		{
			m_head = nullptr;
			return;
		}
		m_head = next;
		next->m_left = last;
		last->m_right = next;
	}

	bool   empty() const 
	{
		return m_size == 0u; 
	}

	yyListNode<T>* head(){ return m_head; }
};

#endif