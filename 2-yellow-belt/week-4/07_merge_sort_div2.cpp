#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	if (range_end - range_begin < 2)
		return;

	vector<typename RandomIt::value_type> elements(range_begin, range_end);

	auto it_split = elements.begin() + elements.size() / 2;
	MergeSort(begin(elements), it_split);
	MergeSort(it_split, end(elements));

	merge(begin(elements), it_split, it_split, end(elements), range_begin);
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
