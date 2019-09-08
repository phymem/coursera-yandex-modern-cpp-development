#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int main() {

	int n;
	cin >> n;

	vector<int> v;
	while (n--) {
		int i;
		cin >> i;
		v.push_back(i);
	}

	sort(begin(v), end(v), [](int a, int b) { return abs(a) < abs(b); });

	for (auto i : v) {
		cout << i << ' ';
	}
	cout << endl;

	return 0;
}
