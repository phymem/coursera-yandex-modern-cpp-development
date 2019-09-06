//#define DBG

#ifdef DBG
#include <iostream>
#include <string>
#include <set>
#include <map>
using namespace std;
#endif

set<string> BuildMapValuesSet(const map<int, string>& m) {
	set<string> ret;
	for (const auto& val : m) {
		ret.insert(val.second);
	}
	return ret;
}

#ifdef DBG
int main() {

	set<string> vals =
		BuildMapValuesSet({
			{1, "odd"},
			{2, "even"},
			{3, "odd"},
			{4, "even"},
			{5, "odd"}
		});

	for (const string& val : vals) {
		cout << val << " ";
	}
	cout << endl;

	return 0;
}
#endif
