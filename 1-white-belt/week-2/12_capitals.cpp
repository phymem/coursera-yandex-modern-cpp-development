#include <iostream>
#include <string>
#include <map>

using namespace std;

int main() {

	int n;
	cin >> n;

	map<string, string> m;

	while (n--) {
		string cmd;
		cin >> cmd;

		if (cmd == "CHANGE_CAPITAL") {
			string country, new_capital;
			cin >> country >> new_capital;

			auto it = m.find(country);
			if (it == m.end()) {
				cout << "Introduce new country " << country
					<< " with capital " << new_capital;
				m[country] = new_capital;
			}
			else if (it->second == new_capital) {
				cout << "Country " << country << " hasn't changed its capital";
			}
			else {
				cout << "Country " << country << " has changed its capital from "
					<< it->second << " to " << new_capital;
				m[country] = new_capital;
			}
		}
		else if (cmd == "RENAME") {
			string old_country, new_country;
			cin >> old_country >> new_country;

			if (old_country == new_country
				|| !m.count(old_country)
				|| m.count(new_country)) {
				cout << "Incorrect rename, skip";
			}
			else {
				string capital = m[old_country];
				cout << "Country " << old_country << " with capital " << capital
					<< " has been renamed to " << new_country;
				m.erase(old_country);
				m[new_country] = capital;
			}
		}
		else if (cmd == "ABOUT") {
			string country;
			cin >> country;

			auto it = m.find(country);
			if (it == m.end()) {
				cout << "Country " << country << " doesn't exist";
			}
			else {
				cout << "Country " << country << " has capital " << it->second;
			}
		}
		else if (cmd == "DUMP") {
			if (m.empty()) {
				cout << "There are no countries in the world";
			}
			else {
				for (const auto& val : m) {
					cout << val.first << "/" << val.second << " ";
				}
			}
		}

		cout << '\n';
	}

	return 0;
}
