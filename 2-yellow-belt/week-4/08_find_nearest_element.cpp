#include <iostream>
#include <algorithm>
#include <set>

using namespace std;

set<int>::const_iterator
FindNearestElement(const set<int>& numbers, int border) {

	const auto first_lower_or_equal = numbers.lower_bound(border);
	if (first_lower_or_equal == begin(numbers))
		return first_lower_or_equal;

	if (first_lower_or_equal == end(numbers))
		return prev(end(numbers));

	const auto first_lower = prev(first_lower_or_equal);
	return (border - *first_lower <= *first_lower_or_equal - border)
		? first_lower : first_lower_or_equal;
}

int main() {
  set<int> numbers = {1, 4, 6};
  cout <<
      *FindNearestElement(numbers, 0) << " " <<
      *FindNearestElement(numbers, 3) << " " <<
      *FindNearestElement(numbers, 5) << " " <<
      *FindNearestElement(numbers, 6) << " " <<
      *FindNearestElement(numbers, 100) << endl;

  set<int> empty_set;

  cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << endl;
  return 0;
}
