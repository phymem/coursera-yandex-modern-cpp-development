#include <iostream>
#include <string>

using namespace std;

bool IsPalindrom(const string& str) {
	if (str.empty())
		return true;

	const char* ps = str.c_str();
	const char* pe = ps + str.length() - 1;

	while (ps < pe) {
		if (*ps != *pe)
			return false;
		++ps;
		--pe;
	}

	return true;
}

int main() {

	string str;
	cin >> str;

	cout << (IsPalindrom(str) ? "true" : "false");

	return 0;
}
