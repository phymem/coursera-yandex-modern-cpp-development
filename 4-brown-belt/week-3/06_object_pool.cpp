#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;

template <class T>
class ObjectPool {
public:
	T* Allocate() {
		if (T* ret = TryAllocate())
			return ret;

		T* ret = new T();
		allocated.insert(ret);
		return ret;
	}
	T* TryAllocate() {
		if (freed.size()) {
			T* ret = freed.front();
			freed.pop();
			allocated.insert(ret);
			return ret;
		}
		return nullptr;
	}

	void Deallocate(T* obj) {
		auto it = allocated.find(obj);
		if (it == allocated.end())
			throw invalid_argument("invalid obect pointer");

		allocated.erase(it);
		freed.push(obj);
	}

	~ObjectPool() {
		for (T* t : allocated)
			delete t;
		for (; freed.size(); freed.pop())
			delete freed.front();
	}

private:

	set<T*> allocated;
	queue<T*> freed;
};

void TestObjectPool() {
	ObjectPool<string> pool;

	auto p1 = pool.Allocate();
	auto p2 = pool.Allocate();
	auto p3 = pool.Allocate();

	*p1 = "first";
	*p2 = "second";
	*p3 = "third";

	pool.Deallocate(p2);
	ASSERT_EQUAL(*pool.Allocate(), "second");

	pool.Deallocate(p3);
	pool.Deallocate(p1);
	ASSERT_EQUAL(*pool.Allocate(), "third");
	ASSERT_EQUAL(*pool.Allocate(), "first");

	pool.Deallocate(p1);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestObjectPool);
	return 0;
}
