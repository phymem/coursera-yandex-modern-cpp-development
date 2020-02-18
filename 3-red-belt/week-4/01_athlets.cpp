#include <iostream>
#include <vector>
#include <list>

using namespace std;

int main() {
	list<int> athlets;
	vector<list<int>::iterator> athlet_pos(
		100000 /*max_athlet number*/ + 1, athlets.end());

	unsigned int num;
	cin >> num;

	while (num--) {
		int athlet, before_athlet;
		cin >> athlet >> before_athlet;

		athlet_pos[athlet] = athlets.insert(
			athlet_pos[before_athlet], athlet);
	}

	for (auto i : athlets) {
		cout << i << ' ';
	}
	cout << endl;

	return 0;
}
