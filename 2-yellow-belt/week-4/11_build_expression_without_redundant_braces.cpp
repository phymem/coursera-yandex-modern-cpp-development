#include <iostream>
#include <string>
#include <deque>

int get_op_rank(char op) {
	return op == '*' || op == '/';
}

bool is_greater(char op1, char op2) {
	return get_op_rank(op1) > get_op_rank(op2);
}

using namespace std;

int main() {

	string val;
	cin >> val;

	int num;
	cin >> num;

	deque<string> expr;
	expr.push_back(val);

	char prev_op = '*';


	while (num--) {
		string op;
		cin >> op >> val;

		if (!is_greater(op[0], prev_op)) {
			expr.push_back(" ");
		}
		else {
			expr.push_front("(");
			expr.push_back(") ");
		}
		prev_op = op[0];

		expr.push_back(op + " " + val);
	}

	for (auto s : expr) {
		cout << s;
	}
	cout << endl;

	return 0;
}
