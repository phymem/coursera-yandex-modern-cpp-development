#include <iostream>
#include <string>

using namespace std;

int main() {
	string a, b, c;
	cin >> a >> b >> c;

	string min_str = a;
	if (min_str > b) min_str = b;
	if (min_str > c) min_str = c;
	cout << min_str;

	return 0;
}
