#include <iostream>
#include <string>

using namespace std;

int main() {

	string str;
	cin >> str;

	int num = 0;
	int ret = -2;

	for (int pos = 0; pos < str.length(); ++pos) {
		if (str[pos] == 'f') {
			num++;
			if (num == 1)
				ret = -1;
			if (num == 2) {
				ret = pos;
				break;
			}
		}
	}

	cout << ret;

	return 0;
}
