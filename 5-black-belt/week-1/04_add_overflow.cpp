#include <iostream>
#include <limits>

using namespace std;

int main() {

	int64_t a, b;
	cin >> a >> b;

	if ((a > 0 && b > numeric_limits<int64_t>::max() - a)
		|| (a < 0 && b < numeric_limits<int64_t>::min() - a)) {
		cout << "Overflow!\n";
		return 2;
	}

	cout << a + b << endl;

	return 0;
}
