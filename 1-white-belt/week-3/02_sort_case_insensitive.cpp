#include <algorithm>
#include <iostream>
#include <vector>
#include <cctype>

using namespace std;

int main() {

	int n;
	cin >> n;

	vector<string> v;
	while (n--) {
		string str;
		cin >> str;
		v.push_back(str);
	}

	sort(begin(v), end(v),
		[](string l, string r) {
			transform(begin(l), end(l), begin(l), [](char c) { return tolower(c); });
			transform(begin(r), end(r), begin(r), [](char c) { return tolower(c); });
			return l < r;
		});

	for (const auto& i : v) {
		cout << i << ' ';
	}
	cout << endl;

	return 0;
}
