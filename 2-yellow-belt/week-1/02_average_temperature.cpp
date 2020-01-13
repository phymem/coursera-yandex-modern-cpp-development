#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

int64_t calc_avg(const vector<int64_t>& vec) {
	int64_t tot = 0;
	for (auto v : vec) {
		tot += v;
	}
	return tot / (int64_t)vec.size();
}

int main() {

	int n;
	cin >> n;

	vector<int64_t> vec(n);
	for (auto& v : vec) {
		cin >> v;
	}

	int64_t avg = calc_avg(vec);
	vector<int> res;
	for (unsigned int i = 0; i < vec.size(); ++i) {
		if (vec[i] > avg) {
			res.push_back(i);
		}
	}

	cout << res.size() << '\n';
	for (auto v : res) {
		cout << v << ' ';
	}
	cout << '\n';

	return 0;
}
