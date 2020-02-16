#include "test_runner.h"

#include <algorithm> // is_sorted
using namespace std;

template <typename T>
void Swap(T* first, T* second) {
	T tmp = *first;
	*first = *second;
	*second = tmp;
}

template <typename T>
void SortPointers(vector<T*>& pointers) {
	for (auto i = begin(pointers); i != end(pointers) - 1; ++i) {
		for (auto j = i + 1; j != end(pointers); ++j) {
			if (**i > **j) {
				Swap(*i, *j);
			}
		}
	}
}

template <typename T>
void ReversedCopy(T* source, size_t count, T* destination) {

	if (count && destination < source) {
		T* src = source + count - 1;
		while (count && destination != source) {
			*destination++ = *src--;
			count--;
		}
	}

	if (count && destination > source) {
		T* dst = destination + count - 1;
		while (count && destination != source) {
			*dst-- = *source++;
			count--;
		}
	}

	if (count && destination == source) {
		T* last = source + count - 1;
		while (source < last) {
			Swap(source++, last--);
		}
	}
}

void TestSwap() {
	int a = 1;
	int b = 2;
	Swap(&a, &b);
	ASSERT_EQUAL(a, 2);
	ASSERT_EQUAL(b, 1);

	string h = "world";
	string w = "hello";
	Swap(&h, &w);
	ASSERT_EQUAL(h, "hello");
	ASSERT_EQUAL(w, "world");
}

void TestSortPointers() {
	int one = 1;
	int two = 2;
	int three = 3;

	vector<int*> pointers;
	pointers.push_back(&two);
	pointers.push_back(&three);
	pointers.push_back(&one);

	SortPointers(pointers);

	ASSERT_EQUAL(pointers.size(), 3u);
	ASSERT_EQUAL(*pointers[0], 1);
	ASSERT_EQUAL(*pointers[1], 2);
	ASSERT_EQUAL(*pointers[2], 3);
}

void TestReverseCopy() {
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))
	{
		int src[] = { 0, 1, 2, 3, 4, 5 };
		int dst[] = { 0, 0, 0, 0, 0, 0 };
		ReversedCopy(src, ARRAY_SIZE(src), dst);

		const vector<int> res = { 5, 4, 3, 2, 1, 0 };
		ASSERT_EQUAL(vector<int>(dst, dst + ARRAY_SIZE(dst)), res);
	}
	{
		int dst[] = { 0, 0, 0, 0, 0, 0 };
		int src[] = { 0, 1, 2, 3, 4, 5 };
		ReversedCopy(src, sizeof(src) / sizeof(*src), dst);

		const vector<int> res = { 5, 4, 3, 2, 1, 0 };
		ASSERT_EQUAL(vector<int>(dst, dst + ARRAY_SIZE(dst)), res);
	}
	{
		int src[] = { 0, 1, 2, 3, 4, 5 };
		ReversedCopy(src, 5, src + 1);

		const vector<int> res = { 0, 4, 3, 2, 1, 0 };
		ASSERT_EQUAL(vector<int>(src, src + ARRAY_SIZE(src)), res);
	}
	{
		int src[] = { 0, 1, 2, 3, 4, 5 };
		ReversedCopy(src + 1, 4, src);

		const vector<int> res = { 4, 3, 2, 1, 4, 5 };
		ASSERT_EQUAL(vector<int>(src, src + ARRAY_SIZE(src)), res);
	}
	{
		int src[] = { 5 };
		ReversedCopy(src, 1, src);

		const vector<int> res = { 5 };
		ASSERT_EQUAL(vector<int>(src, src + ARRAY_SIZE(src)), res);
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSwap);
	RUN_TEST(tr, TestSortPointers);
	RUN_TEST(tr, TestReverseCopy);
	return 0;
}
