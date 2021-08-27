#include "vector.h"

#include "test_runner.h"

using namespace std;

class C {
public:
	inline static int created = 0;
	inline static int assigned = 0;
	inline static int deleted = 0;
	static void Reset() {
		created = assigned = deleted = 0;
	}

	C() {
		++created;
	}
	C(const C& other) {
		++created;
	}
	C& operator=(const C& other) {
		++assigned;
		return *this;
	}
	~C() {
		++deleted;
	}
};

void TestInit() {
	{
		C::Reset();
		Vector<C> v;
		ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0);
		ASSERT(v.Capacity() == 0 && v.Size() == 0);
	}

	{
		C::Reset();
		Vector<C> v(3);
		ASSERT(C::created == 3 && C::assigned == 0 && C::deleted == 0);
		ASSERT(v.Capacity() == 3 && v.Size() == 3);
	}
	ASSERT(C::deleted == 3);

	{
		C::Reset();
		Vector<C> v1(3);
		Vector<C> v2(v1);
		ASSERT(C::created == 6 && C::assigned == 0 && C::deleted == 0);
		ASSERT(v2.Capacity() == 3 && v2.Size() == 3);
	}
	ASSERT(C::deleted == 6);

	{
		C::Reset();
		Vector<C> v1(3);
		Vector<C> v2(std::move(v1));
		ASSERT(C::created == 3 && C::assigned == 0 && C::deleted == 0);
		ASSERT(v2.Capacity() == 3 && v2.Size() == 3);
	}
	ASSERT(C::deleted == 3);
}

void TestReserve() {
	{
		C::Reset();
		Vector<C> v;
		v.Reserve(2);
		ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0);
		ASSERT(v.Capacity() == 2 && v.Size() == 0);
	}
	ASSERT(C::deleted == 0);

	{
		C::Reset();
		Vector<C> v(2);
		v.Reserve(1);
		ASSERT(C::created == 2 && C::assigned == 0 && C::deleted == 0);
		ASSERT(v.Capacity() == 2 && v.Size() == 2);
	}
	ASSERT(C::deleted == 2);

	{
		C::Reset();
		Vector<C> v(2);
		v.Reserve(4);
		ASSERT(C::created == 4 && C::assigned == 0 && C::deleted == 2);
		ASSERT(v.Capacity() == 4 && v.Size() == 2);
	}
	ASSERT(C::deleted == 4);
}

void TestResize() {
	{
		C::Reset();
		Vector<C> v;
		v.Resize(2);
		ASSERT(C::created == 2 && C::assigned == 0 && C::deleted == 0);
		ASSERT(v.Capacity() == 2 && v.Size() == 2);
	}
	ASSERT(C::deleted == 2);

	{
		C::Reset();
		Vector<C> v(3);
		v.Resize(2);
		ASSERT(C::created == 3 && C::assigned == 0 && C::deleted == 1);
		ASSERT(v.Capacity() == 3 && v.Size() == 2);
	}
	ASSERT(C::deleted == 3);

	{
		C::Reset();
		Vector<C> v(1);
		v.Resize(2);
		ASSERT(C::created == 3 && C::assigned == 0 && C::deleted == 1);
		ASSERT(v.Capacity() == 2 && v.Size() == 2);
	}
	ASSERT(C::deleted == 3);
}

void TestAssign() {
	{
		C::Reset();
		Vector<C> v1(2), v2(3);
		ASSERT(C::created == 5 && C::assigned == 0 && C::deleted == 0);
		v1 = v2;
		ASSERT(C::created == 8 && C::assigned == 0 && C::deleted == 2);
		ASSERT(v1.Size() == 3 && v2.Size() == 3);
	}
	ASSERT(C::deleted == 8);

	{
		C::Reset();
		Vector<C> v1(3), v2(2);
		ASSERT(C::created == 5 && C::assigned == 0 && C::deleted == 0);
		v1 = v2;
		ASSERT(C::created == 5 && C::assigned == 2 && C::deleted == 1);
		ASSERT(v1.Size() == 2 && v2.Size() == 2);
	}
	ASSERT(C::deleted == 5);
}

void TestPopBack() {
	{
		C::Reset();
		Vector<C> v(2);
		v.PopBack();
		ASSERT(C::created == 2 && C::assigned == 0 && C::deleted == 1);
		ASSERT(v.Capacity() == 2 && v.Size() == 1);
	}
	ASSERT(C::deleted == 2);
}

void TestPushBack() {
	{
		C::Reset();
		Vector<C> v;
		C c;
		v.PushBack(c);
		ASSERT(C::created == 2 && C::assigned == 0 && C::deleted == 0);

		v.PushBack(v[0]);  // reallocation
		ASSERT(C::created == 4 && C::assigned == 0 && C::deleted == 1);
	}
	ASSERT(C::deleted == 4);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestInit);
	RUN_TEST(tr, TestReserve);
	RUN_TEST(tr, TestResize);
	RUN_TEST(tr, TestAssign);
	RUN_TEST(tr, TestPopBack);
	RUN_TEST(tr, TestPushBack);
	return 0;
}
