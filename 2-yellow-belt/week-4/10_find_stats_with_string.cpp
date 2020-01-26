#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

template <typename RandomIt>
pair<RandomIt, RandomIt> FindStartsWith(
	RandomIt range_begin, RandomIt range_end, const string& prefix) {
	size_t pref_len = prefix.length();
	return equal_range(range_begin, range_end, prefix,
		[pref_len](const string& s1, const string& s2) {
			return s1.substr(0, pref_len) < s2.substr(0, pref_len);
		});
}

int main() {
	const vector<string> sorted_strings = {"moscow", "motovilikha", "murmansk"};

	const auto mo_result =
			FindStartsWith(begin(sorted_strings), end(sorted_strings), "mo");
	for (auto it = mo_result.first; it != mo_result.second; ++it) {
		cout << *it << " ";
	}
	cout << endl;

	const auto mt_result =
			FindStartsWith(begin(sorted_strings), end(sorted_strings), "mt");
	cout << (mt_result.first - begin(sorted_strings)) << " " <<
			(mt_result.second - begin(sorted_strings)) << endl;

	const auto na_result =
			FindStartsWith(begin(sorted_strings), end(sorted_strings), "na");
	cout << (na_result.first - begin(sorted_strings)) << " " <<
			(na_result.second - begin(sorted_strings)) << endl;

	return 0;
}

