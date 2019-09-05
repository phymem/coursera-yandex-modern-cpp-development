#include <iostream>
#include <cmath>

using namespace std;

int main() {

	double a, b, c;
	cin >> a >> b >> c;

	if (a == 0 && b == 0)
		return 0;

	if (a == 0) {
		cout << -c / b;
	} else if (b == 0 && c == 0) {
		cout << 0.0;
	} else if (b == 0) {
		double d = -c / a;
		if (d > 0) {
			double x = sqrt(d);
			cout << x << " " << -x;
		}
	} else if (c == 0) {
		cout << -b / a << " " << 0.0;
	} else {
		double d = b * b - 4 * a * c;
		double _2a = 2 * a;

		if (d == 0.0) {
			cout << -b / _2a;
		} else if (d > 0) {
			cout << (-b + sqrt(d)) / _2a;
			cout << " ";
			cout << (-b - sqrt(d)) / _2a;
		}
	}

	return 0;
}
