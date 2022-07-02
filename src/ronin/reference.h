#pragma once

#include <stdio.h>

#undef nullptr
#define nullptr 0

//#define _ref std::shared_ptr

/*
template <typename _Ty = Object>
class refin
{
	_Ty* _ptr;
	uint8_t refCount;
public:
	using type = _Ty;
	shared_ptr
	//create empty
	refin() {
		_ptr = nullptr; refCount = 0;
	}

	refin(refin& ref) {
		_ptr = ref._ptr;
		refCount = ++refCount;
	}

	~refin() {
		if (!--refCount)
			GC::gc_unload(_ptr);
	}

	bool isUnique() {
		return refCount == 1;
	}

	void reset() {
		_ptr = nullptr;
	}

	explicit bool operator() {
		return _ptr != 0;
	}

	_Ty& opearator* () {
		return _ptr;
	}

	_Ty& operator=(nullptr_t) {
		if (*this())
		{
			reset();
		}
	}
};


*/