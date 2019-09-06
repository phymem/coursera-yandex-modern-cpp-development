#define DBG

#ifdef DBG
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#endif

vector<string> PalindromFilter(const vector<string>& words, int minLen) {
	vector<string> ret;
	for (const auto& s : words) {

		if (s.length() < minLen)
			continue;

		const char* ps = s.c_str();
		const char* pe = ps + s.length() - 1;

		bool palindrom = true;
		while (ps < pe) {
			if (*ps != *pe) {
				palindrom = false;
				break;
			}
			++ps;
			--pe;
		}

		if (palindrom)
			ret.push_back(s);
	}
	return ret;
}

#ifdef DBG
void PrintWords(const vector<string>& words) {
	for (const auto& w : words) {
		cout << w << " ";
	}
	cout << "\n";
}

int main() {
	PrintWords(PalindromFilter({ "abacaba", "aba" }, 5));
	PrintWords(PalindromFilter({ "abacaba", "aba" }, 2));
	PrintWords(PalindromFilter({ "weew", "bro", "code"}, 4));
	return 0;
}
#endif

