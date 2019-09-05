#include <iostream>

using namespace std;

int main() {

	int a, b;
	cin >> a >> b;

	if (a > b) {
		int t = a;
		a = b;
		b = t;
	}

	while (int r = b % a) {
		b = a;
		a = r;
	}

	cout << a;

	return 0;
}
