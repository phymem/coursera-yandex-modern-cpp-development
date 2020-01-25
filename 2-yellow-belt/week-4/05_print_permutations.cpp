#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main() {
	int n;
	cin >> n;

	vector<int> vec;
	while (n > 0) {
		vec.push_back(n--);
	}

	do {
		for (int i : vec) {
			cout << i << ' ';
		}
		cout << endl;
	} while (prev_permutation(begin(vec), end(vec)));

	return 0;
}
