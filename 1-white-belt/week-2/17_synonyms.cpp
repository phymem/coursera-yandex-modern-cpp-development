#include <iostream>
#include <string>
#include <map>
#include <set>

using namespace std;

int main() {

	int n;
	cin >> n;

	map<string, set<string>> m;

	while (n--) {
		string cmd;
		cin >> cmd;

		if (cmd == "ADD") {
			string w1, w2;
			cin >> w1 >> w2;

			m[w1].insert(w2);
			m[w2].insert(w1);
		}
		else if (cmd == "COUNT") {
			string w;
			cin >> w;

			auto it = m.find(w);
			if (it == m.end())
				cout << "0\n";
			else
				cout << it->second.size() << '\n';
		}
		else if (cmd == "CHECK") {
			string w1, w2;
			cin >> w1 >> w2;

			auto it = m.find(w1);
			if (it == m.end() || it->second.count(w2) == 0)
				cout << "NO\n";
			else
				cout << "YES\n";
		}
	}

	return 0;
}
