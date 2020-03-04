#include "test_runner.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	if (range_end - range_begin < 3)
		return;

	vector<typename RandomIt::value_type> elements(
		make_move_iterator(range_begin), make_move_iterator(range_end));

	size_t off = elements.size() / 3;

	auto it_split1 = elements.begin() + off;
	auto it_split2 = it_split1 + off;
	MergeSort(begin(elements), it_split1);
	MergeSort(it_split1, it_split2);
	MergeSort(it_split2, end(elements));

	vector<typename RandomIt::value_type> tmp;
	merge(
		make_move_iterator(begin(elements)),
		make_move_iterator(it_split1),
		make_move_iterator(it_split1),
		make_move_iterator(it_split2),
		back_inserter(tmp));
	merge(
		make_move_iterator(begin(tmp)),
		make_move_iterator(end(tmp)),
		make_move_iterator(it_split2),
		make_move_iterator(end(elements)),
		range_begin);
}

void TestIntVector() {
  vector<int> numbers = {6, 1, 3, 9, 1, 9, 8, 12, 1};
  MergeSort(begin(numbers), end(numbers));
  ASSERT(is_sorted(begin(numbers), end(numbers)));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestIntVector);
  return 0;
}
