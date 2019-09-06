#include <iostream>
#include <vector>

using namespace std;

enum StateType {
	ST_CALM,
	ST_WORRIED
};

int main() {

	vector<StateType> queue;

	int n;
	cin >> n;
	while (n--) {
		string cmd;
		cin >> cmd;

		if (cmd == "WORRY") {
			int i;
			cin >> i;
			queue[i] = ST_WORRIED;
		}
		else if (cmd == "QUIET") {
			int i;
			cin >> i;
			queue[i] = ST_CALM;
		}
		else if (cmd == "COME") {
			int k;
			cin >> k;
			queue.resize(queue.size() + k);
		}
		else if (cmd == "WORRY_COUNT") {
			int num = 0;
			for (auto p : queue) {
				if (p == ST_WORRIED)
					num++;
			}
			cout << num << '\n';
		}
	}

	return 0;
}
