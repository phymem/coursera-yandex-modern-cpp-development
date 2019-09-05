#include <iostream>
#include <algorithm>

using namespace std;

int main() {

	double n, a, b, x, y;
	cin >> n >> a >> b >> x >> y;

	if (n > b)
		cout << n * (1.0 - y * 0.01);
	else if (n > a)
		cout << n * (1.0 - x * 0.01);
	else
		cout << n;

	return 0;
}
