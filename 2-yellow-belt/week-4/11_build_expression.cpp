#include <iostream>
#include <string>
#include <deque>

using namespace std;

int main() {

	string val;
	cin >> val;

	int num;
	cin >> num;

	deque<string> expr;
	expr.push_back(val);

	while (num--) {
		string op;
		cin >> op >> val;

		expr.push_front("(");
		expr.push_back(") " + op + " " + val);
	}

	for (auto s : expr) {
		cout << s;
	}
	cout << endl;

	return 0;
}
