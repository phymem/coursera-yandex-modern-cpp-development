#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void PrintVectorPart(const vector<int>& numbers) {
	auto it = find_if( begin(numbers), end(numbers),
		[](int x) { return x < 0; });

	const char* delim = "";
	while (it != begin(numbers)) {
		cout << delim << *(--it);
		delim = " ";
	}
}

int main() {
	PrintVectorPart({6, 1, 8, -5, 4});
	cout << endl;
	PrintVectorPart({-6, 1, 8, -5, 4});
	cout << endl;
	PrintVectorPart({6, 1, 8, 5, 4});
	cout << endl;
	return 0;
}
