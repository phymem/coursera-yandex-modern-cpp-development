#include <iostream>
#include <string>
#include <map>
#include <set>

using namespace std;

int main() {

	int n;
	cin >> n;

	map<set<string>, int> m;

	while (n--) {
		int num;
		cin >> num;

		set<string> stops;
		while (num--) {
			string stop;
			cin >> stop;
			stops.insert(stop);
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
