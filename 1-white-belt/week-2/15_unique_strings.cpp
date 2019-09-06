#include <iostream>
#include <string>
#include <set>

using namespace std;

int main() {

	int n;
	cin >> n;

	set<string> names;

	while (n--) {
		string name;
		cin >> name;
		names.insert(name);
	}

	cout << names.size() << endl;

	return 0;
}
