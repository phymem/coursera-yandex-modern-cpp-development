#include <iostream>

#include "sum_reverse_sort.h"

int main() {
	cout << Sum(1, 4) << endl;

	cout << Reverse("abcd") << endl;

	vector<int> vec = { 4, 3, 2, 1 };
	Sort(vec);
	for (auto v : vec) {
		cout << v << " ";
	}
	cout << endl;

	return 0;
}
