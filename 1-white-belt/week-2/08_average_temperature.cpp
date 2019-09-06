#include <iostream>
#include <vector>

using namespace std;

float calc_avg(const vector<int>& vec) {
	int tot = 0;
	for (auto v : vec) {
		tot += v;
	}
	return (float)tot / (float)vec.size();
}

int main() {

	int n;
	cin >> n;

	vector<int> vec(n);
	for (auto& v : vec) {
		cin >> v;
	}

	float avg = calc_avg(vec);
	vector<int> res;
	for (unsigned int i = 0; i < vec.size(); ++i) {
		if (vec[i] > avg) {
			res.push_back(i);
		}
	}

	cout << res.size() << "\n";
	for (auto v : res) {
		cout << v << ' ';
	}

	return 0;
}
