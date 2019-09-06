#define DBG

#ifdef DBG
#include <iostream>
#include <vector>
using namespace std;
#endif

void Reverse(vector<int>& vec) {

	if (vec.empty())
		return;

	int f = 0;
	int l = vec.size() - 1;

	for (; f < l; ++f, --l) {
		int tmp = vec[f];
		vec[f] = vec[l];
		vec[l] = tmp;
	}
}

#ifdef DBG
ostream& operator << (ostream& os, const vector<int>& vec) {
	for (auto i : vec) {
		os << i << " ";
	}
	return os;
}

int main() {

	vector<int> vec = { 1, 2, 3, 4, 5 };

	Reverse(vec);

	cout << vec;

	return 0;
}
#endif
