#pragma once 

#include <stdio.h>
#include <memory>
#include <stdexcept>

using namespace std;

template<class _Ty>
class linkElement
{
public:
	shared_ptr<linkElement<_Ty>> left;
	shared_ptr<linkElement<_Ty>> right;
	_Ty value;

	~linkElement()
	{
	}

	linkElement(const _Ty& value, shared_ptr<linkElement<_Ty>> left, shared_ptr<linkElement<_Ty>> right)
	{
		this->left = left;
		this->right = right;
		this->value = value;
	}

	const inline shared_ptr<linkElement>& operator--()
	{
		return left;
	}
	const inline shared_ptr<linkElement>& operator++()
	{
		return right;
	}
};

template<class T>
class linkedList : public initializer_list<T>
{
private:

	using link = shared_ptr<T>;
	using SharedElementLink = shared_ptr<linkElement<T>>;

	inline int iterator_index(const T& value)
	{
		int i = 0;
		iterator_continue(_first, value, i);
		return i;
	}

	inline bool iterator_continue(SharedElementLink& fPos, const T& value, int& _index)
	{
		SharedElementLink target = fPos;
		while (target) // существует сылка на цель while{}
		{
			if (target->value == value)
			{
				return true;
			}
			else
			{
				target = target->right;
			}
			++_index; // продолжаем с текущего индекса
		}
		_index = -1;
		return false;
	}

	//отцепляет ссылку или удаляет элемент между элементами
	inline bool iterator_delink(SharedElementLink fPos)
	{
		bool result = false;

		if (fPos)
		{
			if (fPos == _first)
			{
				_first = fPos->right;
				fPos->left = nullptr;
				if (!_first)
					_last = nullptr;
				return true;
			}
			else if (fPos == _last)
			{
				_last = fPos->left;
				if (!_last)
					_first = nullptr;
				_last->right = nullptr;
				return true;
			}

			//прикрепляем левую сылку к правому 
			if (fPos->left)
			{
				fPos->left->right = fPos->right;
				result = true;
			}
			//приклепляем правую сылку к левому
			if (fPos->right)
			{
				fPos->right->left = fPos->left;
				result = true;
			}
		}
		return result;
	}

	//вставляю значение в позицию 
	inline SharedElementLink iterator_dolink(SharedElementLink fPos, const T& val, bool _offLeft = true)
	{
		SharedElementLink temp = make_shared<linkElement<T>>(linkElement<T>(val, nullptr, nullptr));

		//главный вход first
		if (fPos == _first)
		{
			if (_first)
			{
				if (!_offLeft && !_first->right)
				{
					_last = temp;
				}
			}
			else
			{
				_last = _first = temp;
				return temp;
			}
		}

		if (_offLeft)
		{
			temp->left = fPos->left;
			temp->right = fPos;
			fPos->left = temp;
			if (fPos == _first)
				_first = temp;
		}
		else
		{
			temp->right = fPos->right;
			temp->left = fPos;
			fPos->right = temp;
			if (fPos == _last)
				_last = temp;
		}


		return temp;
	}
protected:
	SharedElementLink _first; // first
	SharedElementLink _last;  // last
public:

	~linkedList()
	{
		clear();
	}

	//constructor
	linkedList() : initializer_list<T>()
	{

	}

	const T& at(const int& index)
	{
		return ((SharedElementLink)this->operator[](index))->value;
	}

	inline void set(const int& index, const T& value)
	{
		operator[](index)->value = value;
	}

	void remove(const int& index)
	{
		iterator_delink(this->operator[](index));
	}

	bool remove(SharedElementLink& link)
	{
		return iterator_delink(link);
	}

	inline bool empty()
	{
		return !_first;
	}

	bool removeFirst()
	{
		return remove(_first);
	}

	bool removeLast()
	{
		return remove(_last);
	}

	SharedElementLink addFirst(const T& value)
	{
		return insert(_first, value, true);
	}

	SharedElementLink addLast(const T& value)
	{
		return insert(_last, value, false);
	}

	void clear()
	{
		_last = _first = nullptr;
	}

	int indexOf(const T& value)
	{
		return iterator_index(value);
	}

	// сущеcтвует ? :)
	bool contains(const T& value)
	{
		return iterator_index((T)value) != -1;
	}

	// аргумент _off это-индекс, куда надо поместить вниз
	SharedElementLink insert(int _off, const T& value)
	{
		return insert(this->operator[](_off), value, true);
	}

	SharedElementLink insert(SharedElementLink& pos, const T& value, bool _offLeft)
	{
		//буквально приклеиваем сылку 
		return iterator_dolink(pos, value, _offLeft);
	}

	int size()
	{
		if (empty())
			return 0;
		int n = 0;
		SharedElementLink* ref = &_first;
		do
		{
			n++;
		} while (*(ref = &(*ref)->right));
		return n;
	}

	const T& first()
	{
		if (_first)
			return _first->value;
		else nullptr;
	}
	const T& last()
	{
		if (_last)
			return _last->value;
		else nullptr;
	}

	//индексатор
	SharedElementLink operator[](const int& index) throw()
	{
		if (index < 0 || !_first)
		{
		throwing:
			throw std::out_of_range("Index");
		}

		register size_t i;
		auto ref = _first;
		for (i = 0; i <= index; ++i)
		{
			if (i == index)
				return ref;
			else if (!ref) // конец!
				goto throwing;
			else // идем далее по ссылкам до конца!
				ref = ref->right;
		}
	}

};

