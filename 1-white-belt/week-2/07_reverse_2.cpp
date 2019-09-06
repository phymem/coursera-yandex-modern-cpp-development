#include <iostream>
#include <vector>

using namespace std;

vector<int> Reversed(const vector<int>& vec) {
	vector<int> ret;
	if (vec.size()) {
		for (int i = vec.size() - 1; i >= 0; --i) {
			ret.push_back(vec[i]);
		}
	}
	return ret;
}

ostream& operator << (ostream& os, const vector<int>& vec) {
	for (auto v : vec) {
		os << v << " ";
	}
	return os;
}

int main() {

	vector<int> vin = { 1, 2, 3, 4, 5 };
	vector<int> vout = Reversed(vin);

	cout << vout;

	return 0;
}
