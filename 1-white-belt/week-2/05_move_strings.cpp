// #define DBG

#ifdef DBG
#include <iostream>
#include <vector>
#include <string>
using namespace std;
#endif

void MoveStrings(vector<string>& src, vector<string>& dst) {
	dst.insert(dst.end(), src.begin(), src.end());
	src.clear();
}

#ifdef DBG
ostream& operator << (ostream& os, const vector<string>& vec) {
	for (auto s : vec) {
		os << s << " ";
	}
	return os;
}

int main() {
	vector<string> src = { "s1", "s2", "s3" };
	vector<string> dst = { "d1", "d2", "d3" };

	MoveStrings(src, dst);

	cout << "src: " << src << "\n";
	cout << "dst: " << dst << "\n";

	return 0;
}
#endif
