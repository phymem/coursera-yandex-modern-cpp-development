#include <iostream>
#include <cstdio>

using namespace std;

int main() {

	int a;
	cin >> a;

	if (a == 0) {
		cout << 0;
		return 0;
	}

	bool print_zero = false;
	for (unsigned int msk = 0x80000000; msk; msk >>= 1) {
		if (a & msk) {
			print_zero = true;
			cout << '1';
		}
		else if (print_zero) {
			cout << '0';
		}
	}

	return 0;
}
