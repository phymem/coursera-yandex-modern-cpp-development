#include <stdexcept>
#include <vector>

using namespace std;

template <typename T>
class Deque {
public:

	Deque() {}

	bool Empty() const {
		return m_front.empty() && m_back.empty();
	}
	size_t Size() const {
		return m_front.size() + m_back.size();
	}

	void PushFront(const T& val) {
		m_front.push_back(val);
	}
	void PushBack(const T& val) {
		m_back.push_back(val);
	}

	T& operator[](size_t index) {
		if (index < m_front.size()) {
			return m_front[m_front.size() - 1 - index];
		}
		return m_back[index - m_front.size()];
	}
	const T& operator[](size_t index) const {
		if (index < m_front.size()) {
			return m_front[m_front.size() - 1 - index];
		}
		return m_back[index - m_front.size()];
	}

	T& At(size_t index) {
		if (index < Size())
			return operator[](index);
		throw out_of_range("Deque::At()");
	}
	const T& At(size_t index) const {
		if (index < Size())
			return operator[](index);
		throw out_of_range("Deque::At() const");
	}

	T& Front() {
		if (m_front.empty())
			return m_back.front();
		return m_front.back();
	}
	const T& Front() const  {
		if (m_front.empty())
			return m_back.front();
		return m_front.back();
	}

	T& Back() {
		if (m_back.empty())
			return m_front.front();
		return m_back.back();
	}
	const T& Back() const {
		if (m_back.empty())
			return m_front.front();
		return m_back.back();
	}
private:

	vector<T> m_front;
	vector<T> m_back;
};

//#define TEST_DEQUE
#ifdef TEST_DEQUE

#include "test_runner.h"

void TestDeque () {
	{
		Deque<int> d;
		ASSERT_EQUAL(d.Size(), 0);
		ASSERT(d.Empty());
	}
	{
		Deque<int> d;

		d.PushBack(1);
		ASSERT_EQUAL(d.Size(), 1);
		ASSERT(!d.Empty());

		d.PushFront(2);
		ASSERT_EQUAL(d.Size(), 2);
		ASSERT(!d.Empty());

		d.PushBack(3);
		ASSERT_EQUAL(d.Size(), 3);
		ASSERT(!d.Empty());
	}
	{
		Deque<int> d;

		d.PushBack(1);
		ASSERT_EQUAL(d.Front(), 1);
		ASSERT_EQUAL(d.Back(), 1);

		d.PushFront(2);
		ASSERT_EQUAL(d.Front(), 2);
		ASSERT_EQUAL(d.Back(), 1);

		d.PushBack(3);
		ASSERT_EQUAL(d.Front(), 2);
		ASSERT_EQUAL(d.Back(), 3);

		d.PushFront(4);
		ASSERT_EQUAL(d.Front(), 4);
		ASSERT_EQUAL(d.Back(), 3);
	}
	{
		Deque<int> d;
		d.PushFront(2);
		d.PushFront(1);
		d.PushBack(3);
		d.PushBack(4);

		ASSERT_EQUAL(d[0], 1);
		ASSERT_EQUAL(d[1], 2);
		ASSERT_EQUAL(d[2], 3);
		ASSERT_EQUAL(d[3], 4);
	}
	{
		Deque<int> d;
		bool caught_ex1;
		bool caught_ex2;

		caught_ex1 = false;
		caught_ex2 = false;
		try {
			d.At(10);
		}
		catch (out_of_range& e) {
			caught_ex1 = true;
		}
		ASSERT(caught_ex1);

		d.PushFront(1);
		caught_ex1 = true;
		try {
			ASSERT_EQUAL(d.At(0), 1);
			caught_ex1 = false;
			d.At(10);
		 }
		catch (out_of_range& e) {
			caught_ex2 = true;
		}
		ASSERT(!caught_ex1);
		ASSERT(caught_ex2);
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestDeque);
	return 0;
}
#endif // TEST_DEQUE
