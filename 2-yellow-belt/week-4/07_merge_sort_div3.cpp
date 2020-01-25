#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	if (range_end - range_begin < 3)
		return;

	vector<typename RandomIt::value_type> elements(range_begin, range_end);

	size_t off = elements.size() / 3;

	auto it_split1 = elements.begin() + off;
	auto it_split2 = it_split1 + off;
	MergeSort(begin(elements), it_split1);
	MergeSort(it_split1, it_split2);
	MergeSort(it_split2, end(elements));

	vector<typename RandomIt::value_type> tmp;
	merge(begin(elements), it_split1, it_split1, it_split2, back_inserter(tmp));
	merge(begin(tmp), end(tmp), it_split2, end(elements), range_begin);
}

int main() {
  vector<int> v = {6, 4, 7, 6, 4, 4, 0, 1, 5};
  MergeSort(begin(v), end(v));
  for (int x : v) {
    cout << x << " ";
  }
  cout << endl;
  return 0;
}
