#ifndef _yyList_H_
#define _yyList_H_

#include "common/allocator.h"

template<typename T>
struct yyListNode
{
	yyListNode()
		:
		m_left(nullptr),
		m_right(nullptr)
	{}

	T m_data;

	yyListNode* m_left;
	yyListNode* m_right;
};

template<typename T>
class yyListFast
{
	yyListFast(const yyListFast& other) {};
	yyListFast(yyListFast&& other) {};
	yyListNode<T>* m_head;
	void _setHead(yyListNode<T>* newHead)
	{
		m_head = newHead;
		m_head->m_right = m_head;
		m_head->m_left = m_head;
	}
public:
	yyListFast()
		:
		m_head(nullptr)
	{

	}
	~yyListFast()
	{
		clear();
	}

	void clear()
	{
		if (m_head)
		{
			auto _end = m_head->m_left;
			while (true)
			{
				auto next = m_head->m_right;
				yyDestroy(m_head);
				if (m_head == _end)
				{
					m_head = nullptr;
					return;
				}
				m_head = next;
			}
		}
	}

	yyListNode<T>* head() { return m_head; }
	
	yyListNode<T>* push_back(const T& data)
	{
		yyListNode<T>* newNode = yyCreate<yyListNode<T>>();
		newNode->m_data = data;
		if (!m_head)
		{
			_setHead(newNode);
		}
		else
		{
			m_head->m_left->m_right = newNode;
			newNode->m_left = m_head->m_left;
			newNode->m_right = m_head;
			m_head->m_left = newNode;
		}
		return newNode;
	}

	yyListNode<T>* push_front(const T& data)
	{
		yyListNode<T>* newNode = yyCreate<yyListNode<T>>();
		newNode->m_data = data;
		if (!m_head)
		{
			_setHead(newNode);
		}
		else
		{
			m_head->m_left->m_right = newNode;
			newNode->m_left = m_head->m_left;
			newNode->m_right = m_head;
			m_head->m_left = newNode;
			m_head = newNode;
		}
		return newNode;
	}

	yyListNode<T>* insert_before(const T& data, yyListNode<T>* before_this_node)
	{
		assert(before_this_node);

		if (m_head == before_this_node)
			return push_front(data);
		else
		{
			yyListNode<T>* newNode = yyCreate<yyListNode<T>>();
			newNode->m_data = data;
			if (!m_head)
			{
				_setHead(newNode);
			}
			else
			{
				before_this_node->m_left->m_right = newNode;
				newNode->m_left = before_this_node->m_left;
				newNode->m_right = before_this_node;
				before_this_node->m_left = newNode;
			}
			return newNode;
		}
	}

	yyListNode<T>* insert_after(const T& data, yyListNode<T>* after_this_node)
	{
		assert(after_this_node);

		if (m_head->m_left == after_this_node)
			return push_back(data);
		else
		{
			yyListNode<T>* newNode = yyCreate<yyListNode<T>>();
			newNode->m_data = data;
			if (!m_head)
			{
				_setHead(newNode);
			}
			else
			{
				after_this_node->m_right->m_left = newNode;
				newNode->m_right = after_this_node->m_right;
				newNode->m_left = after_this_node;
				after_this_node->m_right = newNode;
			}
			return newNode;
		}
	}

	void erase_node(yyListNode<T>* node)
	{
		assert(node);

		node->m_left->m_right = node->m_right;
		node->m_right->m_left = node->m_left;

		auto second = m_head->m_right;

		yyDestroy(node);
		if (m_head == node)
		{
			if (m_head != second)
				m_head = second;
			else
				m_head = nullptr;
		}
	}

};

template<typename T>
class yyList
{
	yyList(const yyList& other){};
	yyList(yyList&& other){};
	

	std::size_t m_size;
	yyListNode<T>* m_head;
	yyAllocator<yyListNode<T>> m_allocator;
	
public:

	yyList()
		:
		m_size(0),
		m_head(nullptr)
	{}


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

	void clear()
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

	/*void insert_after(const T& data, const T& after)
	{
		if (!m_head)
		{
			push_back(data);
			return;
		}

	}*/

	//void push_front( const T& data )
	//{
	//	/*yyListNode<T>* node = m_allocator.allocate(1);
	//	m_allocator.construct(node, data);
	//	++m_size;
	//	if(!m_first)
	//	{
	//		m_first = node;
	//		m_last  = node;
	//	}
	//	else
	//	{
	//		node->m_next = m_first;
	//		m_first->m_prev = node;
	//		m_first = node;
	//	}*/
	//}

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