#include <iostream>
#include <vector>
#include <string>

using namespace std;

const unsigned int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

vector<vector<string>> calendar;

void dump_calendar() {
	for (unsigned int i = 0; i < calendar.size(); ++i) {
		auto& vs = calendar[i];
		if (vs.size()) {
			cout << "i=" << i << " size=" << vs.size();
			for (const auto& s : vs)
				cout << ' ' << s;
			cout << '\n';
		}
	}
}

int main() {

	unsigned int cur_month = 0;
	calendar.resize(days_in_month[cur_month]);

	int n;
	cin >> n;
	while (n--) {
		string cmd;
		cin >> cmd;

		if (cmd == "ADD") {
			unsigned int i;
			string s;
			cin >> i >> s;
			i--;
			if (i < calendar.size())
				calendar[i].push_back(s);
		}
		else if (cmd == "NEXT") {
			cur_month++;
			cur_month %= sizeof(days_in_month) / sizeof(*days_in_month);
			int new_size = days_in_month[cur_month];

			if (calendar.size() > new_size) {
				vector<string>& last_day = calendar[new_size - 1];
				for (int i = new_size; i < calendar.size(); ++i) {
					vector<string>& day = calendar[i];
					last_day.insert(last_day.end(), day.begin(), day.end());
				}
			}

			calendar.resize(new_size);
		}
		else if (cmd == "DUMP") {
			unsigned int i;
			cin >> i;
			i--;
			if (i < calendar.size()) {
				const vector<string>& vs = calendar[i];
				cout << vs.size();
				for (const auto& s : vs) {
					cout << ' ' << s;
				}
				cout << '\n';
			}
		}
	}

	return 0;
}
