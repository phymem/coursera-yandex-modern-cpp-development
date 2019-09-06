#include <iostream>

using namespace std;

int Factorial(int x) {
	if (x < 0 || x == 1 || x == 0)
		return 1;
	return x * Factorial(x - 1);
}

int main() {

	int a;
	cin >> a;

	cout << Factorial(a);

	return 0;
}
