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

	class Iterator
	{
	//	friend class ConstIterator;

		yyListNode<T>* m_node;
		yyListNode<T>* m_nodeEnd;
		bool m_isEnd;
	public:
		Iterator() :m_node(0), m_isEnd(true){}
		Iterator(yyListNode<T>* head) :m_node(head), m_isEnd(false)
		{
			if (!head)
			{
				m_isEnd = true;
			}
			else
			{
				m_nodeEnd = head->m_left;
			}
		}
		~Iterator() {}
		Iterator& operator ++() { 
			if (m_node == m_nodeEnd) {
				m_isEnd = true;
			}
			m_node = m_node->m_right;
			return *this; 
		}
		//Iterator& operator --() { return *this; }
		//Iterator  operator ++(s32) { Iterator tmp = *this; return tmp; }
		//Iterator  operator --(s32) { Iterator tmp = *this;  return tmp; }
		bool operator ==(const Iterator&      other) const { 
			if (m_isEnd != other.m_isEnd) return false;
			return true;
		}
		bool operator !=(const Iterator&      other) const {
			if (m_isEnd != other.m_isEnd) return true;
			return false;
		}
		yyListNode<T>& operator*() { 
			return *m_node;
		}
		yyListNode<T>* operator->() {
			return m_node;
		}
	};

	Iterator begin()
	{
		return Iterator(m_head);
	}
	Iterator end()
	{
		return Iterator();
	}

	u32 find_size()
	{
		u32 size = 0;
		if (m_head)
		{
			auto curr = m_head;
			auto last = m_head->m_left;
			while (true)
			{
				++size;
				if (curr == last)
					break;
				curr = curr->m_right;
			}
		}
		return size;
	}

	yyListNode<T>* find(u32 index)
	{
		if (m_head)
		{
			u32 indexCounter = 0;
			auto curr = m_head;
			auto last = m_head->m_left;
			while (true)
			{
				if (indexCounter == index)
					return curr;

				++indexCounter;

				if (curr == last)
				{
					break;
				}

				curr = curr->m_right;
			}
		}
		return nullptr;
	}

	yyListNode<T>* find_by_value(const T& data)
	{
		if (m_head)
		{
			auto curr = m_head;
			auto last = m_head->m_left;
			while (true)
			{
				if (curr->m_data == data)
					return curr;

				if (curr == last)
				{
					break;
				}

				curr = curr->m_right;
			}
		}
		return nullptr;
	}

	private:
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


	~yyList(){
		clear();
	}

	void erase_by_node(yyListNode<T>* object) {
		if (!m_head)
			return false;

		object->m_left->m_right = object->m_right;
		object->m_right->m_left = object->m_left;
		
		if (object == m_head)
			m_head = m_head->m_right;

		if (object == m_head)
			m_head = 0;

		m_allocator.deallocate(object);
		m_allocator.destruct(object);
		
		--m_size;
	}

	bool erase_first(const T& object){
		if (!m_head)
			return false;
		
		auto node = m_head;
		auto last = m_head->m_left;
		while(true)
		{
			auto next = node->m_right;
			if(node->m_data == object)
			{
				if(node == m_head)
					pop_front();
				else
				{
					--m_size;
					node->m_left->m_right = node->m_right;
					node->m_right->m_left = node->m_left;
					m_allocator.destruct(node);
					m_allocator.deallocate(node);
				}
				return true;
			}
			if(node == last)
				break;
			node = next;
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

	void push_front(const T& data)
	{
		yyListNode<T>* node = m_allocator.allocate(1);
		m_allocator.construct(node, yyListNode<T>());
		node->m_data = data;

		++m_size;
		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			node->m_right = m_head;
			node->m_left = m_head->m_left;
			m_head->m_left->m_right = node;
			m_head->m_left = node;
			m_head = node;
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

	yyListNode<T>* head() { return m_head; }
	yyListNode<T>* tail(){ return m_head ? m_head->m_left : nullptr; }

	class Iterator
	{
		//	friend class ConstIterator;

		yyListNode<T>* m_node;
		yyListNode<T>* m_nodeEnd;
		bool m_isEnd;
	public:
		Iterator() :m_node(0), m_isEnd(true) {}
		Iterator(yyListNode<T>* head) :m_node(head), m_isEnd(false)
		{
			if (!head)
			{
				m_isEnd = true;
			}
			else
			{
				m_nodeEnd = head->m_left;
			}
		}
		~Iterator() {}
		Iterator& operator ++() {
			if (m_node == m_nodeEnd) {
				m_isEnd = true;
			}
			m_node = m_node->m_right;
			return *this;
		}
		//Iterator& operator --() { return *this; }
		//Iterator  operator ++(s32) { Iterator tmp = *this; return tmp; }
		//Iterator  operator --(s32) { Iterator tmp = *this;  return tmp; }
		bool operator ==(const Iterator&      other) const {
			if (m_isEnd != other.m_isEnd) return false;
			return true;
		}
		bool operator !=(const Iterator&      other) const {
			if (m_isEnd != other.m_isEnd) return true;
			return false;
		}
		const T& operator*() {
			return *m_node->m_data;
		}
		T* operator->() {
			return m_node->m_data;
		}
	};

	Iterator begin()	{
		return Iterator(m_head);
	}
	Iterator end()	{
		return Iterator();
	}
};

#endif