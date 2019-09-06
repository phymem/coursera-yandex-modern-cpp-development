// #define DBG

#ifdef DBG
#include <iostream>
using namespace std;
#endif

void UpdateIfGreater(int first, int& second) {
	if (first > second) second = first;
}

#ifdef DBG
int main() {

	int a, b;
	cin >> a >> b;

	UpdateIfGreater(a, b);

	cout << "a=" << a << " b=" << b << "\n";

	return 0;
}
#endif
