#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <stdexcept>
#include <list>
#include <set>

using namespace std;

template <class T>
class ObjectPool {
public:

	T* Allocate() {
		if (T* ret = TryAllocate())
			return ret;

		T* ret = new T();
		m_allocated.insert(ret);
		return ret;
	}
	T* TryAllocate() {
		if (m_freed.empty())
			return nullptr;

		T* ret = m_freed.front();
		m_freed.pop_front();
		m_allocated.insert(ret);
		return ret;
	}

	void Deallocate(T* object) {
		auto it = m_allocated.find(object);
		if (it == end(m_allocated))
			throw invalid_argument("ObjectPool::Deallocate");

		m_allocated.erase(it);
		m_freed.push_back(object);
	}

	~ObjectPool() {
		for (auto p : m_allocated) {
			delete p;
		}
		while (m_freed.size()) {
			delete m_freed.front();
			m_freed.pop_front();
		}
	}

private:

	set<T*> m_allocated;
	list<T*> m_freed;
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
