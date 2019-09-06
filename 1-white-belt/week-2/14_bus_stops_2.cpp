#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

int main() {

	int n;
	cin >> n;

	map<vector<string>, int> m;

	while (n--) {

		int num;
		cin >> num;

		vector<string> stops;
		string stop;

		while (num--) {
			cin >> stop;
			stops.push_back(stop);
		}

		auto it = m.find(stops);
		if (it != m.end()) {
			cout << "Already exists for " << it->second;
		}
		else {
			int bus = m.size() + 1;
			m[stops] = bus;
			cout << "New bus " << bus;
		}
		cout << '\n';
	}

	return 0;
}
