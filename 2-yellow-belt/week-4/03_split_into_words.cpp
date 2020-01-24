#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

vector<string> SplitIntoWords(const string& s) {
	vector<string> ret;

	auto first = begin(s);
	while (true) {
		auto last = find(first, end(s), ' ');
		ret.push_back({first, last});

		if (last == end(s))
			break;

		first = ++last;
	}

	return ret;
}

int main() {
	string s = "C Cpp Java Python";

	vector<string> words = SplitIntoWords(s);
	cout << words.size() << " ";
	for (auto it = begin(words); it != end(words); ++it) {
		if (it != begin(words)) {
			cout << "/";
		}
		cout << *it;
	}
	cout << endl;

	return 0;
}
