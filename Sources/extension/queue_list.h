#pragma once 

#include "linked_list.h"

template<class T> class Queue : private linkedList<T>
{
public:
	Queue():linkedList<T>()
	{

	}
	void enqueue(const T& value)
	{
		this->addLast(value);
	}
	const T dequeue()
	{
		auto val = ((linkedList<T>*)this)->at(0);
		this->removeFirst();
		return val;
	}

	template<typename _Cont>
	void append(_Cont& container) {
		for_each(begin(container), end(container), [this](auto&el) {
			this->enqueue(el);
		});
	}

	const T peek()
	{
		return ((linkedList<T>*)this)->operator[](0);
	}
        const int size() noexcept
	{
		return ((linkedList<T>*)this)->size();
	}
	void clear()
	{
		((linkedList<T>*)this)->clear();
	}
};
