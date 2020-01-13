#include <iostream>
#include <cstdint>

using namespace std;

int main() {

	unsigned int num, den;
	cin >> num >> den;

	uint64_t res = 0;
	while (num--) {
		uint64_t w, h, d;
		cin >> w >> h >> d;

		res += w * h * d;
	}

	res *= den;

	cout << res << '\n';

	return 0;
}
